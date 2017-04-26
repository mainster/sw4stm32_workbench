/**
 * @file        irq_callbacks.c
 *
 * @date        26 Jan 2017
 * @author      Manuel Del Basso (mainster)
 * @email       manuel.delbasso@gmail.com
 *
 * @ide         System Workbench ac6 (eclipse stm32)
 * @stdperiph   STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 *
 * @brief       Callback handler implementations.
 *
   @verbatim

    ------------------------------------------------------------------------

    Copyright (C) 2016  Manuel Del Basso

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    ------------------------------------------------------------------------

   @endverbatim
 *
 *  @snippet this
 */
#include "stdint.h"
#include "arm_math.h"
#include "main.h"
#include "pid.h"
#include "signalGen.h"
#include "md_stm32f4_disco.h"
#include "actuators.h"
#include "irq_callbacks.h"

/**
 * @addtogroup MD_APP
 * @{
 */

/**
 * @addtogroup  APP_IRQ_Handler
 * @{
 */

/**
 * @addtogroup IRQ_Handler_Macros
 * @{
 */

/** @} */

/**
 * @addtogroup IRQ_Handler_Typedefs
 * @{
 */

/** @} */

/**
 * @addtogroup IRQ_Handler_Variables
 * @{
 */

extern arm_pid_instance_f32 PIDX __attribute__ ((unused));
extern arm_pid_instance_f32 PIDY;        

extern float setpoint_tgl;
extern struct triangWave triStruct;
extern struct PID_DATA  pidDataX; 
extern struct PID_DATA  pidDataY; 
extern float ADC_fBuff[5];  
extern int16_t ADC_MultiConvBuff[5];     
extern ActuatorSafestateGuard_t ASG;
extern uint32_t ticks;

static uint16_t vectorCtr = 0;
static int8_t dir = 1;


/** @} */

/**
 * @addtogroup IRQ_Handler_Functions
 * @{
 */

/**
 * @brief      DMA Channel 2 Stream 0 transmission complete IRQ handler.
 *
 *             PID: This callback provides compensator calculation.
 */
void DMA2_STREAM_0_IRQ_Handler (void) {
    volatile float32_t setpnt=0;
    /* FIXME: DMA_IT_TC */
    DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TC);
    DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, DISABLE);
    DBG_ADC_TIMING(0);

    if (!ticks--) {
        MD_DISCO_LedOff(LED_GREEN);
    }

    /* Test on DMA Stream Half Transfer interrupt */
    if(DMA_GetITStatus(DMA2_Stream0, DMA_IT_HTIF0))
    {
        DBG_PID_TIMING(1);
        /* Clear DMA Stream Half Transfer interrupt pending bit */
        DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_HTIF0);

        /* Turn LED3 off: Half Transfer */
        MD_DISCO_LedOff(LED_RED);

        /**
         * @par First half of ADC group:
         *     Add code here to process the first half of ADC group scan-buffer
         *     It's good to call the X-channel PID instance some cycles earlier.
         *     There is nothing wrong under the condition that both compensator
         *     outputs are passed to DAC queue at the same time!
         */
    }

    /* Test on DMA Stream Transfer Complete interrupt */
    if(DMA_GetITStatus(DMA2_Stream0, DMA_IT_TCIF0))
    {
        DBG_ADC_TIMING(0);
        DBG_PID_TIMING(0);
        /* Clear DMA Stream Transfer Complete interrupt pending bit */
        DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);

        /* Turn LED3 on: End of Transfer */
        MD_DISCO_LedOn(LED_RED);

        /**
         * @par Process Y-channel position signal here:
         *     If the Y-PID instance has processed the y position, call the
         *     dualSecureActuator update function to hold the discrete deadtime at a
         *     minimum. BEFOR updating the DAC, assure you that this is not a nested
         *     call of a running x-channel pid algorithm !!! Maybe the NVIC
         *     prioritys could help for this?!
         */
    }

    /**
     * @par Analog balanced position signal:
     *     The analog balanced position signal must be set before the AD conversion
     *     positive offset due to the input voltage range of the integrated ADC
     *     cells. Analogously, the sym. Pos. Signal is then changed to AREF / 2
     *     Pushed upwards. Here, the offset is digitally subtracted again
     *     Controller-internal with symmetrical sequences can be worked. The same
     *     applies to the DAC hardware, the + offset (from before ADC) Thus
     *     subtracted analogously again after the DA converter. Therefore, Balanced
     *     signal before describing the DAC data hold register AREF / 2.
     */

    /**
     * @par Direct memory access to array @ref ADC_MultiConvBuff:
     *     Take a copy of direct-memory-accessed array ADC_MultiConvBuff up to this
     *     point, the access macros @ref SETPOINT_Y_FLOAT, @ref POS_Y_FLOAT ... also
     *     holds the new values
     */
    for (__IO uint8_t k=0; k < ADC_N_REGULAR_CHANNELS; k++) {
        ADC_fBuff[k] = (float) ((float) (ADC_MultiConvBuff[k] - AN_BIAS_INT) * VLSB);
    }

    // --------------------------------------------------------------
    //            Laser interrupter control
    // --------------------------------------------------------------
    /* To implement a blanking function controled by setpoint vectors,
     * simply add a blanking information column to the setpoint datastream
     *
     * [x-coordinate; y-coordinate; blankingBit]
     */

    // --------------------------------------------------------------
    //            Setpoint source selector
    // --------------------------------------------------------------
    while (1) {

        /* Internal only, no remote offset or something else. */
        if (g.setpointSrc == INTERNAL_SETPOINT) {   
            pidErr_x =  setpoint_tgl - POS_X_FLOAT;
            pidErr_y =  setpoint_tgl - POS_Y_FLOAT;
            setpnt = setpoint_tgl;
        }
        /* setpoint is sourced by analog input. */
        if (g.setpointSrc == ANALOG_SETPOINT) {     
            /* flot conversion! */
        }
        /* Remote only, no internal signal or offset. */
        if (g.setpointSrc == REMOTE_SETPOINT) {
            pidErr_x =  pidDataX.W_remf - POS_X_FLOAT;
            pidErr_y =  pidDataY.W_remf - POS_Y_FLOAT;
            setpnt = pidDataX.W_remf;
        }
        /* PID bypassed, remote setpoint feed throgh. */
        if (g.setpointSrc == REMOTE_OPENLOOP) {
            updateActuator_f(pidDataY.W_remf, pidDataY.W_remf);
            break;
        }
        /* Mixed mode, summing internal + external setpoint. */
        if (g.setpointSrc == REMOTE_INTERNAL_MIXED) {
            pidErr_x = ((float)(setpoint_tgl + pidDataX.W_remf)) - POS_X_FLOAT;
            pidErr_y = ((float)(setpoint_tgl + pidDataY.W_remf)) - POS_Y_FLOAT;
            setpnt = ((float)(setpoint_tgl + pidDataX.W_remf));
        }
        if ((g.setpointSrc != INTERNAL_SETPOINT) && 
            (g.setpointSrc != ANALOG_SETPOINT) && 
            (g.setpointSrc != REMOTE_OPENLOOP) && 
            (g.setpointSrc == REMOTE_OPENLOOP)) {
            /* Error state, halt! */
            MD_DISCO_LedOn(LED_GREEN);
        MD_DISCO_LedOn(LED_RED);
    }

        /**
         * @brief      Invoke PID calculation and trigger TicToc Tic.
         */
    DBG_PID_TIMING_TIC();
    updateActuator_f (setpnt,arm_pid_f32(&PIDY, pidErr_y));

        /**
         * @brief      Trigger TicToc Toc.
         */
    DBG_PID_TIMING_TOC();

    break;
}
    /* Set LEDs according to which angular position is greater */
    //    if ( POS_Y_FLOAT > SETPOINT_Y_FLOAT) {
    //        MD_DISCO_LedOn(LED_GREEN);
    //        MD_DISCO_LedOff(LED_RED);
    //    } else if ( POS_Y_FLOAT <  SETPOINT_Y_FLOAT) {
    //        MD_DISCO_LedOn(LED_RED);
    //        MD_DISCO_LedOff(LED_GREEN);
    //    } else {
    //        MD_DISCO_LedOff(LED_ALL);
    //    }
}

/**
 * @brief      Analog to digital converter IRQ callback handler.
 *
 *             Provides realtime service functionality to handle discretization
 *             process of the closed-loop position feedback signal.
 *
 * @note       For in-depth documentation of the analog subsystem, refer to
 *             [LINK MARKDOWN CHAPTER]
 *
 * @snippet this ADC_IRQ_Flag_Handling
 *
 * @anchor AnalogWatchdogIntHandler
 * @anchor ADC_EndOfConversionIntHandler
 */
    //! [ADC_IRQ_Flag_Handling]
void ADC_IRQ_Handler (void) {
    /*
     * @brief      End of conversion interrupt occured.
     *
     *             In normal processing, this line never should be reached and
     *             must treated as error state!
     */
    if (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == SET) {
        ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);

        MD_DISCO_LedOn(LED_ALL);
        return;
    }

    /**
     * @brief      Check if an analog watchdog interrupt has been requested.
     *
     *             In case of an analog watchdog interrupt has occured, it must
     *             be determined wether if the ASG systems fuse has to be
     *             triggerd or if only the out-of-range integrator must be
     *             partially charged. @{
     */
    if (ADC_GetFlagStatus(ADC1, ADC_FLAG_AWD) == SET) {
        ADC_ClearITPendingBit(ADC1, ADC_IT_AWD);
        
        /**
         * @brief      Check ASG out-of-range integrator for limit violations in
         *             upper and lower direction.
         */
        if ((ASG.integrator <= ASG.lowerThreshold) ||
            (ASG.integrator >= ASG.upperThreshold)) {
            if (! ASG.tripped) {

                /**
                 * @brief      Set ASG into "tripped" state.
                 */
                ASG.tripped = 1;

                /**
                 * @brief      Swap DAC output-register-write function pointer
                 *             reference to the "ASG tripped" handler
                 *             DAC_SetDualChanSigned_Tripped 
                 */
                DAC_SecureSetDualChanSigned = &DAC_SetDualChanSigned_Tripped;  
            }
            return;
        }
        else {
            /**
             * @brief      If the ASG system isn't already registered to charge
             *             the integrator (ASG_CHARGING_INTEGRATOR), alter the
             *             ASG state correspondingly.
             */
            if ((ASG.state == ASG_STATIONARY_INTEGRATOR) ||
                (ASG.state == ASG_DISCHARGING_INTEGRATOR)) {
                ASG.state = ASG_CHARGING_INTEGRATOR;
            printf("ASS:charging_start...\n");
        }

            /**
             * @brief      Charge integrator since this is the analog watchdog
             *             branch where the tripping limit has not been reached
             *             yet.
             */
            ASG.integrator += ASG.trippingLevel * (double)(TS*1e-6)/(ASG.tripTime*1e-3);
        }
        return;
    }

    /** @} */

    printf("bad ADC IntReq source\n");
    //! [ADC_IRQ_Flag_Handling]
}


/**
 * @brief      Timer 4 overrun IRQ callback.
 * 
 * Provides waveform generation as a part of . 
 */
void TIM4_IRQ_Handler (void) {
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);

        if (ASG.tripped) {
            MDB_GPIO_Toggle(BEAM_INTERRUPT);
        }

        /**
         * Check if line1 or line2 waveform is currently active or not.
         */
        if ((g.waveForm == CMD_TESTVECT_1) ||
            (g.waveForm == CMD_TESTVECT_2)) {

            /**
             * Calculate vector array row count.
             */
            short nRows = (g.waveForm == CMD_TESTVECT_1)
        ? sizeof(line1)/sizeof(line1[0])
        : sizeof(line2)/sizeof(line2[0]);

        if (vectorCtr >= nRows-1)   dir = -1;
        if (vectorCtr == 0)         dir = 1;

        vectorCtr += dir;

        (g.waveForm == CMD_TESTVECT_1)
        ?  ((line1[vectorCtr][0] == 0)
            ? beamCtrl(BEAM_CTRL_SOURCE_MANUAL, GPIO_OFF)
            : beamCtrl(BEAM_CTRL_SOURCE_MANUAL, GPIO_ON))
        :  ((line2[vectorCtr][0] == 0)
            ? beamCtrl(BEAM_CTRL_SOURCE_MANUAL, GPIO_OFF)
            : beamCtrl(BEAM_CTRL_SOURCE_MANUAL, GPIO_ON));

        setpoint_tgl = (g.waveForm == CMD_TESTVECT_1)
        ? ((float)line1[vectorCtr][1] * g.ampl_f)
        : ((float)line1[vectorCtr][1] * g.ampl_f);
        return;
    }

        /* >>>>>>>>>>>>  Squarewave  >>>>>>>>>>>> */
    if (g.waveForm == SQUAREWAV) {
        if (*pSeq_f == '\0') {
            pSeq_f = &squarewaveBuff[0];
            pInt = &interrupterBuff[0];
            MDB_GPIO_Toggle(TRIGGER_SRC);
        }
        setpoint_tgl = (float) * pSeq_f++ * g.ampl_f;
        beamCtrl(BEAM_CTRL_SOURCE_MANUAL, (*pInt++) ? GPIO_ON : GPIO_OFF );
        return;
    }

        /* >>>>>>>>>>>>  Triangular  >>>>>>>>>>>> */
    if (g.waveForm == TRIANG) {
        if (setpoint_tgl  <= triStruct.top) {
            setpoint_tgl += triStruct.derivate;
        }
        else {
            setpoint_tgl = triStruct.bottom;
        }
    }
    return;
}
printf("bad TIM4 IntReq source\n");
}



// ==============================================================
//     IRQ callback:    Timer 2 overrun 
// ==============================================================
void TIM2_IRQ_Handler (void) {

    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
        DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);
        return;
    }

    if (TIM_GetITStatus(TIM2, TIM_IT_Trigger ) != RESET) {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Trigger);
        DBG_ADC_TIMING(1);
        return;
    }
    /* Error state */
    //    MD_DISCO_LedOn(LED_ALL);
    while (1);;
}


/** @} */

/** @} */

/** @} */


// ==============================================================
//    IRQ callback:    DMA2_Stream0 transmission-complete
// ==============================================================
//     PID: This callback initiates compensator calculation
// ==============================================================
// void DMA2_Stream0_IRQHandler(void) {
//    //!< Since this is the only irq flag for this handler, clear direct. 
//    DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TC);
//    DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, DISABLE);
//    DBG_ADC_TIMING(0);



//  //!< Test on DMA Stream Half Transfer interrupt 
//  if(DMA_GetITStatus(DMA2_Stream0, DMA_IT_HTIF0))
//  {
//    DBG_PID_TIMING(1);
//    //!< Clear DMA Stream Half Transfer interrupt pending bit 
//    DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_HTIF0);

//    //!< Turn LED3 off: Half Transfer 
//    MD_DISCO_LedOff(LED_RED);

//    // Add code here to process first half of buffer (ping)
//  }

//  //!< Test on DMA Stream Transfer Complete interrupt 
//  if(DMA_GetITStatus(DMA2_Stream0, DMA_IT_TCIF0))
//  {
//    //!< Clear DMA Stream Transfer Complete interrupt pending bit 
//    DBG_ADC_TIMING(0);
//    DBG_PID_TIMING(0);
//    DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);

//    //!< Turn LED3 on: End of Transfer 
//    MD_DISCO_LedOn(LED_RED);

//    // Add code here to process second half of buffer (pong)
//  }
// }

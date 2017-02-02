/**
 * @file        isr_callbacks.c
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

	Copyright (C) 2016	Manuel Del Basso

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

    @endverbatim
 *
 */
#include "stdint.h"
#include "arm_math.h"
#include "main.h"
#include "pid.h"
#include "signalGen.h"
#include "md_stm32f4_disco.h"
#include "actuators.h"
#include "isr_callbacks.h"

/**
 * @addtogroup MD_APP
 * @{
 */

/**
 * @addtogroup  APP_IRQ_Handler
 * @brief   Provides project specific callback handlers to service interrupt requests.
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
extern autoSaveSystem_t ASG;
extern uint32_t ticks;
extern int8_t dir;

/** @} */

/**
 * @addtogroup IRQ_Handler_Functions
 * @{
 */

/**
 * @brief      DMA2_Stream0 transmission-complete.
 *
 *             PID: This callback provides compensator calculation.
 */
void DMA2_Stream0_IRQHandler(void) {
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
     *     Add code here to process the first half of ADC group
     *     scan-buffer It's good to call the X-channel PID instance some cycles
     *     earlier. There is nothing wrong under the condition that both
     *     compensator outputs are passed to DAC queue at the same time!
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
        if (g.setpointSrc == INTERNAL_SETPOINT) {   ///< Internal only, no remote offset or something else 
          pidErr_x =  setpoint_tgl - POS_X_FLOAT;              
          pidErr_y =  setpoint_tgl - POS_Y_FLOAT;    
          setpnt = setpoint_tgl;
        }
        if (g.setpointSrc == ANALOG_SETPOINT) {     ///< setpoint is sourced by analog input 
                // flot conversion!
        }
        if (g.setpointSrc == REMOTE_SETPOINT) {     ///< Remote only, no internal signal or offset 
          pidErr_x =  pidDataX.W_remf - POS_X_FLOAT;              
          pidErr_y =  pidDataY.W_remf - POS_Y_FLOAT;              
          setpnt = pidDataX.W_remf;
        }
        if (g.setpointSrc == REMOTE_OPENLOOP) {     ///< PID bypassed, remote setpoint feed throgh 
            updateActuator_f(pidDataY.W_remf, pidDataY.W_remf);
            break;
          }
        if (g.setpointSrc == REMOTE_INTERNAL_MIXED) {     ///< Mixed mode, summing internal + external setpoint 
          pidErr_x = ((float)(setpoint_tgl + pidDataX.W_remf)) - POS_X_FLOAT;
          pidErr_y = ((float)(setpoint_tgl + pidDataY.W_remf)) - POS_Y_FLOAT;
          setpnt = ((float)(setpoint_tgl + pidDataX.W_remf));
        }
        if (  (g.setpointSrc != INTERNAL_SETPOINT) && (g.setpointSrc != ANALOG_SETPOINT)  && \
          (g.setpointSrc != REMOTE_OPENLOOP)   && (g.setpointSrc == REMOTE_OPENLOOP) ) {
             /* Error state, halt */
         MD_DISCO_LedOn(LED_GREEN);
       MD_DISCO_LedOn(LED_RED);
     }

        /**< Calculate PID here */
     DBG_PID_TIMING_TOG();
     updateActuator_f (setpnt,arm_pid_f32(&PIDY, pidErr_y));
     DBG_PID_TIMING_TOG();

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


// ==============================================================
//     IRQ callback:    ADC end-of-conversion 
//                      Analog Watchdog IRQ
// ==============================================================

/**
 * @brief      Analog to digital converter IRQ callback handler.
 *
 *             Provides realtime service functionality to handle discretization
 *             process of the closed-loop position feedback signal.
 *
 * @note       For in-depth documentation of the analog subsystem, refer to
 *             [LINK MARKDOWN CHAPTER] 
 * \sideeffect
 * @sideeffect
 *
 * @anchor AnalogWatchdogIntHandler
 * @anchor ADC_EndOfConversionIntHandler
 */
 void ADC_IRQHandler(void) {
  if (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == SET) {
    ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
        /* End of conversion interrupt occured */

//        /* Error state */
//        MD_DISCO_LedOn(LED_ALL);
//        uint16_t W_now = ADC_MultiConvBuff[1];   // setpoint (Isens_y)
    while (1);;
//        return;
  }


    /* Analog watchdog interrupt occured */
  if (ADC_GetFlagStatus(ADC1, ADC_FLAG_AWD) == SET) {
    ADC_ClearITPendingBit(ADC1, ADC_IT_AWD);
    if ((ASG.state == ASG_STATIONARY_INTEGRATOR) ||
      (ASG.state == ASG_DISCHARGING_INTEGRATOR)) {
      ASG.state = ASG_CHARGING_INTEGRATOR;
    printf("ASS:charging_start...\n");
  }
        /**
         * @brief      Check ASG for integrator limit */
  if ((ASG.integrator <= ASG.lowerVal) || (ASG.integrator >= ASG.upperVal)) {   
    if (! ASG.tripped) {
                ASG.tripped = 1;                    /**< Set "tripped" state if it is so */
      DAC_SecureSetDualChanSigned = \
                      &DAC_SetDualChanSigned_Tripped;  /**< set function pointer to the "ASG tripped" handler*/
    }
    return;
  }
        else {                                  /**< else increment integrator */
  ASG.integrator+= ASG.upperVal * (double)TS*1e-6/(ASG.tripTime);
}
return;
}
printf("bad ADC IntReq source\n");
}



// ==============================================================
//     IRQ callback:    Timer 4 overrun     Waveform Generator
// ==============================================================


/**
 * @brief      { function_description }
 */
void TIM4_IRQHandler(void) {
  if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) {
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);

    if (ASG.tripped) {
      MDB_GPIO_Toggle(BEAM_INTERRUPT);
    }

        /* Check which waveform mode is active
         */

        /* >>>>>>>>>>>>  Test vector  >>>>>>>>>>>> */
    if (g.waveForm == CMD_TESTVECT) {
      if (vectorCtr >= NVECTORS-1) {
        dir = -1;
      }
      if (vectorCtr == 0) {
        dir = 1;
      }
      vectorCtr = vectorCtr + (dir);
      if (line1[vectorCtr][0] == 0)
        beamCtrl(BEAM_CTRL_SOURCE_MANUAL, GPIO_OFF );
      else
        beamCtrl(BEAM_CTRL_SOURCE_MANUAL, GPIO_ON );

      setpoint_tgl = (float)line1[vectorCtr][1] * g.ampl_f;


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
  }
}



// ==============================================================
//     IRQ callback:    Timer 2 overrun 
// ==============================================================
void TIM2_IRQHandler(void) {

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

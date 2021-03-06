/**
 *	Keil project for XY-GalvoScanner
 *  29-04-2015
 *
 *
 *	@author		Manuel Del Basso
 *	@email		Manuel.DelBasso@googlemail.com  
 *	@ide		Keil uVision 5
 *	@packs		STM32F4xx Keil packs version 2.2.0 or greater required
 *	@stdperiph	STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 *
 *	Additional defines in "Options for Target" > "C/C++" > "Defines"
 *		- ARM_MATH_CM4
 *		- __FPU_PRESENT = 1
 */
/* Include core modules */
#include "stm32f4xx.h"
/* Include my libraries here */
#include "defines.h"
#include "tm_stm32f4_delay.h"
//#include "tm_stm32f4_onewire.h"
//#include "tm_stm32f4_ds18b20.h"
//#include "tm_stm32f4_disco.h"
#include "tm_stm32f4_usart.h"
#include "tm_stm32f4_pwm.h"

#include "tm_stm32f4_adc.h"
#include "md_stm32f4_dac.h"
#include "md_stm32f4_disco.h"

#include "tm_stm32f4_timer.h"
#include "defines.h"
//#include "tm_stm32f4_ili9341.h"
//#include "tm_stm32f4_stmpe811.h"


#include <stdio.h>
#include "arm_architect.h"
/* Include ARM math */
#include "arm_math.h"
#include "main.h"
#include "adc_dac_dma.h"
#include "signalGen.h"

#include <string.h>
#include <stdlib.h>
#include "pid.h"
#include "tools.h"
#include "mdb_gpio.h"
#include "stm32f4xx_adc.h"


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//          IMPORT from MATLAB code generated project pid_lib_v3
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#include "pid4.h"                /* Model's header file */
#include "rtwtypes.h"                  /* MathWorks types */
#include "stm32f4xx.h"

/* RCC_Configuration defined in RCC_Configuration.c file */
/* Clock configuration function provided as example */
void RCC_Configuration(void);

/* Real-time model */
extern RT_MODEL_pid4 *const pid4_M;

/* Set which subrates need to run this base step (base rate always runs).*/
/* Defined in pid4.c file */
extern void pid4_SetEventsForThisBaseStep(boolean_T*);

/* Flags for taskOverrun */
static boolean_T OverrunFlags[1];

/* Number of auto reload timer rotation computed */
static uint32_t autoReloadTimerLoopVal_S = 10;

/* Remaining number of auto reload timer rotation to do */
static uint32_t remainAutoReloadTimerLoopVal_S = 10;

/****************************************************
   SysTick_Handler function
   This handler is called every tick and schedules tasks
 *****************************************************/
void SysTick_Handler(void);
void TimingDelay_Decrement(void);

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//    END of    IMPORT from MATLAB code generated project pid_lib_v3
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


/* actual position */

static struct triangWave_t triStruct;
static struct autoSaveSystem ass;


// ==============================================================
//   This is needed in order to provide printf functionality
// ==============================================================
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)

struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
    
PUTCHAR_PROTOTYPE
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
    USART_SendData(USART1, (uint8_t) ch);

    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
    {}

    return ch;
}


	
// ==============================================================
//                         Prototypes 
// ==============================================================
void gpio_init(void);
void exportInternalClks (void);
void gpio_init_mco1(void);
void gpio_init_mco2(void);
//static void Console (struct PID_DATA *pid, struct FPID_DATA *fpid, structType type);
void resetPID (void);
//void consoleCase (struct PID_DATA *pid);
void adc12_init (uint16_t gpio_pin_adc1, 
								 uint16_t gpio_pin_adc2, 
								 uint16_t EndOfConvInt, 
								 ADC_TypeDef* ADCxEOT);
void fastConsoleCase(arm_pid_instance_f32 *pid);

int updateActuator_f(float I_set_x, float I_set_y);
int beamCtrl(beamCtrlSource_t src, MDB_GPIO_STATE_t newState );


#define RX_FRAME_PADDING_CHAR   '~'

/* The function pointer updateActuator is used as non-privileged access function 
 * to internal DAC output registers. After ass detects a tripping condition ,
 * updateActuator function-pointer targets to updateActuator_fused_callback */
//int (*updateActuator)                (float, float);
//int (*updateActuator_callback)       (float, float);
//int (*updateActuator_fused_callback) (float, float);
// ==============================================================
//                  Compensator things
// ==============================================================
#define ALGO_OLD 				10
#define ADJUSTING_SETPOINT	    11		// Stellungsalgorithmus
#define SPEED_SETPOINT			12		// Geschwindigkeitsalgorithmus
#define ALGORITHM 	ADJUSTING_SETPOINT

/** This is the factor used in the gui to enlarge resolution on int-only
 * QSlider ranges
 */
#define SLIDER_VAL_DEVIDER  10000UL
//int SLIDER_VAL_DEVIDER  = 10000;


//@@@#pragma O0
struct PID_DATA     pidDataX;
struct PID_DATA     pidDataY;
struct FPID_DATA    fpidData;

//@@@#pragma O2

volatile float UNIT_MIKRO = 1/1000000;
volatile int16_t pidOut = 0;

/**< ADC dual mode DMA receive buffer. */
__IO int16_t ADC_MultiConvBuff[5];     
/* ADC values access buffer
 * Because we use a float based instance of PID, we need to cast the 
 * ADC_MultiConvBuff[0] buffer from integral type to float !!!
 */
__IO float ADC_fBuff[5];     
/**< Define indices for ADC_MultiConvBuff[ ] acording to setpoint (INDEX_W) ans
 * process output (INDEX_P). */
#ifndef INDEX_Py
    #define INDEX_Px     0
    #define INDEX_Py     1
    #define INDEX_Ix     2
    #define INDEX_W      3
#endif
/* Acess macros for actual ADC values 
 * 
 * ToDo:    Maybe this macros needs to be atomic! 
 */
#define POS_X_FLOAT         (float) ADC_fBuff[INDEX_Px]	/* actual position */
#define POS_Y_FLOAT         (float) ADC_fBuff[INDEX_Py]	/* actual position */
#define SETPOINT_Y_FLOAT	(float) ADC_fBuff[INDEX_W]	/* position we want to have */


#define SIGNW -1

#define INTERNAL_SETPOINT 
#undef INTERNAL_SETPOINT 

#if  defined (KEIL_IDE)
	#pragma O0
#elif  defined (__GNUC__)
	#pragma GCC push_options
	#pragma GCC optimize ("O0")
#endif

    //float temps[EXPECTING_SENSORS];
    
float pidErrBuff[2];        /* PID error buffer*/
#define pidErr_x   pidErrBuff[0]
#define pidErr_y   pidErrBuff[1]

float toPlantBuff[2];        /* PID output buffer*/
#define toPlant_x   toPlantBuff[0]
#define toPlant_y   toPlantBuff[1]

    float duty = 0;             /* Duty cycle for PWM */
    arm_pid_instance_f32 PIDX;   /* ARM PID Instance, float_32 format */
    arm_pid_instance_f32 PIDY;   /* ARM PID Instance, float_32 format */
__IO float toPlant;              /* next actuator value in Volt */
    int16_t toPlant_int;       /* next actuator value integral type */

    int16_t test1 = UPPER_DAC_LIMIT_SIGNED;
    int16_t test2 = LOWER_DAC_LIMIT_SIGNED;

#if  defined (KEIL_IDE)
	#pragma O2
#elif  defined (__GNUC__)
	#pragma GCC pop_options
#endif


/* Choose PID parameters */
#define PID_PARAM_KP		0.5			/* Proporcional */
#define PID_PARAM_KI		0.0		/* Integral */
#define PID_PARAM_KD		0.0			/* Derivative */

// ==============================================================
//                      beam interrupter
// ==============================================================
 
// ==============================================================
//                ass - auto shutdown system
// ==============================================================
#define ASS_TRIPPING_LOWER_DEFAULT    -1.2f
#define ASS_TRIPPING_UPPER_DEFAULT    1.2f
#define ASS_SAFEVALUE_DEFAULT         0
#define ASS_TRIPPING_TIME_DEFAULT    0.750f      // s


//#define ASS_100US                   100
// ==============================================================
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// ==============================================================
//      This indices HAVE TO STAY IN SYNC WITH Qt GUI SOURCES 
// ==============================================================
    volatile int NC = 3;		// index 0...2  (short command)
    volatile int FS = 2;		// index 3...4	(fieldsep)
    volatile int NV = 10;	    // index 5...14 (value)
// ==============================================================
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// ==============================================================
    
#define dbg(IO_PIN, LEVEL) ((LEVEL) == 1 ? \
	 GPIO_WriteBit((GPIO_TypeDef*)DBG_IO, IO_PIN, Bit_SET) :\
	 GPIO_WriteBit((GPIO_TypeDef*)DBG_IO, IO_PIN, Bit_RESET))

#define dbgT(IO_PIN) TM_GPIO_TogglePinValue \
                                ((GPIO_TypeDef*)DBG_IO, IO_PIN)

/* Define static usart strings used as GUI error / status messages  
 */
#define  MSG_GUI_UART1_RECV_BUFF_FULL   "@GUI:usart1RecvBuffFull\n"  ///< Inform Gui about a full usart1 buffer
 
// ==============================================================
//            Signal generator things
// ==============================================================

uint16_t sinctr=0;
/*volatile uint16_t tab[1024];

//const int16_t sintab2[]= {0,110,220,329,437,543,647,748,846,941,1032,1119,1201,1279,1351,1418,1479,1534,1583,1626,1662,1692,1714,1730,1739,1741,1735,1723,1704,1678,1645,1606,1560,1508,1449,1385,1316,1241,1161,1076,987,894,798,698,595,490,384,275,165,55,-55,-165,-275,-384,-490,-595,-698,-798,-894,-987,-1076,-1161,-1241,-1316,-1385,-1449,-1508,-1560,-1606,-1645,-1678,-1704,-1723,-1735,-1741,-1739,-1730,-1714,-1692,-1662,-1626,-1583,-1534,-1479,-1418,-1351,-1279,-1201,-1119,-1032,-941,-846,-748,-647,-543,-437,-329,-220,-110,-0};

    //int16_t sintab[]= { 0,262,519,769,1005,1225,1425,1601,1751,1873,
//									1963,2022,2047,2039,1997,1922,1816,1680,1516,
//									1328,1117,889,645,391,131,-131,-391,-645,-889,
//									-1117,-1328,-1516,-1680,-1816,-1922,-1997,-2039,
//								-2047,-2022,-1963,-1873,-1751,-1601,-1425,-1225,
//									-1005,-769,-519,-262,-0};
*/
// ==============================================================
// ==============================================================
// ==============================================================
volatile uint16_t gW = 0;
volatile uint16_t gY = 0;
volatile uint16_t multiADC[2];

volatile int16_t P_now, W_now;

volatile float  KP_INIT  =	1;
volatile float  KI_INIT, \
                KD_INIT, \
                TF_INIT = 0;


static struct items items_list[] = {
        { .name = "kp:", .id = KP },
        { .name = "ki:", .id = KI },
        { .name = "kd:", .id = KD },
        { .name = "w::", .id = W  }
};

static struct itemsw itemsw_list[] = {
        { .name = "sin", .idw = COS },
        { .name = "tri", .idw = TRIANG },
        { .name = "rec", .idw = SQUAREWAV },
        { .name = "s2 ", .idw = QUADRATIC },
        { .name = "is2", .idw = I_QUADRATIC },
        { .name = "saw", .idw = SAWTOOTH},
        { .name = "rem", .idw = CMD_REMOTE_SETPOINT},
        { .name = "orm", .idw = CMD_OPENLOOP_REMOTE},
        { .name = "ast", .idw = CMD_ANALOG_SETPOINT},
        { .name = "ist", .idw = CMD_INTERNAL_SETPOINT},
        { .name = "rfr", .idw = CMD_REFRESH_RATE},
};

static struct itemsm itemsm_list[] = {
        { .name = "pid_init", .idm = misc_pid_init },
        { .name = "pid_Controller", .idm = misc_pid_Controller },
        { .name = "pid_Reset_Integrator", .idm = misc_pid_Reset_Integrator },
        { .name = "update_pid_data", .idm = misc_update_pid_data  },
        { .name = "safetyEnable" , .idm = misc_assOnOff_cmd  },
        { .name = "safetyUpperLim", .idm = misc_assUpperLim_cmd  },
        { .name = "safetyLowerLim", .idm = misc_assLowerLim_cmd  },
        { .name = "safetyTripp", .idm = misc_assTrippTime_cmd  },
        { .name = "safetyVal", .idm = misc_assSaveVal_cmd  },
        { .name = "EnableBeam", .idm = misc_beamOn_cmd  },
        { .name = "DisableBeam", .idm = misc_beamOff_cmd  },
};


//float W_INT_BUFF[33]= { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0 };

//float *TEMP_WANT2 = W_INT_BUFF;	/* position we want to have */
//volatile uint32_t ct = 20000;

volatile float setpoint_tgl = (1.25-0.4);

/*
 * function pointer */

//int (*f2) (float);

//int func14(float fp1, float fp2) {
//    printf("func14dir1: %f func14dir2: %f\n", fp1,fp2);//     %f\n",*fp,  *(fp+1));
//    return 14;//updateActuator_f(*fp, *(fp+1)); 
//}


// ==============================================================
//     IRQ callback:    Timer 5 overrun     Heartbeat timer
// ==============================================================
//void TIM5_IRQHandler(void) {
//    if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET) {
//		TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
//        
//    }
//}
// ==============================================================
//     IRQ callback:    Timer 4 overrun     Waveform Generator    
// ==============================================================
#define NO1 1
#ifdef NO1
void TIM4_IRQHandler(void) {
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);

        if (ass.tripped) {
            MDB_GPIO_Toggle(BEAM_INTERRUPT);
        }
        
        /* Check which waveform mode is active */
        if (g.waveForm == SQUAREWAV) {
            if (*pSeq_f == '\0') {
                pSeq_f = &squarewaveBuff[0];
                pInt = &interrupterBuff[0];
                MDB_GPIO_Toggle(TRIGGER_SRC);
            }
            setpoint_tgl = (float)*pSeq_f++ * g.ampl_f;
            beamCtrl(BEAM_CTRL_SOURCE_MANUAL, (*pInt++) ? GPIO_ON : GPIO_OFF );
            return;
        }
        
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
#else
void TIM4_IRQHandler(void) {
fff
volatile float ft=0;

    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
        
        /* Check which waveform mode is active */
        if (g.waveForm == SQUAREWAV) {
            if (vectorCtr >= NVECTORS) {
                vectorCtr = 0;
            }
            setpoint_tgl = (float)line1[vectorCtr][1] * g.ampl_f;
            if (line1[vectorCtr++][0] == 0) 
                beamCtrl(BEAM_CTRL_SOURCE_MANUAL, GPIO_OFF );
            else
                beamCtrl(BEAM_CTRL_SOURCE_MANUAL, GPIO_ON );

        }
    }
}
#endif
// ==============================================================
//    IRQ callback:    DMA2_Stream0 transmission-complete
// ==============================================================
//     PID: This callback initiates compensator calculation 
// ==============================================================
void DMA2_Stream0_IRQHandler(void) {
    /**< Since this is the only irq flag for this handler, clear direct. */
    DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TC);
    DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, DISABLE);

/*    
    if (ct >= 5) {
        ct--;
    }
    else {
        ct = 100000;
        if (setpoint_tgl >= (float)1.25) {
            setpoint_tgl = (float)(1.25-0.05);
        }
        else {
            setpoint_tgl = (float)(1.25+0.05);
        }
    }
*/

/**< Das analoge symmetrische Positionssignal muss vor der AD- Wandlung einen positiven 
 * Offset erhalten wegen des Eingangsspannungsbereichs der intergrierten ADC-Zellen.
 * Analog wird also das sym. Pos. Signal um AREF/2 nach oben geschoben.
 * Hier wird der Offset digital wieder abgezogen damit auch Regler-Intern mit symmetrischen 
 * Sequenzen gearbeitet werden kann. 
 * 
 * ==>  Fuer die DAC-Hardware gilt das Gleiche, der +Offset (von vor ADC) wird also nach dem 
 *      DA-Wandler analog wieder subtrahiert. Darum muss dem intern symmetrischen Signal
 *      vor dem beschreiben des DAC-Data-hold-registers wieder AREF/2 aufaddiert werden.
 */
 
/**< 
* Take a copy of direct-memory-accessed array ADC_MultiConvBuff 
* Up to this point, the access macros SETPOINT_Y_FLOAT, POS_Y_FLOAT ... 
* also holds the new values 
*/
    for (__IO uint8_t k=0; k<4; k++) {
        ADC_fBuff[k] = (float) ((float) (ADC_MultiConvBuff[k] - AN_OFFSET) * LSB);     
    }
    
    pid4_U.ym = (real_T) ADC_fBuff[INDEX_Py];
    
    
// ==============================================================
//            Laser interrupter control
// ==============================================================
/* To implement the functionality of blanking a setpoint vector, 
 * simply add a blanking information column to the setpoint 
 * datastream
 *
 * [x-coordinate; y-coordinate; blankingBit] 
 */

// ==============================================================
//            Setpoint source selector
// ==============================================================
    while (1) {
        if (g.setpointSrc == INTERNAL_SETPOINT) {   ///< Internal only, no remote offset or something else 
            pidErr_x =  setpoint_tgl - POS_X_FLOAT;              
            pidErr_y =  setpoint_tgl - POS_Y_FLOAT;              
        }
        if (g.setpointSrc == ANALOG_SETPOINT) {     ///< setpoint is sourced by analog input 
                // flot conversion!
        }
        if (g.setpointSrc == REMOTE_SETPOINT) {     ///< Remote only, no internal signal or offset 
            pidErr_x =  pidDataX.W_remf - POS_X_FLOAT;              
            pidErr_y =  pidDataY.W_remf - POS_Y_FLOAT;              
        }
        if (g.setpointSrc == REMOTE_OPENLOOP) {     ///< PID bypassed, remote setpoint feed throgh 
            updateActuator_f( /*pidDataY.W_remf*/ \
                                pidDataY.W_remf, pidDataY.W_remf);
            break;
        }
        if (g.setpointSrc == REMOTE_INTERNAL_MIXED) {     ///< Mixed mode, summing internal + external setpoint 
            pidErr_x = ((float)(setpoint_tgl + pidDataX.W_remf)) - POS_X_FLOAT;
            pidErr_y = ((float)(setpoint_tgl + pidDataY.W_remf)) - POS_Y_FLOAT;
        }
        if (  (g.setpointSrc != INTERNAL_SETPOINT) && (g.setpointSrc != ANALOG_SETPOINT)  && \
              (g.setpointSrc != REMOTE_OPENLOOP)   && (g.setpointSrc == REMOTE_OPENLOOP) ) {
             /* Error state, halt */
             TM_DISCO_LedOn(LED_GREEN);
             TM_DISCO_LedOn(LED_RED);
        }
        
        /**< Calculate PID here */
//        ////dbg(DBG_PID_TIMING_PE6, hi);
//        updateActuator_f(   arm_pid_f32(&PIDY, pidErr_y), \
//                            arm_pid_f32(&PIDY, pidErr_y));
//        ////dbg(DBG_PID_TIMING_PE6, lo);

        
        break;
    }    
   
   

    /* Set LEDs according to which angular position is greater */
//    if ( POS_Y_FLOAT > SETPOINT_Y_FLOAT) {
//        TM_DISCO_LedOn(LED_GREEN);
//        TM_DISCO_LedOff(LED_RED);
//    } else if ( POS_Y_FLOAT <  SETPOINT_Y_FLOAT) {
//        TM_DISCO_LedOn(LED_RED);
//        TM_DISCO_LedOff(LED_GREEN);
//    } else {
//        TM_DISCO_LedOff(LED_ALL);
//    }
   
}

// ==============================================================
//     IRQ callback:    ADC end-of-conversion 
// ==============================================================
void ADC_IRQHandler(void) {
    if (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == SET) {
        ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
        /* End of conversion interrupt occured */
        ////dbg(DBG0, lo);
        uint16_t W_now = ADC_MultiConvBuff[1];   // setpoint (Isens_y)
        return;
    }
    
   
    if (ADC_GetFlagStatus(ADC1, ADC_FLAG_AWD) == SET) {
        ADC_ClearITPendingBit(ADC1, ADC_IT_AWD);
        if ((ass.state == ASS_STATIONARY_INTEGRATOR) ||
            (ass.state == ASS_DISCHARGING_INTEGRATOR)) {
            ass.state = ASS_CHARGING_INTEGRATOR;
            printf("ASS:charging_start...\n");
        }
    /* Analog watchdog interrupt occured */
        if ((ass.integrator <= ass.lowerVal) || (ass.integrator >= ass.upperVal)) {   /**< check for integrator limit */
            if (! ass.tripped) {
                ass.tripped = 1;                    /**< Set "tripped" state if it is so */
                DAC_SecureSetDualChanSigned = \
                      &DAC_SetDualChanSigned_Tripped;  /**< set function pointer to the "ass tripped" handler*/
            }
            return;
        }
        else {                                  /**< else increment integrator */
            ass.integrator+= ass.upperVal * (double)TS_US*1e-6/(ass.tripTime);
        }
        return;
    }
    printf("bad ADC IntReq source\n");
}
	
// ==============================================================
//     IRQ callback:    Timer 2 overrun 
// ==============================================================
void TIM2_IRQHandler(void) {
	////dbgT(DBG2);
//	GPIO_ToggleBits(DBG_IO, DBG4);
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

        ////dbg(DBG4, lo);
        DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);
        
//        if (ass.stateCtr >= (uint8_t)(ASS_100US / TS)) {
//            ass.stateCtr = 0;
//            if (ass.integrator > 0) {
//                ass.state = ASS_DISCHARGING_INTEGRATOR;
//                ass.tripped = 0;
//                ass.integrator -= ass.upperVal * (double)TS*1e-6/(ass.tripTime);
//            }
//        }
	}
    else
        while (1);;
}





/* ---------- */
/* Used Timer */
/* ---------- */
// (4)  TM_Timer4_config(ENABLE, ENABLE, 450 / g.freq);         waveformGeneration
// (3)  TM_PWM_InitTimer(TIM3, &TIM_Data, 1000);
// (2)  TM_Timer2_config(DISABLE, ENABLE, SAMPLE_INTERVAL);     Base sample rate
//


/**
  * @brief  This function decrement timing variable
  *	@with __weak parameter to prevent errors
  * @param  None
  * @retval None
  */
__weak void TimingDelay_Decrement(void) {

}

#pragma O0
// ==============================================================
//     IRQ callback:    SysTick Interrupt
// ==============================================================
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//        IMPORT from MATLAB code generated project pid_lib_v3
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void SysTick_Handler(void)
{
    uint16_t ypid_uint=0;
    int16_t ypid_int=0;

    
    TimingDelay_Decrement();
    
    ////dbgT(DBG_PID_TIMING_PE6);
    MDB_GPIO_Toggle(DBG_TIMING_PE2);

    /* Manage nb of loop before interrupt has to be processed */
    remainAutoReloadTimerLoopVal_S--;
    if (remainAutoReloadTimerLoopVal_S) {
        MDB_GPIO_Toggle(DBG_TIMING_PE4);
        return;
    }

    remainAutoReloadTimerLoopVal_S = autoReloadTimerLoopVal_S;

    /* Check base rate for overrun */
    if (OverrunFlags[0]) {
        rtmSetErrorStatus(pid4_M, "Overrun");
        TM_DISCO_LedOn(LED_RED);
        return;
    }

    OverrunFlags[0] = TRUE;

    /* Step the model for base rate */
MDB_GPIO_Toggle(DBG_TIMING_PE6); 
    pid4_step();
MDB_GPIO_Toggle(DBG_TIMING_PE6); 
   
    /* Get model outputs here */
    ypid_int = decodeDbl_toInt( pid4_Y.yc);
    ypid_uint = ypid_int + AN_OFFSET;
    DAC_SetDualChanSigned( ypid_uint, ypid_uint );
    //updateActuator_f( (float)pid4_Y.yc, (float)pid4_Y.yc);
        
    /* Indicate task for base rate complete */
    OverrunFlags[0] = FALSE;
}
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//    END of    IMPORT from MATLAB code generated project pid_lib_v3
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#pragma O1

double lastVal=0;

int main(void) {
	g.ampl = 0;

   /**< Define private main variables */
    TM_PWM_TIM_t TIM_Data;    ///< Timer data for PWM 
    volatile uint8_t c = 0;
    volatile uint16_t rxval = 0;
    volatile uint16_t test = 0;




   /**< Initialize System, Delay library and GPIOs */
    SystemInit();       /**< Initialize system. */
    TM_DELAY_Init();    /**< Initialize Delay library. */

    /* ==============================================================
              Enable SYSCLK routing to MCO2 (PC9)
    ============================================================== */
//    gpio_init_mco1();
//    RCC_MCO1Config(RCC_MCO1Source_HSE,RCC_MCO1Div_1);
//    gpio_init();
//    
    
    MDB_GPIO_Init();    ///< Init interrupter GPIOs


   /**< Initialize autoShutdown system struct. */
    ass.lowerVal = ASS_TRIPPING_LOWER_DEFAULT;
    ass.upperVal = ASS_TRIPPING_UPPER_DEFAULT;
    ass.lowerVal = -1.25;
    ass.upperVal = 1.25;
    
    ass.integrator = 0;
    ass.safeVal = ASS_SAFEVALUE_DEFAULT ;
    ass.tripped = 0;
    ass.ack = 0;
    ass.tripTime = ASS_TRIPPING_TIME_DEFAULT; // 750ms initial
    ass.state = ASS_STATIONARY_INTEGRATOR;
    ass.stateCtr = 0;
    
   /**< Initialize global structure */
    g.waveForm = NN; g.duty = 999; g.freq = 999; g.lookAt = 999;
    g.dacHw[TM_DAC1].upperLim = (int16_t)( 2113 - ADC_RNG/2 );
    g.dacHw[TM_DAC1].lowerLim = (int16_t)( 1917 - ADC_RNG/2 );
    g.dacHw[TM_DAC2].upperLim = (int16_t)( 2113 - ADC_RNG/2 );
    g.dacHw[TM_DAC2].lowerLim = (int16_t)( 1917 - ADC_RNG/2 );
    g.setpointSrc = REMOTE_INTERNAL_MIXED;
    g.refresh = 20;    
    g.beamEnabled = GPIO_ON;
 
   /**< Initialize PID system, float32_t format */
//    arm_pid_init_f32(&PIDX, 1);       @@@NOARMPID
//    arm_pid_init_f32(&PIDY, 1);       @@@NOARMPID

 
   /**< Initialize discovery button and leds */
    TM_DISCO_ButtonInit();

    /* Initialize Leds */
    TM_DISCO_LedInit();

   /**< Initialize USART1-> TX: PA9, RX: PA10. */
    TM_USART_Init(USART1, TM_USART_PinsPack_1, 115200);			

   /**< Initialize TIM3, 1kHz frequency */
    TM_PWM_InitTimer(TIM3, &TIM_Data, 1000);

   /**< Initialize TIM2, Channel 1, PinsPack 2 = PA5 */
    TM_PWM_InitChannel(&TIM_Data, TM_PWM_Channel_3, TM_PWM_PinsPack_2);  // ???? PB0

    /* Set default duty cycle */
    TM_PWM_SetChannelPercent(&TIM_Data, TM_PWM_Channel_3, duty);


   /**< Initialize TIM5, heartbeat timer */
    //TM_Timer5_config(ENABLE, ENABLE, 48000);        // 48MHz*1ms
       
   /**< Initialize DAC outputs DAC1-> PA4  DAC2-> PA5 */
    TM_DAC_Init(TM_DAC1);       
    TM_DAC_Init(TM_DAC2);       

    printf("connect Write pointer to DAC output register\n");
    DAC_SecureSetDualChanSigned = &DAC_SetDualChanSigned;    
    
   /**< Initialize TIM2 as part of sampling hardware */
    TM_Timer2_config(DISABLE, ENABLE, SAMPLE_INTERVAL);
    ADC_ContScanMode_w_DMA_timeTrigd( ADC_MultiConvBuff, 4);

   /**< Set PID parameters */
    PIDX.Kp = PID_PARAM_KP;		/* Proporcional */
    PIDX.Ki = PID_PARAM_KI;		/* Integral */
    PIDX.Kd = PID_PARAM_KD;		/* Derivative */
    PIDY.Kp = PID_PARAM_KP;		/* Proporcional */
    PIDY.Ki = PID_PARAM_KI;		/* Integral */
    PIDY.Kd = PID_PARAM_KD;		/* Derivative */

   /**< Set threshold for analog watchdog */
    ADC_AnalogWatchdogThresholdsConfig(ADC1,    
                                decode_toUint(ass.upperVal + AREF_BY2), 
                                decode_toUint(ass.lowerVal + AREF_BY2));

   /**< Transmitt bootup message to UART1 */
    char *tok;
    char *delimit = "\\";
    tok = strtok(__FILE__, delimit);
    tok = strtok (NULL, delimit);

    printf("\n\n--- %s ---\n", tok);
    printf("--- Compiled: %s  %s ---\n\n", __DATE__, __TIME__);
    printf("__TARGET_ARCH_ARM: %s\n", TARGET_ARCH_ARM[__TARGET_ARCH_ARM]);
    printf("__BASE_FILE__: %s\n", __BASE_FILE__);


/* ================================================================================ */       
/**< ===============     ENABLE_ON_POWER-ON-SEQUENCE     ======================================== */       
/* ================================================================================ */       

	TIM_Cmd(TIM3, ENABLE);          ///< Enable PWM timer 3
    TIM_Cmd(TIM2, ENABLE);          ///< Enable sampling timer 2
    ADC_DMACmd(ADC1, ENABLE);       ///< Enable DMA functionality
    ADC_Cmd(ADC1, ENABLE);          ///< Enable onboard hardware ADC 1
    ADC_SoftwareStartConv(ADC1);    ///< Trigger the regular group scan 

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//        IMPORT from MATLAB code generated project pid_lib_v3
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    /* Data initialization */
    int_T i;
    for (i = 0; i < 1; i++) {
        OverrunFlags[i] = 0;
    }

    /* Clock has to be configured first*/
    // RCC_Configuration();
    
//    pid4_U.ym = (float *) &ADC_fBuff[INDEX_Px];
    /* Model initialization call */
    pid4_initialize();


    /* Systick configuration and enable SysTickHandler interrupt */
    if (SysTick_Config((uint32_t)(SystemCoreClock * TS))) {
        autoReloadTimerLoopVal_S = 1;
        do {
            autoReloadTimerLoopVal_S++;
        } while ((uint32_t)(SystemCoreClock * TS) / autoReloadTimerLoopVal_S >
                 SysTick_LOAD_RELOAD_Msk);

        SysTick_Config((uint32_t)(SystemCoreClock * TS) / autoReloadTimerLoopVal_S);
    }

    remainAutoReloadTimerLoopVal_S = autoReloadTimerLoopVal_S;//Set nb of loop to do

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//    END of    IMPORT from MATLAB code generated project pid_lib_v3
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    TM_DISCO_LedOn(LED_GREEN);
    TM_DISCO_LedOn(LED_RED);
    TM_DISCO_LedOff(LED_GREEN);
    TM_DISCO_LedOff(LED_RED);
    
  /* Infinite loop */
  /* Real time from systickHandler */
    while (1) {
        while (! TM_DISCO_ButtonOnPressed()) {};
        TM_DISCO_LedOn(LED_GREEN);

        while (! TM_DISCO_ButtonOnPressed()) {
//            printf("x: % 1.6f  y: % 1.6f\n", ADC_fBuff[INDEX_Px], ADC_fBuff[INDEX_Py]);
            printf("W: % 3.2f\tPy: % 3.2f\tE: % 3.2f\tDuty: % 3.2f%%\n", \
                                SETPOINT_Y_FLOAT, POS_Y_FLOAT, pidErr_y, toPlant_y); 
            Delayms(100);
        }
         pid4_DW.pulseGenSw = 0x01;
//         TM_DISCO_LedOn(LED_RED);
         printf("pid4.pulseGenEN_CurrentSetting = 0x01\n");

        while (! TM_DISCO_ButtonOnPressed()) {
        }
    }   
    /**< Main loop */

	while (1) {
/* ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: */    
/*                      fast Console                               */
/* ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: */ 
        while (! TM_DISCO_ButtonOnPressed()) {
            //Console(&pidDataY, &fpidData, _INT_);    
            fastConsoleCase(&PIDY);     
            
            for (uint8_t k=0; k < 20; k++) {
                if (TM_USART_BufferFull(USART1)) {
                    printf( MSG_GUI_UART1_RECV_BUFF_FULL );
                    fastConsoleCase(&PIDY); 
                    TM_USART_ClearBuffer(USART1);
                };
                Delayms((uint32_t)g.refresh/20);
            }
//            if ((ass.state == ASS_CHARGING_INTEGRATOR) && (! ass.tripped)) {
            if (ass.integrator != lastVal) {
                printf("%.8f\n", ass.integrator);
            }
            if ((ass.tripped) && (! ass.ack)) {
                printf("Tripped!");
                ass.ack = 1;
                lastVal = ass.integrator;
                TM_Timer4_config(ENABLE, ENABLE, 45000 / 3);
            }
            
        }



/* ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: */    
/*              fast Console / REMOTE_OPENLOOP                     */
/* ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: */ 
        while (! TM_DISCO_ButtonOnPressed()) {
            fastConsoleCase(&PIDY);            
            g.setpointSrc = REMOTE_OPENLOOP;

            for (uint8_t k=0; k < 20; k++) {
                if (TM_USART_BufferFull(USART1)) {
                    printf( MSG_GUI_UART1_RECV_BUFF_FULL );
                    fastConsoleCase(&PIDY); 
                    TM_USART_ClearBuffer(USART1);
                };
                Delayms((uint32_t)g.refresh/20);
            }     

            printf("W: % 3.2f\tPy: % 3.2f\tE: % 3.2f\tDuty: % 3.2f%%\n", \
                SETPOINT_Y_FLOAT, POS_Y_FLOAT, pidErr_y, toPlant_y);                        
        }
/* ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: */    
/*              fast Console / W:  P:  Duty:                       */
/* ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: */   
        while (! TM_DISCO_ButtonOnPressed()) {
            //Console(&pidDataY, &fpidData, _INT_);    
            fastConsoleCase(&PIDY);            

            for (uint8_t k=0; k < 20; k++) {
                if (TM_USART_BufferFull(USART1)) {
                    printf( MSG_GUI_UART1_RECV_BUFF_FULL );
                    fastConsoleCase(&PIDY); 
                    TM_USART_ClearBuffer(USART1);
                };
                Delayms((uint32_t)g.refresh/20);
            }     

            printf("W: % 3.2f\tPy: % 3.2f\tE: % 3.2f\tDuty: % 3.2f%%\n", \
                SETPOINT_Y_FLOAT, POS_Y_FLOAT, pidErr_y, toPlant_y);                        
        }
        

/* ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: */    
/*              fast Console / W:  P:  Duty:                       */
/* ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: */     
        while (! TM_DISCO_ButtonOnPressed()) {
            //Console(&pidDataY, &fpidData, _INT_);    
            fastConsoleCase(&PIDY);            

            for (uint8_t k=0; k < 20; k++) {
                if (TM_USART_BufferFull(USART1)) {
                    printf( MSG_GUI_UART1_RECV_BUFF_FULL );
                    fastConsoleCase(&PIDY); 
                    TM_USART_ClearBuffer(USART1);
                };
                Delayms((uint32_t)g.refresh/20);
            }     

            printf("W: % 3.2f\tPy: % 3.2f\tE: % 3.2f\tDuty: % 3.2f%%\n", \
                SETPOINT_Y_FLOAT, POS_Y_FLOAT, pidErr_y, toPlant_y);                        
        }
    }
}
/*	
	while (1) {
        / * Read temperature 1, reference temperature * /
//        TM_DS18B20_Read(&OneWire, device[0], &SETPOINT_Y_FLOAT);
        

        / * Format string * /
        printf("W: % 3.2f\tPy: % 3.2f\tE: % 3.2f\tDuty: % 3.2f%%\n", \
            SETPOINT_Y_FLOAT, POS_Y_FLOAT, pidErr_y, toPlant_y);
        Delayms(g.refresh);
    }
    */
 /* End of main function */

/** 
 * @brief   Set beam control source and new state
 */ 
int beamCtrl(beamCtrlSource_t src, MDB_GPIO_STATE_t newState ) {
    if ((src == BEAM_CTRL_SOURCE_GLOBAL) && (newState != DNI)) {
        printf("Error, Beam source can't be GLOBAL if new state is not DNI");
        return -1;
    }

    if ((src == BEAM_CTRL_SOURCE_GLOBAL) && (newState == DNI)) {
        MDB_GPIO_Switch(BEAM_INTERRUPT, g.beamEnabled);
        return 0;
    }

    if (src == BEAM_CTRL_SOURCE_MANUAL) {
        MDB_GPIO_Switch(BEAM_INTERRUPT, newState);
        return 0;
    }
    
    return -1;
}













/*
 *       @,!,# are newline indicators or line separators
 *
 *       @kp   :=   58
 *       #sin  :=   F F F F  d  d  A  A   A   A
 *       @w:   :=   0.0005452
 *     |______|____|________________|  
 *     |  NC  | FS |      NV        |  
 *     
 *        3      2          16          
 *
 *  => UART_BUFF_SIZE = 23 (newline + 21 + \0)
 *
 */


/*! \brief Fast console command parser
 *
 * Interface to a flat implemented signal generator based on lookup tables
 * Implemented waveforms:
 *
 * Sin/Cos:			Measurements 
 *
 */

#define IDX_CMD     1       // index 0 is @ or ! or #
#define IDX_VAL     5   // index 5 

#define UART_BUFF_SIZE      (1+NC+FS+NV+1)
#define VALUE_BUFF_SIZE     (NV+1)
#define CMD_BUFF_SIZE       (NC+1)

//void fastConsoleCase (volatile struct PID_DATA *pid) {
#pragma O0
void fastConsoleCase (arm_pid_instance_f32 *pid) {
   /**< private member declarations */
	char sUart[UART_BUFF_SIZE],		///< string buffer to hold received input string
		 sCmd[CMD_BUFF_SIZE],		///< string buffer to hold selected command as substring
         sVal[VALUE_BUFF_SIZE];     ///< string buffer to hold given value as substring
    /**< 
     * sVal holds the string representation of a received float value.
     * For useing atof() , a buffer of 4xsizeof(int8_t)+1 [--> float32_t] or 8xsizeof(int8_t) [--> double]
     * is necessary
     */

	char *pCmd = sCmd;		        ///< pointer for buffer arrays
	char *pVal = sVal;		        ///< pointer for buffer arrays
    
    enum cmd_items cmd;
    enum wav_items wav;
    struct items *choice = NULL;
    struct itemsw *choicew = NULL;

	int i;

    volatile uint8_t nChars = TM_USART_Gets( USART1, &sUart[0], UART_BUFF_SIZE);

   /**< Check for serial data frames >= UART_BUFF_SIZE */
	if (nChars < UART_BUFF_SIZE) {
        memset(&sUart, 0, UART_BUFF_SIZE*sizeof(char));    ///< malloc the receive buffer
        return;
    }
  
   /**< malloc() if a frame received where sizeoff() >= UART_BUFF_SIZE */
    memset(&sCmd,0, CMD_BUFF_SIZE*sizeof(char));     ///< malloc command buffer
    memset(&sVal,0, VALUE_BUFF_SIZE*sizeof(char));      ///< malloc value buffer

   /**< Check leading character via receive buffer 
     *      @   frame indicator for pid related config/param.
     *      !   frame indicator for a misc command 
     *      #   frame indicator for a signal generator command 
     */
	if ( sUart[0] == '@') {
    /* pid related config/param */
		strncpy( pCmd, &sUart[IDX_CMD], NC);		///< command substring 
		strncpy( pVal, &sUart[IDX_VAL], NV);		///< Value substring

        /* Check for end of char coded float representation 
         * numeric char:            0...9
         * decimal delimiter:       . or ,  !!!Take caution for forbidden thousands separator!!! 
         * exponental indicator:    e or E
         * signdness:               -
         */
		for (uint8_t k=0; k <= NV; k++) {                   ///< Max NC numbers ... 
            if ( ((*pVal >= '0') && (*pVal <= '9')) || \
                  (*pVal == '.') || (*pVal == ',')  || \
                  (*pVal == 'e') || (*pVal == 'E') || \
                  (*pVal == '-') ) {    
                pVal++;
            }
            else {
                break;
            }
		}
		
		*pVal='\0';	                ///< place EOF marker
        pVal = &sVal[0];

        while ( *pCmd != '=' ) {    ///< check for end of command  indicator '='
			pCmd++;
		}		
		*(pCmd-1)='\0';		        ///< place EOS marker									
        pCmd = &sCmd[0];

        /* parse sCmd buffer and enumerate the command/error */
		for(i = 0, choice = NULL; i < sizeof items_list/sizeof (struct items); i++) {
			if (strcasecmp(sCmd, items_list[i].name) == 0)
			{
				choice = items_list + i;
				break;
			}
		}    

		cmd = choice ? choice->id : UNKNOWN;

        char sValtemp1[] = {"-0.7e-09"};
        volatile float fl1 = atof(&sValtemp1[0]);

#ifdef  FLOAT_PARAMETERS
//            volatile float valueDecoded = (float)tmp/SLIDER_VAL_DEVIDER;
            volatile float valueDecoded = atof( pVal );

#elif   DOUBLE_PARAMETERS
            double valueDecoded = (double)tmp/SLIDER_VAL_DEVIDER;
#else 
        error "datatype?"
#endif

		switch(cmd) {
			case KP:	{   
                            pid->Kp = valueDecoded;
//                            arm_pid_init_f32(&PIDY, 0);   @@@NOARMPID
                            printf("Kp % f set!\n", valueDecoded);
                        }	
                        break;

			case KI:	{
                            pid->Ki = valueDecoded;
//                            arm_pid_init_f32(&PIDY, 0);   @@@NOARMPID
                            printf("Ki % f set!\n", valueDecoded);
//                            printf("Ki %s given\n", &sval[0]);
                        }	
                        break;

			case KD:	{
                            pid->Kd = valueDecoded;
//                            arm_pid_init_f32(&PIDY, 0);   @@@NOARMPID
                            printf("Kd % f set!\n", valueDecoded);
   //                         printf("Kd %s given\n", &sval);
                        }	
                        break;

			case W:		{
                            pidDataX.W_remf = valueDecoded;
                            pidDataY.W_remf = valueDecoded;
                            pidDataY.remoteCmdActive = 1;
                            printf("W = % g\n", pidDataY.W_remf);
                        }
                        break;

			default:
						printf("unknown choice\n");
			break;
		}
        
        
	}
	else { 	
        /** A trailing ! char in a new Rx frame indicates that a misc command
         * follows...
         */
        if (sUart[0] == '#') {   ///< True if signal generator command received

/** 
 * Trailing # indicates a command frame for signal generation
 *
 * Frame Layout: 
 * 	c		waveform
 * 	v		freq
 * 	d 	dutycycle
 *
 * # c c c : = v v v v  d  d 
 * 0 1 2 3 4 5 6 7 8 9 10  11 12 
 *
 *
 *
 * ------ Aus Qt software v8.3 
 * Frame Layout:
 * cwaveform
 * vfreq
 * d dutycycle
 *
 * # c c c : = F F F F  d  d  A  A   A   A
 * 0 1 2 3 4 5 6 7 8 9 10  11 12 13  14  15     
 *
 * Amplitude als char coded float
 *
 * # c c c : = F F F F  d  d  A ..16x.. A 
 * 0 1 2 3 4 5 6 7 8 9 10  11 12 13  14  .... 27     
 *
 */
     
    //char 	sWav[NC+1],		///< string buffer to hold selected waveform as substring
    //        sFreq[NV+1],	///< string buffer to hold given frequency as substring
    //        sDuty[ND+1];	///< string buffer to hold given duty cycle as substring

            uint8_t NC = 3;		// index 0...3
    //		uint8_t FS = 2;		// index 4...5	(fieldsep)
            uint8_t NV = 4;		// index 6...9
            uint8_t ND = 2;		// index 10...11
            uint8_t NA = 16;		// index 12...15

            char 	sWav[NC+1],		///< string buffer to hold selected waveform as substring
                    sFreq[NV+1],	///< string buffer to hold given frequency as substring
                    sDuty[ND+1],	///< string buffer to hold given duty cycle as substring
                    sAmp[16];	    ///< string buffer to hold given amplitude as substring

//            char *pWav = sWav;
            char *pDuty = sDuty;
            char *pFreq = sFreq;
            char *pAmp = sAmp;
            
            strncpy(sWav,  &sUart[1], NC);	///< waveform substring 
            strncpy(sFreq, &sUart[6],  4);		///< frequency substring
            strncpy(sDuty, &sUart[10], 2);		///< duty cycle substring
            strncpy(sAmp, &sUart[12], 16);		///< peak amplitude substring


        /* Check for end of char coded float representation 
         * numeric char:            0...9
         * decimal delimiter:       . or ,  !!!Take caution for forbidden thousands separator!!! 
         * exponental indicator:    e or E
         * signdness:               -
         */
        for (uint8_t k=0; k<NC; k++) {  ///< check for end of command  indicator '='
            if( *pCmd == RX_FRAME_PADDING_CHAR )        
                break;
			pCmd++;
		}		
		*(pCmd-1)='\0';		 ///< place EOS marker									
        pCmd = &sCmd[0];

        for (uint8_t k=0; k<NV; k++) {  ///< check for end of command  indicator '='
            if( *pFreq == RX_FRAME_PADDING_CHAR )        
                break;
			pFreq++;
		}		
		*pFreq='\0';		 ///< place EOS marker									
        pFreq = &sFreq[0];

       for (uint8_t k=0; k<ND; k++) {  ///< check for end of command  indicator '='
            if( *pDuty == RX_FRAME_PADDING_CHAR )        
                break;
			pDuty++;
		}		
		*pDuty='\0';		 ///< place EOS marker									
        pDuty = &sDuty[0];

		for (uint8_t k=0; k <= NA; k++) {                   ///< Max NC numbers ... 
            if ( ((*pAmp >= '0') && (*pAmp <= '9')) || \
                  (*pAmp == '.') || (*pAmp == ',')  || \
                  (*pAmp == 'e') || (*pAmp == 'E') || \
                  (*pAmp == '-') ) {    
                pAmp++;
            }
            else {
                break;
            }
		}
		*pAmp='\0';	    ///< place EOF marker
        pAmp = &sWav[0];


    /* parse sCmd buffer and enumerate the command/error */
        for(i = 0, choicew = NULL; i < sizeof itemsw_list/sizeof (struct itemsw); i++)
        {
            if (strcasecmp(sWav, itemsw_list[i].name) == 0)
            {
                choicew = itemsw_list + i;
                break;
            }
        }   

		wav = choicew ? choicew->idw : NN;
//    /**	
//     * Check for invalied chars in command string
//     *	Only [a-zA-Z] is threated as part of command string
//     *
//     */
//            while ( (*pWav >= 0x41) && (*pWav <= 0x5A) || 
//                            (*pWav >= 0x61) && (*pWav <= 0x7A) ) {	
//                pWav++;
//            }		

//    /**
//     * Same item structure used for PID-param and 
//     * waveform generation commands! Don't forget 
//     * to offset the for loop while while read in 
//     * a waveform command.
//     *
//     */				
//            for(i = 0, choicew = NULL; i < sizeof itemsw_list/sizeof (struct itemsw); i++)
//            {
//                if (strcasecmp(sWav, itemsw_list[i].name) == 0)
//                {
//                    choicew = itemsw_list + i;
//                    break;
//                }
//            }    

//            wav = choicew ? choicew->idw : NN;
        volatile float valueDecoded = atof( pVal );

            g.waveForm 	= wav;
            g.freq		= atoi(sFreq);
            g.duty 		= atoi(sDuty);
            g.ampl_f 	= (float)atof(sAmp);
            g.ampl      = (int) g.ampl_f * 1000;
            
            //g.ampl_f = (float)g.ampl / (float)AMPL_FLOAT_DIVISOR;
            
            if (g.ampl_f > (float)AREF_IN_VOLT/2)
                g.ampl_f = (float)AREF_IN_VOLT/2;
            if (g.ampl_f < -(float)AREF_IN_VOLT/2)
                g.ampl_f = -(float)AREF_IN_VOLT/2;
//            g.ampl_f = (float)g.ampl / (float)AMPL_FLOAT_DIVISOR;
//            
//            if (g.ampl_f > (float)AREF_IN_VOLT/2)
//                g.ampl_f = (float)AREF_IN_VOLT/2;
//            if (g.ampl_f < -(float)AREF_IN_VOLT/2)
//                g.ampl_f = -(float)AREF_IN_VOLT/2;
            
    /**
     *	Calculate the interval for lookup table index increment 
     *	based on @SAMPLE_INTERVAL / TS, sizeof table and given 
     *  waveform frequency.
     *	
     *	g.freq  	[Hz] 	
     *	TS_US			[us]	* 10^(-6)	
     *
     *  g.lookAt = g.freq/(TABLESIZE * TS_US * 10^(-6))
     *           = g.freq * 10^4 / TS_US        
     */


    //        g.lookAt = (short) 1/((double)TABLESIZE * TS_US * 10^(-6));
    //		g.lookAt = (uint32_t) g.freq * pow(10,6) / (TABLESIZE * TS_US);
    //        printf(" g.lookAt: %i", g.lookAt);

    //        g.lookAt = g.lookAt / 100;
    //        printf(" g.lookAt/100: %i", g.lookAt);
            g.tctr = g.lookAt;

    /**
     *  Precalculate a lookup table based on normed array
     *
     */
            for (uint16_t k=0; k<TABLESIZE; k++) {
              calcTab[k] = (uint16_t) g.ampl * CosineN[k];  
            };
            
            switch(wav) {
                    case COS:			{
                                            printf("Cosine \n");
                                            pSeq = &Cosine[0];
                                            g.pBase = pSeq;
                                            g.gen = EN;
                    }	
                    break;

                    case TRIANG:		{
                                            printf("Triangle \n");
    //										pSeq = &Triangle[0];
                                            /* 450/g.freq --> 100 Timer Ovf / periode */
                                            triStruct.bottom = ((float)AREF_BY2 - g.ampl_f / 2);
                                            triStruct.top = ((float)AREF_BY2 + g.ampl_f / 2);
                                            triStruct.derivate = g.ampl_f / (float)100;
//                                            g.pBase = pSeq;
                                            g.waveForm = TRIANG;
                                            triStruct.triCtr = 0;
                                            TM_Timer4_config(ENABLE, ENABLE, 450 / g.freq);
                                            g.gen = EN;
                    }	
                    break;

                    case SQUAREWAV:	{
                                            printf("Squarewave\n");
                                            TM_Timer4_config(ENABLE, ENABLE, 45000 / g.freq);
                                            g.setpointSrc = REMOTE_INTERNAL_MIXED;
                                            g.waveForm = SQUAREWAV;
                                            g.gen = EN;
                                            pSeq_f = &squarewaveBuff[0];
                    }	
                    break;

                    case QUADRATIC:	{
                                            printf("Quadratic\n");
                                            pSeq = &Quadratic[0];
                                            g.gen = EN;
                    }	
                    break;
                    
                    case I_QUADRATIC:	{
                                            printf("Inv Quadratic\n");
                                            pSeq = &I_Quadratic[0];
                                            g.gen = EN;
                    }	
                    break;

                    case SAWTOOTH:		{
                                            printf("Sawtooth\n");
                                            pSeq = &Sawtooth[0];
                                            g.gen = EN;
                    }	
                    break;

                    case CMD_REMOTE_SETPOINT: {
                                            printf("CMD_REMOTE_SETPOINT\n");
                                            g.setpointSrc = REMOTE_SETPOINT;
                    }	
                    break;

                    case CMD_OPENLOOP_REMOTE: {
                                            printf("CMD_OPENLOOP_REMOTE\n");
                                            g.setpointSrc = REMOTE_OPENLOOP;
                    }	
                    break;

                    case CMD_ANALOG_SETPOINT: {
                                            printf("CMD_ANALOG_SETPOINT\n");
                                            g.setpointSrc = ANALOG_SETPOINT;
                    }	
                    break;

                    case CMD_INTERNAL_SETPOINT: {
                                            printf("CMD_INTERNAL_SETPOINT\n");
                                            g.setpointSrc = INTERNAL_SETPOINT;
                    }	
                    break;
                    
                    
                    default: {
                                            printf("unknown choice\n");
                                            g.waveForm = NN;
                                            g.duty 	= 999;
                                            g.freq	= 999;
                                            g.lookAt = 999;
                                            g.pBase 	= pSeq;
                                            g.tctr	= 999;
                                            g.gen		= DIS;
                    }
                    
                    /* Disable waveform generation if wgm-off command received */
            }
            
    //		g.pBase = (uint32_t)&pSeq;
            
            
            
        }
        else {                   ///< True if a misc command received
/** 
 * Trailing ! indicates that a misc command frame was received
 *
 * Frame Layout: 
 * 	c	command
 *
 * ! c c c c c c c c c c  c  c 
 * 0 1 2 3 4 5 6 7 8 9 10 11 12 
 *        
 */       
#define MISC_CMD_LENGTH     35  // + trailing !
            enum misc_items misc;
            struct itemsm *choicem = NULL;

            ///< string buffer to hold given command substring
            char sMisc[MISC_CMD_LENGTH + 1]; 
            char *pMisc = sMisc;
            
            strncpy(sMisc, 	&sUart[1],  MISC_CMD_LENGTH);	///< command substring
            
            /**< Check for valied characters and substitute first unvalied char
             * by EOS char
             */

            while ( (*pMisc >= 'A') && (*pMisc <= 'Z') || \
                    (*pMisc >= 'a') && (*pMisc <= 'z') || \
                    (*pMisc >= '0') && (*pMisc <= '9') || \
                    (*pMisc == '-') || (*pMisc == '_') || \
                    (*pMisc == '.') || (*pMisc == ',') || \
                    (*pMisc == 'e') || (*pMisc == 'E') ){	    

                pMisc++;
            }
            
            *pMisc='\0';				///< End of string


            for(i = 0, choicem = NULL; i < sizeof itemsm_list/sizeof (struct itemsm); i++)
            {
                if (strcasecmp(sMisc, itemsm_list[i].name) == 0)
                {
                    choicem = itemsm_list + i;
                    break;
                }
            }    

            misc = choicem ? choicem->idm : misc_NN;
            
            g.miscReq 	= misc;
            
            switch (misc) {
                case misc_assOnOff_cmd:
                    break;                
                case misc_assUpperLim_cmd:     
                    break;                
                case misc_assLowerLim_cmd:     
                    break;                
                case misc_assTrippTime_cmd:     
                    break;                
                case misc_assSaveVal_cmd:     
                    break;                

                case misc_pid_init:     
//                    pid_Init(KP_INIT, KI_INIT, KD_INIT, TF_INIT, TS_US, &pidDataY, NONBUFFERED);
                    resetPID();
                    break;
                case misc_beamOff_cmd:   
                        MDB_GPIO_BeamPin(BEAM_INTERRUPT, GPIO_DRIVER_OFF);
                    break;
                case misc_beamOn_cmd:   
                        MDB_GPIO_BeamPin(BEAM_INTERRUPT, GPIO_DRIVER_ON);
                    break;
                    
                default:
                    printf("function unknown\n");
            }
        }
        memset(&sUart,0, UART_BUFF_SIZE*sizeof(char));
    }

}
#pragma O2




void gpio_init(void) {
	//GPIO Init structure
//	GPIO_InitTypeDef GPIO_InitDef;
//	//Enable GPIOG peripheral clock for leds
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

//	GPIO_InitDef.GPIO_Pin = GPIO_Pin_4;		// this can be or'd
//	GPIO_InitDef.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitDef.GPIO_Mode = GPIO_Mode_OUT;
//	GPIO_InitDef.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	GPIO_InitDef.GPIO_Speed = GPIO_Speed_100MHz;
//	//Initialize pins
//	GPIO_Init(GPIOC, &GPIO_InitDef);
	
	// Init debug/timing IOs
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

//	GPIO_InitDef.GPIO_Pin = ((uint16_t)0x00ff);		// this can be or'd
//	GPIO_InitDef.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitDef.GPIO_Mode = GPIO_Mode_OUT;
//	GPIO_InitDef.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	GPIO_InitDef.GPIO_Speed = GPIO_Speed_100MHz;
//	//Initialize pins
//	GPIO_Init(GPIOE, &GPIO_InitDef);
	
}



/**< gpio_PA_init_output(GPIO_Pin_4 | GPIO_Pin_5);
 */
void resetPID (void) {
//	pid_Init(KP_INIT, KI_INIT, KD_INIT, TF_INIT, TS_US, &pidDataY, UNBUFFERED); @@@NOARMPID
//	calc_coeff(&pidDataY, BACK_SQUARE); @@@NOARMPID
}
void gpio_init_mco1(void) {
	//GPIO Init structure
	GPIO_InitTypeDef GPIO_InitDef;
	//Enable GPIOG peripheral clock for leds
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_InitDef.GPIO_Pin = GPIO_Pin_8;		// this can be or'd
	GPIO_InitDef.GPIO_OType = GPIO_OType_PP;
	GPIO_InitDef.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitDef.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitDef.GPIO_Speed = GPIO_Speed_100MHz;
	//Initialize pins
	GPIO_Init(GPIOA, &GPIO_InitDef);
}


void gpio_init_mco2(void) {
	//GPIO Init structure
	GPIO_InitTypeDef GPIO_InitDef;
	//Enable GPIOG peripheral clock for leds
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	GPIO_InitDef.GPIO_Pin = GPIO_Pin_9;		// this can be or'd
	GPIO_InitDef.GPIO_OType = GPIO_OType_PP;
	GPIO_InitDef.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitDef.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitDef.GPIO_Speed = GPIO_Speed_100MHz;
	//Initialize pins
	GPIO_Init(GPIOC, &GPIO_InitDef);
}



/*    
#ifdef MULTI_SETPOINT
    if (g.setpointSrc == INTERNAL_SETPOINT) {
        W_now = pidDataY.W_int;
    }  else {
    if (g.setpointSrc == ANALOG_SETPOINT) {
        W_now = ADC_fBuff[INDEX_W];
    }  else {
    if (g.setpointSrc == REMOTE_SETPOINT) {
        W_now = pidDataY.W_rem;
    }  else {
    if (g.setpointSrc == REMOTE_OPENLOOP) {
        W_now = pidDataY.W_rem;
    }}}} 
#endif    
*/    













///*
// * File: main.c
// *
// * Code generated for Simulink model :pid_lib_v1.
// *
// * Model version      : 1.23
// * Simulink Coder version    : 8.5 (R2013b) 08-Aug-2013
// * TLC version       : 8.5 (Aug  6 2013)
// * C/C++ source code generated on  : Fri May 15 09:22:38 2015
// *
// * Target selection: stm32F4xx.tlc
// * Embedded hardware selection: STMicroelectronics->STM32F4xx 32-bit Cortex-M4
// * Code generation objectives: Unspecified
// * Validation result: Not run
// *
// *
// *
// * ******************************************************************************
// * * attention
// * *
// * * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
// * * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
// * * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
// * * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
// * * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
// * * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
// * *
// * ******************************************************************************
// */

//#include "adc_dac_dma.h"
//#include "arm_architect.h"
//#include "arm_math.h"
//#include "defines.h"
//#include "main.h"
//#include "mdb_gpio.h"
//#include "pid.h"
//#include "signalGen.h"
//#include <stdio.h>
//#include <stdlib.h>
//#include "stm32f4xx_adc.h"
//#include "stm32f4xx.h"
//#include <string.h>
//#include "tm_stm32f4_adc.h"
//#include "tm_stm32f4_dac.h"
//#include "tm_stm32f4_delay.h"
//#include "tm_stm32f4_disco.h"
//#include "tm_stm32f4_ds18b20.h"
//#include "tm_stm32f4_ili9341.h"
//#include "tm_stm32f4_onewire.h"
//#include "tm_stm32f4_pwm.h"
//#include "tm_stm32f4_stmpe811.h"
//#include "tm_stm32f4_timer.h"
//#include "tm_stm32f4_usart.h"
//#include "tools.h"


//// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
////          IMPORT from MATLAB code generated project pid_lib_v3
//// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//#include "pid_lib_v3.h"                /* Model's header file */
//#include "rtwtypes.h"                  /* MathWorks types */
//#include "stm32f4xx.h"

///* RCC_Configuration defined in RCC_Configuration.c file */
///* Clock configuration function provided as example */
//void RCC_Configuration(void);

///* Real-time model */
//extern RT_MODEL_pid_lib_v3 *const pid_lib_v3_M;

///* Set which subrates need to run this base step (base rate always runs).*/
///* Defined in pid_lib_v3.c file */
//extern void pid_lib_v3_SetEventsForThisBaseStep(boolean_T*);

///* Flags for taskOverrun */
//static boolean_T OverrunFlags[1];

///* Number of auto reload timer rotation computed */
//static uint32_t autoReloadTimerLoopVal_S = 10;

///* Remaining number of auto reload timer rotation to do */
//static uint32_t remainAutoReloadTimerLoopVal_S = 10;

///****************************************************
//   SysTick_Handler function
//   This handler is called every tick and schedules tasks
// *****************************************************/
//void SysTick_Handler(void);

//// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
////    END of    IMPORT from MATLAB code generated project pid_lib_v3
//// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//static struct global g;
////static struct triangWave triStruct;
//static struct autoSaveSystem ass;

//static DAC_WritePointer_t (*DAC_SecureSetDualChanSigned) (int16_t, int16_t);

//// ==============================================================
////   This is needed in order to provide printf functionality
//// ==============================================================
//#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)

//struct __FILE { int handle; /* Add whatever you need here */ };
//FILE __stdout;
//    
//PUTCHAR_PROTOTYPE
//{
//    /* Place your implementation of fputc here */
//    /* e.g. write a character to the USART */
//    USART_SendData(USART1, (uint8_t) ch);

//    /* Loop until the end of transmission */
//    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
//    {}

//    return ch;
//}


//	
//// ==============================================================
////                         Prototypes 
//// ==============================================================
//void gpio_init(void);
//void exportInternalClks (void);
//void gpio_init_mco1(void);
//void gpio_init_mco2(void);
////static void Console (struct PID_DATA *pid, struct FPID_DATA *fpid, structType type);
//void resetPID (void);
////void consoleCase (struct PID_DATA *pid);
//void adc12_init (uint16_t gpio_pin_adc1, 
//								 uint16_t gpio_pin_adc2, 
//								 uint16_t EndOfConvInt, 
//								 ADC_TypeDef* ADCxEOT);
//void fastConsoleCase(arm_pid_instance_f32 *pid);

//int updateActuator_f(float I_set_x, float I_set_y);
//int beamCtrl(beamCtrlSource_t src, MDB_GPIO_STATE_t newState );


//#define RX_FRAME_PADDING_CHAR   '~'

///* The function pointer updateActuator is used as non-privileged access function 
// * to internal DAC output registers. After ass detects a tripping condition ,
// * updateActuator function-pointer targets to updateActuator_fused_callback */
////int (*updateActuator)                (float, float);
////int (*updateActuator_callback)       (float, float);
////int (*updateActuator_fused_callback) (float, float);
//// ==============================================================
////                  Compensator things
//// ==============================================================
//#define ALGO_OLD 				10
//#define ADJUSTING_SETPOINT	    11		// Stellungsalgorithmus
//#define SPEED_SETPOINT			12		// Geschwindigkeitsalgorithmus
//#define ALGORITHM 	ADJUSTING_SETPOINT

///** This is the factor used in the gui to enlarge resolution on int-only
// * QSlider ranges
// */
//#define SLIDER_VAL_DEVIDER  10000UL
////int SLIDER_VAL_DEVIDER  = 10000;


////@@@#pragma O0
//struct PID_DATA     pidDataX;
//struct PID_DATA     pidDataY;
//struct FPID_DATA    fpidData;

////@@@#pragma O2

//volatile float UNIT_MIKRO = 1/1000000;
//volatile int16_t pidOut = 0;

///**< ADC dual mode DMA receive buffer. */
//__IO int16_t ADC_MultiConvBuff[5];     
///* ADC values access buffer
// * Because we use a float based instance of PID, we need to cast the 
// * ADC_MultiConvBuff[0] buffer from integral type to float !!!
// */
//__IO float ADC_fBuff[5];     
///**< Define indices for ADC_MultiConvBuff[ ] acording to setpoint (INDEX_W) ans
// * process output (INDEX_P). */
//#ifndef INDEX_Py
//    #define INDEX_Px     0
//    #define INDEX_Py     1
//    #define INDEX_Ix     2
//    #define INDEX_W      3
//#endif
///* Acess macros for actual ADC values 
// * 
// * ToDo:    Maybe this macros needs to be atomic! 
// */
//#define POS_X_FLOAT         (float) ADC_fBuff[INDEX_Px]	/* actual position */
//#define POS_Y_FLOAT         (float) ADC_fBuff[INDEX_Py]	/* actual position */
//#define SETPOINT_Y_FLOAT	(float) ADC_fBuff[INDEX_W]	/* position we want to have */

//#define SIGNW -1

//#define INTERNAL_SETPOINT 
//#undef INTERNAL_SETPOINT 

//#pragma O0
//    //float temps[EXPECTING_SENSORS];
//    
//float pidErrBuff[2];        /* PID error buffer*/
//#define pidErr_x   pidErrBuff[0]
//#define pidErr_y   pidErrBuff[1]

//float toPlantBuff[2];        /* PID output buffer*/
//#define toPlant_x   toPlantBuff[0]
//#define toPlant_y   toPlantBuff[1]

//    float duty = 0;             /* Duty cycle for PWM */
//    arm_pid_instance_f32 PIDX;   /* ARM PID Instance, float_32 format */
//    arm_pid_instance_f32 PIDY;   /* ARM PID Instance, float_32 format */
//__IO float toPlant;              /* next actuator value in Volt */
//    int16_t toPlant_int;       /* next actuator value integral type */

//    int16_t test1 = UPPER_DAC_LIMIT_SIGNED;
//    int16_t test2 = LOWER_DAC_LIMIT_SIGNED;
//#pragma O2

///* Choose PID parameters */
//#define PID_PARAM_KP		0.5			/* Proporcional */
//#define PID_PARAM_KI		0.0		/* Integral */
//#define PID_PARAM_KD		0.0			/* Derivative */

//// ==============================================================
////                      beam interrupter
//// ==============================================================
// 
//// ==============================================================
////                ass - auto shutdown system
//// ==============================================================
//#define ASS_TRIPPING_LOWER_DEFAULT    -1.2f
//#define ASS_TRIPPING_UPPER_DEFAULT    1.2f
//#define ASS_SAFEVALUE_DEFAULT         0
//#define ASS_TRIPPING_TIME_DEFAULT    0.750f      // s


////#define ASS_100US                   100
//// ==============================================================
//// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//// ==============================================================
////      This indices HAVE TO STAY IN SYNC WITH Qt GUI SOURCES 
//// ==============================================================
//    volatile int NC = 3;		// index 0...2  (short command)
//    volatile int FS = 2;		// index 3...4	(fieldsep)
//    volatile int NV = 10;	    // index 5...14 (value)
//// ==============================================================
//// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//// ==============================================================
//    
//#define ////dbg(IO_PIN, LEVEL) ((LEVEL) == 1 ? \
//	 GPIO_WriteBit((GPIO_TypeDef*)DBG_IO, IO_PIN, Bit_SET) :\
//	 GPIO_WriteBit((GPIO_TypeDef*)DBG_IO, IO_PIN, Bit_RESET))

//#define ////dbgT(IO_PIN) TM_GPIO_TogglePinValue \
//                                ((GPIO_TypeDef*)DBG_IO, IO_PIN)

///* Define static usart strings used as GUI error / status messages  
// */
//#define  MSG_GUI_UART1_RECV_BUFF_FULL   "@GUI:usart1RecvBuffFull\n"  ///< Inform Gui about a full usart1 buffer
// 
//// ==============================================================
////            Signal generator things
//// ==============================================================

//uint16_t sinctr=0;
///*volatile uint16_t tab[1024];

////const int16_t sintab2[]= {0,110,220,329,437,543,647,748,846,941,1032,1119,1201,1279,1351,1418,1479,1534,1583,1626,1662,1692,1714,1730,1739,1741,1735,1723,1704,1678,1645,1606,1560,1508,1449,1385,1316,1241,1161,1076,987,894,798,698,595,490,384,275,165,55,-55,-165,-275,-384,-490,-595,-698,-798,-894,-987,-1076,-1161,-1241,-1316,-1385,-1449,-1508,-1560,-1606,-1645,-1678,-1704,-1723,-1735,-1741,-1739,-1730,-1714,-1692,-1662,-1626,-1583,-1534,-1479,-1418,-1351,-1279,-1201,-1119,-1032,-941,-846,-748,-647,-543,-437,-329,-220,-110,-0};

//    //int16_t sintab[]= { 0,262,519,769,1005,1225,1425,1601,1751,1873,
////									1963,2022,2047,2039,1997,1922,1816,1680,1516,
////									1328,1117,889,645,391,131,-131,-391,-645,-889,
////									-1117,-1328,-1516,-1680,-1816,-1922,-1997,-2039,
////								-2047,-2022,-1963,-1873,-1751,-1601,-1425,-1225,
////									-1005,-769,-519,-262,-0};
//*/
//// ==============================================================
//// ==============================================================
//// ==============================================================
//volatile uint16_t gW = 0;
//volatile uint16_t gY = 0;
//volatile uint16_t multiADC[2];

//volatile int16_t P_now, W_now;

//volatile float  KP_INIT  =	1;
//volatile float  KI_INIT, \
//                KD_INIT, \
//                TF_INIT = 0;



////float W_INT_BUFF[33]= { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0 };

////float *TEMP_WANT2 = W_INT_BUFF;	/* position we want to have */
////volatile uint32_t ct = 20000;

//volatile float setpoint_tgl = (1.25-0.4);


//// ==============================================================
////    IRQ callback:    DMA2_Stream0 transmission-complete
//// ==============================================================
////     PID: This callback initiates compensator calculation 
//// ==============================================================
//void DMA2_Stream0_IRQHandler(void) {
//    /**< Since this is the only irq flag for this handler, clear direct. */
//    DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TC);
//    DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, DISABLE);

///*    
//    if (ct >= 5) {
//        ct--;
//    }
//    else {
//        ct = 100000;
//        if (setpoint_tgl >= (float)1.25) {
//            setpoint_tgl = (float)(1.25-0.05);
//        }
//        else {
//            setpoint_tgl = (float)(1.25+0.05);
//        }
//    }
//*/

///**< Das analoge symmetrische Positionssignal muss vor der AD- Wandlung einen positiven 
// * Offset erhalten wegen des Eingangsspannungsbereichs der intergrierten ADC-Zellen.
// * Analog wird also das sym. Pos. Signal um AREF/2 nach oben geschoben.
// * Hier wird der Offset digital wieder abgezogen damit auch Regler-Intern mit symmetrischen 
// * Sequenzen gearbeitet werden kann. 
// * 
// * ==>  Fuer die DAC-Hardware gilt das Gleiche, der +Offset (von vor ADC) wird also nach dem 
// *      DA-Wandler analog wieder subtrahiert. Darum muss dem intern symmetrischen Signal
// *      vor dem beschreiben des DAC-Data-hold-registers wieder AREF/2 aufaddiert werden.
// */
// 
///**< 
//* Take a copy of direct-memory-accessed array ADC_MultiConvBuff 
//* Up to this point, the access macros SETPOINT_Y_FLOAT, POS_Y_FLOAT ... 
//* also holds the new values 
//*/
//    for (__IO uint8_t k=0; k<4; k++) {
//        ADC_fBuff[k] = (float) ((float) (ADC_MultiConvBuff[k] - AN_OFFSET) * LSB);     
//    }

//// ==============================================================
////            Laser interrupter control
//// ==============================================================
///* To implement the functionality of blanking a setpoint vector, 
// * simply add a blanking information column to the setpoint 
// * datastream
// *
// * [x-coordinate; y-coordinate; blankingBit] 
// */

//// ==============================================================
////            Setpoint source selector
//// ==============================================================
//    while (1) {
//        if (g.setpointSrc == INTERNAL_SETPOINT) {   ///< Internal only, no remote offset or something else 
//            pidErr_x =  setpoint_tgl - POS_X_FLOAT;              
//            pidErr_y =  setpoint_tgl - POS_Y_FLOAT;              
//        }
//        if (g.setpointSrc == ANALOG_SETPOINT) {     ///< setpoint is sourced by analog input 
//                // flot conversion!
//        }
//        if (g.setpointSrc == REMOTE_SETPOINT) {     ///< Remote only, no internal signal or offset 
//            pidErr_x =  pidDataX.W_remf - POS_X_FLOAT;              
//            pidErr_y =  pidDataY.W_remf - POS_Y_FLOAT;              
//        }
//        if (g.setpointSrc == REMOTE_OPENLOOP) {     ///< PID bypassed, remote setpoint feed throgh 
//            updateActuator_f( /*pidDataY.W_remf*/ \
//                                pidDataY.W_remf, pidDataY.W_remf);
//            break;
//        }
//        if (g.setpointSrc == REMOTE_INTERNAL_MIXED) {     ///< Mixed mode, summing internal + external setpoint 
//            pidErr_x = ((float)(setpoint_tgl + pidDataX.W_remf)) - POS_X_FLOAT;
//            pidErr_y = ((float)(setpoint_tgl + pidDataY.W_remf)) - POS_Y_FLOAT;
//        }
//        if (  (g.setpointSrc != INTERNAL_SETPOINT) && (g.setpointSrc != ANALOG_SETPOINT)  && \
//              (g.setpointSrc != REMOTE_OPENLOOP)   && (g.setpointSrc == REMOTE_OPENLOOP) ) {
//             /* Error state, halt */
//             TM_DISCO_LedOn(LED_GREEN);
//             TM_DISCO_LedOn(LED_RED);
//        }
//        
//        /**< Calculate PID here */
//        ////dbg(DBG_PID_TIMING_PE6, hi);
////        updateActuator_f(   arm_pid_f32(&PIDY, pidErr_y), \
////                            arm_pid_f32(&PIDY, pidErr_y));
//        ////dbg(DBG_PID_TIMING_PE6, lo);

//        
//        break;
//    }    
//   
//   

//    /* Set LEDs according to which angular position is greater */
////    if ( POS_Y_FLOAT > SETPOINT_Y_FLOAT) {
////        TM_DISCO_LedOn(LED_GREEN);
////        TM_DISCO_LedOff(LED_RED);
////    } else if ( POS_Y_FLOAT <  SETPOINT_Y_FLOAT) {
////        TM_DISCO_LedOn(LED_RED);
////        TM_DISCO_LedOff(LED_GREEN);
////    } else {
////        TM_DISCO_LedOff(LED_ALL);
////    }
//   
//}

//// ==============================================================
////     IRQ callback:    ADC end-of-conversion 
//// ==============================================================
//void ADC_IRQHandler(void) {
//    if (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == SET) {
//        ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
//        /* End of conversion interrupt occured */
//        ////dbg(DBG0, lo);
//        uint16_t W_now = ADC_MultiConvBuff[1];   // setpoint (Isens_y)
//        return;
//    }
//    
//   
//    if (ADC_GetFlagStatus(ADC1, ADC_FLAG_AWD) == SET) {
//        ADC_ClearITPendingBit(ADC1, ADC_IT_AWD);
//        if ((ass.state == ASS_STATIONARY_INTEGRATOR) ||
//            (ass.state == ASS_DISCHARGING_INTEGRATOR)) {
//            ass.state = ASS_CHARGING_INTEGRATOR;
//            printf("ASS:charging_start...\n");
//        }
//    /* Analog watchdog interrupt occured */
//        if ((ass.integrator <= ass.lowerVal) || (ass.integrator >= ass.upperVal)) {   /**< check for integrator limit */
//            if (! ass.tripped) {
//                ass.tripped = 1;                    /**< Set "tripped" state if it is so */
//                DAC_SecureSetDualChanSigned = \
//                      &DAC_SetDualChanSigned_Tripped;  /**< set function pointer to the "ass tripped" handler*/
//            }
//            return;
//        }
//        else {                                  /**< else increment integrator */
//            ass.integrator+= ass.upperVal * (double)TS_US*1e-6/(ass.tripTime);
//        }
//        return;
//    }
//    printf("bad ADC IntReq source\n");
//}
//	
//// ==============================================================
////     IRQ callback:    Timer 2 overrun 
//// ==============================================================
//void TIM2_IRQHandler(void) {
//	////dbgT(DBG2);
////	GPIO_ToggleBits(DBG_IO, DBG4);
//	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
//		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

//        ////dbg(DBG4, lo);
//        DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);
//        
////        if (ass.stateCtr >= (uint8_t)(ASS_100US / TS_US)) {
////            ass.stateCtr = 0;
////            if (ass.integrator > 0) {
////                ass.state = ASS_DISCHARGING_INTEGRATOR;
////                ass.tripped = 0;
////                ass.integrator -= ass.upperVal * (double)TS_US*1e-6/(ass.tripTime);
////            }
////        }
//	}
//    else
//        while (1);;
//}


//// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
////        IMPORT from MATLAB code generated project pid_lib_v3
//// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//#pragma O0
//// ==============================================================
////     IRQ callback:    SysTick Interrupt
//// ==============================================================
//void SysTick_Handler(void)
//{
//    uint16_t ypid_uint=0;
//    int16_t ypid_int=0;
//    /* Manage nb of loop before interrupt has to be processed */
//    remainAutoReloadTimerLoopVal_S--;
//    if (remainAutoReloadTimerLoopVal_S) {
//        return;
//    }

//    remainAutoReloadTimerLoopVal_S = autoReloadTimerLoopVal_S;

//    /* Check base rate for overrun */
//    if (OverrunFlags[0]) {
//        rtmSetErrorStatus(pid_lib_v3_M, "Overrun");
//        return;
//    }

//    OverrunFlags[0] = TRUE;

//    /* Step the model for base rate */
//    pid_lib_v3_step();
//    
//    /* Get model outputs here */
//    ypid_int = decodeDbl_toInt( pid_lib_v3_Y.Ypid );
//    ypid_uint = ypid_int + AN_OFFSET;
//    DAC_SetDualChanSigned( ypid_uint, ypid_uint );
//    
//    /* Indicate task for base rate complete */
//    OverrunFlags[0] = FALSE;
//}
//// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
////    END of    IMPORT from MATLAB code generated project pid_lib_v3
//// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//#pragma O1


//int main (void)
//{
//   /**< Define private main variables */
////    TM_PWM_TIM_t TIM_Data;    ///< Timer data for PWM 

//  /* Clock has to be configured first*/
////  RCC_Configuration();
//   SystemInit();       /**< Initialize system. */
//    TM_DELAY_Init();    /**< Initialize Delay library. */

//   /**< Initialize discovery button and leds */
//    TM_DISCO_ButtonInit();

//    /* Initialize Leds */
//    TM_DISCO_LedInit();

//   /**< Initialize USART1-> TX: PA9, RX: PA10. */
//    TM_USART_Init(USART1, TM_USART_PinsPack_1, 115200);			

////   /**< Initialize TIM3, 1kHz frequency */
////    TM_PWM_InitTimer(TIM3, &TIM_Data, 1000);

////   /**< Initialize TIM2, Channel 1, PinsPack 2 = PA5 */
////    TM_PWM_InitChannel(&TIM_Data, TM_PWM_Channel_3, TM_PWM_PinsPack_2);  // ???? PB0

////    /* Set default duty cycle */
////    TM_PWM_SetChannelPercent(&TIM_Data, TM_PWM_Channel_3, duty);

////    TM_ADC_Init(ADC1, ADC_X_CHANNEL);
////    TM_ADC_Init(ADC1, ADC_Y_CHANNEL);
//   
//    MDB_GPIO_Init();    ///< Init interrupter GPIOs

//                                
//   /**< Initialize autoShutdown system struct. */
//    ass.lowerVal = ASS_TRIPPING_LOWER_DEFAULT;
//    ass.upperVal = ASS_TRIPPING_UPPER_DEFAULT;
//    ass.lowerVal = -1.25;
//    ass.upperVal = 1.25;
//    
//    ass.integrator = 0;
//    ass.safeVal = ASS_SAFEVALUE_DEFAULT ;
//    ass.tripped = 0;
//    ass.ack = 0;
//    ass.tripTime = ASS_TRIPPING_TIME_DEFAULT; // 750ms initial
//    ass.state = ASS_STATIONARY_INTEGRATOR;
//    ass.stateCtr = 0;
//    
//   /**< Initialize global structure */
//    g.waveForm = NN; g.duty = 999; g.freq = 999; g.lookAt = 999;
//    g.dacHw[TM_DAC1].upperLim = (int16_t)( 2113 - ADC_RNG/2 );
//    g.dacHw[TM_DAC1].lowerLim = (int16_t)( 1917 - ADC_RNG/2 );
//    g.dacHw[TM_DAC2].upperLim = (int16_t)( 2113 - ADC_RNG/2 );
//    g.dacHw[TM_DAC2].lowerLim = (int16_t)( 1917 - ADC_RNG/2 );
//    g.setpointSrc = REMOTE_INTERNAL_MIXED;
//    g.refresh = 20;    
//    g.beamEnabled = GPIO_ON;

//    
//   /**< Transmitt bootup message to UART1 */
//    char *tok;
//    char *delimit = "\\";
//    tok = strtok(__FILE__, delimit);
//    tok = strtok (NULL, delimit);

//    printf("\n\n--- %s ---\n", tok);
//    printf("--- Compiled: %s  %s ---\n\n", __DATE__, __TIME__);
////    printf("__TARGET_ARCH_ARM: %s\n", TARGET_ARCH_ARM[__TARGET_ARCH_ARM]);
//    printf("__BASE_FILE__: %s\n", __BASE_FILE__);
// 
// 
//   /**< Initialize DAC outputs DAC1-> PA4  DAC2-> PA5 */
//    TM_DAC_Init(TM_DAC1);       
//    TM_DAC_Init(TM_DAC2);       

//    printf("connect Write pointer to DAC output register\n");
//    DAC_SecureSetDualChanSigned = &DAC_SetDualChanSigned;    
//    
//    
//   /**< Initialize TIM2 as part of sampling hardware */
//    TM_Timer2_config(DISABLE, ENABLE, SAMPLE_INTERVAL);
//    ADC_ContScanMode_w_DMA_timeTrigd( ADC_MultiConvBuff, 4);
//    
//   /**< Set threshold for analog watchdog */
//    ADC_AnalogWatchdogThresholdsConfig(ADC1,    
//                                decode_toUint(ass.upperVal + AREF_BY2), 
//                                decode_toUint(ass.lowerVal + AREF_BY2));
// 
// /* ================================================================================ */       
///**< ===============     ENABLE_ON_POWER-ON-SEQUENCE     ======================================== */       
///* ================================================================================ */       

//	TIM_Cmd(TIM3, ENABLE);          ///< Enable PWM timer 3
//    TIM_Cmd(TIM2, ENABLE);          ///< Enable sampling timer 2
//    ADC_DMACmd(ADC1, ENABLE);       ///< Enable DMA functionality
//    ADC_Cmd(ADC1, ENABLE);          ///< Enable onboard hardware ADC 1
//    ADC_SoftwareStartConv(ADC1);    ///< Trigger the regular group scan 
//// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
////        IMPORT from MATLAB code generated project pid_lib_v3
//// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//  /* Data initialization */
//  int_T i;
//  for (i=0;i<1;i++) {
//    OverrunFlags[i] = 0;
//  }

//  /* Clock has to be configured first*/
// // RCC_Configuration();

//  /* Model initialization call */
//  pid_lib_v3_initialize();

//   

//  /* Systick configuration and enable SysTickHandler interrupt */
//  if (SysTick_Config((uint32_t)(SystemCoreClock * TS_US))) {
//    autoReloadTimerLoopVal_S = 1;
//    do {
//      autoReloadTimerLoopVal_S++;
//    } while ((uint32_t)(SystemCoreClock * TS_US)/autoReloadTimerLoopVal_S >
//             SysTick_LOAD_RELOAD_Msk);

//    SysTick_Config((uint32_t)(SystemCoreClock * TS)/autoReloadTimerLoopVal_S);
//  }

//  remainAutoReloadTimerLoopVal_S = autoReloadTimerLoopVal_S;//Set nb of loop to do
// 

//// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
////    END of    IMPORT from MATLAB code generated project pid_lib_v3
//// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//    TM_DISCO_LedOn(LED_GREEN);
//    TM_DISCO_LedOn(LED_RED);
//    TM_DISCO_LedOff(LED_GREEN);
//    TM_DISCO_LedOff(LED_RED);
//    
//  /* Infinite loop */
//  /* Real time from systickHandler */
//    while (1) {
//        while (! TM_DISCO_ButtonOnPressed()) {};
//        TM_DISCO_LedOn(LED_RED);

//        while (! TM_DISCO_ButtonOnPressed()) {
//            printf("x: % 1.6f  y: % 1.6f\n", ADC_fBuff[INDEX_Px], ADC_fBuff[INDEX_Py]);
//            Delayms(100);
//        }
//         pid_lib_v3_P.pulseGenEN_CurrentSetting = 0x01;
//         printf("pid_lib_v3_P.pulseGenEN_CurrentSetting = 0x01\n");

//        while (! TM_DISCO_ButtonOnPressed()) {
//        }
//    }
//}
///* File trailer for Real-Time Workshop generated code.
// *
// * [EOF] main.c
// */


///** 
// * @brief   Set beam control source and new state
// */ 
//int beamCtrl(beamCtrlSource_t src, MDB_GPIO_STATE_t newState ) {
//    if ((src == BEAM_CTRL_SOURCE_GLOBAL) && (newState != DNI)) {
//        printf("Error, Beam source can't be GLOBAL if new state is not DNI");
//        return -1;
//    }

//    if ((src == BEAM_CTRL_SOURCE_GLOBAL) && (newState == DNI)) {
//        MDB_GPIO_Switch(BEAM_INTERRUPT, g.beamEnabled);
//        return 0;
//    }

//    if (src == BEAM_CTRL_SOURCE_MANUAL) {
//        MDB_GPIO_Switch(BEAM_INTERRUPT, newState);
//        return 0;
//    }
//    
//    return -1;
//}



///**
// * @brief   Safely update actuator control signals
// * Safely means in terms of missplaced or oscillating controller outputs.
// * The analog watchdog peripheral takes care about "out of normal range" 
// * events. If a converted position signal is out of range, the watchdog 
// * handler starts time integration and after "out of range" integrator 
// * reaches a defineable limit, the watchdog takes the system to into
// * "Tripped" state.
// * Therefor a global error flag becomes true AND the function pointer
// * DAC_SetDualChanSigned() that points to a wraper function, gots to be
// * replaced by a pointer that points to DAC_SetDualChanSigned_Tripped().
// * In the DAC_SetDualChanSigned() function, that is pointed to in tripped 
// * state, only outputs a zero level DAC-Signal to take external hardware 
// * in a safe state.
// */
//int updateActuator_f(float I_set_x, float I_set_y) {
//    int toPlant_intBuff[2] = { 0, 0 };
//    int *toPlant_int = &toPlant_intBuff[0];
//    
//    if (!ass.tripped) {     ///< only if ass state is NOT tripped

//        /**< decode the float values to an integral type */
//        *toPlant_int     = decode_toInt(I_set_x);
//        *(toPlant_int+1) = decode_toInt(I_set_y);

//        /**
//         * Check decoded values to be in DAC output range. Clipping 
//         * would be necessary 
//         */
//        for (uint8_t k=0; k<2; k++) {
//            if (*(toPlant_int+k) > UPPER_DAC_LIMIT_SIGNED)
//                    *(toPlant_int+k) = UPPER_DAC_LIMIT_SIGNED;
//            if (*(toPlant_int+k) < LOWER_DAC_LIMIT_SIGNED)
//                    *(toPlant_int+k) = LOWER_DAC_LIMIT_SIGNED;
//        }
//    } 
//    else{   
//    /**<><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>    
//       <>  This branch eror-handles an ass integrator_full event i.e.<>
//       <><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>    
//       <>                    FUSE TRIPPED                            <>
//       <><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<> */  
//       /* All safety related actions are done by the watchdog handler */ 
//    }

//   /**
//    * @brief    Return argument toPlant_int is of type int16_t because 
//    *           it holds the --un--biased DAC output register values...
//    *           This is a function pointer!
//    * Casting from (float32_t) toPlant to int16_t type introduces 
//    * rounding errors which couldn't be prevented! 
//    */
//    DAC_SecureSetDualChanSigned( *toPlant_int, *(toPlant_int+1));  
//        
//    return 0;
//}

/**
 * @file        main.c
 *
 * @date        14 Mar 2017
 * @author      Manuel Del Basso (mainster)
 * @email       manuel.delbasso@gmail.com
 *
 * @ide         System Workbench ac6 (eclipse stm32)
 * @stdperiph   STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 * @license     GNU GPL v3
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
 *  Additional symbol definitions under "C/C++" > "Symbols"
 *      - ARM_MATH_CM4
 *      - __FPU_PRESENT = 1
 */

/* Include */
#include "main.h"
#include <string.h>

/* ARM architecture names */
#include "arm_architect.h"

/**
 * @addtogroup MD_APP
 * @{
 */

/**
 * @addtogroup APP_Main
 * @{
 */

/**
 * @addtogroup Main_Macros
 * @{
 */

#define RX_FRAME_PADDING_CHAR   '~'

/** @} */

/**
 * @addtogroup Main_Typedefs
 * @{
 */

/** @} */

/**
 * @addtogroup Main_Variables
 * @{
 */

global_t g;
uint32_t ticks = 0;
extern DAC_WP_t (*DAC_SecureSetDualChanSigned)(int16_t, int16_t);

/** @} */

/**
 * @addtogroup Main_Function
 * @{
 */

// ==============================================================
//   This is needed in order to provide printf functionality
// ==============================================================
#define PUTCHAR_PROTOTYPE int fputc(int c, FILE *f)

//struct __FILE { int handle; };
FILE __stdout;

PUTCHAR_PROTOTYPE {
    /* Place implementation of device specific fputc here */
    USART_SendData(USART1, (uint8_t) c);

    /* Polling loop until the end of transmission */
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {
    }

    return c;
}

// ==============================================================
//                         Prototypes
// ==============================================================
void fastConsoleCase(arm_pid_instance_f32 *pid);

/** @} */


// ==============================================================
//                  Compensator things
// ==============================================================

/**
 * @brief      Use old implementation of PID algorithm.
 */
#define ALGO_OLD                10
#define ADJUSTING_SETPOINT      11      // Stellungsalgorithmus
#define SPEED_SETPOINT          12      // Geschwindigkeitsalgorithmus
#define ALGORITHM               ADJUSTING_SETPOINT

/* Get two PID_DATA struct instances */
struct PID_DATA pidDataX;
struct PID_DATA pidDataY;

/**
 * This is the factor used in the gui to enlarge resolution on int-only
 * QSlider ranges
 */
#define SLIDER_VAL_DEVIDER  10000UL

volatile float UNIT_MIKRO = 1 / 1000000;
volatile int16_t pidOut = 0;

/**< Define indices for ADC_MultiConvBuff[ ] acording to setpoint (INDEX_Wx) ans
 * process output (INDEX_P). */
#ifndef INDEX_Py
#define INDEX_Px     0
#define INDEX_Py     1
#if (ADC_N_REGULAR_CHANNELS > 2)
#define INDEX_Ix     2
#define INDEX_Wx      3
#endif
#endif

#define SIGNW -1

#define INTERNAL_SETPOINT
#undef INTERNAL_SETPOINT

#pragma GCC push_options
#pragma GCC optimize ("O0")
//float temps[EXPECTING_SENSORS];

float setpoint_tgl = (1.25 - 0.4);

float duty = 0;             //!< Duty cycle for PWM
arm_pid_instance_f32 PIDX;   //!< ARM PID Instance, float_32 format
arm_pid_instance_f32 PIDY;   //!< ARM PID Instance, float_32 format

__IO float toPlant;              //!< next actuator value in Volt
int16_t toPlant_int;       //!< next actuator value integral type

int16_t test1 = UPPER_DAC_LIMIT_SIGNED;
int16_t test2 = LOWER_DAC_LIMIT_SIGNED;
#pragma GCC pop_options

/**
 * @brief   Choose PID parameters
 */
#define PID_PARAM_KP        0.5         //!< Proporcional
#define PID_PARAM_KI        0.0     //!< Integral
#define PID_PARAM_KD        0.0         //!< Derivative

// ==============================================================
//                      beam interrupter
// ==============================================================

//#define TS                             10.0e-6

//#define ASG_100US                   100
// ==============================================================
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// ==============================================================
//      This indices HAVE TO STAY IN SYNC WITH Qt GUI SOURCES
// ==============================================================
volatile int NC = 3;        // index 0...2  (short command)
volatile int FS = 2;        // index 3...4  (fieldsep)
volatile int NV = 10;       // index 5...14 (value)
// ==============================================================
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// ==============================================================

//#define
//   GPIO_WriteBit((GPIO_TypeDef*)DBG_IO, IO_PIN, Bit_SET)
//   GPIO_WriteBit((GPIO_TypeDef*)DBG_IO, IO_PIN, Bit_RESET))

//#define
//                                ((GPIO_TypeDef*)DBG_IO, IO_PIN)

/* Define static usart strings used as GUI error / status messages
 */
#define  MSG_GUI_UART1_RECV_BUFF_FULL   "@GUI:usart1RecvBuffFull\n"  //!< Inform Gui about a full usart1 buffer

// ==============================================================
// ==============================================================
// ==============================================================
volatile uint16_t gW = 0;
volatile uint16_t gY = 0;
volatile uint16_t multiADC[2];

float pidErrBuff[2]; /* PID error buffer*/
float toPlantBuff[2]; /* PID output buffer*/

/**
 * @brief      ADC dual mode DMA access buffer.
 * 
 * This __IO buffer 
 */
__IO int16_t ADC_MultiConvBuff[5];

/**
 * @brief      ADC values access buffer.
 *
 * @attention  Because a float based instance of PID is used, casting of ADC_MultiConvBuff
 *             buffer values is necessary.
 */
__IO float ADC_fBuff[5];

#define NO1 1

extern ActuatorSafestateGuard_t ASG;

/* ---------- */
/* Used Timer */
/* ---------- */
// (4)  TM_Timer4_config(ENABLE, ENABLE, 450 / g.freq);         waveformGeneration
// (3)  TM_PWM_InitTimer(TIM3, &TIM_Data, 1000);
// (2)  MD_Timer2_config(DISABLE, ENABLE, SAMPLE_INTERVAL);     Base sample rate
static items_t items_list[] = { 
    { .name = "kp:", .id = KP }, 
    { .name = "ki:", .id = KI }, 
    { .name = "kd:", .id = KD }, 
    { .name = "w::", .id = W } 
};

static itemsw_t itemsw_list[] = { 
    { .name = "sin", .idw = COS }, 
    { .name = "tri", .idw = TRIANG }, 
    { .name = "rec", .idw = SQUAREWAV }, 
    { .name = "s2 ", .idw = QUADRATIC }, 
    { .name = "is2", .idw = I_QUADRATIC }, 
    { .name = "saw", .idw = SAWTOOTH }, 
    { .name = "rem", .idw = CMD_REMOTE_SETPOINT }, 
    { .name = "orm", .idw = CMD_OPENLOOP_REMOTE }, 
    { .name = "ast", .idw = CMD_ANALOG_SETPOINT }, 
    { .name = "ist", .idw = CMD_INTERNAL_SETPOINT }, 
    { .name = "rfr", .idw = CMD_REFRESH_RATE }, 
    { .name = "tv1", .idw = CMD_TESTVECT_1 }, 
    { .name = "tv2", .idw = CMD_TESTVECT_2 }, 
    { .name = "mat", .idw = CMD_MATLAB_LINK } 
};

static MiscCmds_t itemsm_list[] = { 
    { .name = "pid_init",           .idm = misc_pid_init }, 
    { .name = "pid_Controller",     .idm = misc_pid_Controller }, 
    { .name = "pid_Reset_Integ",    .idm = misc_pid_Reset_Integrator }, 
    { .name = "update_pid_data",    .idm = misc_update_pid_data }, 
    { .name = "safetyEnable",       .idm = misc_asgOnOff_cmd }, 
    { .name = "safetyUpperLim",     .idm = misc_asgUpperLim_cmd }, 
    { .name = "safetyLowerLim",     .idm = misc_asgLowerLim_cmd }, 
    { .name = "safetyTripp",        .idm = misc_asgTrippTime_cmd }, 
    { .name = "safetyVal",          .idm = misc_asgSaveVal_cmd }, 
    { .name = "EnableBeam",         .idm = misc_beamOn_cmd }, 
    { .name = "DisableBeam",        .idm = misc_beamOff_cmd }, 
 };

void resetPID(void) {
    pid_Init(KP_INIT, KI_INIT, KD_INIT, TF_INIT, TS, 
            &pidDataY, PID_StructType_Unbuffered);
    PID_Calc_Coeffs(&pidDataY, PID_IntMethod_RwdRect);
}

double lastVal = 0;

/**
 * @brief Check target architecture
 */
#define __TARGET_ARCH_ARM   0

/**
 * @brief      USART Pinout select enumeration.
 *
 * @note       In case of custom PinsPack usage, refer to @ref
 *             TM_USART_InitCustomPinsCallback.
 */
typedef enum {
    TM2_USART_PinsPack_1,      /*!< Select PinsPack1 from Pinout table for specific USART */
    TM2_USART_PinsPack_2,      /*!< Select PinsPack2 from Pinout table for specific USART */
    TM2_USART_PinsPack_3,      /*!< Select PinsPack3 from Pinout table for specific USART */
    TM2_USART_PinsPack_Custom  /*!< Select custom pins for specific USART, callback will be called */
} TM2_USART_PinsPack_t;

/**
 * @addtogroup Main_Function
 * @{
 */

/**
 * @brief      Main function entry.
 *
 *             Invokes initial core and peripheral initialiation routines.
 *             Provides endless loop functionality.
 *
 * @return     0
 */
/******************************************************************************/
/*                              Main entry point                              */
/******************************************************************************/
int main(void) {
    /**< Private main variables */

    /**< Initialize System */
    SystemInit();

    /**< Enable some AHB clock sources */
    RCC_Periph2DMA_Conf();

    /**< All used GPIOs should be initialized via this call */
    MDB_GPIO_Init();
    //  beamCtrl(BEAM_CTRL_SOURCE_MANUAL, GPIO_OFF );

    /**< Initialize PID system, float32_t format */
    //  arm_pid_init_f32(&PIDX, 1);
    //  arm_pid_init_f32(&PIDY, 1);
#ifndef NO_FPU
    arm_pid_instance_f32 pidInst;

    arm_pid_init_f32(&pidInst, 1);
#endif

    /**< Initialize discovery button and leds */
    MD_DISCO_ButtonInit();

    /**< Initialize Leds */
    MD_DISCO_LedInit();

    /**< Initialize delay library */
    MD_DELAY_Init();

    while (1) {
        Delayms(500);
        MDB_GPIO_Toggle(LED_GREEN_A);
        MDB_GPIO_Toggle_m2(LED_RED_A);
    }

    /**< Initialize USART1-> TX: PA9, RX: PA10. */
    MD_USART_Init(USART1, (MD_USART_PinsPack_t) 0, 115200);

    /**< Initialize Delay library. */
    //  TM_DELAY_Init();

    /**< Initialize TIM5, heart beat timer */
    //MD_Timer5_config(ENABLE, ENABLE, 48000);        // 48MHz*1ms

    /**< Initialize DAC outputs DAC1-> PA4  DAC2-> PA5 */
    MD_DAC_Init(MD_DAC1);
    MD_DAC_Init(MD_DAC2);

    /**< Configure function address for initial target function  */
    DAC_SecureSetDualChanSigned = &DAC_SetDualChanSigned;
    printf("connect Write pointer to DAC output register\n");

    /**< Configure the nested Vector interrupt controller */
    NVIC_Configuration();

    /**< Initialize TIM2 as part of sampling hw, also enable DMA trigger source */
    TIM2_DMA_Trigger_Config(DISABLE, ENABLE, SAMPLE_INTERVAL);

    /**< Configure DMA controller for ADC2memory */
    DMA_Config(ADC_MultiConvBuff, ADC_N_REGULAR_CHANNELS);

    /**< Configure ADC channels for regular scan, DMA channel ... */
    ADC_Scan_Group_Config();

    /**< Set initial values on global structs */
    init_globalStructs();

    /**< Configure analog watch dog for selected X/Y ADC channels (security feature) */
    AN_Watchdog_Config();

    /**< Set threshold for analog watch dog */
    ADC_AnalogWatchdogThresholdsConfig(ADC1,decode_toUint(ASG.upperThreshold + VA_BIAS),
                                            decode_toUint(ASG.lowerThreshold + VA_BIAS));

    /**< Transmit boot up message to UART1 */
    char *tok;
    char *delimit = "\\";
    tok = strtok(__FILE__, delimit);
    tok = strtok(NULL, delimit);

    printf("\n\n--- %s ---\n", tok);
    printf("--- Compiled: %s  %s ---\n\n", __DATE__, __TIME__);
    printf("__TARGET_ARCH_ARM: %s\n", TARGET_ARCH_ARM[__TARGET_ARCH_ARM]);
    printf("__BASE_FILE__: %s\n", __FILE__);

    printf("items_list[%d].name: %s\t .id: %d", 0, items_list[0].name, items_list[0].id);
    /* ================================================================================
     * ===============     ENABLE_ON_POWER-ON-SEQUENCE     ============================
     * ================================================================================ */

    TIM_Cmd(TIM3, ENABLE);          //!< Enable PWM timer 3
    TIM_Cmd(TIM2, ENABLE);          //!< Enable sample timer 2
    ADC_DMACmd(ADC1, ENABLE);       //!< Enable DMA functionality
    ADC_Cmd(ADC1, ENABLE);          //!< Enable on-board hardware ADC 1
    ADC_SoftwareStartConv(ADC1);    //!< Trigger the regular group scan

    g.waveForm = CMD_TESTVECT_1;

    /**
     * Main loop
     */
    while (1) {

        /* ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: */
        /*                      fast Console                               */
        /* ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: */
        while (!MD_DISCO_ButtonOnPressed()) {
            //Console(&pidDataY, &fpidData, _INT_);
            fastConsoleCase(&PIDY);

            for (uint8_t k = 0; k < 20; k++) {
                //                if (MD_USART_BufferFull(USART1)) {
                //                    printf( MSG_GUI_UART1_RECV_BUFF_FULL );
                //                    fastConsoleCase(&PIDY);
                //                    TM_USART_ClearBuffer(USART1);
                //                };
                Delayms((uint32_t) g.refresh / 20);
            }
            //            if ((ASG.state == ASG_CHARGING_INTEGRATOR) && (! ASG.tripped)) {
            if (ASG.integrator != lastVal) {
                printf("%.8f\n", ASG.integrator);
            }
            if ((ASG.tripped) && (!ASG.ack)) {
                printf("Tripped!");
                ASG.ack = 1;
                lastVal = ASG.integrator;
                TM_Timer4_config(ENABLE, ENABLE, 45000 / 3);
            }

        }

        /* ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: */
        /*              fast Console / REMOTE_OPENLOOP                     */
        /* ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: */
        while (!MD_DISCO_ButtonOnPressed()) {
            fastConsoleCase(&PIDY);
            g.setpointSrc = REMOTE_OPENLOOP;

            for (uint8_t k = 0; k < 20; k++) {
                if (MD_USART_BufferFull(USART1)) {
                    printf( MSG_GUI_UART1_RECV_BUFF_FULL);
                    fastConsoleCase(&PIDY);
                    TM_USART_ClearBuffer(USART1);
                };
                Delayms((uint32_t) g.refresh / 20);
            }

            printf("W: % 3.2f\tPy: % 3.2f\tE: % 3.2f\tDuty: % 3.2f%%\n",
                    SETPOINT_Y_FLOAT, POS_Y_FLOAT, pidErr_y, toPlant_y);
        }

        /* ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: */
        /*              fast Console / W:  P:  Duty:                       */
        /* ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: */
        while (!MD_DISCO_ButtonOnPressed()) {
            //Console(&pidDataY, &fpidData, _INT_);
            fastConsoleCase(&PIDY);

            for (uint8_t k = 0; k < 20; k++) {
                if (MD_USART_BufferFull(USART1)) {
                    printf( MSG_GUI_UART1_RECV_BUFF_FULL);
                    fastConsoleCase(&PIDY);
                    TM_USART_ClearBuffer(USART1);
                };
                Delayms((uint32_t) g.refresh / 20);
            }

            printf("W: % 3.2f\tPy: % 3.2f\tE: % 3.2f\tDuty: % 3.2f%%\n",
                    SETPOINT_Y_FLOAT, POS_Y_FLOAT, pidErr_y, toPlant_y);
        }

        /* ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: */
        /*              fast Console / W:  P:  Duty:                       */
        /* ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: */
        while (!MD_DISCO_ButtonOnPressed()) {
            //Console(&pidDataY, &fpidData, _INT_);
            fastConsoleCase(&PIDY);

            for (uint8_t k = 0; k < 20; k++) {
                if (MD_USART_BufferFull(USART1)) {
                    printf( MSG_GUI_UART1_RECV_BUFF_FULL);
                    fastConsoleCase(&PIDY);
                    TM_USART_ClearBuffer(USART1);
                };
                Delayms((uint32_t) g.refresh / 20);
            }

            printf("W: % 3.2f\tPy: % 3.2f\tE: % 3.2f\tDuty: % 3.2f%%\n",
                    SETPOINT_Y_FLOAT, POS_Y_FLOAT, pidErr_y, toPlant_y);
        }
    }
}


/** @} */

/** @} */

/** @} */


void init_globalStructs(void) {

    /**< Set PID parameters */
    PIDX.Kp = PID_PARAM_KP; /* Proporcional */
    PIDX.Ki = PID_PARAM_KI; /* Integral */
    PIDX.Kd = PID_PARAM_KD; /* Derivative */
    PIDY.Kp = PID_PARAM_KP; /* Proporcional */
    PIDY.Ki = PID_PARAM_KI; /* Integral */
    PIDY.Kd = PID_PARAM_KD; /* Derivative */

    /** 
     * Initialize ASG system structure. 
     */
    ASG.lowerThreshold  = ASG_LOWER_THRESHOLD_DEFAULT;
    ASG.upperThreshold  = ASG_UPPER_THRESHOLD_DEFAULT;
    ASG.trippingLevel   = ASG_INTEGRATOR_TRIPPING_DEFAULT;

    ASG.integrator = 0;
    ASG.safeVal = ASG_SAFEVALUE_DEFAULT;
    ASG.tripped = 0;
    ASG.ack = 0;
    ASG.tripTime = ASG_TRIPPING_TIME_DEFAULT;   // 750ms initial
    ASG.state = ASG_STATIONARY_INTEGRATOR;

    /** 
     * Initialize the global state structure. 
     */
    g.waveForm = NN;
    g.duty = 999;
    g.freq = 999;
    g.lookAt = 999;
    g.dacHw[MD_DAC1].upperLim = (int16_t) (2113 - ADC_RNG / 2);
    g.dacHw[MD_DAC1].lowerLim = (int16_t) (1917 - ADC_RNG / 2);
    g.dacHw[MD_DAC2].upperLim = (int16_t) (2113 - ADC_RNG / 2);
    g.dacHw[MD_DAC2].lowerLim = (int16_t) (1917 - ADC_RNG / 2);
    g.setpointSrc = REMOTE_INTERNAL_MIXED;
    g.refresh = 20;
    g.beamEnabled = GPIO_ON;
}

/** @} */

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

#define IDX_CMD     1       // index 0 is @ or ! or #
#define IDX_VAL     5   // index 5 

#define UART_BUFF_SIZE      (1+NC+FS+NV+1)
#define VALUE_BUFF_SIZE     (NV+1)
#define CMD_BUFF_SIZE       (NC+1)

//void fastConsoleCase (volatile struct PID_DATA *pid) {
#if  defined (KEIL_IDE)
#pragma O0
#elif  defined (__GNUC__)
#pragma GCC push_options
#pragma GCC optimize ("O0")
#endif

/**
 * @brief   Holds the row- count of waveform arrays. Must be initialized.
 */
static short rowCount_2D_array = 0;

/**
 * @brief   Holds the count direction of waveform array index.
 */
static int8_t dir = 0;

/**
 * @brief      Fast console command parser.
 *
 *             Interface to a signal generator based on lookup tables.
 *             Implemented waveforms:   - Śin/Cos
 *
 * @param      pid   Pointer to a pid instance from dsp library.  
 */
void fastConsoleCase(arm_pid_instance_f32 *pid) {
    /**
     * @brief      private member declarations.
     */

    /**
     * @brief      Char buffer to hold received value as substring.
     */
    char sUart[UART_BUFF_SIZE];

    /**
     * @brief      Char buffer to hold selected command as substring.
     */
    char sCmd[CMD_BUFF_SIZE];

    /**
     * @brief      Char buffer to hold given value as substring.
     *
     *             sVal holds the string representation of a received float
     *             value. For useing atof(), a buffer of
     *              - 4 x sizeof(int8_t)+1 (float32_t) or
     *              - 8 x sizeof(int8_t) (double)
     *             is necessary.
     */
    char sVal[VALUE_BUFF_SIZE];

    char *pCmd = sCmd;              //!< pointer for buffer arrays
    char *pVal = sVal;              //!< pointer for buffer arrays

    CmdItem_t cmd;
    WavItems_t wav;
    items_t *choice = NULL;
    itemsw_t *choicew = NULL;

    //    int i;

    volatile uint8_t nChars = MD_USART_Gets( USART1, &sUart[0], UART_BUFF_SIZE);

    /**
     * Check for serial data frames >= UART_BUFF_SIZE
     */
    if (nChars < UART_BUFF_SIZE) {
        MD_DISCO_LedOn(LED_RED);

        /**
         * malloc the receive buffer.
         */
        memset(&sUart, 0, UART_BUFF_SIZE * sizeof(char));
        return;
    }

    MD_DISCO_LedOff(LED_RED);
    MD_DISCO_LedOn(LED_GREEN);
    ticks = 100000;   // 200k *10us = 2s
    /**
     * @brief      Char buffer to hold given value as substring.
     */

    /**
     * @brief malloc() if a frame received where sizeoff() >= UART_BUFF_SIZE.
     */

    /**
     * malloc the command buffer.
     */
    memset(&sCmd, 0, CMD_BUFF_SIZE * sizeof(char));

    /**
     * malloc the value buffer.
     */
    memset(&sVal, 0, VALUE_BUFF_SIZE * sizeof(char));

    /**
     * @brief      Check leading character via receive buffer.
     * @li         @@   frame indicator for pid related config/param.
     * @li         \!   frame indicator for a misc command.
     * @li         @#   frame indicator for a signal generator command.
     */
    if (sUart[0] == '@') {
        /* pid related config/param */
        strncpy(pCmd, &sUart[IDX_CMD], NC);        //!< command substring
        strncpy(pVal, &sUart[IDX_VAL], NV);        //!< Value substring

        /* Check for end of char coded float representation
         * numeric char:            0...9
         * decimal delimiter:       . or ,  !!!Take caution for forbidden thousands separator!!!
         * exponental indicator:    e or E
         * signdness:               -
         */
        for (uint8_t k = 0; k <= NV; k++) {                 //!< Max NC numbers ...
            if (((*pVal >= '0') && (*pVal <= '9')) || (*pVal == '.') || (*pVal == ',')
                    || (*pVal == 'e') || (*pVal == 'E') || (*pVal == '-')) {
                pVal++;
            } else {
                break;
            }
        }

        *pVal = '\0';                 //!< place EOF marker
        pVal = &sVal[0];

        while (*pCmd != '=') {    //!< check for end of command  indicator '='
            pCmd++;
        }
        *(pCmd - 1) = '\0';         //!< place EOS marker
        pCmd = &sCmd[0];

        choice = NULL;
        /* parse sCmd buffer and enumerate the command/error */
        for (int i = 0; i < sizeof items_list / sizeof(items_t); i++) {
            if (strcasecmp(sCmd, items_list[i].name) == 0) {
                choice = items_list + i;
                break;
            }
        }

        cmd = choice ? choice->id : UNKNOWN;

        char sValtemp1[] = { "-0.7e-09" };

        /* FIXME */
        volatile float fl1 __attribute__ ((unused)) = atof(&sValtemp1[0]);

#ifdef  FLOAT_PARAMETERS
        //            volatile float valueDecoded = (float)tmp/SLIDER_VAL_DEVIDER;
        volatile float valueDecoded = atof(pVal);

#elif   DOUBLE_PARAMETERS
        double valueDecoded = (double)tmp / SLIDER_VAL_DEVIDER;
#else
        error "datatype?"
#endif

        switch (cmd) {
            case KP: {
                pid->Kp = valueDecoded;
#ifndef NO_FPU
                arm_pid_init_f32(&PIDY, 0);
#endif
                printf("Kp % f set!\n", valueDecoded);
            }
            break;

            case KI: {
                pid->Ki = valueDecoded;
#ifndef NO_FPU
                arm_pid_init_f32(&PIDY, 0);
#endif
                printf("Ki % f set!\n", valueDecoded);
                //                            printf("Ki %s given\n", &sval[0]);
            }
            break;

            case KD: {
                pid->Kd = valueDecoded;
#ifndef NO_FPU
                arm_pid_init_f32(&PIDY, 0);
#endif
                printf("Kd % f set!\n", valueDecoded);
                //                         printf("Kd %s given\n", &sval);
            }
            break;

            case W: {
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

    } else {

        /**
         * A trailing ! char in a new Rx frame indicates that a misc command
         * follows...
         */
        if (sUart[0] == '#') {   //!< True if signal generator command received

            /**
             * @par  Command frame layout
             *
             * Trailing \# indicates a command frame for signal generation
             *
             * Frame Layout:
             *    c     waveform
             *    v     freq
             *    d  dutycycle
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

            uint8_t NC = 3;      // index 0...3
            uint8_t NV = 4;      // index 6...9
            uint8_t ND = 2;      // index 10...11
            uint8_t NA = 16;     // index 12...15

            char sWav[NC + 1],      //!< string buffer to hold selected waveform as substring
            sFreq[NV + 1],     //!< string buffer to hold given frequency as substring
            sDuty[ND + 1],     //!< string buffer to hold given duty cycle as substring
            sAmp[16];          //!< string buffer to hold given amplitude as substring

            char *pDuty = sDuty;
            char *pFreq = sFreq;
            char *pAmp = sAmp;

            strncpy(sWav, &sUart[1], NC);      //!< waveform substring
            strncpy(sFreq, &sUart[6], 4);      //!< frequency substring
            strncpy(sDuty, &sUart[10], 2);      //!< duty cycle substring
            strncpy(sAmp, &sUart[12], 16);      //!< peak amplitude substring

            /**
             * Check for end of char coded float representation.
             * numeric char:            0...9
             * decimal delimiter:       . or ,  !!!Take caution for forbidden thousands separator!!!
             * exponental indicator:    e or E
             * signdness:               -
             */
            for (uint8_t k = 0; k < NC; k++) {   //!< check for end of value (padding) char ~
                if (*pCmd == RX_FRAME_PADDING_CHAR) break;
                pCmd++;
            }
            *(pCmd - 1) = '\0';   //!< place EOS marker
            pCmd = &sCmd[0];

            for (uint8_t k = 0; k < NV; k++) {   //!< check for end of command  indicator '='
                if (*pFreq == RX_FRAME_PADDING_CHAR) break;
                pFreq++;
            }
            *pFreq = '\0';     //!< place EOS marker
            pFreq = &sFreq[0];

            for (uint8_t k = 0; k < ND; k++) {   //!< check for end of command  indicator '='
                if (*pDuty == RX_FRAME_PADDING_CHAR) break;
                pDuty++;
            }
            *pDuty = '\0';     //!< place EOS marker
            pDuty = &sDuty[0];

            for (uint8_t k = 0; k <= NA; k++) {                 //!< Max NC numbers ...
                if (((*pAmp >= '0') && (*pAmp <= '9')) || (*pAmp == '.') || (*pAmp == ',')
                        || (*pAmp == 'e') || (*pAmp == 'E') || (*pAmp == '-')) {
                    pAmp++;
                } else
                    break;
            }

            *pAmp = '\0';   //!< place EOF marker
            pAmp = &sWav[0];

            choicew = NULL;
            /* parse sCmd buffer and enumerate the command/error */
            for (int i = 0; i < sizeof(itemsw_list) / sizeof(itemsw_t); i++) {
                if (strcasecmp(sWav, itemsw_list[i].name) == 0) {
                    choicew = itemsw_list + i;
                    break;
                }
            }

            wav = choicew ? choicew->idw : NN;
            //    /**
            //     * Check for invalied chars in command string
            //     *    Only [a-zA-Z] is threated as part of command string
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
            //            for(i = 0, choicew = NULL; i < sizeof itemsw_list/sizeof (struct itemsw_t); i++)
            //            {
            //                if (strcasecmp(sWav, itemsw_list[i].name) == 0)
            //                {
            //                    choicew = itemsw_list + i;
            //                    break;
            //                }
            //            }

            //            wav = choicew ? choicew->idw : NN;
            /* FIXME */
            volatile float valueDecoded __attribute__ ((unused)) = atof(pVal);

            g.waveForm = wav;
            g.freq = atoi(sFreq);
            g.duty = atoi(sDuty);
            g.ampl_f = (float) atof(sAmp);
            g.ampl = (int) g.ampl_f * 1000;

            //g.ampl_f = (float)g.ampl / (float)AMPL_FLOAT_DIVISOR;

            if (g.ampl_f > (float) AREF_IN_VOLT / 2) g.ampl_f = (float) AREF_IN_VOLT / 2;
            if (g.ampl_f < -(float) AREF_IN_VOLT / 2) g.ampl_f = -(float) AREF_IN_VOLT / 2;

            /**
             *  Calculate the interval for lookup table index increment
             *  based on SAMPLE_INTERVAL / TS, sizeof table and given
             *  waveform frequency.
             *
             *  g.freq      [Hz]
             *  TS          [us]    * 10^(-6)
             *
             *  g.lookAt = g.freq/(TABLESIZE * TS * 10^(-6))
             *           = g.freq * 10^4 / TS
             */

            g.tctr = g.lookAt;

            /**
             *  Precalculate a lookup table based on normed cosine array values.
             */
            for (uint16_t k = 0; k < rowCount_2D_array; k++) {
                calcTab[k] = (uint16_t) g.ampl * CosineN[k];
            };

            switch (wav) {
                case COS: {
                    printf("Cosine \n");
                    pSeq = &Cosine[0];
                    g.pBase = pSeq;
                }
                break;

                case TRIANG: {
                    printf("Triangle \n");
                    //  pSeq = &Triangle[0];
                    /* 450/g.freq --> 100 Timer Ovf / periode */
                    triStruct.bottom = ((float) VA_BIAS - g.ampl_f / 2);
                    triStruct.top = ((float) VA_BIAS + g.ampl_f / 2);
                    triStruct.derivate = g.ampl_f / (float) 100;
                    //  g.pBase = pSeq;
                    g.waveForm = TRIANG;
                    triStruct.triCtr = 0;
                    TM_Timer4_config(ENABLE, ENABLE, 450 / g.freq);
                }
                break;

                case SQUAREWAV: {
                    printf("Squarewave\n");
                    TM_Timer4_config(ENABLE, ENABLE, 45000 / g.freq);
                    g.setpointSrc = REMOTE_INTERNAL_MIXED;
                    g.waveForm = SQUAREWAV;
                    pSeq_f = &squarewaveBuff[0];
                }
                break;

                case QUADRATIC: {
                    printf("Quadratic\n");
                    pSeq = &Quadratic[0];
                }
                break;

                case I_QUADRATIC: {
                    printf("Inv Quadratic\n");
                    pSeq = &I_Quadratic[0];
                }
                break;

                case SAWTOOTH: {
                    printf("Sawtooth\n");
                    pSeq = &Sawtooth[0];
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

                case CMD_TESTVECT_1: {
                    rowCount_2D_array = sizeof(line1) / sizeof(line1[0]);
                    printf("CMD_TESTVECT_1\n");
                    dir = 1;
                    g.waveForm = CMD_TESTVECT_1;
                    TM_Timer4_config(ENABLE, ENABLE, 45000 / g.freq);
                }
                break;

                case CMD_TESTVECT_2: {
                    rowCount_2D_array = sizeof(line2) / sizeof(line2[0]);
                    printf("CMD_TESTVECT_2\n");
                    dir = 1;
                    g.waveForm = CMD_TESTVECT_2;
                    TM_Timer4_config(ENABLE, ENABLE, 45000 / g.freq);
                }
                break;

                case CMD_MATLAB_LINK: {
                    /**
                     * TDOD:    Implement MATLAB link-up!
                     *
                     * \#mat:=1~~~1~1.6635~~~..  vector #1, beam on,  xpos=1.6635
                     * \#mat:=2~~~0~0.2635~~~..  vector #2, beam off, xpos=0.6635
                     * \#mat:=3~~~1~-1.663~~~..  vector #3, beam on,  xpos=-1.663
                     */
                }
                break;

                /**
                 * Disable waveform generation if wgm-off command received or if
                 * command is unknown.
                 */
                default: {
                    printf("unknown choice\n");
                    g.waveForm = NN;
                    g.duty = 999;
                    g.freq = 999;
                    g.lookAt = 999;
                    g.pBase = pSeq;
                    g.tctr = 999;
                    g.gen = DIS;
                }
            }

            /**
             * Enable generator.
             */
            if ((wav == COS) || (wav == TRIANG) || (wav == SQUAREWAV) || (wav == QUADRATIC)
                    || (wav == I_QUADRATIC) || (wav == SAWTOOTH)) g.gen = EN;
        } else {                   //!< True if a misc command received
            /**
             * Trailing ! indicates that a misc command frame was received
             *
             * Frame Layout:
             *  c   command
             *
             * ! c c c c c c c c c c  c  c
             * 0 1 2 3 4 5 6 7 8 9 10 11 12
             *
             */
#define MISC_CMD_LENGTH     35  // + trailing !
            MiscItem_t misc;
            MiscCmds_t *choicem = NULL;

            //!< string buffer to hold given command substring
            char sMisc[MISC_CMD_LENGTH + 1];
            char *pMisc = sMisc;

            strncpy(sMisc, &sUart[1], MISC_CMD_LENGTH);   //!< command substring

            /**< Check for valied characters and substitute first unvalied char
             * by EOS char
             */

            while (((*pMisc >= 'A') && (*pMisc <= 'Z')) || ((*pMisc >= 'a') && (*pMisc <= 'z'))
                    || ((*pMisc >= '0') && (*pMisc <= '9')) || ((*pMisc == '-') || (*pMisc == '_'))
                    || ((*pMisc == '.') || (*pMisc == ',')) || ((*pMisc == 'e') || (*pMisc == 'E'))) {

                pMisc++;
            }

            *pMisc = '\0';              //!< End of string

            choicem = NULL;
            for (int i = 0; i < sizeof itemsm_list / sizeof(MiscCmds_t); i++) {
                if (strcasecmp(sMisc, itemsm_list[i].name) == 0) {
                    choicem = itemsm_list + i;
                    break;
                }
            }

            misc = choicem ? choicem->idm : misc_NN;

            g.miscReq = misc;

            switch (misc) {
                case misc_asgOnOff_cmd:
                    break;
                case misc_asgUpperLim_cmd:
                    break;
                case misc_asgLowerLim_cmd:
                    break;
                case misc_asgTrippTime_cmd:
                    break;
                case misc_asgSaveVal_cmd:
                    break;

                case misc_pid_init:
                    //                    pid_Init(KP_INIT, KI_INIT, KD_INIT, TF_INIT, TS, &pidDataY, NONBUFFERED);
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
        memset(&sUart, 0, UART_BUFF_SIZE * sizeof(char));
    }

}
#if  defined (KEIL_IDE)
#pragma O2
#elif  defined (__GNUC__)
#pragma GCC pop_options
#endif


// #ifdef MULTI_SETPOINT
//     if (g.setpointSrc == INTERNAL_SETPOINT) {
//         W_now = pidDataY.W_int;
//     }  else {
//     if (g.setpointSrc == ANALOG_SETPOINT) {
//         W_now = ADC_fBuff[INDEX_Wx];
//     }  else {
//     if (g.setpointSrc == REMOTE_SETPOINT) {
//         W_now = pidDataY.W_rem;
//     }  else {
//     if (g.setpointSrc == REMOTE_OPENLOOP) {
//         W_now = pidDataY.W_rem;
//     }}}}
// #endif

//    gpio_init_mco1();
//    RCC_MCO1Config(RCC_MCO1Source_HSE,RCC_MCO1Div_1);
//    gpio_init();

// ==============================================================
//     IRQ callback:    Timer 5 overrun     Heartbeat timer
// ==============================================================
// void TIM5_IRQHandler(void) {
//    if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET) {
//      TIM_ClearITPendingBit(TIM5, TIM_IT_Update);

//    }
// }
// ==============================================================
//     IRQ callback:    Timer 4 overrun     Waveform Generator
// ==============================================================
// #ifdef NO1
// void TIM4_IRQ_Handler(void) {
//    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) {
//      TIM_ClearITPendingBit(TIM4, TIM_IT_Update);

//        if (ASG.tripped) {
//            MDB_GPIO_Toggle(BEAM_INTERRUPT);
//        }

//         Check which waveform mode is active 
//        if (g.waveForm == SQUAREWAV) {
//            if (*pSeq_f == '\0') {
//                pSeq_f = &squarewaveBuff[0];
//                pInt = &interrupterBuff[0];
//                MDB_GPIO_Toggle(TRIGGER_SRC);
//            }
//            setpoint_tgl = (float)*pSeq_f++ * g.ampl_f;
//            beamCtrl(BEAM_CTRL_SOURCE_MANUAL, (*pInt++) ? GPIO_ON : GPIO_OFF );
//            return;
//        }

//        if (g.waveForm == TRIANG) {
//            if (setpoint_tgl  <= triStruct.top) {
//                setpoint_tgl += triStruct.derivate;
//            }
//            else {
//                setpoint_tgl = triStruct.bottom;
//            }
//        }
//    }
// }
// #else
// void TIM4_IRQ_Handler(void) {
// fff
// volatile float ft=0;

//    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) {
//      TIM_ClearITPendingBit(TIM4, TIM_IT_Update);

//        /* Check which waveform mode is active 
//        if (g.waveForm == SQUAREWAV) {
//            if (vectorCtr >= NVECTORS) {
//                vectorCtr = 0;
//            }
//            setpoint_tgl = (float)line1[vectorCtr][1] * g.ampl_f;
//            if (line1[vectorCtr++][0] == 0)
//                beamCtrl(BEAM_CTRL_SOURCE_MANUAL, GPIO_OFF );
//            else
//                beamCtrl(BEAM_CTRL_SOURCE_MANUAL, GPIO_ON );

//        }
//    }
// }
// #endif

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
//        ADC_fBuff[k] = (float) ((float) (ADC_MultiConvBuff[k] - AN_BIAS_INT) * VLSB);
//    }

//// ==============================================================
////            Laser interrupter control
//// ==============================================================
///* To implement a blanking function controled by setpoint vectors,
// * simply add a blanking information column to the setpoint datastream
// *
// * [x-coordinate; y-coordinate; blankingBit]
// */

//// ==============================================================
////            Setpoint source selector
//// ==============================================================
//    while (1) {
//        if (g.setpointSrc == INTERNAL_SETPOINT) {   //!< Internal only, no remote offset or something else
//            pidErr_x =  setpoint_tgl - POS_X_FLOAT;
//            pidErr_y =  setpoint_tgl - POS_Y_FLOAT;
//        }
//        if (g.setpointSrc == ANALOG_SETPOINT) {     //!< setpoint is sourced by analog input
//                // flot conversion!
//        }
//        if (g.setpointSrc == REMOTE_SETPOINT) {     //!< Remote only, no internal signal or offset
//            pidErr_x =  pidDataX.W_remf - POS_X_FLOAT;
//            pidErr_y =  pidDataY.W_remf - POS_Y_FLOAT;
//        }
//        if (g.setpointSrc == REMOTE_OPENLOOP) {     //!< PID bypassed, remote setpoint feed throgh
//            updateActuator_f( /*pidDataY.W_remf*/pidDataY.W_remf, pidDataY.W_remf);
//            break;
//        }
//        if (g.setpointSrc == REMOTE_INTERNAL_MIXED) {     //!< Mixed mode, summing internal + external setpoint
//            pidErr_x = ((float)(setpoint_tgl + pidDataX.W_remf)) - POS_X_FLOAT;
//            pidErr_y = ((float)(setpoint_tgl + pidDataY.W_remf)) - POS_Y_FLOAT;
//        }
//        if (  (g.setpointSrc != INTERNAL_SETPOINT) && (g.setpointSrc != ANALOG_SETPOINT)  && (g.setpointSrc != REMOTE_OPENLOOP)   && (g.setpointSrc == REMOTE_OPENLOOP) ) {
//             /* Error state, halt */
//             MD_DISCO_LedOn(LED_GREEN);
//             MD_DISCO_LedOn(LED_RED);
//        }
//
//        /**< Calculate PID here */
//        DBG_PID_TIMING_TOG();
//        updateActuator_f(   arm_pid_f32(&PIDY, pidErr_y), arm_pid_f32(&PIDY, pidErr_y));
//        DBG_PID_TIMING_TOG();

//        break;
//    }

//    /* Set LEDs according to which angular position is greater */
////    if ( POS_Y_FLOAT > SETPOINT_Y_FLOAT) {
////        MD_DISCO_LedOn(LED_GREEN);
////        MD_DISCO_LedOff(LED_RED);
////    } else if ( POS_Y_FLOAT <  SETPOINT_Y_FLOAT) {
////        MD_DISCO_LedOn(LED_RED);
////        MD_DISCO_LedOff(LED_GREEN);
////    } else {
////        MD_DISCO_LedOff(LED_ALL);
////    }
//
//}

//// ==============================================================
////     IRQ callback:    ADC end-of-conversion
////                      Analog Watchdog IRQ
//// ==============================================================
//void ADC_IRQ_Handler(void) {
//    if (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == SET) {
//        ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
//        /* End of conversion interrupt occured */

////        /* Error state */
////        MD_DISCO_LedOn(LED_ALL);
////        uint16_t W_now = ADC_MultiConvBuff[1];   // setpoint (Isens_y)
//        while (1);;
////        return;
//    }
//
//
//    /* Analog watchdog interrupt occured */
//    if (ADC_GetFlagStatus(ADC1, ADC_FLAG_AWD) == SET) {
//        ADC_ClearITPendingBit(ADC1, ADC_IT_AWD);
//        if ((ASG.state == ASG_STATIONARY_INTEGRATOR) ||
//            (ASG.state == ASG_DISCHARGING_INTEGRATOR)) {
//            ASG.state = ASG_CHARGING_INTEGRATOR;
//            printf("ASS:charging_start...\n");
//        }
//        if ((ASG.integrator <= ASG.upperThreshold) || (ASG.integrator >= ASG.upperThreshold)) {   /**< check for integrator limit */
//            if (! ASG.tripped) {
//                ASG.tripped = 1;                    /**< Set "tripped" state if it is so */
//                DAC_SecureSetDualChanSigned = &DAC_SetDualChanSigned_Tripped;  /**< set function pointer to the "ASG tripped" handler*/
//            }
//            return;
//        }
//        else {                                  /**< else increment integrator */
//            ASG.integrator+= ASG.upperThreshold * (double)TS*1e-6/(ASG.tripTime);
//        }
//        return;
//    }
//    printf("bad ADC IntReq source\n");
//}
//

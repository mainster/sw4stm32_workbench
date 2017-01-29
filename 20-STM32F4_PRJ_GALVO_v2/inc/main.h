/**
 * @file        main.h
 *
 * @date        29 Apr 2015
 * @author      Manuel Del Basso (mainster)
 * @email       manuel.delbasso@gmail.com
 *
 * @ide         System Workbench ac6 (eclipse stm32)
 * @stdperiph   STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 *
 * @brief       Main header file
 *
   @verbatim

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

   @endverbatim
 *
 */
#ifndef _MAIN_H_
#define _MAIN_H_

// /* Library include */
// /* Core include */
// /* Project specific include */
//  FIXME:	Shouldn't be necessary at this point
#include "defines.h"
#include "attributes.h"
#include "mdb_gpio.h"
#include "pid.h"
#include "arm_math.h"
#include "core_cm4.h"
#include "md_stm32f4_delay.h"
#include "tm_stm32f4_pwm.h"
#include "tm_stm32f4_usart.h"
 #include "md_stm32f4_disco.h"
 #include "md_stm32f4_timer.h"
// #include "md_stm32f4_dac.h"
// #include "md_stm32f4_dac_waveform.h"
// #include "stm32f4xx.h"
// #include "stm32f4xx_adc.h"
 #include "actuators.h"
// #include "adc_dac_dma.h"
// #include "arm_architect.h"
// #include "arm_math.h"
// #include "isr_callbacks.h"
// #include "pid.h"
// #include "signalGen.h"
// #include "tools.h"

/**
 * @addtogroup MD_APP
 * @{
 */
/**
 * @addtogroup MD_APP_Main
 * @{
 */

//external __IO int16_t ADC_MultiConvBuff[5];
//external __IO float ADC_fBuff[5];

/**
 * @addtogroup APP_Main_Macros
 * @{
 */

#define POS_X_FLOAT         ADC_fBuff[INDEX_Px]         /* actual position */
#define POS_Y_FLOAT         ADC_fBuff[INDEX_Py]         /* actual position */
#define SETPOINT_Y_FLOAT    ADC_fBuff[INDEX_Wx]         /* position we want to have */


#define pidErr_x    pidErrBuff[0]
#define pidErr_y    pidErrBuff[1]
#define toPlant_x   toPlantBuff[0]
#define toPlant_y   toPlantBuff[1]

/** @} */

/**
 * @addtogroup MD_APP_Main_Typedefs
 * @{
 */

/**
 * @brief      { item_description }
 */

typedef enum {
    _FLOAT_,
    _INT_,
    _UINT_
} structType;



/* ================================================================================ */
enum cmd_items { 
    KP, KI, KD, W, UNKNOWN
};      //!< Enumeration for switch-by-string casees

//Struct for switch by string casees
struct items {
    char *name;
    enum cmd_items id;
};



//struct A {
//    int a;
//    int b;
//};
//
//struct B {
//    struct A a;
//    int b;
//};
//
//static struct B a = {{5,3}, 2};
// ==============================================================
//            wave command items struct
// ==============================================================
typedef enum {                            //!< Enumeration for switch-by-string casees 
    NN,                                         //!< unknown wave form received     
    EN,                                         //!< Generator enable
    DIS,                                        //!< Generator disable
    COS, TRIANG, SQUAREWAV,             //!< Usable waveforms
    QUADRATIC, I_QUADRATIC, SAWTOOTH,   
    CMD_REMOTE_SETPOINT,
    CMD_OPENLOOP_REMOTE,
    CMD_ANALOG_SETPOINT,
    CMD_INTERNAL_SETPOINT,
    CMD_REFRESH_RATE,
    CMD_TESTVECT,

    /* CMD_MATLAB_LINK
     *
     * # c c c : = F F F F  d  d  A ..16x.. A
     * #mat:=25~~0~
     *
     */
    CMD_MATLAB_LINK
    /**< Quadratic: testsignal zum vergleich mit ݢersichtsblatt "Was f𲠍
      Regler notwendig um quadratischen sollwertverl嶦en noch...."
      (siehe Vorlesung H. Brunner) 
     */
} wav_items_t;

/* ================================================================================ */

/**
 * @brief      Lookup table to provide switch-by-string functionality.
 *
 *             Structure holds a string and id value to provide simple
 *             switch-by-string functionality
 */
typedef struct {
    char *name;
    wav_items_t idw;
} itemsw_t;


/**
 * @brief      MISC commands enumerations for switch-by-string identifiers.
 */
typedef enum { 
    misc_NN,
    misc_none,
    misc_pid_init, 
    misc_pid_Controller,
    misc_pid_Reset_Integrator,
    misc_update_pid_data,
    misc_assOnOff_cmd,          //!< @enum  AutoSafetyShutdown Enable/Disable cmd
    misc_assUpperLim_cmd,       //!< @enum  AutoSafetyShutdown set Upper Limit command
    misc_assLowerLim_cmd,       //!< @enum  AutoSafetyShutdown set Lower Limit command
    misc_assTrippTime_cmd,      //!< @enum  AutoSafetyShutdown set Tripping time command
    misc_assSaveVal_cmd,        //!< @enum  AutoSafetyShutdown set Safe value command
    misc_beamOn_cmd,            //!< @enum  Beam enable disable command enumeration
    misc_beamOff_cmd,           //!< @enum  Beam enable disable command enumeration
} misc_items_t; 


struct itemsm
{
    char *name;
     misc_items_t idm;
};

/* ================================================================================ */
typedef struct {
    int16_t lowerLim;
    int16_t upperLim;
} dac_lim_t;

/* ================================================================================ */
////!< typedef for different setpoint source select
//typedef enum {
//    INTERNAL_SETPOINT,          //!< Setpoint signal generated by software...
//    ANALOG_SETPOINT,            //!< Setpoint by analog waveform, converted by I_L adc line yet
//    REMOTE_SETPOINT,            //!< Setpoint commands/Signal over serial interface, NO internal setpoint sources
//    REMOTE_OPENLOOP,            //!< No active PID - Feed remote setpoint to hardware
//    REMOTE_INTERNAL_MIXED       //!< Iternal setpoint + remote setpoint
//} setpoint_src_t;
//!< typedef for different setpoint source select

#if ! (defined (KEIL_IDE) || defined (__GNUC__))
    #error "Please specify an IDE or GNUC tools!"
#endif

#if  defined (KEIL_IDE)
    #pragma O0
#elif  defined (__GNUC__)
    #pragma GCC push_options
    #pragma GCC optimize ("O0")
#endif

enum setpoint_src_t{
    INTERNAL_SETPOINT,          //!< Setpoint signal generated by software...
    ANALOG_SETPOINT,            //!< Setpoint by analog waveform, converted by I_L adc line yet
    REMOTE_SETPOINT,            //!< Setpoint commands/Signal over serial interface, NO internal setpoint sources
    REMOTE_OPENLOOP,            //!< No active PID - Feed remote setpoint to hardware
    REMOTE_INTERNAL_MIXED       //!< Iternal setpoint + remote setpoint
};



// ==============================================================
//            Global state struct
// ==============================================================
struct global {
    wav_items_t waveForm; //!< Holds enum for selected wave form.
    wav_items_t gen;      //!< This is the signal generator enable/disable param.

    uint32_t    lookAt;         /**< Integer multiple of @SAMPLE_TIME. USe it as interval 
                                        counter for lookup table index increments. */
    uint16_t    freq;           //!< Frequency for waveform generation [kHz].
    uint16_t    tctr;           //!< lookup sequence counter.
    uint16_t   ampl;           //!< Holds the given value of peak amplitude.
    int16_t *pBase;         //!< Base address holder for look up ptr.
    uint16_t    duty;           //!< Duty parameter (uint16_t).
    float       dutyCyc;        //!< Duty cycle for waveform generation [%].    
    dac_lim_t   dacHw[2];       //!< DAC1/DAC2 range values to implement hardware protection 
    enum    setpoint_src_t     setpointSrc;
    misc_items_t miscReq; /**< Holds the misc command requested by serial interface
     * @NOTE   clear this member to misc_none at the end of the misc cmd */
    uint16_t    refresh;       //!< refresh rate in ms for mainloop
    float        ampl_f;        //!< float amplitude for generated waveform
    MDB_GPIO_STATE_t      beamEnabled;    //!< On Off Flag for Laser beam
};  

//struct triangWave {
//    uint16_t    triCtr;
//    float    bottom;     //!< (top-bottom)*n Timer ovf's => Amplitude     
//    float    top;
//    float    derivate;   //!< derivative to increase V-DAC by Amplitude after n Timer Ovf's
//};




/** @} *///!< MD_APP_Main_Typedefs

/* ================================================================================ */


/**
 * @addtogroup MD_APP_Main_Variables
 * @{
 */
extern struct global g;
//extern autoSaveSystem_t ass;
extern struct triangWave triStruct;

extern float setpoint_tgl;
extern float pidErrBuff[2];        /* PID error buffer*/
extern float toPlantBuff[2];        /* PID output buffer*/

/** @} *///!< MD_APP_Main_Typedefs

#if  defined (KEIL_IDE)
    #pragma O2
#elif  defined (__GNUC__)
    #pragma GCC pop_options
#endif

/** @} */   /* MD_APP_Main */
/** @} */   /* MD_APP */

#endif

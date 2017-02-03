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
 * @brief       Main header definition.
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

#ifdef __cplusplus
 extern "C" {
#endif 
    
/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint-gcc.h>
#include "attributes.h"
#include "globals.h"

/* Library includes */
#include "md_stm32f4_delay.h"
#include "md_stm32f4_disco.h"
#include "md_stm32f4_timer.h"
#include "tm_stm32f4_pwm.h"
#include "tm_stm32f4_usart.h"

/* Core includes */
#include "core_cm4.h"
#include "arm_math.h"

/* Specific includes */
#include "actuators.h"
#include "adc_dac_dma.h"
#include "periph2dma_conf.h"
#include "arm_math.h"
#include "isr_callbacks.h"
#include "mdb_gpio.h"
#include "pid.h"
#include "pid.h"
#include "signalGen.h"
#include "helper.h"

/**
 * @addtogroup MD_APP Application
 * @{
 */

/**
 * @addtogroup APP_Main Main
 * @{
 */

/**
 * @brief      Check compiler selection flags.
 */
#if ! (defined (KEIL_IDE) || defined (__GNUC__))
#error "Please specify an IDE or GNUC tools!"
#endif

#if  defined (KEIL_IDE)
#pragma O0
#elif  defined (__GNUC__)
#pragma GCC push_options
#pragma GCC optimize ("O0")
#endif

/* Compiler code optimization disabled */

#if  defined (KEIL_IDE)
#pragma O2
#elif  defined (__GNUC__)
#pragma GCC pop_options
#endif


/**
 * @addtogroup Main_Macros
 * @{
 */

/**
 * @brief      Atomic read access macro for "actual x-position".
 */
#define POS_X_FLOAT         ADC_fBuff [INDEX_Px]

/**
 * @brief      Atomic read access macro for "actual y-position".
 */
#define POS_Y_FLOAT         ADC_fBuff [INDEX_Py]

/**
 * @brief      Atomic read access macro for "analog set-point position".
 */
#define SETPOINT_Y_FLOAT    ADC_fBuff [INDEX_Wx]

/**
 * @brief      Atomic access macro for actual channel-x error signal.
 */
#define pidErr_x            pidErrBuff [0]

/**
 * @brief      Atomic access macro for actual channel-y error signal.
 */
#define pidErr_y            pidErrBuff [1]

/**
 * @brief      Atomic access macro for actual channel-x compensator output signal.
 */
#define toPlant_x           toPlantBuff [0]

/**
 * @brief      Atomic access macro for actual channel-y compensator output signal.
 */
#define toPlant_y           toPlantBuff [1]

/** @} */

/**
 * @addtogroup Main_Typedefs
 * @{
 */

/**
 * @brief      Direct compensator terminal command enumeration.
 *
 *             Update compensator parameters and set-point values via remote
 *             terminal.
 */
typedef enum {
    KP,             //!< Set proportional gain.
    KI,             //!< Set integral gain.
    KD,             //!< Set derivative gain.
    W,              //!< Set set-point.
    UNKNOWN         //!< Unknown command received.
} CmdItem_t;

/**
 * @brief      Data type to link between command strings and an enumerated, and
 *             therefore integral type, command identifier.
 */
typedef struct {
    char *name;     //!< Remote command string buffer.
    CmdItem_t id;   //!< Corresponding command identifier. .
} items_t;

/**
 * @defgroup   testSig Test Signals
 *
 * Generate Test signals for comparison with lecture stuff (survey sheet).
 * "What kind of controller is required for quadratic setpoint ...". (See
 * lecture HsKA, RT2, H. Brunner)
 */

/**
 * @brief      Waveform and set-point source selector command item enumeration.
 *
 *             Identifiers for different waveforms and commands for switching
 *             set-point source.
 */
typedef enum {
    NN,                     //!< Unknown remote frame received.
    EN,                     //!< Generator enable.
    DIS,                    //!< Generator disable.
    COS,                    //!< Generate waveform "Cosine"     @ingroup testSig.
    TRIANG,                 //!< Generate waveform "Triangle"   @ingroup testSig.
    SQUAREWAV,              //!< Generate waveform "Squarewave" @ingroup testSig.
    SAWTOOTH,               //!< Generate waveform "Sawtooth"   @ingroup testSig.
    QUADRATIC,              //!< Generate waveform "Quadratic"  @ingroup testSig.
    I_QUADRATIC,            //!< Generate waveform "Inverse Quadratic" @ingroup testSig.
    CMD_REMOTE_SETPOINT,    //!< Switch set-point source to "Remote".
    CMD_OPENLOOP_REMOTE,    //!< Switch set-point source to "Openloop remote".
    CMD_ANALOG_SETPOINT,    //!< Switch set-point source to "Analog".
    CMD_INTERNAL_SETPOINT,  //!< Switch set-point source to "Internal".
    CMD_TESTVECT_1,         //!< Switch set-point source to "Test vector sequence 1" @ingroup testSig.
    CMD_TESTVECT_2,         //!< Switch set-point source to "Test vector sequence 2" @ingroup testSig.
    CMD_REFRESH_RATE,       //!< Set set-point refresh rate

    /**
     * @brief      Enter MATLAB link-up.
     *
     *             When linked to MATLAB interface, another set-point command
     *             data format is used.
     *
     * @note       About frame layout
     *
     *             #mat:=1~~~1~1.6635~~~..  vector #1, beam on, xpos=1.6635
     *             #mat:=2~~~0~0.2635~~~..  vector #2, beam off, xpos=0.6635
     *             #mat:=3~~~1~-1.663~~~..  vector #3, beam on, xpos=-1.663
     */
    CMD_MATLAB_LINK
} WavItems_t;


/**
 * @brief      Data type to link between waveform selection strings and an
 *             enumerated, and therefore integral type, waveform selection
 *             identifier.
 */
typedef struct {
    char *name;                 //!< Remote waveform selection string buffer.
    WavItems_t idw;             //!< Corresponding waveform selection identifier.
} itemsw_t;


/**
 * @brief      Miscellaneous (MISC) command item enumeration.
 *
 *             Different remote commandment identifiers are declared here.
 */
typedef enum {
    misc_NN,                    //!< Unknown remote frame received.
    misc_none,                  //!< Used to acknowledge after misc command has been processed
    misc_pid_init,              //!< (Re) Initialize pid instances.
    misc_pid_Controller,        //!< For later use, feature is in progress.
    misc_pid_Reset_Integrator,  //!< Reset integration buffer of pid instances.  
    misc_update_pid_data,       //!< For later use, feature is in progress.
    misc_asgOnOff_cmd,          //!< AutoSafetyShutdown Enable/Disable cmd.
    misc_asgUpperLim_cmd,       //!< AutoSafetyShutdown set Upper Limit command.
    misc_asgLowerLim_cmd,       //!< AutoSafetyShutdown set Lower Limit command.
    misc_asgTrippTime_cmd,      //!< AutoSafetyShutdown set Tripping time command.
    misc_asgSaveVal_cmd,        //!< AutoSafetyShutdown set Safe value command.
    misc_beamOn_cmd,            //!< Beam enable disable command enumeration.
    misc_beamOff_cmd,           //!< Beam enable disable command enumeration.
} MiscItem_t;

/**
 * @brief      Data type to link between miscellaneous (Misc) selection strings
 *             and an enumerated, and therefore integral type, waveform
 *             selection identifier.
 */
typedef struct {
    char *name;                 //!< Remote misc command string buffer.
    MiscItem_t idm;             //!< Corresponding misc command identifier.
} MiscCmds_t;

/**
 * @brief      DAC output register value range data type declaration.
 */
typedef struct {
    int16_t lowerLim;           //!< Lower DAC output register limit.
    int16_t upperLim;           //!< Upper DAC output register limit.
} DacLimit_t;

/**
 * @brief      Set-point source selector enumeration.
 */
typedef enum {
    INTERNAL_SETPOINT,          //!< Setpoint signal generated by software...
    ANALOG_SETPOINT,            //!< Setpoint by analog waveform, converted by I_L adc line yet.
    REMOTE_SETPOINT,            //!< Setpoint commands/Signal over serial interface, NO internal setpoint sources.
    REMOTE_OPENLOOP,            //!< No active PID - Feed remote setpoint to hardware.
    REMOTE_INTERNAL_MIXED       //!< Iternal setpoint + remote setpoint.
} SetPointSrc_t;

/**
 * @brief      Global state structure definition.
 */
typedef struct {
    /**
     * @brief      Integer multiple of SAMPLE_TIME.
     * @details    Use it as interval counter for lookup table index increments.
     */
    uint32_t       lookAt;

    /**
     * @brief      Holds the misc command requested by serial interface.
     * @details    clear this member to misc_none at the end of the misc cmd.
     */
    MiscItem_t      miscReq;
    WavItems_t      waveForm;       //!< Holds enum for selected wave form.
    WavItems_t      gen;            //!< This is the signal generator enable/disable param.
    uint16_t        freq;           //!< Frequency for waveform generation [kHz].
    uint16_t        tctr;           //!< lookup sequence counter.
    uint16_t        ampl;           //!< Holds the given value of peak amplitude.
    int16_t         *pBase;         //!< Base address holder for look up ptr.
    uint16_t        duty;           //!< Duty parameter (uint16_t).
    float           dutyCyc;        //!< Duty cycle for waveform generation [%].
    DacLimit_t      dacHw[2];       //!< DAC1/DAC2 range limits. Used to implement hardware protection.
    SetPointSrc_t   setpointSrc;    //!< Holds the actual set-point source to use.
    uint16_t        refresh;        //!< refresh rate in ms for mainloop.
    float           ampl_f;         //!< float amplitude for generated waveform.
    MD_GPIO_STATE_t beamEnabled;    //!< On Off Flag for Laser beam.
} global_t;

/** @} */

/**
 * @addtogroup Main_Variables
 * @{
 */
extern global_t g;
extern struct triangWave triStruct;
extern float setpoint_tgl;
extern float pidErrBuff[2];         /* PID error buffer*/
extern float toPlantBuff[2];        /* PID output buffer*/

/** @} */

/**
 * @addtogroup Main_Functions
 * @{
 */

/**
 * @brief      Set initial values (POR state) of the ASG system and the global
 *             state structure members.
 */
void init_globalStructs(void);

/** @} */

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

#endif

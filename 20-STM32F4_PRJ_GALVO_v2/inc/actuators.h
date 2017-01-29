/**
 * @file        actuators.h
 *
 * @date        29 Apr 2015
 * @author      Manuel Del Basso (mainster)
 * @email       manuel.delbasso@gmail.com
 *
 * @ide         System Workbench ac6 (eclipse stm32)
 * @stdperiph   STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 *
 * @brief       Controlling beam and primary actuators.
 *
 * @verbatim

	------------------------------------------------------------------------

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

	------------------------------------------------------------------------

 * @endverbatim
 *
 */
#ifndef _ACTUATORS_H_
#define _ACTUATORS_H_
 
#include <stdio.h>
#include "adc_dac_dma.h"
#include "defines.h"
#include "isr_callbacks.h"
//#include "md_stm32f4_disco.h"
#include "mdb_gpio.h"
#include "pid.h"
//#include "tm_stm32f4_usart.h"
#include "tools.h"

/**
 * @addtogroup MD_APP
 * @{
 */

/**
 * @addtogroup  APP_Actuators
 * @brief       Provides actuator related update and configuration methods. 
 * 
 * @par Refreshable actuators:
 * 		- Variable current sources (channels X,Y) to drive the Galvo coils.
 * 		- Laseer beam 
 * @{
 */

/**
 * @addtogroup  APP_Actuators_Macros 
 * @brief       Actuator methods related macro definitions. @{
 */

/**
 * @brief      The Auto Shutdown System provides a security layer 
 */
#define ASS_TRIPPING_LOWER_DEFAULT    	-1.22f
#define ASS_TRIPPING_UPPER_DEFAULT    	1.22f
#define ASS_SAFEVALUE_DEFAULT        	0
#define ASS_TRIPPING_TIME_DEFAULT   	0.750f      // s

/** @} */

/**
 * @addtogroup  APP_Actuators_Typedefs 
 * @brief       Actuator methods related type definitions. @{
 */

/**
 * @brief		Beam control source types enumerations.
 */
typedef enum {
    BEAM_CTRL_SOURCE_GLOBAL,	//!< BEAM_CTRL_SOURCE_GLOBAL
    BEAM_CTRL_SOURCE_MANUAL 	//!< BEAM_CTRL_SOURCE_MANUAL
} beamCtrlSource_t;

/**
 * @brief      Integrator state enumerations.
 *
 *             To manipulate the fuse tripping transfer function (fuse
 *             characteristics) of the ASS, the process output to be secured, is
 *             feded to an integrator which have to be controlled.
 */
typedef enum {
    ASS_CHARGING_INTEGRATOR,   //!< ASS_CHARGING_INTEGRATOR
    ASS_DISCHARGING_INTEGRATOR,//!< ASS_DISCHARGING_INTEGRATOR
    ASS_STATIONARY_INTEGRATOR  //!< ASS_STATIONARY_INTEGRATOR
} ass_int_state_t;


/**
 * @brief      The Auto Safestate System provides a security layer to prevent
 *             thermal overloads.   
 */
typedef struct {
    double  	integrator;  	//!< runtime integrator - holds the "amount" of overcurrent - states
    float   	lowerVal;    	//!< [V] Lower value ADC_f less than this value triggers the ass*/
    float   	safeVal;     	//!< [V] If asg tripped, this output value becomes active*/
    float   	tripTime;    	//!< [ms] Time constant, used to change tripping characteristics at runtime
    float   	upperVal;    	//!< [V] Upper value ADC_f greater than this value triggers the ass*/
    uint8_t 	ack;        	//!< acknowlage tripped stat
    uint8_t 	tripped;    	//!< tripped != 0 if tripping is active
    ass_int_state_t state;
} autoSaveSystem_t;

/** @} */

/**
 * @addtogroup  APP_Actuators_Functions 
 * @brief       Actuator function definitions. @{
 */

/**
 * @addtogroup  APP_Guarding
 * @brief       This group includes all functions related to safety and
 *              guarding. @{
 */

/**
 * @brief      Safely update actuator control signals. Sicherer z
 *
 *             Safely means in terms of missplaced or oscillating controller
 *             outputs. The analog watchdog peripheral takes care about
 *             "out of normal range" events. If a converted position signal is
 *             out of range, the watchdog handler starts time integration and
 *             after "out of range" integrator reaches a defineable limit, the
 *             watchdog takes the system to into "Tripped" state. Therefor a
 *             global error flag becomes true AND the function pointer
 *             DAC_SetDualChanSigned() that points to a wraper function, gots to
 *             be replaced by a pointer that points to
 *             DAC_SetDualChanSigned_Tripped(). In the DAC_SetDualChanSigned()
 *             function, that is pointed to in tripped state, only outputs a
 *             zero level DAC-Signal to take external hardware in a safe state.
 *
 * @param[in]  I_set_x   Pass current setpoint for x-channel.
 * @param[in]  I_set_y   Pass current setpoint for y-channel.
 *
 * @return     Is Zero.
 */
int updateActuator_f(float I_set_x, float I_set_y);

/**
 * @brief      Set beam control source and new state
 *
 * @param[in]  src        The beam control source 
 * @param[in]  newState   The new state
 *
 * @return     { description_of_the_return_value }
 */
int beamCtrl(beamCtrlSource_t src, tribool_state_t newState );

/** @} */

/** @} */

/** @} */

#endif


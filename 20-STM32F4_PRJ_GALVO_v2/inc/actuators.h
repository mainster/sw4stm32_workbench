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

 * @endverbatim
 *
 */
#ifndef _ACTUATORS_H_
#define _ACTUATORS_H_

#ifdef __cplusplus
   extern "C" {
#endif 
    
#include <stdio.h>
#include "adc_dac_dma.h"
#include "globals.h"
#include "irq_callbacks.h"
#include "mdb_gpio.h"
#include "pid.h"
#include "helper.h"

/**
 * @addtogroup MD_APP
 * @{
 */

/**
 * @addtogroup  APP_Actuators Actuators
 * @brief       Provides actuator related update and configuration methods.
 * @{
 *
 * @par Refreshable actuators:
 * @li          Variable current sources to drive the Galvo coils.
 * @li          Laser-beam interrupter 
 */

/**
 * @defgroup   ASG Actuator Safestate Guard (ASG)
 * @brief      The ASG system is designed to protect the output drivers from
 *             overload in case of actuator blocking states
 *
 *             The ASG system is designed to protect the actuators and theire
 *             current drivers from overload in case of actuator blocking
 *             states. The prototype electronics also includes coil current
 *             measurement hardware to provide actuator current feedback. In a
 *             project milestone to be planned for later, the implementation of
 *             a model-driven compensator (state space model) is being
 *             considered.
 */

/**
 * @addtogroup  ASG
 * @{
 */

/**
 * @brief      Lower ADC threshold (POR value). @SI{V}
 *
 *             ADC values across this limit are captured from the ASG.
 */
#define ASG_LOWER_THRESHOLD_DEFAULT      -1.22f

/**
 * @brief      Upper ADC threshold (POR value). @SI{V}
 *
 *             ADC values across this limit are captured from the ASG.
 */
#define ASG_UPPER_THRESHOLD_DEFAULT      1.22f

/**
 * @brief      ASG integrator tripping level (POR value). @SI{V}
 *
 *             If the ASG integrator reaches this level, the actuator guard
 *             becomes active and enter safe state. 
 */
#define ASG_INTEGRATOR_TRIPPING_DEFAULT      1.2

/**
 * @brief      Float representation of DAC output value.
 *
 *             DAC output value in case of safe state (tripped ASG). @SI{V}
 *
 * @attention  This value needs to be calibrated once after altering parts of
 *             the scanner hardware. In any case, this value has to result in
 *             zero-actuator current!
 */
#define ASG_SAFEVALUE_DEFAULT           -0.02f

/**
 * @brief      ASG integration time constant (POR value). @SI{s}
 *
 *             The ASG tripping behavior (in other words: ... it's fuse
 *             characteristic) depends on integrator input amplitude and time
 *             constant.
 */
#define ASG_TRIPPING_TIME_DEFAULT       0.750f

/** @} */

/**
 * @addtogroup  Actuators_Typedefs
 * @brief       Actuator methods related type definitions. @{
 */

/**
 * @brief       Beam control source enumeration.
 */
typedef enum {
    BEAM_CTRL_SOURCE_GLOBAL,    //!< Global laser beam control source. 
    BEAM_CTRL_SOURCE_MANUAL     //!< Manuel laser beam control source.
} beamCtrlSource_t;

/**
 * @brief      ASG Integrator state enumeration.
 *
 *             To manipulate the fuse tripping transfer function (fuse
 *             characteristics) of the ASG, the process output to be secured, is
 *             feded to an integrator which have to be controlled.
 */
typedef enum {
    ASG_CHARGING_INTEGRATOR,    //!< ASG_CHARGING_INTEGRATOR
    ASG_DISCHARGING_INTEGRATOR, //!< ASG_DISCHARGING_INTEGRATOR
    ASG_STATIONARY_INTEGRATOR   //!< ASG_STATIONARY_INTEGRATOR
} ASG_INT_STATE_t;


/**
 * @brief      Actuator Safestate Guard structure typedef.
 *
 *             The Actuator Safestate Guard provides a security layer to protect
 *             actuator and current amplifier from thermal overloads.
 */
typedef struct {
    /**
     * @brief      ASG "Out-of-range" integrator variable.
     *
     *             Used to measure the "amount" of analog watchdog events that
     *             have occurred.
     */
    double      integrator;     

    /**
     * @brief      Represents the remote configureable integrator tripping level
     *             (see ASG_INTEGRATOR_TRIPPING_DEFAULT). If the integrator
     *             reaches this level, the ASG systems "digital fuse" will be
     *             tripped and the DAC outputs are forced into safe state (see
     * @see        feVal). @si{V}
     */    
    float       trippingLevel;

    /**
     * @brief      Lower ADC_f threshold. Used as trigger value of analog
     *             watchdog interrupts. @si{V}
     */
    float       lowerThreshold;  

    /**
     * @brief      Upper ADC_f threshold. Used as trigger value of analog
     *             watchdog interrupts. @si{V}
     */
    float       upperThreshold; 

    /**
     * @brief      This is the DAC output safe state value which is forced to
     *             become active if the ASG system has "tripped" its fuse. @si{V}
     */    
    float       safeVal;


    float       tripTime;       //!< [ms] Time constant, used to change tripping characteristics at runtime
    uint8_t     ack;            //!< acknowlage tripped stat
    uint8_t     tripped;        //!< tripped != 0 if tripping is active
    ASG_INT_STATE_t state;      //!< ASG integrator state (See ASG_INT_STATE_t) 
} ActuatorSafestateGuard_t;

/** @} */



/**
 * @addtogroup  Actuators_Functions
 * @brief       Actuator function definitions. 
 * @{
 */

/**
 * @addtogroup  Actuators_Guarding
 * @brief       This group includes all functions related to safety and
 *              guarding.
 *              @{
 */

/**
 * @brief      Safely update actuator control signals.
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
 * @brief      Set beam control source and new state.
 *
 * @param[in]  src        The beam control source
 * @param[in]  newState   The new state
 */
int beamCtrl(beamCtrlSource_t src, tribool_state_t newState );

/** @} */

/** @} */

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

#endif


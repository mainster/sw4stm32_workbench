/**
 * @file        actuators.c
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
   @verbatim

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
#include "actuators.h"
#include "globals.h"

ActuatorSafestateGuard_t ASG;

extern DAC_WP_t (*DAC_SecureSetDualChanSigned) (int16_t, int16_t);


int beamCtrl(beamCtrlSource_t src, tribool_state_t newState ) {
    
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

    /* FIXME: TEST */
    float test = ASG.lowerThreshold;
    g.dutyCyc = test;
    
    return -1;
}

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
int updateActuator_f(float I_set_x, float I_set_y) {
    int toPlant_intBuff[2] = { 0, 0 };
    int *toPlant_int = &toPlant_intBuff[0];
    
    /* only if ASG state is NOT tripped */
    if (!ASG.tripped) {     

        /* Decode the float values to an integral type */
        *toPlant_int     = decode_toInt(I_set_x);
        *(toPlant_int+1) = decode_toInt(I_set_y);

        /**
         * Check decoded values against valied DAC output range. 
         * Clipping would be necessary! 
         */
        for (uint8_t k=0; k<2; k++) {
            if (*(toPlant_int+k) > UPPER_DAC_LIMIT_SIGNED)
                    *(toPlant_int+k) = UPPER_DAC_LIMIT_SIGNED;
            if (*(toPlant_int+k) < LOWER_DAC_LIMIT_SIGNED)
                    *(toPlant_int+k) = LOWER_DAC_LIMIT_SIGNED;
        }
    } 
    else {   
    /* <><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>    
       <>  This branch eror-handles an ASG integrator_full event i.e.<>
       <><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>    
       <>                    FUSE TRIPPED                            <>
       <><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<> */  
       /* All safety related actions are done by the watchdog handler */ 
    }

   /**
    * @note       The return argument toPlant_int is of type int16_t because it
    *             holds the @b unbiased DAC output register values.
    *
    * This is a function pointer! Casting from (float32_t) toPlant to int16_t
    * type introduces rounding errors which couldn't be prevented!
    */
    DAC_SecureSetDualChanSigned( *toPlant_int, *(toPlant_int+1));  
        
    return 0;
}

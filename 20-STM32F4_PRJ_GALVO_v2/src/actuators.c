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

/* Includes */
#include "actuators.h"
#include "defines.h"

/**
 * @addtogroup MD_APP
 * @{
 */

/**
 * @addtogroup _Macros
 * @{
 */

/**
 * @addtogroup _Typedefs
 * @{
 */

/**
 * @addtogroup _Variables
 * @{
 */

/**
 * @addtogroup _Functions
 * @{
 */


/** @} */

/** @} */

/** @} */

/** @} */

/** @} */



//extern autoSaveSystem_t ASG;
autoSaveSystem_t ASG;

extern DAC_WP_t (*DAC_SecureSetDualChanSigned) (int16_t, int16_t);

/** 
 * @brief   Set beam control source and new state
 */ 
int beamCtrl(beamCtrlSource_t src, tribool_state_t newState ) {
    float test = ASG.lowerVal;
    
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
    
    g.dutyCyc = test;
    
    return -1;
}



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
    *             holds the @b un\bbiased DAC output register values...
    *
    * This is a function pointer! Casting from (float32_t) toPlant to int16_t
    * type introduces rounding errors which couldn't be prevented!
    */
    DAC_SecureSetDualChanSigned( *toPlant_int, *(toPlant_int+1));  
        
    return 0;
}

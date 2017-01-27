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
 #ifndef _ACTUATORS_H
 #define _ACTUATORS_H
 
//#include <string.h>
//#include <stdlib.h>
//#include <stdio.h>

#include "adc_dac_dma.h"
//#include "arm_architect.h"
//#include "arm_math.h"
#include "defines.h"
#include "main.h"
#include "mdb_gpio.h"
#include "pid.h"



int updateActuator_f(float I_set_x, float I_set_y);
int beamCtrl(beamCtrlSource_t src, tribool_state_t newState );


#endif

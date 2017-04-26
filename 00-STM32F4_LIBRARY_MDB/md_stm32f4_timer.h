/**
 * @file        md_stm32f4_gpio.h
 *
 * @date        30 Jan 2017
 * @author      Manuel Del Basso (mainster)
 * @email       manuel.delbasso@gmail.com
 *
 * @ide         System Workbench ac6 (eclipse stm32)
 * @stdperiph   STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 * @license     GNU GPL v3
 *
 * @brief       Provides generic methods for Timer module initialization and manipulation.
 *
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
 */
#ifndef _MD_TIMER_H_
#define _MD_TIMER_H_

#ifdef __cplusplus
 extern "C" {
#endif 

/**
 * Includes
 */
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
// #include "stm32f4xx_gpio.h"
//#include "stm32f4xx_adc.h"

/**
 * This values can be overwriten in globals.h file
 */

//#ifndef TM_ADC3_RESOLUTION
//#define TM_ADC3_RESOLUTION		ADC_Resolution_12b
//#endif

/**
 * Initialize ADCx with ADCx channel
 *
 * Parameters:
 * 	- ADC_TypeDef* ADCx: ADCx
 * 		ADC1, ADC2, ADC3
 * 	- uint8_t channel: channel (pin) for ADCx
 * 		- ADC_Channel_0, ADC_Channel_1, ..., ADC_Channel_15
 */
//extern int TM_Timer2_config(FunctionalState TimerRun, FunctionalState IntOn, uint16_t peri);
//extern int TM_Timer3_config(FunctionalState TimerRun, FunctionalState IntOn, uint16_t peri);
//extern int TM_Timer6_config(FunctionalState TimerRun, FunctionalState IntOn, uint16_t peri);	

void timer2_interrupt_cmd(FunctionalState NewState);					 
void timer3_interrupt_cmd(FunctionalState NewState);					 
int TM_Timer2_Init_adc_sample_trigger(FunctionalState TimerRun, FunctionalState IntOn, uint16_t peri);
int TM_Timer2_config(FunctionalState TimerRun, FunctionalState IntOn, uint16_t peri);
int TM_Timer3_config(FunctionalState TimerRun, FunctionalState IntOn, uint16_t peri);
int TM_Timer4_config(FunctionalState TimerRun, FunctionalState IntOn, uint16_t peri);
int TM_Timer6_config(FunctionalState TimerRun, FunctionalState IntOn, uint16_t peri);


#ifdef __cplusplus
}
#endif

#endif

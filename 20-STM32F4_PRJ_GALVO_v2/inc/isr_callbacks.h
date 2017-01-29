/**
 * @file        isr_callbacks.h
 *
 * @date        28 Jan 2017
 * @author      Manuel Del Basso (mainster)
 * @email       manuel.delbasso@gmail.com
 *
 * @ide         System Workbench ac6 (eclipse stm32)
 * @stdperiph   STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 * @license		GNU GPL v3
 *
 * @brief       Analog processing related callback handler definition.
 *
 * @verbatim

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

	@endverbatim
 *
 */
#ifndef _ISR_CALLBACKS_H_
#define _ISR_CALLBACKS_H_

#include "main.h"
#include "defines.h"
#include "adc_dac_dma.h"

/**
 * @addtogroup MD_APP
 * @{
 */

/**
 * @addtogroup 	APP_Callback_Handlers
 * @brief		Provides project specific callback handlers to service interrupt requests.
 * @{
 */

/* See implementation file for documentation comments. */
void TIM2_IRQHandler(void);

/* See implementation file for documentation comments. */
void TIM4_IRQHandler(void);

/* See implementation file for documentation comments. */
void ADC_IRQHandler(void);

/** @} */

/** @} */

#endif


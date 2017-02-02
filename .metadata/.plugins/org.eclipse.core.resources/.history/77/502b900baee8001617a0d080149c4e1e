/**
 * @file        md_stm32f4_dac.c
 *
 * @date        28 Jan 2017
 * @author      Manuel Del Basso (mainster)
 * @email       manuel.delbasso@gmail.com
 *
 * @ide         System Workbench ac6 (eclipse stm32)
 * @stdperiph   STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 * @license		GNU GPL v3
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
#include "md_stm32f4_dac.h"

void MD_DAC_Init(MD_DAC_Channel_t DACx) {
	DAC_InitTypeDef DAC_InitStruct;
	uint16_t gpioPin;

	/* Select proper GPIO pin */
	if (DACx == MD_DAC1) {
		gpioPin = GPIO_PIN_4;
	} else {
		gpioPin = GPIO_PIN_5;
	}

	/* Initialize proper GPIO pin */
	MD_GPIO_Init (GPIOA, gpioPin, MD_GPIO_Mode_AF, GPIO_OType_PP, MD_GPIO_PuPd_NOPULL, MD_GPIO_Speed_Fast);

	/* Enable DAC clock */
	RCC->APB1ENR |= RCC_APB1ENR_DACEN;

	/* Set DAC options */
	DAC_InitStruct.DAC_Trigger = DAC_Trigger_None;
	DAC_InitStruct.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_InitStruct.DAC_OutputBuffer = DAC_OutputBuffer_Enable;

	/* Init and enable proper DAC */
	if (DACx == MD_DAC1) {
		DAC_Init(DAC_Channel_1, &DAC_InitStruct);
		/* Enable DAC channel 1 */
		DAC->CR |= DAC_CR_EN1;
	} else {
		DAC_Init(DAC_Channel_2, &DAC_InitStruct);
		/* Enable DAC channel 2 */
		DAC->CR |= DAC_CR_EN2;
	}
}

void MD_DAC_SetValue(MD_DAC_Channel_t DACx, uint16_t value) {
	/* Check value */
	if (value > 4095) {
		value = 4095;
	}

	/* Set 12-bit value, right aligned */
	if (DACx == MD_DAC1) {
		DAC->DHR12R1 = value;
	} else {
		DAC->DHR12R2 = value;
	}
}

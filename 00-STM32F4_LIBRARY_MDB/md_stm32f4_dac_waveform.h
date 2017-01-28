/**
 * @file        md_stm32f4_dac_waveform.h
 *
 * @date        27 Jan 2017
 * @author      Manuel Del Basso (mainster)
 * @email       manuel.delbasso@gmail.com
 *
 * @ide         System Workbench ac6 (eclipse stm32)
 * @stdperiph   STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 * @license		GNU GPL v3
 *
 * @brief       Waveform generation extension for DAC library.
 *
	@verbatim

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
#ifndef _MD_DAC_H_
#define _MD_DAC_H_

#include "stm32f4xx.h"
#include "stm32f4xx_dac.h"
#include "md_stm32f4_dac.h"
#include "defines.h"

/**
 * @addtogroup MD_STM32F4_Libraries
 * @{
 */

/**
 * @addtogroup DAC_SignalGen
 * @{
 */

/**
 * @addtogroup DAC_SignalGen_Macros
 * @{
 */

/**
 * @brief      DAC module hardware register address definition.
 *
 *             Address definitions for DAC data hold registers (8-Bit and
 *             12-Bit). To provide direct __IO register access for DAC data
 *             registers.
 * @ref DM00031020.pdf: 14.5.15 DAC register map.
 */
#define DAC_DHR12R1_ADDRESS    0x40007408
#define DAC_DHR12R2_ADDRESS    0x40007414
#define DAC_DHR8R1_ADDRESS     0x40007410
#define DAC_DHR8R2_ADDRESS     0x4000741C

/** @} *//* DAC_SignalGen_Macros */

/**
 * @addtogroup DAC_SignalGen_Variables
 * @{
 */
DAC_InitTypeDef     DAC_InitStructure;
DMA_InitTypeDef     DMA_InitStructure;

/** @} *//* DAC_SignalGen_Variables */

/**
 * @addtogroup DAC_SignalGen_Functions
 * @{
 */

/**
 * @brief      Configuration method  
 *
 * @param[in]  DACx          The da cx
 * @param[in]  NewStateDAC   The new state dac
 * @param[in]  NewStateDMA   The new state dma
 */
void DAC_DMA_SineWaveConfig(MD_DAC_Channel_t DACx,
                            FunctionalState NewStateDAC, 
                            FunctionalState NewStateDMA);

void DAC_Chx_NoiseConfig(MD_DAC_Channel_t DACx);
void DAC_TriangleConfig(MD_DAC_Channel_t DACx);
void DAC_Chx (MD_DAC_Channel_t DACx);

/** @} */	/* DAC_SignalGen_Functions */
/** @} */	/* DAC_SignalGen */
/** @} */	/* MD_STM32F4_Libraries */
#endif

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
 * @addtogroup MD_DAC_SigGen
 * @{
 */

/**
 * @addtogroup MD_DAC_SigGen_Macros
 * @{
 */

/**
 * @addtogroup General_Notes
 * @{
 */

/**
 * @brief      General notes and informative stuff.
 *
 * @verbatim
 
    ===========================================================================
    ===========  Determine base addresses of peripheral registers  ============
    ===========================================================================

    @link CMSIS/device/stm32f4xx.h: 
        #define PERIPH_BASE         ((uint32_t)0x40000000)
        #define APB1PERIPH_BASE     PERIPH_BASE
        #define DAC_BASE            (APB1PERIPH_BASE + 0x7400)

    
    <a href="</a>
        DAC_DHR12R1_Reg_Offset  = 0x08  
        DAC_DHR12R2_Reg_Offset  = 0x14  
        DAC_DHR8R1_Reg_Offset   = 0x10  
        DAC_DHR8R2_Reg_Offset   = 0x1C  

    RegisterAddress = DAC_BASE + Regg_Offset

    #define DAC_DHR12R1_ADDRESS 0x40007408
    #define DAC_DHR12R2_ADDRESS 0x40007414
    #define DAC_DHR8R1_ADDRESS   0x40007410

 @endverbatim
 *
 */

/**
 * @brief      DAC module __IO register address definition.
 *
 *             Address definitions for "DAC data hold registers" (8-Bit and
 *             12-Bit). To provide direct __IO register access for DAC data
 *             registers.
 */
#define DAC_DHR12R1_ADDRESS    0x40007408
#define DAC_DHR12R2_ADDRESS    0x40007414
#define DAC_DHR8R1_ADDRESS     0x40007410
#define DAC_DHR8R2_ADDRESS     0x4000741C

/** @} */
/** @} */

/**
 * @addtogroup MD_DAC_SigGen_Variables
 * @{
 */
DAC_InitTypeDef     DAC_InitStructure;
DMA_InitTypeDef     DMA_InitStructure;

/** @} */

/**
 * @addtogroup MD_DAC_SigGen_Functions
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

/**
 * @brief      DAC Channels noise config
 *
 * @param[in]  DACx   The da cx
 */
void DAC_Chx_NoiseConfig(MD_DAC_Channel_t DACx);

/**
 * @brief      { function_description }
 *
 * @param[in]  DACx   The da cx
 */
void DAC_TriangleConfig(MD_DAC_Channel_t DACx);

// *
//  * @brief      { function_description }
//  *
//  * @param[in]  DACx   The da cx
 
// void DAC_Chx (MD_DAC_Channel_t DACx);

/** @} */
/** @} */
/** @} */

#endif

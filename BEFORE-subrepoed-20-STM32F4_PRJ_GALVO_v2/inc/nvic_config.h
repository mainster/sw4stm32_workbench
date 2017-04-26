/**
 * @file        nvic_config.h
 *
 * @date        30 Jan 2017
 * @author      Manuel Del Basso (mainster)
 * @email       manuel.delbasso@gmail.com
 *
 * @ide         System Workbench ac6 (eclipse stm32)
 * @stdperiph   STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 * @license		GNU GPL v3
 * 
 * @brief       Provides nested vector interrupt controller configuration methods.
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
#ifndef _NVIC_CONFIG_H_
#define _NVIC_CONFIG_H_

#ifdef __cplusplus
 extern "C" {
#endif 
 	
#include "misc.h"

/**
 * @addtogroup MD_APP
 * @{
 */

/**
 * @addtogroup APP_NVIC_Config Interrupt Controller
 * @{
 */

/**
 * @addtogroup NVIC_Config_Macros
 * @{
 */

/** @} */

/**
 * @addtogroup NVIC_Config_Typedefs
 * @{
 */

/** @} */

/**
 * @addtogroup NVIC_Config_Variables
 * @{
 */

/**
 * @brief      Private initialization structure instances to configure NVIC.
 */
NVIC_InitTypeDef
NVIC_InitStruct_DMA,	//!< DMA NVIC initialization struct.
NVIC_InitStruct_TIM,	//!< TIM NVIC initialization struct.
NVIC_InitStruct_ADC;	//!< ADC NVIC initialization struct.

/** @} */

/**
 * @addtogroup NVIC_Config_Functions
 * @{
 */

/**
 * @brief	Provides the initial configuration method for the nested vector interrupt controller.
 */
void NVIC_Configuration (void);


/** @} */

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* NVIC_CONFIG_H_ */

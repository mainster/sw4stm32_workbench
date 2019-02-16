/**
 * @file        md_stm32f4_usart.h
 *
 * @date        30 Jan 2017
 * @author      Manuel Del Basso (mainster)
 * @email       manuel.delbasso@gmail.com
 *
 * @ide         System Workbench ac6 (eclipse stm32)
 * @stdperiph   STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 * @license		GNU GPL v3
 *
 * @brief       DESCRIPTION
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

#ifndef _MD_USART_H_
#define _MD_USART_H_

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

typedef enum MD_USART_PinsPack {
    MD_USART_PinsPack_1 = 0x00,
    MD_USART_PinsPack_2,
    MD_USART_PinsPack_3,
    MD_USART_PinsPack_Custom,
} MD_USART_PinsPack_t;

//typedef enum TM_USART_PinsPack_t TM_USART_PinsPack;

//typedef enum {
//    MD_USART_Hw_FlowControl_None = 0x0000,
//    MD_USART_Hw_RTS = 0x0100,
//    MD_USART_Hw_CTS = 0x0200,
//    MD_USART_Hw_RTS_CTS = 0x0300
//} MD_USART_HardwareFlowControl_t;

/* Default buffer size for USART and NVIC prio */
#define MD_USART_DEFAULT_BUFFER_SIZE  	64
#define MD_USART_NVIC_PRIORITY		0x00

/* Set default buffer size for specific USART */
#define MD_USART1_BUFFER_SIZE  USART_DEFAULT_BUFFER_SIZE
#define MD_USART2_BUFFER_SIZE  USART_DEFAULT_BUFFER_SIZE

void MD_USART_Init(USART_TypeDef* USARTx, MD_USART_PinsPack_t pinspack, uint32_t baudrate);
uint16_t MD_USART_Gets(USART_TypeDef* USARTx, char* buffer, uint16_t bufsize);
uint8_t MD_USART_Getc(USART_TypeDef* USARTx);

#ifdef __cplusplus
}
#endif

#endif

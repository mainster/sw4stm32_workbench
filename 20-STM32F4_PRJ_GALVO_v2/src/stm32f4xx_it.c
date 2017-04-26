/**
 * @file        stm32f4xx_it.c
 *
 * @date        14 Mar 2017
 * @author      Manuel Del Basso (mainster)
 * @email       manuel.delbasso@gmail.com
 *
 * @ide         System Workbench ac6 (eclipse stm32)
 * @stdperiph   STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 * @license     GNU GPL v3
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
 *
 */

/* Includes */
#include "stm32f4xx_it.h"
#include "main.h"

/**
 * @addtogroup MD_APP
 * @{
 */

/**
 * @addtogroup exceptions Exceptions Handlers 
 * @{
 */

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
 * @brief      This function handles NMI exception.
 */
void NMI_Handler(void) {
}

/**
 * @brief      This function handles Hard Fault exception.
 */
void HardFault_Handler(void) {
	/**< Go to infinite loop when Hard Fault exception occurs */
	while (1) ;;
}

/**
 * @brief      This function handles Memory Manage exception.
 */
void MemManage_Handler(void) {
	/**< Go to infinite loop when Memory Manage exception occurs */
	while (1) ;;
}

/**
 * @brief      This function handles Bus Fault exception.
 */
void BusFault_Handler(void) {
	/**< Go to infinite loop when Bus Fault exception occurs */
	while (1) ;;
}

/**
 * @brief      This function handles Usage Fault exception.
 */
void UsageFault_Handler(void) {
	/**< Go to infinite loop when Usage Fault exception occurs */
	while (1) ;;
}

/**
 * @brief      This function handles SVCall exception.
 */
void SVC_Handler(void) {
}

/**
 * @brief      This function handles Debug Monitor exception.
 */
void DebugMon_Handler(void) {
}

/**
 * @brief      This function handles PendSVC exception.
 */
void PendSV_Handler(void) {
}

/**
 * @brief      This function decrement timing variable
 *
 *             @with __weak parameter to prevent errors
 */
__weak void TimingDelay_Decrement(void) {
}

/**
 * @brief      This function handles SysTick Handler.
 */
//void SysTick_Handler(void) {
//	TimingDelay_Decrement();
//}

/**
 * @brief      This function handles PPP interrupt request.
 */
// void PPP_IRQHandler(void) {
// }

/** @} */ 

/** @} */ 


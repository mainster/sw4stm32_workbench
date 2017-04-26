/**
 * @file        stm32f4xx_it.c
 *
 * @date        28 Jan 2017
 * @author      Manuel Del Basso (mainster)
 * @email       manuel.delbasso@gmail.com
 *
 * @ide         System Workbench ac6 (eclipse stm32)
 * @stdperiph   STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 * @license		GNU GPL v3
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
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
 *
 * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *        http://www.st.com/software_license_agreement_liberty_v2
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
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
 * @retval     None   
 */
void NMI_Handler(void) {

}

/**
 * @brief      This function handles Hard Fault exception.
 * @retval     None   
 */
void HardFault_Handler(void) {
	/**< Go to infinite loop when Hard Fault exception occurs */
	while (1)
	{
	}
}

/**
	* @brief  This function handles Memory Manage exception.
	* @param  None
	* @retval None
	*/
void MemManage_Handler(void) {
	/**< Go to infinite loop when Memory Manage exception occurs */
	while (1) ;;
}

/**
	* @brief  This function handles Bus Fault exception.
	* @param  None
	* @retval None
	*/
void BusFault_Handler(void)
{
	/**< Go to infinite loop when Bus Fault exception occurs */
	while (1)
	{
	}
}

/**
	* @brief  This function handles Usage Fault exception.
	* @param  None
	* @retval None
	*/
void UsageFault_Handler(void)
{
	/**< Go to infinite loop when Usage Fault exception occurs */
	while (1)
	{
	}
}

/**
	* @brief  This function handles SVCall exception.
	* @param  None
	* @retval None
	*/
void SVC_Handler(void)
{
}

/**
	* @brief  This function handles Debug Monitor exception.
	* @param  None
	* @retval None
	*/
void DebugMon_Handler(void)
{
}

/**
	* @brief  This function handles PendSVC exception.
	* @param  None
	* @retval None
	*/
void PendSV_Handler(void)
{
}

/**
	* @brief  This function decrement timing variable
	*	@with __weak parameter to prevent errors
	* @param  None
	* @retval None
	*/
__weak void TimingDelay_Decrement(void) {

}

///**
//  * @brief  This function handles SysTick Handler.
//  * @param  None
//  * @retval None
//  */
////void SysTick_Handler(void)
////{
////	TimingDelay_Decrement();
////}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
	* @brief  This function handles PPP interrupt request.
	* @param  None
	* @retval None
	*/
/*void PPP_IRQHandler(void)
{
}*/

/** @} */ 

/** @} */ 


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

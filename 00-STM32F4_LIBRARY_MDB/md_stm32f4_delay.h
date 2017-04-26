/**
 * @file        md_stm32f4_delay.h
 *
 * @date        27 Jan 2017
 * @author      Manuel Del Basso (mainster)
 * @email       manuel.delbasso@gmail.com
 *
 * @ide         System Workbench ac6 (eclipse stm32)
 * @stdperiph   STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 * @license		GNU GPL v3
 * 
 * @brief       Provides SysTick-Timer driven delay functions.
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
#ifndef MD_STM32F4_DELAY_H_
#define MD_STM32F4_DELAY_H_

#ifdef __cplusplus
 extern "C" {
#endif 
    
/* Includes */
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "globals.h"

//CMSIS_glob_defs
//static __IO uint32_t MD_TimingDelay;
//static uint32_t MD_TimingDelayNoIo;

/**
 * This variable can be used in main
 * It is automatically increased every time SysTick make an interrupt
 */
static __IO uint32_t MD_Time;
static __IO uint32_t MD_Time2;

/**
 *
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in microseconds.
  * @retval None
  */
void Delay(__IO uint32_t nTime);

/**
 *
 * @brief  Inserts a delay time.
 * @param  nTime: specifies the delay time length, in milliseconds.
 * @retval None
 */
void Delayms(__IO uint32_t nTime);

/**
 *
 *
 * Initialize timer settings for delay
 */
void MD_DELAY_Init(void);

/**
 * Get the MD_Time variable value
 */
uint32_t MD_DELAY_Time(void);

/**
 * Set value for MD_Time variable
 */
void MD_DELAY_SetTime(uint32_t time);

/**
 * Get the MD_Time variable value
 */
uint32_t MD_DELAY_Time2(void);

/**
 * Set value for MD_Time variable
 */
void MD_DELAY_SetTime2(uint32_t time);

/**
 * Re-enable Systick. It has to be configured before with MD_DELAY_Init();
 */
void MD_DELAY_EnableSystick(void);

/**
 * Disable Systick. Usef when going into sleep mode, so systick will not
 */
void MD_DELAY_DisableSystick(void);

#ifdef __cplusplus
}
#endif

#endif /* MD_STM32F4_DELAY_H_ */

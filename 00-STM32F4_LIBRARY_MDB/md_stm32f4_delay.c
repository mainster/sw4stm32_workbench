/**
 * @file        md_stm32f4_delay.c
 *
 * @date        27 Jan 2017
 * @author      Manuel Del Basso (mainster)
 * @email       manuel.delbasso@gmail.com
 *
 * @ide         System Workbench ac6 (eclipse stm32)
 * @stdperiph   STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 * @license		GNU GPL v3
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
#include "md_stm32f4_delay.h"

static __IO uint32_t MD_TimingDelay = 0;
static __IO uint32_t MD_Time = 0;
uint8_t MD_DELAY_Initialized = 0;

void Delay(__IO uint32_t nTime) {
	MD_TimingDelay = nTime;

	while (MD_TimingDelay != 0);
}

void Delayms(__IO uint32_t nTime) {
	MD_TimingDelay = 1000 * nTime;

	while (MD_TimingDelay != 0);
}

#ifdef KEIL_IDE
void TimingDelay_Decrement(void) {
	MD_Time++;
	if (MD_Time2 != 0x00) {
		MD_Time2--;
	}
	if (MD_TimingDelay != 0x00) {
		MD_TimingDelay--;
	}
}
#else
void SysTick_Handler(void) {
	MD_Time++;
	if (MD_Time2 != 0x00) {
		MD_Time2--;
	}
	if (MD_TimingDelay != 0x00) {
		MD_TimingDelay--;
	}
}
#endif

void MD_DELAY_Init(void) {
	if (MD_DELAY_Initialized) {
		return;
	}
	RCC_HSEConfig(RCC_HSE_ON);
	while (!RCC_WaitForHSEStartUp());
	/* Set Systick interrupt every 1us */
	if (SysTick_Config(SystemCoreClock / 1000000)) {
		/* Capture error */
		while (1);
	}
	MD_DELAY_Initialized = 1;
}

uint32_t MD_DELAY_Time(void) {
	return MD_Time;
}

uint32_t MD_DELAY_Time2(void) {
	return MD_Time2;
}

void MD_DELAY_SetTime(uint32_t time) {
	MD_Time = time;
}

void MD_DELAY_SetTime2(uint32_t time) {
	MD_Time2 = time;
}

void MD_DELAY_EnableSystick(void) {
	if (!MD_DELAY_Initialized) {
		return;
	}

	/* Enable systick interrupts, useful when you wakeup from sleep mode */
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
}

void MD_DELAY_DisableSystick(void) {
	/* Disable systick, useful when you go to sleep mode */
	SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
}


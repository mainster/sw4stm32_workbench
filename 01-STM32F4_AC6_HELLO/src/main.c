/**
 * @file        main.c
 *
 * @date        4 Feb 2017
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
 */

#include "main.h"

int main(void) {
	/**< Initialize System */
	SystemInit();

	/**< All used GPIOs should be initialized by this call */
	MDB_GPIO_Init();

	/**< Initialize discovery button and leds */
	MD_DISCO_ButtonInit();

	/**< Initialize Leds */
	MD_DISCO_LedInit();

	/**< Initialize delay library */
	MD_DELAY_Init();


//	MD_GPIO_SetPinHigh(MDB_GPIO[0].);
//	MD_GPIO_SetPinHigh(MDB_GPIO[1]);

	MDB_GPIO_Off(LED_GREEN_A);
	MDB_GPIO_On(LED_RED_A);

	while (1) {
		Delayms(500);
		MDB_GPIO_Toggle(LED_GREEN_A);
		MDB_GPIO_Toggle_m2(LED_RED_A);
	}

}

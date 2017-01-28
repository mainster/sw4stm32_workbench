/**
 * @file        md_stm32f4_disco.h
 *
 * @date        27 Jan 2017
 * @author      Manuel Del Basso (mainster)
 * @email       manuel.delbasso@gmail.com
 *
 * @ide         System Workbench ac6 (eclipse stm32)
 * @stdperiph   STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 * @license		GNU GPL v3
 *
 * @brief       Definitions to extend board support for STM32F429i-DISCO and equivalent boards.
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
#ifndef MD_DISCO_H_
#define MD_DISCO_H_ 1110

/**
 * @addtogroup MD_STM32F4_Libraries
 * @{
 */

/**
 * @defgroup	MD_DISCO Discovery Board
 * @brief		Provides initialization and access methods for on-board LEDs and Button. @{
 *
 * @par Dependencies
 *
@verbatim
 - STM32F4xx
 - defines.h
 - MD GPIO
@endverbatim
 */
#include "stm32f4xx.h"
#include "defines.h"
#include "md_stm32f4_gpio.h"

/* Definitions according to "STM32F429I-DISCO schematics" */
#ifdef STM32F429ZI
	/* STM32F429 discovery kit on-board LEDs */
	#define MD_DISCO_LED_PORT			GPIOG
	#define LED_GREEN					GPIO_PIN_13
	#define LED_RED						GPIO_PIN_14
	#define LED_ALL						LED_GREEN | LED_RED
	#define MD_DISCO_LED_PINS			LED_GREEN | LED_RED
		
	/* STM32F429 discovery kit on-board button */
	#define MD_DISCO_BUTTON_PORT		GPIOA
	#define MD_DISCO_BUTTON_PIN			GPIO_PIN_0
	#define MD_DISCO_BUTTON_PULL		MD_GPIO_PuPd_Down
	#define MD_DISCO_BUTTON_PRESSED		1
#endif

/**
 * @addtogroup	MD_DISCO_Functions
 * @brief    	STM32F429i-Disc evaluation hardware library functions.
 * @{
 */

/**
 * @brief      Configure all pins for on-board LEDs as outputs.
 */
void MD_DISCO_LedInit(void);

/**
 * @brief  Configures Button pin as input
 * @param  None
 * @retval None
 */
void MD_DISCO_ButtonInit(void);

/**
 * @brief  Turns on LED on board
 * @note   STM32F4x9-Eval board uses inverse logic for leds
 * @param  led: LED you want to turn on
 *            - LED_RED: Red led
 *            - LED_GREEN: Green led
 *            - LED_BLUE: Blue led
 *            - LED_ORANGE: Orange led
 *            - LED_ALL: All leds
 * @retval None
 */
#ifndef MD_DISCO_SWAP_LOGIC
	#define MD_DISCO_LedOn(led)        MD_GPIO_SetPinHigh(MD_DISCO_LED_PORT, (led))
#else
	#define MD_DISCO_LedOn(led)        MD_GPIO_SetPinLow(MD_DISCO_LED_PORT, (led))
#endif

/**
 * @brief  Turns off LED on board
 * @note   STM32F4x9-Eval board uses inverse logic for leds
 * @param  led: LED you want to turn off
 *            - LED_RED: Red led
 *            - LED_GREEN: Green led
 *            - LED_BLUE: Blue led
 *            - LED_ORANGE: Orange led
 *            - LED_ALL: All leds
 * @retval None
 */
#ifndef MD_DISCO_SWAP_LOGIC
	#define MD_DISCO_LedOff(led)       MD_GPIO_SetPinLow(MD_DISCO_LED_PORT, (led))
#else
	#define MD_DISCO_LedOff(led)       MD_GPIO_SetPinHigh(MD_DISCO_LED_PORT, (led))
#endif

/**
 * @brief  Toggles LED on board
 * @param  led: LED you want to toggle
 *            - LED_RED: Red led
 *            - LED_GREEN: Green led
 *            - LED_BLUE: Blue led
 *            - LED_ORANGE: Orange led
 *            - LED_ALL: All leds
 * @retval None
 */
#define MD_DISCO_LedToggle(led)        MD_GPIO_TogglePinValue(MD_DISCO_LED_PORT, (led))

/**
 * @brief  Checks if led is on
 * @note   STM32F4x9-Eval board uses inverse logic for leds
 * @param  led: Led you want to checking
 *            - LED_RED: Red led
 *            - LED_GREEN: Green led
 *            - LED_BLUE: Blue led
 *            - LED_ORANGE: Orange led
 *            - LED_ALL: All leds
 * @retval 1 if led is on or 0 if not
 */
#ifndef MD_DISCO_SWAP_LOGIC
#define MD_DISCO_LedIsOn(led)          MD_GPIO_GetOutputPinValue(MD_DISCO_LED_PORT, (led))
#else
#define MD_DISCO_LedIsOn(led)          !MD_GPIO_GetOutputPinValue(MD_DISCO_LED_PORT, (led))
#endif

/**
 * @brief  Sets led value
 * @param  led: LED you want to set value
 *            - LED_RED: Red led
 *            - LED_GREEN: Green led
 *            - LED_BLUE: Blue led
 *            - LED_ORANGE: Orange led
 *            - LED_ALL: All leds
 * @param  state: Set or clear led 
 *            - 0: led is off
 *            - > 0: led is on
 * @retval None
 */
#define MD_DISCO_SetLed(led, state)    ((state) ? MD_DISCO_LedOn(led): MD_DISCO_LedOff(led))

/**
 * @brief  Checks if user button is pressed
 * @param  None
 * @retval Button status
 *            - 0: Button is not pressed
 *            - > 0: Button is pressed
 */
#define MD_DISCO_ButtonPressed()       ((MD_GPIO_GetInputPinValue(MD_DISCO_BUTTON_PORT, MD_DISCO_BUTTON_PIN) == 0) != MD_DISCO_BUTTON_PRESSED)

/**
 * @brief  Checks if button was pressed now, but was not already pressed before
 * @param  None
 * @retval Button on pressed value
 *           - 0: In case that button has been already pressed on last call or was not pressed at all yet
 *           - > 0: Button was pressed, but state before was released
 */
uint8_t MD_DISCO_ButtonOnPressed(void);

/**
 * @brief  Checks if button was released now, but was already pressed before
 * @param  None
 * @retval Button on released value
 *           - 0: In case that button has been already released on last call or was not released at all yet
 *           - > 0: Button was released, but state before was pressed
 */
uint8_t MD_DISCO_ButtonOnReleased(void);

/** @} */
/** @} */
/** @} */
 
#endif

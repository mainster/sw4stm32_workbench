/**
 * @file        md_stm32f4_gpio.h
 *
 * @date        24 Jan 2017
 * @author      Manuel Del Basso (mainster)
 * @email       manuel.delbasso@gmail.com
 *
 * @ide         System Workbench ac6 (eclipse stm32)
 * @stdperiph   STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 *
 * @brief       GPIO basic manipulation
 *
 * @verbatim

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

 * @endverbatim
 *
 */
#ifndef MD_GPIO_H
#define MD_GPIO_H 150

/**
 * @addtogroup MD_STM32F4xx_Libraries
 * @{
 */

/**
 * @defgroup MD_GPIO
 * @brief    GPIO basic manipulation
 * @{
 *
 * GPIO basic library is used for GPIO phy manipulation.
 *
 * It is a user level implementation of Std/HAL driver GPIO library.
 *
 * It provides
 *  - register initialization methods
 *  - I/O accessing functions
 *
 * \par Quest1
 *
 */

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"

/* Handle C++ */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup MD_GPIO_Macros
 * @brief    Macro definitions for GPIO library
 * @{
 */

/**
 * @brief GPIO Pins register address definition
 * @note  Compatibility layer to service STD_PERIPH driver library
 */
#ifndef GPIO_Pin_0
#define GPIO_Pin_0      ((uint16_t)0x0001)
#define GPIO_Pin_1      ((uint16_t)0x0002)
#define GPIO_Pin_2      ((uint16_t)0x0004)
#define GPIO_Pin_3      ((uint16_t)0x0008)
#define GPIO_Pin_4      ((uint16_t)0x0010)
#define GPIO_Pin_5      ((uint16_t)0x0020)
#define GPIO_Pin_6      ((uint16_t)0x0040)
#define GPIO_Pin_7      ((uint16_t)0x0080)
#define GPIO_Pin_8      ((uint16_t)0x0100)
#define GPIO_Pin_9      ((uint16_t)0x0200)
#define GPIO_Pin_10     ((uint16_t)0x0400)
#define GPIO_Pin_11     ((uint16_t)0x0800)
#define GPIO_Pin_12     ((uint16_t)0x1000)
#define GPIO_Pin_13     ((uint16_t)0x2000)
#define GPIO_Pin_14     ((uint16_t)0x4000)
#define GPIO_Pin_15     ((uint16_t)0x8000)
#define GPIO_Pin_All    ((uint16_t)0xFFFF)
#endif

/**
 * @brief GPIO Pins register address definition
 * @note  Compatibility layer to service HAL driver library
 */
#ifndef GPIO_PIN_0
#define GPIO_PIN_0      ((uint16_t)0x0001)
#define GPIO_PIN_1      ((uint16_t)0x0002)
#define GPIO_PIN_2      ((uint16_t)0x0004)
#define GPIO_PIN_3      ((uint16_t)0x0008)
#define GPIO_PIN_4      ((uint16_t)0x0010)
#define GPIO_PIN_5      ((uint16_t)0x0020)
#define GPIO_PIN_6      ((uint16_t)0x0040)
#define GPIO_PIN_7      ((uint16_t)0x0080)
#define GPIO_PIN_8      ((uint16_t)0x0100)
#define GPIO_PIN_9      ((uint16_t)0x0200)
#define GPIO_PIN_10     ((uint16_t)0x0400)
#define GPIO_PIN_11     ((uint16_t)0x0800)
#define GPIO_PIN_12     ((uint16_t)0x1000)
#define GPIO_PIN_13     ((uint16_t)0x2000)
#define GPIO_PIN_14     ((uint16_t)0x4000)
#define GPIO_PIN_15     ((uint16_t)0x8000)
#define GPIO_PIN_ALL    ((uint16_t)0xFFFF)
#endif

/**
 * @}
 */

/**
 * @defgroup    MD_GPIO_Typedefs
 * @brief       GPIO Typedef is used to improve init/access method structure.
 * @{
 */

/**
 * @brief   Explicitly enumerate driver/logic mode.
 *
 */
typedef enum {
    MD_GPIO_Mode_IN  = 0x00,	/*!< Port/Pin as general purpose input. */
    MD_GPIO_Mode_OUT = 0x01,	/*!< Port/Pin as general purpose output. */
    MD_GPIO_Mode_ALT = 0x02,	/*!< Port/Pin operates in alternate functions mode. */
    MD_GPIO_Mode_ANA = 0x03,	/*!< Port/Pin operates in analog mode. */
} MD_GPIO_Mode_t;

/**
 * @brief   Explicitly enumerate GPIO driver output type.
 */
typedef enum {
    MD_GPIO_OType_PP = 0x00,	/*!< Output stage has active driver (push/pull) */
    MD_GPIO_OType_OD = 0x01,	/*!< Output stage has only one active driver (open-drain) */
} MD_GPIO_OType_t;

/**
 * @brief   Explicitly enumerate the driving strength (speed)
 */
typedef enum {
    MD_GPIO_Speed_Low    = 0x00,	/*!< Enumerate Low driving strength */
    MD_GPIO_Speed_Medium = 0x01,	/*!< GPIO Speed Medium */
    MD_GPIO_Speed_Fast   = 0x02,	/*!< GPIO Speed Fast */
    MD_GPIO_Speed_High   = 0x03,	/*!< GPIO Speed High */
} MD_GPIO_Speed_t;

/**
 * @brief   GPIO pull resistors enumeration
 */
typedef enum {
    MD_GPIO_PuPd_NOPULL = 0x00,	/*!< No pull resistor */
    MD_GPIO_PuPd_UP = 0x01,	/*!< Pull up resistor enabled */
    MD_GPIO_PuPd_DOWN = 0x02,	/*!< Pull down resistor enabled */
} MD_GPIO_PuPd_t;

/**
 * @} MD_GPIO_Typedefs
 */

/**
 * @defgroup MD_GPIO_Functions
 * @brief    GPIO Functions
 * @{
 */
 
/**
 * @brief      Initialize GPIO pins.
 *
 * @param      GPIOx       Pointer to GPIO module which should be initialized. 
 * @param[in]  GPIO_Pin    Pass GPIO mask of pins which should be initialized.  
 * @param[in]  GPIO_Mode   Pass GPIO logic/driver mode for GPIO_Pin (see @ref MD_GPIO_Mode_t enumeration typdef).
 * @param[in]  GPIO_OType  Pass GPIO driver stage type for GPIO_Pin (see @ref MD_GPIO_OType_t enumeration typdef).
 * @param[in]  GPIO_PuPd   Select GPIO pull resistors for GPIO_Pin (see @ref MD_GPIO_PuPd_t enumeration typdef).
 * @param[in]  GPIO_Speed  Select GPIO driver speed for GPIO_Pin (see @ref MD_GPIO_Speed_t enumeration typdef).
 */
void MD_GPIO_Init(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, MD_GPIO_Mode_t GPIO_Mode, MD_GPIO_OType_t GPIO_OType, MD_GPIO_PuPd_t GPIO_PuPd, MD_GPIO_Speed_t GPIO_Speed);

/**
 * @brief      Initialize GPIO pins for alternate port operation.
 *
 * @param      GPIOx        The gpi ox
 * @param[in]  GPIO_Pin     The gpio pin
 * @param[in]  GPIO_OType   The gpio o type
 * @param[in]  GPIO_PuPd    The gpio pu pd
 * @param[in]  GPIO_Speed   The gpio speed
 * @param[in]  Alternate    The alternate
 */
void MD_GPIO_InitAlternate(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, MD_GPIO_OType_t GPIO_OType, MD_GPIO_PuPd_t GPIO_PuPd, MD_GPIO_Speed_t GPIO_Speed, uint8_t Alternate);

/**
 * @brief  Deinitializes pin(s)
 * @note   Pins(s) will be set as analog mode to get low power consumption
 * @param  GPIOx: GPIOx PORT where you want to set pin as input
 * @param  GPIO_Pin: Select GPIO pin(s). You can select more pins with | (OR) operator to set them as input
 * @retval None
 */


/**
 * @brief      { function_description }
 *
 * @param      GPIOx      The gpi ox
 * @param[in]  GPIO_Pin   The gpio pin
 */
void MD_GPIO_DeInit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

/**
 * @brief  Sets pin(s) as input 
 * @note   Pins HAVE to be initialized first using @ref MD_GPIO_Init() or @ref MD_GPIO_InitAlternate() function
 * @note   This is just an option for fast input mode
 * @param  GPIOx: GPIOx PORT where you want to set pin as input
 * @param  GPIO_Pin: Select GPIO pin(s). You can select more pins with | (OR) operator to set them as input
 * @retval None
 */
void MD_GPIO_SetPinAsInput(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

/**
 * @brief  Sets pin(s) as output
 * @note   Pins HAVE to be initialized first using @ref MD_GPIO_Init() or @ref MD_GPIO_InitAlternate() function
 * @note   This is just an option for fast output mode 
 * @param  GPIOx: GPIOx PORT where you want to set pin as output
 * @param  GPIO_Pin: Select GPIO pin(s). You can select more pins with | (OR) operator to set them as output
 * @retval None
 */
void MD_GPIO_SetPinAsOutput(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

/**
 * @brief  Sets pin(s) as analog
 * @note   Pins HAVE to be initialized first using @ref MD_GPIO_Init() or @ref MD_GPIO_InitAlternate() function
 * @note   This is just an option for fast analog mode 
 * @param  GPIOx: GPIOx PORT where you want to set pin as analog
 * @param  GPIO_Pin: Select GPIO pin(s). You can select more pins with | (OR) operator to set them as analog
 * @retval None
 */
void MD_GPIO_SetPinAsAnalog(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

/** 
 * @brief  Sets pin(s) as alternate function
 * @note   For proper alternate function, you should first init pin using @ref MD_GPIO_InitAlternate() function.
 *            This functions is only used for changing GPIO mode
 * @param  GPIOx: GPIOx PORT where you want to set pin as alternate
 * @param  GPIO_Pin: Select GPIO pin(s). You can select more pins with | (OR) operator to set them as alternate
 * @retval None
 */
void MD_GPIO_SetPinAsAlternate(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

/**
 * @brief  Sets pull resistor settings to GPIO pin(s)
 * @note   Pins HAVE to be initialized first using @ref MD_GPIO_Init() or @ref MD_GPIO_InitAlternate() function
 * @param  *GPIOx: GPIOx PORT where you want to select pull resistor
 * @param  GPIO_Pin: Select GPIO pin(s). You can select more pins with | (OR) operator to set them as output
 * @param  GPIO_PuPd: Pull resistor option. This parameter can be a value of @ref MD_GPIO_PuPd_t enumeration
 * @retval None
 */
void MD_GPIO_SetPullResistor(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, MD_GPIO_PuPd_t GPIO_PuPd);

/**
 * @brief  Sets pin(s) low
 * @note   Defined as macro to get maximum speed using register access
 * @param  GPIOx: GPIOx PORT where you want to set pin low
 * @param  GPIO_Pin: Select GPIO pin(s). You can select more pins with | (OR) operator to set them low
 * @retval None
 */
#define MD_GPIO_SetPinLow(GPIOx, GPIO_Pin)          ((GPIOx)->BSRRH = (GPIO_Pin))

/**
 * @brief  Sets pin(s) high
 * @note   Defined as macro to get maximum speed using register access
 * @param  GPIOx: GPIOx PORT where you want to set pin high
 * @param  GPIO_Pin: Select GPIO pin(s). You can select more pins with | (OR) operator to set them high
 * @retval None
 */
#define MD_GPIO_SetPinHigh(GPIOx, GPIO_Pin)         ((GPIOx)->BSRRL = (GPIO_Pin))

/**
 * @brief  Sets pin(s) value
 * @note   Defined as macro to get maximum speed using register access
 * @param  GPIOx: GPIOx PORT where you want to set pin value
 * @param  GPIO_Pin: Select GPIO pin(s). You can select more pins with | (OR) operator to set them value
 * @param  val: If parameter is 0 then pin will be low, otherwise high
 * @retval None
 */
#define MD_GPIO_SetPinValue(GPIOx, GPIO_Pin, val)   ((val) ? MD_GPIO_SetPinHigh(GPIOx, GPIO_Pin) : MD_GPIO_SetPinLow(GPIOx, GPIO_Pin))

/**
 * @brief  Toggles pin(s)
 * @note   Defined as macro to get maximum speed using register access
 * @param  GPIOx: GPIOx PORT where you want to toggle pin value
 * @param  GPIO_Pin: Select GPIO pin(s). You can select more pins with | (OR) operator to toggle them all at a time
 * @retval None
 */
#define MD_GPIO_TogglePinValue(GPIOx, GPIO_Pin)     ((GPIOx)->ODR ^= (GPIO_Pin))

/**
 * @brief  Sets value to entire GPIO PORT
 * @note   Defined as macro to get maximum speed using register access
 * @param  GPIOx: GPIOx PORT where you want to set value
 * @param  value: Value for GPIO OUTPUT data
 * @retval None
 */
#define MD_GPIO_SetPortValue(GPIOx, value)          ((GPIOx)->ODR = (value))

/**
 * @brief  Gets input data bit
 * @note   Defined as macro to get maximum speed using register access
 * @param  GPIOx: GPIOx PORT where you want to read input bit value
 * @param  GPIO_Pin: GPIO pin where you want to read value
 * @retval 1 in case pin is high, or 0 if low
 */
#define MD_GPIO_GetInputPinValue(GPIOx, GPIO_Pin)   (((GPIOx)->IDR & (GPIO_Pin)) == 0 ? 0 : 1)

/**
 * @brief  Gets output data bit
 * @note   Defined as macro to get maximum speed using register access
 * @param  GPIOx: GPIOx PORT where you want to read output bit value
 * @param  GPIO_Pin: GPIO pin where you want to read value
 * @retval 1 in case pin is high, or 0 if low
 */
#define MD_GPIO_GetOutputPinValue(GPIOx, GPIO_Pin)  (((GPIOx)->ODR & (GPIO_Pin)) == 0 ? 0 : 1)

/**
 * @brief  Gets input value from entire GPIO PORT
 * @note   Defined as macro to get maximum speed using register access
 * @param  GPIOx: GPIOx PORT where you want to read input data value
 * @retval Entire PORT INPUT register
 */
#define MD_GPIO_GetPortInputValue(GPIOx)            ((GPIOx)->IDR)

/**
 * @brief  Gets output value from entire GPIO PORT
 * @note   Defined as macro to get maximum speed using register access
 * @param  GPIOx: GPIOx PORT where you want to read output data value
 * @retval Entire PORT OUTPUT register
 */
#define MD_GPIO_GetPortOutputValue(GPIOx)           ((GPIOx)->ODR)

/**
 * @brief  Gets port source from desired GPIOx PORT
 * @note   Meant for private use, unless you know what are you doing
 * @param  GPIOx: GPIO PORT for calculating port source
 * @retval Calculated port source for GPIO
 */
uint16_t MD_GPIO_GetPortSource(GPIO_TypeDef* GPIOx);

/**
 * @brief  Gets pin source from desired GPIO pin
 * @note   Meant for private use, unless you know what are you doing
 * @param  GPIO_Pin: GPIO pin for calculating port source
 * @retval Calculated pin source for GPIO pin
 */
uint16_t MD_GPIO_GetPinSource(uint16_t GPIO_Pin);

/**
 * @brief  Locks GPIOx register for future changes
 * @note   You are not able to config GPIO registers until new MCU reset occurs
 * @param  *GPIOx: GPIOx PORT where you want to lock config registers
 * @param  GPIO_Pin: GPIO pin(s) where you want to lock config registers
 * @retval None
 */
void MD_GPIO_Lock(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

/** 
 * @brief  Gets bit separated pins which were used at least once in library and were not deinitialized
 * @param  *GPIOx: Pointer to GPIOx peripheral where to check used GPIO pins
 * @retval Bit values for used pins
 */
uint16_t MD_GPIO_GetUsedPins(GPIO_TypeDef* GPIOx);

/** 
 * @brief  Gets bit separated pins which were not used at in library or were deinitialized
 * @param  *GPIOx: Pointer to GPIOx peripheral where to check used GPIO pins
 * @retval Bit values for free pins
 */
uint16_t MD_GPIO_GetFreePins(GPIO_TypeDef* GPIOx);

/**
 * @}
 */
/**
 * @}
 */
/**
 * @}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif
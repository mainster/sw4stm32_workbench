/**
 * @file        mdb_gpio.h
 *
 * @date        28 Jan 2017
 * @author      Manuel Del Basso (mainster)
 * @email       manuel.delbasso@gmail.com
 *
 * @ide         System Workbench ac6 (eclipse stm32)
 * @stdperiph   STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 * @license		GNU GPL v3
 *
 * @brief       Project MD_APP scanner specific pin mappings.
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
#ifndef _MDB_GPIO_H_
#define _MDB_GPIO_H_

#ifdef __cplusplus
 extern "C" {
#endif 
  
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

/**
 * @addtogroup MD_APP
 * @{
 */

/**
 * @addtogroup APP_GPIO General Purpose I/O
 * @{
 */

/**
 * @addtogroup GPIO_Macros
 * @{
 */

/**
 * @brief      Define the number of enumerated pins in MD_GPIO_NAME_t  
 */
#define   GPIO_COUNT   11

/**
 * @brief      Provides pin toggle functionality for timing related debugging
 *             purposes.
 */
#define   DBG_PID_TIMING_TOG()    (MDB_GPIO_Toggle(DBG_TIMING_PE6))

/**
 * @brief      Provides Tic functionality for timing related debugging
 *             purposes.
 */
#define   DBG_PID_TIMING_TIC()    (MDB_GPIO_On(DBG_TIMING_PE6))

/**
 * @brief      Provides Toc functionality for timing related debugging
 *             purposes.
 */
#define   DBG_PID_TIMING_TOC()    (MDB_GPIO_Off(DBG_TIMING_PE6))

/**
 * @brief      Provides pin set/clear functionality for PID timing debugging
 *             purposes.
 *
 * @param      s     0: Set debug port to state "off"
 *                  >0: Set debug port to state "on"
 */
#define   DBG_PID_TIMING(s)       ((s) > 0) ? (MDB_GPIO_On(DBG_TIMING_PE6)) : \
                                              (MDB_GPIO_Off(DBG_TIMING_PE6))

/**
 * @brief      Provides pin toggle functionality for ADC timing related
 *             debugging purposes.
 */
#define   DBG_ADC_TIMING_TOG( )   (MDB_GPIO_Toggle(DBG_TIMING_PE4))

/**
 * @brief      Provides pin set/clear functionality for ADC timing debugging
 *             purposes.
 *
 * @param      s     0: Set debug port to state "off"
 *                  >0: Set debug port to state "on"
 */
#define   DBG_ADC_TIMING(s)       ((s) > 0) ? (MDB_GPIO_On(DBG_TIMING_PE4)) : \
                                              (MDB_GPIO_Off(DBG_TIMING_PE4))

/** @} */

/**
 * @addtogroup GPIO_Typedefs
 * @{
 */

/**
 * @brief      GPIO port pin naming enumeration.
 */
typedef enum {
  LED_GREEN_A       = 0,    //!< LED3 on STM32F429-Discovery.
  LED_RED_A         = 1,    //!< LED4 on STM32F429-Discovery.
  BEAM_INTERRUPT    = 2,    //!< Beam interrupter control pin.
  TRIGGER_SRC       = 3,    //!< Connected to Oscilloscope trigger in. 
  DBG_TIMING_PE2    = 4,    //!< Not used!
  DBG_TIMING_PE4    = 5,    //!< Debug timing of ADC related tasks.
  DBG_TIMING_PE6    = 6,    //!< Debug timing of PID algorithm.
  ADC_X_CHAN_IO     = 7,    //!< ADC AN PA6
  ADC_Y_CHAN_IO     = 8,    //!< ADC AN PB0
  ADC_Ix_CHAN_IO    = 9,    //!< ADC AN PC3
  ADC_W_CHAN_IO     = 10,   //!< ADC AN PA3
} MD_GPIO_NAME_t;

/**
 * @brief      GPIO states enumeration.
 */
typedef enum {
  GPIO_OFF = 0,  
  GPIO_ON        
} MD_GPIO_STATE_t;

/**
 * @brief      GPIO output driver state enumeration.
 *
 * @attention  This type is safety related. To prevent unwanted
 *             beam-enabled-states in case of misspelled argument invocations on
 *             MDB_GPIO_BeamPin.
 */
typedef enum {
  GPIO_DRIVER_OFF = 0,  
  GPIO_DRIVER_ON        
} MD_GPIO_DRIVER_STATE_t;

/**
 * @brief      User level "per-GPIO" typedf to realize a "per GPIO"
 *             configuration array MDB_GPIO.
 */
typedef struct {
  const uint16_t      _GPIO_PIN;        //!< GPIO pin address.
  const uint32_t      _GPIO_AHB_CLK;    //!< GPIO AHB peripheral clock source .
  GPIO_TypeDef*       _GPIO_PORT;       //!< GPIO port pointer.
  GPIOMode_TypeDef    _GPIO_Mode;       //!< GPIO mode.
  GPIOOType_TypeDef   _GPIO_OType;      //!< GPIO output type.
  GPIOPuPd_TypeDef    _GPIO_PuPd;       //!< GPIO pull resistor.
  GPIOSpeed_TypeDef   _GPIO_Speed;      //!< GPIO driving strength.
  MD_GPIO_NAME_t      _GPIO_NAME;       //!< GPIO name
  MD_GPIO_STATE_t     _GPIO_INIT;       //!< Initial GPIO state
} MDB_GPIO_t;

/** @} */

/**
 * @addtogroup GPIO_Variables
 * @{
 */

/** @} */

/**
 * @addtogroup GPIO_Functions
 * @{
 */

/**
 * @brief      Provides GPIO pin toggle functionality for MD_GPIO_NAME_t types
 *             defined in MDB_GPIO.
 *
 * @param[in]  name   Pass the GPIO name of type MD_GPIO_NAME_t.
 *
 * @note       Use in consumption with MDB_GPIO.
 */
void MDB_GPIO_Toggle(MD_GPIO_NAME_t name);

/**
 * @brief      Provides GPIO on/off functionality.
 *
 * @param[in]  name       Pass the GPIO name of type MD_GPIO_NAME_t.
 * @param[in]  newState   Pass the new state for GPIO name. 
 */
void MDB_GPIO_Switch(MD_GPIO_NAME_t name, MD_GPIO_STATE_t newState);

/**
 * @brief      { function_description }
 *
 * @param[in]  name   The name
 */
void MDB_GPIO_On(MD_GPIO_NAME_t name);

//#define MDB_GPIO_On_m(name)     MDB_GPIO[name]._GPIO_PORT->BSRRL = MDB_GPIO[name]._GPIO_PIN
//#define MDB_GPIO_Toggle_m(name) MDB_GPIO[name]._GPIO_PORT->ODR ^= MDB_GPIO[name]._GPIO_PIN

#define   MDB_GPIO_Toggle_m2(s)       ((s) > 0) ? (MDB_GPIO_On(DBG_TIMING_PE6)) \
												: (MDB_GPIO_Off(DBG_TIMING_PE6))


void MDB_GPIO_Off(MD_GPIO_NAME_t name);
void MDB_GPIO_Init(void);
void MDB_GPIO_BeamPin(MD_GPIO_NAME_t name, MD_GPIO_DRIVER_STATE_t newState);
void gpio_init_mco1(void);
void gpio_init_mco2(void);

/** @} */

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

#endif

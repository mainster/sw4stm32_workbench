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

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"


typedef enum {
  LED_GREEN_A       = 0,    ///< LED3 on STM32F429-Discovery
  LED_RED_A         = 1,    ///< LED4 on STM32F429-Discovery
  BEAM_INTERRUPT    = 2,    ///< Beam interrupter control pin
  TRIGGER_SRC       = 3,
  DBG_TIMING_PE2    = 4,
  DBG_TIMING_PE4    = 5,    ///< Debug timing of ADC related tasks    
  DBG_TIMING_PE6    = 6,    ///< Debug timing of PID algorithm
  ADC_X_CHAN_IO     = 7,    ///< ADC AN PA6
  ADC_Y_CHAN_IO     = 8,    ///< ADC AN PB0
  ADC_Ix_CHAN_IO    = 9,    ///< ADC AN PC3
  ADC_W_CHAN_IO     = 10,   ///< ADC AN PA3
  
} MDB_GPIO_NAME_t;

#define  GPIO_COUNT   11

#define DBG_PID_TIMING_TOG( )   (MDB_GPIO_Toggle(DBG_TIMING_PE6))
#define DBG_PID_TIMING(s)       ((s) > 0) ? (MDB_GPIO_On(DBG_TIMING_PE6)) : \
                                            (MDB_GPIO_Off(DBG_TIMING_PE6))
#define DBG_ADC_TIMING_TOG( )   (MDB_GPIO_Toggle(DBG_TIMING_PE4))
#define DBG_ADC_TIMING(s)       ((s) > 0) ? (MDB_GPIO_On(DBG_TIMING_PE4)) : \
                                            (MDB_GPIO_Off(DBG_TIMING_PE4))

/**< GPIO (init) states 
 */
typedef enum {
  GPIO_OFF = 0,  // off
  GPIO_ON        // on
} MDB_GPIO_STATE_t;

typedef enum {
  GPIO_DRIVER_OFF = 0,  // off
  GPIO_DRIVER_ON        // on
} MDB_GPIO_DRIVER_STATE_t;


/**< GPIO structure
 */
typedef struct {

  const uint16_t      _GPIO_PIN;       ///< GPIO Pin address
  const uint32_t      _GPIO_AHB_CLK;       ///< GPIO AHB peripheral clock source 
  GPIO_TypeDef*       _GPIO_PORT;      ///< GPIO Port
  GPIOMode_TypeDef    _GPIO_Mode;
  GPIOOType_TypeDef   _GPIO_OType;
  GPIOPuPd_TypeDef    _GPIO_PuPd;
  GPIOSpeed_TypeDef   _GPIO_Speed;
  MDB_GPIO_NAME_t     _GPIO_NAME;      ///< GPIO name
  MDB_GPIO_STATE_t    _GPIO_INIT;      ///< GPIO initial state

} MDB_GPIO_t;

void MDB_GPIO_Toggle(MDB_GPIO_NAME_t name);
void MDB_GPIO_Switch(MDB_GPIO_NAME_t name, MDB_GPIO_STATE_t newState);
void MDB_GPIO_On(MDB_GPIO_NAME_t name);
void MDB_GPIO_Off(MDB_GPIO_NAME_t name);
void MDB_GPIO_Init(void);
void MDB_GPIO_BeamPin(MDB_GPIO_NAME_t name, MDB_GPIO_DRIVER_STATE_t newState);
void gpio_init_mco1(void);
void gpio_init_mco2(void);

#endif

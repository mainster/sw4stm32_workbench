/**
 *	Keil project for XY-GalvoScanner
 *  29-04-2015
 *
 *
 *	@author		Manuel Del Basso
 *	@email		Manuel.DelBasso@googlemail.com  
 *	@ide		Keil uVision 5
 *	@packs		STM32F4xx Keil packs version 2.2.0 or greater required
 *	@stdperiph	STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 */
#ifndef _MDB_GPIO_H
#define _MDB_GPIO_H

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

/**< Name enumeration of ALL used GPIOs
 */
typedef enum 
{
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
  MDB_GPIO_NAME_t   _GPIO_NAME;      ///< GPIO name
  GPIO_TypeDef*     _GPIO_PORT;      ///< GPIO Port
  const uint16_t   _GPIO_PIN;       ///< GPIO Pin address
  GPIOMode_TypeDef  _GPIO_Mode;
  GPIOSpeed_TypeDef _GPIO_Speed;
  GPIOOType_TypeDef _GPIO_OType;
  GPIOPuPd_TypeDef  _GPIO_PuPd;
  const uint32_t   _GPIO_AHB_CLK;       ///< GPIO AHB peripheral clock source 
  MDB_GPIO_STATE_t _GPIO_INIT;      ///< GPIO initial state
} MDB_GPIO_t;

void MDB_GPIO_Toggle(MDB_GPIO_NAME_t name);
void MDB_GPIO_Switch(MDB_GPIO_NAME_t name, MDB_GPIO_STATE_t newState);
void MDB_GPIO_On(MDB_GPIO_NAME_t name);
void MDB_GPIO_Off(MDB_GPIO_NAME_t name);
void MDB_GPIO_Init(void);
void MDB_GPIO_BeamPin(MDB_GPIO_NAME_t name, MDB_GPIO_DRIVER_STATE_t newState);


#endif

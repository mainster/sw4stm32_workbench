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
  DBG_TIMING_PE4    = 5,
  DBG_TIMING_PE6    = 6,
} MDB_GPIO_NAME_t;

#define  GPIO_COUNT   7


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


//--------------------------------------------------------------
// Globale Funktionen
//--------------------------------------------------------------
//void UB_Led_Init(void);
//void UB_Led_Off(LED_NAME_t led_name);
//void UB_Led_On(LED_NAME_t led_name);
//void UB_Led_Toggle(LED_NAME_t led_name);
//void UB_Led_Switch(LED_NAME_t led_name, LED_STATUS_t wert);




#endif

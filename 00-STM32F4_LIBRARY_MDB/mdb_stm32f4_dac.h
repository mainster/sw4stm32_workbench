#ifndef __MDB_STM32F4xx_DAC_H
#define __MDB_STM32F4xx_DAC_H

#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_dac.h"
#include "defines.h"
//#include "md_stm32f4_dac.h"                  // Device header

/** @addtogroup DAC_SignalGeneration
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define DAC_DHR12R1_ADDRESS    0x40007408
#define DAC_DHR12R2_ADDRESS    0x40007414
#define DAC_DHR8R1_ADDRESS     0x40007410

/* =======================================================
 * ===== Find base addresses of peripheral registers =====
 * =======================================================
 *
 * stm32f4xx.h (1439):  #define PERIPH_BASE           ((uint32_t)0x40000000) 
 *             (1463):  #define APB1PERIPH_BASE       PERIPH_BASE
 *             (1495):  #define DAC_BASE             (APB1PERIPH_BASE + 0x7400)
 *
 *          ==> Register address = 0x40007400 + Register_offset
 */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
DAC_InitTypeDef     DAC_InitStructure;
DMA_InitTypeDef     DMA_InitStructure;


void DAC_Ch1_EscalatorConfig(void);
void DAC_DMA_SineWaveConfig(DAC_Channel_  l_t DACx, FunctionalState NewStateDAC, FunctionalState NewStateDMA);
void DAC_Ch1_NoiseConfig(void);

#endif

/**
 * @file        
 *
 * @date        
 * @author      Manuel Del Basso (mainster)
 * @email       manuel.delbasso@gmail.com
 *
 * @ide         System Workbench ac6 (eclipse stm32)
 * @stdperiph   STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 * @license     GNU GPL v3
 * 
 * @brief       Provides initialization methods to configure analog subsystems and DMA controller. 
 * 
   @verbatim

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

    @endverbatim
 *
 */
#ifndef _PERIPH2DMA_INIT_H_
#define _PERIPH2DMA_INIT_H_

#include "stm32f4xx.h"
#include "md_stm32f4_dac.h"

/**
 * @addtogroup MD_APP
 * @{
 */

/**
 * @addtogroup  APP_Periph2dma Analog DMA Config
 * @{
 *
 * @par Digital to analog converter:
 *     An important factor in choosing the appropriate controller was the
 *     integrated dual channel DAC peripheral. Two instances of digital PID
 *     controllers are used to calculate the analog actuator control signals.
 *
 *
 * @par Timer-triggered ADC-scan mode with DMA to memory
 *
 * @attention   Initial start of ADC conversion:
 *
 * @li          Once initialized, first ADC conversion must be started by a
 *              ADC_SoftwareStartConv or whereever the first channel of the
 *              regular group runs (ADC1 ,2 ,3).
 *
 * @li          Timer controlled group scan restart is configured by
 *              *.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_TRGO; This
 *              means NOT that start-events for ADC regular scan are generated
 *              by the Timer update ISR!!!
 *
 * @li          Configure TIM for generating Output Trigger events!
 *              TIM_SelectOutputTrigger(TIM2,TIM_TRGOSource_Update);
 */

/**
 * @addtogroup Periph2dma_Macros
 * @{
 */

/**
 * @brief      Process output channel for y (ADC)
 */
#define ADC_X_CHAN          ADC_Channel_6

/**
 * @brief      Process output channel for y (ADC)
 */
#define ADC_Y_CHAN          ADC_Channel_8

/**
 * @brief      Actuator coil current sense channel x (I_sens_x)
 */
#define ADC_Ix_CHAN         ADC_Channel_13

/**
 * @brief      Actuator coil current sense channel x (I_sens_y)
 */
#define ADC_Iy_CHAN         ADC_Channel_3

/**
 * @brief      Alternate set-point input for channel x 
 */
#define ADC_W_CHAN          ADC_Channel_3

/**
 * @brief      DMA peripheral base address.
 */
#define ADC_CCR_ADDRESS    ((uint32_t)0x40012308)

/**
 * @brief      Sets the ADC scan channels count.
 *
 *             This MUST be used in ADC init function, DMA init function and at
 *             the point where a complete buffer copy was taken
 */
#define ADC_N_REGULAR_CHANNELS  2

/** @} */

/**
 * @addtogroup Periph2dma_Functions
 * @{
 */

/**
 * @brief      { function_description }
 *
 * @param[in]  TimerRun   The timer run
 * @param[in]  IntOn      The int on
 * @param[in]  peri       The peri
 */
void TIM2_DMA_Trigger_Config (FunctionalState TimerRun,
                              FunctionalState IntOn, uint16_t peri);

/**
 * @brief      { function_description }
 *
 * @param[in]  int16_t   The int 16 t
 * @param[in]  memSize   This is an integer value, the DMA controller transfers
 *                       memSize datagrams befor circulating. Also memSize ADC
 *                       regular channels have to be configured within this init
 *                       procedure (ADC_RegularChannelConfig)
 */
void DMA_Config ( __IO int16_t *MultiConvBuff, uint8_t memSize);
void AN_Watchdog_Config (void);

/**
 * @brief      ADC common and regular scan group init
 */
void ADC_Scan_Group_Config (void);
void RCC_Configuration (void);
void ADC_DMA_DualModeConfig (__IO int16_t *MultiConvBuff);
void ADC_ContScanMode_w_DMA  (__IO int16_t *MultiConvBuff);
void ADC_ContScanMode_w_DMA_timeTrigd  (__IO int16_t *MultiConvBuff, uint8_t memSize);
void DAC_SetSignedValue (MD_DAC_Channel_t DACx, int16_t val);

/** @} */

/** @} */

/** @} */

#endif

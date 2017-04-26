/**
 * @file        periph2dma_conf.c
 *
 * @date        
 * @author      Manuel Del Basso (mainster)
 * @email       manuel.delbasso@gmail.com
 *
 * @ide         System Workbench ac6 (eclipse stm32)
 * @stdperiph   STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 * @license     GNU GPL v3
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
#include "periph2dma_conf.h"

/* ====================================================================================
                    Enable AN, GPIO, TIM and DMA clocks
   ==================================================================================== */
void RCC_Periph2DMA_Conf(void) {
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
}

/* ====================================================================================
                    ADC common and regular scan group init
   ==================================================================================== */
void ADC_Scan_Group_Config(void)
{
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  ADC_InitTypeDef ADC_InitStructure;

  /* ADC Common Init */
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);

  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_TRGO;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 0x02; /* 5 channels in total */
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular channel selection
   * @NOTE Changes to this config affects the channel index for ADC_MultiConvBuff[ ]
   * in the PID algorithm function */
  ADC_RegularChannelConfig(ADC1, ADC_X_CHAN , 1, ADC_SampleTime_3Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_Y_CHAN , 2, ADC_SampleTime_3Cycles);
//    ADC_RegularChannelConfig(ADC1, ADC_Ix_CHAN, 3, ADC_SampleTime_3Cycles);
//    ADC_RegularChannelConfig(ADC1, ADC_W_CHAN , 4, ADC_SampleTime_3Cycles);

  /* Enable DMA request after last transfer (Single-ADC mode) */
  ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

  /* Enable ADC1 DMA */
  ADC_DMACmd(ADC1, ENABLE);

  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);
}

/* ====================================================================================
                    Configure ADC Analog watchdog
   ==================================================================================== */
void AN_Watchdog_Config (void) {
  ADC_AnalogWatchdogSingleChannelConfig(ADC1, ADC_Y_CHAN);
  ADC_AnalogWatchdogThresholdsConfig(ADC1, 0x7ff, 0);
  ADC_AnalogWatchdogCmd(ADC1, ADC_AnalogWatchdog_SingleRegEnable);
//  ADC_AnalogWatchdogCmd(ADC1, ADC_AnalogWatchdog_AllRegEnable);
}

/* ====================================================================================
                    DMA2 Stream0 channel0 configuration
   ==================================================================================== */
void DMA_Config( __IO int16_t *MultiConvBuff, uint8_t memSize) {
  DMA_InitTypeDef DMA_InitStructure;

  /* Initialise DMA */
  DMA_StructInit(&DMA_InitStructure);

  /* config DMA Controller */
  DMA_InitStructure.DMA_Channel = DMA_Channel_0; /* See Tab 20 */
  DMA_InitStructure.DMA_BufferSize = memSize; /* 5 * memsize */
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory; /* direction */

  /*
   *if 4 or more channels / conversions take place, enable FIFO
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enabled;
  DMA_InitStructure.DMA_FIFOThreshold = 1/4 oder so;
   */
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable; /* no FIFO */
  DMA_InitStructure.DMA_FIFOThreshold = 0;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; /* circular buffer */
  DMA_InitStructure.DMA_Priority = DMA_Priority_High; /* high priority */

  /* config target memory */
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)MultiConvBuff; /* target addr */
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; /* 16 bit */
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_Init(DMA2_Stream0, &DMA_InitStructure); /* See Table 20 for mapping */

  /* Enable DMA Stream Half / Transfer Complete interrupt */
  DMA_ITConfig(DMA2_Stream0, DMA_IT_TC | DMA_IT_HT, ENABLE);

  DMA_Cmd(DMA2_Stream0, ENABLE);
}



void TIM2_DMA_Trigger_Config(FunctionalState TimerRun, \
                             FunctionalState IntOn, uint16_t peri) {
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

  /* Time base configuration */
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  TIM_TimeBaseStructure.TIM_Period = peri;
  TIM_TimeBaseStructure.TIM_Prescaler = 1;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  /* TIM2 TRGO selection */
  TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update); // ADC_ExternalTrigConv_T2_TRGO
  TIM_ITConfig(TIM2, TIM_IT_Update, IntOn);
  /* TIM2 enable counter */
  TIM_Cmd(TIM2, TimerRun);
}


#ifdef ADC_DMA_BIS_19_05_2015
void ADC_ContScanMode_w_DMA_timeTrigd (__IO int16_t *MultiConvBuff,
                                       uint8_t memSize) {

  /* ====================================================================================
                      Define init structures
     ==================================================================================== */
  ADC_InitTypeDef         ADC_InitStructure;
  ADC_CommonInitTypeDef   ADC_CommonInitStructure;
  DMA_InitTypeDef         DMA_InitStructure;
  GPIO_InitTypeDef        GPIO_InitStructure;
  NVIC_InitTypeDef        NVIC_InitStructure;

  /* Enable timer as It's update interrupt is used to trigger a new
   * regular channel conversion sequence */

  /* ====================================================================================
                      Enable AN GPIO and DMA clocks
     ==================================================================================== */
  RCC_AHB1PeriphClockCmd(  RCC_AHB1Periph_GPIOB |
                         RCC_AHB1Periph_GPIOC |
                         RCC_AHB1Periph_DMA2, ENABLE);

  /* ====================================================================================
                      ADC GPIO init
     ==================================================================================== */
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3 | GPIO_Pin_6 ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0 ;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* ====================================================================================
                      DMA2 Stream0 channel0 configuration
     ==================================================================================== */
  /* Initialise DMA */
  DMA_StructInit(&DMA_InitStructure);

  /* config DMA Controller */
  DMA_InitStructure.DMA_Channel = DMA_Channel_0; /* See Tab 20 */
  DMA_InitStructure.DMA_BufferSize = memSize; /* 5 * memsize */
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory; /* direction */
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable; /* no FIFO */
  DMA_InitStructure.DMA_FIFOThreshold = 0;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; /* circular buffer */
  DMA_InitStructure.DMA_Priority = DMA_Priority_High; /* high priority */
  /* config receive memory */
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)MultiConvBuff; /* target addr */
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; /* 16 bit */
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_Init(DMA2_Stream0, &DMA_InitStructure); /* See Table 20 for mapping */
  DMA_Cmd(DMA2_Stream0, ENABLE);

  /* ====================================================================================
                      DMA transmission-complete interrupt config
     ==================================================================================== */
  /**< configure nested vectored interrupt controller for DMA transmission complete
       * High priority needed because TC IRQ determines the output sample precision
       * through DAC hardware components. */
  DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);
  DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TC);

  NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 6;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* ====================================================================================
                      ADC common init
     ==================================================================================== */
  /* IMPORTANT: populate default values before use */
  ADC_StructInit(&ADC_InitStructure);
  ADC_CommonStructInit(&ADC_CommonInitStructure);

  /* reset configuration if needed, could be used for previous init */
  ADC_Cmd(ADC1, DISABLE);
  ADC_DeInit();

  /* init ADC clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

  /* init ADC */
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;

  /* Yes, ADC_DMAAccessMode_Disabled is correct */
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);

  /* ====================================================================================
                      ADC regular channel config
     ==================================================================================== */
  /* ADC1 Init: this is mostly done with ADC1->CR */
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_TRGO;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = memSize; /* 5 channels in total */
  ADC_Init(ADC1, &ADC_InitStructure);

  /** Configure channels
   * @NOTE Changes to this config affects the channel index for ADC_MultiConvBuff[ ]
   * in the PID algorithm function */
  ADC_RegularChannelConfig(ADC1, ADC_X_CHAN , 1, ADC_SampleTime_3Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_Y_CHAN , 2, ADC_SampleTime_3Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_Ix_CHAN, 3, ADC_SampleTime_3Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_W_CHAN , 4, ADC_SampleTime_3Cycles);

  /* ....................................................................................
                      ADC end-of-conversion interrupt config
     .................................................................................... */
  /*
   * If higher ADCx used, remember to set IT_EOC config als to ADCx* /
      ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);

      / **< configure vectored interrupt controller
       * High priority needed because EOC IRQ determines the output sample precision
       * through DAC hardware components. * /
      NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQn;
      NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
      NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x04;
      NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
      NVIC_Init(&NVIC_InitStructure);
      */


  /* ====================================================================================
                      Configure ADC Analog watchdog
     ==================================================================================== */
  ADC_AnalogWatchdogSingleChannelConfig(ADC1, ADC_Y_CHAN);
  ADC_AnalogWatchdogThresholdsConfig(ADC1, 0x7ff, 0);
  ADC_AnalogWatchdogCmd(ADC1, ADC_AnalogWatchdog_SingleRegEnable);
//    ADC_AnalogWatchdogCmd(ADC1, ADC_AnalogWatchdog_AllRegEnable);

  /* ====================================================================================
                      ADC Analog watchdog interrupt enable
     ==================================================================================== */
  NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  ADC_ITConfig(ADC1, ADC_IT_AWD, ENABLE);

  /* Enable DMA request after last transfer (Single-ADC mode) */
  ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

  /* Enable ADC1 DMA */


  /* in main */
//    TIM_Cmd(TIM2, ENABLE);
//    ADC_Cmd(ADC1, ENABLE);
//    ADC_SoftwareStartConv(ADC1);

}

#endif




#ifdef ADC_DMA_DUALMODECONFIG

void ADC_DMA_DualModeConfig(__IO int16_t *MultiConvBuff) {
  ADC_CommonInitTypeDef ADC_CommonInitStructure;

  /* ====================================================================================
                      Enable peripheral clocks
     ==================================================================================== */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);

  /* ====================================================================================
                      DMA2 Stream0 channel0 configuration
     ==================================================================================== */
  DMA_InitTypeDef DMA_InitStructure;

  /*  The transfer stops once the DMA_SxNDTR register reaches zero, when the peripheral
   *  requests the end of transfers (in case of a peripheral flow controller) or when the
   *   EN bit in the DMA_SxCR register is cleared by software.    S306
   */
  DMA_InitStructure.DMA_Channel = DMA_Channel_0;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) MultiConvBuff;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) ADC_CCR_ADDRESS;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;


  DMA_InitStructure.DMA_BufferSize = 2;


  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA2_Stream0, &DMA_InitStructure);

  /* ====================================================================================
                      DMA transmission-complete interrupt config
     ==================================================================================== */
  // DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);
  DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, DISABLE);
  DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TC);

  NVIC_InitTypeDef      NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 6;
  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* DMA2_Stream0 enable */
  DMA_Cmd(DMA2_Stream0, DISABLE);

  DMA_DeInit(DMA2_Stream0);
  /* ====================================================================================
                      ADC GPIO init
     ==================================================================================== */
  GPIO_InitTypeDef GPIO_InitStructure;
///<                                    W(t) |   pos_X(t) |   pos_y(t)
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* ====================================================================================
                      ADC common init
     ==================================================================================== */
  ADC_CommonInitStructure.ADC_Mode = ADC_DualMode_RegSimult;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div8;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);

  /* ====================================================================================
                      ADC regular channel config
     ==================================================================================== */
  ADC_InitTypeDef ADC_InitStructure;

  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
//    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_CC2;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_TRGO;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  ADC_Init(ADC1, &ADC_InitStructure);
  /* ADC1 regular channel_06 (pos_X)  */
  ADC_RegularChannelConfig(ADC1, ADC_W_CHAN, 1, ADC_SampleTime_15Cycles);

  ADC_Init(ADC2, &ADC_InitStructure);
  /* ADC2 regular channels 03 (setpoint), 07 (pos_Y)  */
  ADC_RegularChannelConfig(ADC2, ADC_Y_CHAN, 1, ADC_SampleTime_15Cycles);
//    ADC_RegularChannelConfig(ADC2, ADC_W_CHAN, 3, ADC_SampleTime_3Cycles);

  /* ====================================================================================
                      ADC end-of-conversion interrupt config
     ==================================================================================== */
  /* Enable ADC2 end of conversion interrupt */
  ADC_ITConfig(ADC2, ADC_IT_EOC, ENABLE);

  /* configure vectored interrupt controller */
  NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable DMA request after last transfer (Multi-ADC mode)  */
  ADC_MultiModeDMARequestAfterLastTransferCmd(DISABLE);
  ADC_Cmd(ADC1, ENABLE);      ///< Enable ADC1
  ADC_Cmd(ADC2, ENABLE);      ///< Enable ADC2

  /* ====================================================================================  */

  ADC_SoftwareStartConv(ADC1);    ///< Start ADC1 Software Conversion

}
//#pragma O2
#endif

#ifdef ADC_CONTSCANMODE_W_DMA
void ADC_ContScanMode_w_DMA (__IO int16_t *MultiConvBuff) {

  /* Define ADC init structures */
  ADC_InitTypeDef       ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  DMA_InitTypeDef DMA_InitStructure;

  /* Initialise DMA */
  DMA_StructInit(&DMA_InitStructure);

  /* Enable clock on DMA1 */
  /* Enable DMA2, thats where ADC is hooked on -> see Tab 20 (RM00090) */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
  /* config of DMAC */
  DMA_InitStructure.DMA_Channel = DMA_Channel_0;
  DMA_InitStructure.DMA_BufferSize = 2;                       /* 2 * memsize */
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;     /* direction */
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;      /* no FIFO */
  DMA_InitStructure.DMA_FIFOThreshold = 0;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;            /* circular buffer */
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;        /* high priority */
  /* config of memory */
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)MultiConvBuff; /* target address */
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; /* 16 bit */
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; /* increment after wrt */
  /* config of peripheral */
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_Init(DMA2_Stream0, &DMA_InitStructure); /* See Table 20 for mapping */
  DMA_Cmd(DMA2_Stream0, ENABLE);

  ADC_StructInit(&ADC_InitStructure);
  ADC_CommonStructInit(&ADC_CommonInitStructure);

  /* init ADC clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

  /* init ADC */
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);

  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = 0;
  ADC_InitStructure.ADC_ExternalTrigConv = 0;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 2; /* 2 channels in total */
  ADC_Init(ADC1, &ADC_InitStructure);

  /* Enable Vref & Temperature channel */
  ADC_TempSensorVrefintCmd(ENABLE);

  /* Configure channels */
  /* Temp sensor */
  ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_480Cycles);
  /* VREF_int (2nd) */
  ADC_RegularChannelConfig(ADC1, ADC_Channel_17, 2, ADC_SampleTime_480Cycles);

  /* Enable ADC interrupts */
  ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);

  /* Enable DMA request after last transfer (Single-ADC mode) */
  ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

  /* Enable ADC3 DMA */
  ADC_DMACmd(ADC1, ENABLE);
  /* Configure NVIC */
  NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable ADC1 **************************************************************/
  ADC_Cmd(ADC1, ENABLE);
  ADC_SoftwareStartConv(ADC1);
}
#endif
/////////////////////////////////////////////////////////////////////////
//http://survivalengineer.blogspot.de/2013/03/stm32f4-and-most-of-what-you-ever.html
/////////////////////////////////////////////////////////////////////////

/**
  * @brief      Set the specified data holding register value for DAC channel.
  *
  * @param[in]  DACx   The da cx
  * @param      val    signed 16 bit integer truncated to 12bit range. (-2048
  *                    ... 2047) (-0x800 ... 0x7ff)  Data to be loaded.
  * @retval     None   { description }
  */
void DAC_SetSignedValue(MD_DAC_Channel_t DACx, int16_t val) {
    const short OFFS = 2047;

    if (val > 2047)
        val = 2047;
    if (val < -2047)
        val = -2047;

    (DACx == MD_DAC1)   
        ?   DAC_SetChannel1Data(DAC_Align_12b_R, val + OFFS)
        :   DAC_SetChannel2Data(DAC_Align_12b_R, val + OFFS);
}
#ifndef __DB_TIMER_H_ 
#define __DB_TIMER_H_ 
/**
 * Library dependencies

 */
/**
 * Includes
 */
#include "stm32f4xx.h"

#include "../03-STM32F4xx_PID_galvo_2105_0706_ARMPLUG/inc/defines.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
//#include "stm32f4xx_adc.h"

/**
 * This values can be overwriten in defines.h file
 */

//#ifndef TM_ADC3_RESOLUTION
//#define TM_ADC3_RESOLUTION		ADC_Resolution_12b
//#endif

/**
 * Initialize ADCx with ADCx channel
 *
 * Parameters:
 * 	- ADC_TypeDef* ADCx: ADCx
 * 		ADC1, ADC2, ADC3
 * 	- uint8_t channel: channel (pin) for ADCx
 * 		- ADC_Channel_0, ADC_Channel_1, ..., ADC_Channel_15
 */
//extern int TM_Timer2_config(FunctionalState TimerRun, FunctionalState IntOn, uint16_t peri);
//extern int TM_Timer3_config(FunctionalState TimerRun, FunctionalState IntOn, uint16_t peri);
//extern int TM_Timer6_config(FunctionalState TimerRun, FunctionalState IntOn, uint16_t peri);	

void timer2_interrupt_cmd(FunctionalState NewState);					 
void timer3_interrupt_cmd(FunctionalState NewState);					 
int TM_Timer2_Init_adc_sample_trigger(FunctionalState TimerRun, FunctionalState IntOn, uint16_t peri);
int TM_Timer2_config(FunctionalState TimerRun, FunctionalState IntOn, uint16_t peri);
int TM_Timer3_config(FunctionalState TimerRun, FunctionalState IntOn, uint16_t peri);
int TM_Timer6_config(FunctionalState TimerRun, FunctionalState IntOn, uint16_t peri);



#endif

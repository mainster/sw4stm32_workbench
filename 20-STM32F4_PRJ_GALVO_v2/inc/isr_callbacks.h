/**
 *	Keil project for XY-XY-Galvo
 *  29-04-2015
 *
 *
 *	@author		Manuel Del Basso
 *	@email		Manuel.DelBasso@googlemail.com  
 *	@ide		Keil uVision 5
 *	@packs		STM32F4xx Keil packs version 2.2.0 or greater required
 *	@stdperiph	STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 */

#ifndef _ISR_CALLBACKS_H
#define _ISR_CALLBACKS_H

//#include "arm_math.h"
//#include "stm32f4xx.h"
//#include "stm32f4xx_gpio.h"
#include "main.h"
#include "defines.h"


void TIM2_IRQHandler(void);
void TIM4_IRQHandler(void);
void ADC_IRQHandler(void);


#endif


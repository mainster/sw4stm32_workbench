#include "mdb_stm32f4_timer.h"
//#include "misc.h"
//#include ""


void timer2_interrupt_cmd(FunctionalState NewState);					 
void timer3_interrupt_cmd(FunctionalState NewState);					 
int TM_Timer2_Init_adc_sample_trigger(FunctionalState TimerRun, FunctionalState IntOn, uint16_t peri);
int TM_Timer2_config(FunctionalState TimerRun, FunctionalState IntOn, uint16_t peri);
int TM_Timer3_config(FunctionalState TimerRun, FunctionalState IntOn, uint16_t peri);
int TM_Timer6_Config(FunctionalState TimerRun, FunctionalState IntOn, uint16_t peri);


                     
void timer2_interrupt_cmd(FunctionalState NewState){ 
	NVIC_InitTypeDef nvic_cfg;
	// Enable the NVIC if you need to generate the update interrupt. 
	nvic_cfg.NVIC_IRQChannel = TIM2_IRQn;
	nvic_cfg.NVIC_IRQChannelPreemptionPriority = 8;
	nvic_cfg.NVIC_IRQChannelSubPriority = 6;
	nvic_cfg.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic_cfg);
}

void timer3_interrupt_cmd(FunctionalState NewState){ 
	NVIC_InitTypeDef nvic_cfg;
	// Enable the NVIC if you need to generate the update interrupt. 
	nvic_cfg.NVIC_IRQChannel = TIM3_IRQn;
	nvic_cfg.NVIC_IRQChannelPreemptionPriority = 8;
	nvic_cfg.NVIC_IRQChannelSubPriority = 6;
	nvic_cfg.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic_cfg);
}


int TM_Timer2_Init_adc_sample_trigger(FunctionalState TimerRun, FunctionalState IntOn, uint16_t peri) {	
TIM_TimeBaseInitTypeDef timer_conf;
	
// Enable TIM clock using RCC_APBxPeriphClockCmd(RCC_APBxPeriph_TIMx, ENABLE)
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, TimerRun);

	
// Fill the TIM_TimeBaseInitStruct with the desired parameters.
	timer_conf.TIM_Prescaler = 1;
	timer_conf.TIM_CounterMode = TIM_CounterMode_Up;
	timer_conf.TIM_Period = peri;
	timer_conf.TIM_ClockDivision = TIM_CKD_DIV1;
	timer_conf.TIM_RepetitionCounter = 0x00;

// Call TIM_TimeBaseInit(TIMx, &TIM_TimeBaseInitStruct) to configure the Time Base unit
// with the corresponding configuration	
	TIM_TimeBaseInit(TIM2, &timer_conf);

	// Call the TIM_Cmd(ENABLE) function to enable the TIM counter.
	TIM_Cmd(TIM2, ENABLE);

// Enable the corresponding interrupt using the function TIM_ITConfig(TIMx, TIM_IT_Update) 
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);	
	
	timer2_interrupt_cmd(ENABLE);
	return 0;
}



    
    
int TM_Timer2_config(FunctionalState TimerRun, FunctionalState IntOn, uint16_t peri) {	
TIM_TimeBaseInitTypeDef timer_conf;
	
// Enable TIM clock using RCC_APBxPeriphClockCmd(RCC_APBxPeriph_TIMx, ENABLE)
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, TimerRun);

	
// Fill the TIM_TimeBaseInitStruct with the desired parameters.
	timer_conf.TIM_Prescaler = 1;
	timer_conf.TIM_CounterMode = TIM_CounterMode_Up;
	timer_conf.TIM_Period = peri;
	timer_conf.TIM_ClockDivision = TIM_CKD_DIV1;
	timer_conf.TIM_RepetitionCounter = 0x00;

// Call TIM_TimeBaseInit(TIMx, &TIM_TimeBaseInitStruct) to configure the Time Base unit
// with the corresponding configuration	
	TIM_TimeBaseInit(TIM2, &timer_conf);

	// Call the TIM_Cmd(ENABLE) function to enable the TIM counter.
	TIM_Cmd(TIM2, ENABLE);

// Enable the corresponding interrupt using the function TIM_ITConfig(TIMx, TIM_IT_Update) 
	TIM_ITConfig(TIM2, TIM_IT_Update, IntOn);	
	
	timer2_interrupt_cmd(IntOn);
	return 0;
}



int TM_Timer3_config(FunctionalState TimerRun, FunctionalState IntOn, uint16_t peri) {	
TIM_TimeBaseInitTypeDef timer_conf;
	
// Enable TIM clock using RCC_APBxPeriphClockCmd(RCC_APBxPeriph_TIMx, ENABLE)
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, TimerRun);

	
// Fill the TIM_TimeBaseInitStruct with the desired parameters.
	timer_conf.TIM_Prescaler = 1;
	timer_conf.TIM_CounterMode = TIM_CounterMode_Up;
	timer_conf.TIM_Period = peri;
	timer_conf.TIM_ClockDivision = TIM_CKD_DIV1;
	timer_conf.TIM_RepetitionCounter = 0x00;

// Call TIM_TimeBaseInit(TIMx, &TIM_TimeBaseInitStruct) to configure the Time Base unit
// with the corresponding configuration	
	TIM_TimeBaseInit(TIM3, &timer_conf);

	// Call the TIM_Cmd(ENABLE) function to enable the TIM counter.
	TIM_Cmd(TIM3, ENABLE);

// Enable the corresponding interrupt using the function TIM_ITConfig(TIMx, TIM_IT_Update) 
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);	
	
	timer3_interrupt_cmd(ENABLE);
	return 0;
}


//int TM_Timer6_Init(FunctionalState TimerRun, FunctionalState IntOn, uint16_t peri) {	
int TM_Timer6_config(FunctionalState TimerRun, FunctionalState IntOn, uint16_t peri) {	
  TIM_TimeBaseInitTypeDef timer_conf;
    
  /* TIM6 Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, TimerRun);

  /* Time base configuration */
  timer_conf.TIM_Prescaler = 0;
  timer_conf.TIM_CounterMode = TIM_CounterMode_Up; 
  timer_conf.TIM_Period = peri;         //0xff
  timer_conf.TIM_ClockDivision = TIM_CKD_DIV1;
//  TIM_TimeBaseStructInit(&timer_conf);
  TIM_TimeBaseInit(TIM6, &timer_conf);

  /* TIM6 TRGO selection */
  TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);
  
  /* TIM6 enable counter */
  TIM_Cmd(TIM6, TimerRun);
    return 0;
}


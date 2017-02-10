#include "tm_stm32f4_timer.h"
//#include "misc.h"
//#include ""



int TM_Timer2_Init(FunctionalState TimerRun, FunctionalState IntOn, uint16_t peri) {	
    TM_Timer2_config(TimerRun, IntOn, peri);	
    return 0;
}
//void timer2_interrupt_cmd(FunctionalState NewState);					 
//void timer3_interrupt_cmd(FunctionalState NewState);					 
//int TM_Timer2_Init_adc_sample_trigger(FunctionalState TimerRun, FunctionalState IntOn, uint16_t peri);
//int TM_Timer2_config(FunctionalState TimerRun, FunctionalState IntOn, uint16_t peri);
//int TM_Timer3_config(FunctionalState TimerRun, FunctionalState IntOn, uint16_t peri);
//int TM_Timer6_Config(FunctionalState TimerRun, FunctionalState IntOn, uint16_t peri);
//void adc_dma_init(void);
//void TIM2_Config(void);

//#define TIMER2_INTERRUPT
//#define TIMER3_INTERRUPT
//#define TIMER2_INIT_ADC_SAMPLE_TRIGGER
//#define TIMER2_CONFIG_CC
//#define TIMER2_CONFIG
//#define TIMER3_CONFIG
//#define TIMER6_CONFIG
    
    
//#ifdef TIMER2_INTERRUPT
void timer2_interrupt_cmd(FunctionalState NewState){ 
	NVIC_InitTypeDef nvic_cfg;
	// Enable the NVIC if you need to generate the update interrupt. 
	nvic_cfg.NVIC_IRQChannel = TIM2_IRQn;
	nvic_cfg.NVIC_IRQChannelPreemptionPriority = 8;
	nvic_cfg.NVIC_IRQChannelSubPriority = 6;
	nvic_cfg.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic_cfg);
}
//#endif

//#ifdef TIMER3_INTERRUPT
void timer3_interrupt_cmd(FunctionalState NewState){ 
	NVIC_InitTypeDef nvic_cfg;
	// Enable the NVIC if you need to generate the update interrupt. 
	nvic_cfg.NVIC_IRQChannel = TIM3_IRQn;
	nvic_cfg.NVIC_IRQChannelPreemptionPriority = 8;
	nvic_cfg.NVIC_IRQChannelSubPriority = 6;
	nvic_cfg.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic_cfg);
}

//#ifdef TIMER4_INTERRUPT
void timer4_interrupt_cmd(FunctionalState NewState){ 
	NVIC_InitTypeDef nvic_cfg;
	// Enable the NVIC if you need to generate the update interrupt. 
	nvic_cfg.NVIC_IRQChannel = TIM4_IRQn;
	nvic_cfg.NVIC_IRQChannelPreemptionPriority = 8;
	nvic_cfg.NVIC_IRQChannelSubPriority = 6;
	nvic_cfg.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic_cfg);
}
//#endif

/** 
 * Configure TIM2 for generating CC2 trigger event used to initiate a new ADC multichannel
 * conversion (ADC_ExternalTrigConv_T2_CC2 in ADC init function)
 */
/*
int TM_Timer2_Init_adc_sample_trigger_multi(FunctionalState TimerRun, FunctionalState IntOn, uint16_t peri) {	
TIM_TimeBaseInitTypeDef timer_conf;
TIM_OCInitTypeDef 	timer_OcInitStruct;

//  (#) TIM TimeBase management: this group includes all needed functions 
//      to configure the TM Timebase unit:
//    (++) Set/Get Prescaler
//    (++) Counter modes configuration
//    (++) Set Clock division  

    timer_conf.TIM_Prescaler
    timer_conf.TIM_CounterMode
    timer_conf.TIM_ClockDivision
    timer_conf.TIM_Period
    timer_conf.TIM_RepetitionCounter
    
//    (++) Set/Get Autoreload  
//    (++) Select the One Pulse mode
//    (++) Update Request Configuration
//    (++) Update Disable Configuration
//    (++) Auto-Preload Configuration 
//    (++) Enable/Disable the counter     
//               
//  (#) TIM Output Compare management: this group includes all needed 
//      functions to configure the Capture/Compare unit used in Output 
//      compare mode: 
//    (++) Configure each channel, independently, in Output Compare mode
//    (++) Select the output compare modes
//    (++) Select the Polarities of each channel
//    (++) Set/Get the Capture/Compare register values
//    (++) Select the Output Compare Fast mode 
//    (++) Select the Output Compare Forced mode  
//    (++) Output Compare-Preload Configuration 
//    (++) Clear Output Compare Reference
//    (++) Select the OCREF Clear signal
//    (++) Enable/Disable the Capture/Compare Channels 
    
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
}*/

//#ifdef TIMER2_INIT_ADC_SAMPLE_TRIGGER
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
//#endif


//#ifdef TIMER2_CONFIG_CC
int TM_Timer2_config_CC(FunctionalState TimerRun, FunctionalState IntOn, uint16_t peri) {	
TIM_TimeBaseInitTypeDef timer_conf;
TIM_OCInitTypeDef res;
	
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

    /*
// Enable the corresponding interrupt using the function TIM_ITConfig(TIMx, TIM_IT_Update) 
	TIM_ITConfig(TIM2, TIM_IT_Update, IntOn);	
    timer2_interrupt_cmd(IntOn);
    */
//////////////////////////////////////
    res.TIM_OCMode = TIM_OCMode_Timing;
    res.TIM_OutputState = TIM_OutputState_Disable;
    res.TIM_Pulse = peri -2;    /**< Not shure about this !!! 
                                 * Configured as it is: Overflow Interrupt nach peri zyklen, 
                                 * OC2 event nach peri-2 zyklen??? */
    res.TIM_OCPolarity = TIM_OCPolarity_High; /**< Not shure about this !!! */
    
    TIM_OC2Init(TIM2, &res);
    
	return 0;
}
//#endif

    
//#ifdef TIMER2_CONFIG    
int TM_Timer2_config(FunctionalState TimerRun, FunctionalState IntOn, uint16_t peri) {	
TIM_TimeBaseInitTypeDef timer_conf;
	
// Enable TIM clock using RCC_APBxPeriphClockCmd(RCC_APBxPeriph_TIMx, ENABLE)
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	
// Fill the TIM_TimeBaseInitStruct with the desired parameters.
	timer_conf.TIM_Prescaler = 1;
	timer_conf.TIM_CounterMode = TIM_CounterMode_Up;
	timer_conf.TIM_Period = peri;
	timer_conf.TIM_ClockDivision = TIM_CKD_DIV1;
	timer_conf.TIM_RepetitionCounter = 0x00;

    /* Set Timer 2 Update trigger source enabled 
     * ADC start regular group scan is triggerd from that event */
    TIM_SelectOutputTrigger(TIM2,TIM_TRGOSource_Update);
    
    // Call the TIM_Cmd(ENABLE) function to enable the TIM counter.
	TIM_Cmd(TIM2, TimerRun);

// Call TIM_TimeBaseInit(TIMx, &TIM_TimeBaseInitStruct) to configure the Time Base unit
// with the corresponding configuration	
	TIM_TimeBaseInit(TIM2, &timer_conf);

// Enable the corresponding interrupt using the function TIM_ITConfig(TIMx, TIM_IT_Update) 
    TIM_ITConfig(TIM2, TIM_IT_Update, IntOn);	
    /* Try this: Enable TIM_IT_Trigger source for IRQn request. Toggle debug 
     * timing pin and toggle again when dma transmission complete IRQ occures */
    TIM_ITConfig(TIM2, TIM_IT_Trigger, IntOn);	
    
    /* Enable nvic channel */
    timer2_interrupt_cmd(IntOn);
        
	return 0;
}
//#endif

//#ifdef TIMER3_CONFIG
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


int TM_Timer4_config(FunctionalState TimerRun, FunctionalState IntOn, uint16_t peri) {	
TIM_TimeBaseInitTypeDef timer_conf;
	
// Enable TIM clock using RCC_APBxPeriphClockCmd(RCC_APBxPeriph_TIMx, ENABLE)
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, TimerRun);

	
// Fill the TIM_TimeBaseInitStruct with the desired parameters.
	timer_conf.TIM_Prescaler = 1000;
	timer_conf.TIM_CounterMode = TIM_CounterMode_Up;
	timer_conf.TIM_Period = peri;
	timer_conf.TIM_ClockDivision = TIM_CKD_DIV1;
	timer_conf.TIM_RepetitionCounter = 0x00;

// Call TIM_TimeBaseInit(TIMx, &TIM_TimeBaseInitStruct) to configure the Time Base unit
// with the corresponding configuration	
	TIM_TimeBaseInit(TIM4, &timer_conf);

	// Call the TIM_Cmd(ENABLE) function to enable the TIM counter.
	TIM_Cmd(TIM4, ENABLE);

// Enable the corresponding interrupt using the function TIM_ITConfig(TIMx, TIM_IT_Update) 
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);	
	
	timer4_interrupt_cmd(ENABLE);
	return 0;
}


/* Heartbeat */
//int TM_Timer5_config(FunctionalState TimerRun, FunctionalState IntOn, uint16_t peri) {	
//TIM_TimeBaseInitTypeDef timer_conf;
//	
//// Enable TIM clock using RCC_APBxPeriphClockCmd(RCC_APBxPeriph_TIMx, ENABLE)
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, TimerRun);

//	
//// Fill the TIM_TimeBaseInitStruct with the desired parameters.
//	timer_conf.TIM_Prescaler = 1;
//	timer_conf.TIM_CounterMode = TIM_CounterMode_Up;
//	timer_conf.TIM_Period = peri;
//	timer_conf.TIM_ClockDivision = TIM_CKD_DIV1;
//	timer_conf.TIM_RepetitionCounter = 0x00;

//// Call TIM_TimeBaseInit(TIMx, &TIM_TimeBaseInitStruct) to configure the Time Base unit
//// with the corresponding configuration	
//	TIM_TimeBaseInit(TIM5, &timer_conf);

//	// Call the TIM_Cmd(ENABLE) function to enable the TIM counter.
//	TIM_Cmd(TIM5, ENABLE);

//// Enable the corresponding interrupt using the function TIM_ITConfig(TIMx, TIM_IT_Update) 
//	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);	
//	
//	timer5_interrupt_cmd(ENABLE);
//	return 0;
//}


////int TM_Timer6_Init(FunctionalState TimerRun, FunctionalState IntOn, uint16_t peri) {	
//int TM_Timer6_config(FunctionalState TimerRun, FunctionalState IntOn, uint16_t peri) {	
//  TIM_TimeBaseInitTypeDef timer_conf;
//    
//  /* TIM6 Periph clock enable */
//  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, TimerRun);

//  /* Time base configuration */
//  timer_conf.TIM_Prescaler = 0;
//  timer_conf.TIM_CounterMode = TIM_CounterMode_Up; 
//  timer_conf.TIM_Period = peri;         //0xff
//  timer_conf.TIM_ClockDivision = TIM_CKD_DIV1;
////  TIM_TimeBaseStructInit(&timer_conf);
//  TIM_TimeBaseInit(TIM6, &timer_conf);

//  /* TIM6 TRGO selection */
////  TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);
//    TIM_ITConfig(TIM6, TIM_IT_Update, IntOn);	

//  /* TIM6 enable counter */
//  TIM_Cmd(TIM6, TimerRun);
//    return 0;
//}
//#endif

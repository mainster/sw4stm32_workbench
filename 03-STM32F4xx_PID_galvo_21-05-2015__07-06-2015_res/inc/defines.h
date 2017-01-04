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
 
#ifndef DEFINES_H_
#define DEFINES_H_

// Address defines for adc_buffer
//#define ADC1_CH0	0
//#define ADC1_EXT_W	1
//#define ADC1_CH2	2
//#define ADC1_CH3	3

//#define ADC_Y_VAL &adc_buff[0]
//#define ADC_U_VAL &adc_buff[1]
//#define ADC_BACK_VAL &adc_buff[2]

//#define ADC_Y_CHAN 		ADC_Channel_0
//#define ADC_U_CHANNEL 		ADC_Channel_6
//#define ADC_BACK_CHANNEL 	ADC_Channel_3

// ==============================================================
//		                GPIO things
// ==============================================================
#define hi 	Bit_SET
#define lo	Bit_RESET

//#define DBG_IO		    GPIOE
//#define DBG0			GPIO_Pin_0
//#define DBG1			GPIO_Pin_1
//#define DBG2			GPIO_Pin_2
//#define DBG3			GPIO_Pin_3
//#define DBG4			GPIO_Pin_4
//#define DBG5			GPIO_Pin_5
//#define DBG_PID_TIMING_PE6			GPIO_Pin_6

/**< Define indices for ADC_MultiConvBuff[ ] acording to setpoint (INDEX_W) ans
 * process output (INDEX_P). */
#define INDEX_Px     0
#define INDEX_Py     1
#define INDEX_Ix     2
#define INDEX_W      3

/**< Ranges of variables */

/* 4096/2 = 2048 @ VAref=2.5V 
 * => V_dac_offset = 1.25V 
 */
#define ADC_RNG	            0xfff
//const  uint16_t ADC_RNG	= 0xfff;

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// 2.5V/(2^12-1) = 610.5uV/dig
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#define AREF_IN_VOLT        (float) 2.5
#define AREF_BY2            (float) 2.5/2
#define LSB                 (float) ((float)2.5/ADC_RNG)
//volatile float LSB = (float) ((float)2.5/ADC_RNG);
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


//int (*fup11) (float*);

#define AN_OFFSET				(uint16_t) (0xfff+1)/2			 
#define UPPER_DAC_LIMIT_SIGNED	(int16_t) (0xfff-1)/2      /**< is  2047 */
#define LOWER_DAC_LIMIT_SIGNED	(int16_t) -(0xfff+1)/2     /**< is -2048 */
#define AMPL_FLOAT_DIVISOR      1e3

/*! \brief Sampling Time Interval
 *
 * Specify the desired PID sample time interval
	
 */
//                               0xffff * 5us = 0.3277ms = 3.05Hz

#define TS			5			// Sample time [us]	//10khz
#define SAMPLE_INTERVAL   (45*TS)		// 45MHz APBx timer clocks


// ==============================================================
//		                Fast Console
// ==============================================================



#endif 

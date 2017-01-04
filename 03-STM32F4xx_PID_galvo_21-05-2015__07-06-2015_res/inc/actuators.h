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
 #ifndef _ACTUATORS_H
 #define _ACTUATORS_H
 
//#include <string.h>
//#include <stdlib.h>
//#include <stdio.h>

#include "adc_dac_dma.h"
//#include "arm_architect.h"
//#include "arm_math.h"
#include "defines.h"
#include "main.h"
#include "mdb_gpio.h"
#include "pid.h"



int updateActuator_f(float I_set_x, float I_set_y);
int beamCtrl(beamCtrlSource_t src, tribool_state_t newState );


#endif


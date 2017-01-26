#include "pid.h"
#include "stdint.h"
#include "stm32f4xx.h"
#include "defines.h"
#include "tm_stm32f4_usart.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern __IO uint16_t ADC_MultiConvBuff[5];
extern __IO uint16_t ADC_Buff[5];


//volatile float lastE;
//volatile float Int ;


// Update PID controller parameters
void update_pid_data(	float Kp, float Ki, float Kd, 
											uint16_t setTf, 
											struct PID_DATA *pid) { 

	if ((Ki != pid->Ki) || (Kd != pid->Kd) || (setTf != pid->Tf))  {
		memset((int *)&pid->ESUM, 0, PID_BUFF_LENGTH*sizeof(pid->ESUM[0]));
		memset((int *)&pid->P, 0, PID_BUFF_LENGTH*sizeof(pid->P[0]));
	}
	pid->Kp = Kp;
	pid->Ki = Ki;
	pid->Kd = Kd;
	pid->Tf = setTf;

// Limits to avoid overflow
  pid->maxError = MAX_INT / (pid->Kp + 1);
  pid->maxSumError = MAX_I_TERM / (pid->Ki + 1);
}


/*! \brief Initialisation of PID controller parameters.
 *
 *  Initialise the variables used by the PID algorithm.
 *
 *  \param Kp  Proportional term.
 *  \param Ki  Integral term.
 *  \param Kd  Derivate term.
 *  \param pid  Struct with PID status.
		[us]						[us]												*/
void pid_Init_Tp(void) {
    volatile int tt;
    tt = 100;
    printf("pid_Init_Tp called\n");
}
void pid_Init_Tp_arg(int arg) {
    volatile int tt;
    tt = 100-arg;
    printf("pid_Init_Tp_arg called\narg1: %i", arg);
}
#pragma O0
void pid_Init(	float Kp, float Ki, float Kd, 
								uint16_t setTf, uint16_t setTs, 
								struct PID_DATA *pid, PID_struct_type type ) {

									// Set up PID controller parameters
									
    pid->Ts = setTs;
    if (type == BUFFERED) {
        // Start values for PID controller
        memset((int *)&pid->W, 0, PID_BUFF_LENGTH*sizeof(pid->W[0]));
        memset((int *)&pid->Y, 0, PID_BUFF_LENGTH*sizeof(pid->Y[0]));
        memset((int *)&pid->E, 0, PID_BUFF_LENGTH*sizeof(pid->E[0]));
        memset((int *)&pid->P, 0, PID_BUFF_LENGTH*sizeof(pid->P[0]));
        memset((int *)&pid->ESUM[0], 0, PID_BUFF_LENGTH*sizeof(pid->ESUM[0]));
    }
    else {
        InitStruct(pid, pidData_t);
    }
  // Tuning constants for PID loop
  pid->Kp = Kp;
  pid->Ki = Ki;
  pid->Kd = Kd;
	pid->Tf = setTf;

	// Limits to avoid overflow
	// limits are dependent on controller parameters
	// 
//	pid->maxError = MAX_INT / (pid->Kp + 1);
//  pid->maxSumError = MAX_I_TERM / (pid->Ki + 10);
	pid->maxError = 0.25*MAX_INT / (pid->Kp + 1);
  pid->maxSumError = 0.25*MAX_I_TERM / (pid->Ki + 10);
	
	pid->W_int = 0;
    pid->W_rem = 0;

}
#pragma O2


/*! \brief Initialisation of fPID controller parameters.
 *
 *  Initialise the variables used by the PID algorithm.
 *
 *  \param Kp  Proportional term.
 *  \param Ki  Integral term.
 *  \param Kd  Derivate term.
 *  \param pid  Struct with PID status.
		[us]						[us]												*/
#ifdef FPID
void fpid_Init(	double Kp, double Ki, double Kd, 
								uint16_t setTf, uint16_t setTs, 
								struct FPID_DATA *pid) {
    pid->Ts = setTs;
    // Tuning constants for PID loop
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->Tf = setTf;

    // Limits to avoid overflow
    // limits are dependent on controller parameters
    // 
    pid->maxError    = UPPER_SAT / (pid->Kp + 1);
    pid->maxSumError = MAX_F_I_TERM / (pid->Ki + 10);

    pid->W = 0;
    pid->raw2float = (double)2.5 / 0xfff;         /**< Aref / ADC_res */
    pid->float2raw = (int16_t)0xfff / 2.5;       /**< ADC_res / Aref */
}



//#pragma O0
int fpid_Controller(int16_t setPoint, int16_t processValue, \
                                struct FPID_DATA *pid) {
    
    double e         = (double)((int16_t) setPoint - processValue) * pid->raw2float;
    double e_1       = pid->E;
    double eSum_1    = pid->Esum;
    double Kp        = pid->Kp;
    double Ki        = pid->Ki;
    double Kd        = pid->Kd;
    double Tf        = (double)pid->Tf / 1000000;     /**< Filter timeconstant [s] NOT [us] */
	 double Ts        = (double)pid->Ts / 1000000;     /**< Sampletime [s] NOT [us] */
    double y_p, y_i, y_d, y;
    
    /**< Calculate Pterm and limit error overflow. */
    if (e > pid->maxError) {
        y_p = pid->maxError;
    }
    else if (e < -pid->maxError) {
        y_p = -pid->maxError;
    }
    else{
        y_p = Kp * e;
    }


    /**< Calculate Iterm and limit integral runaway. */
    if(eSum_1 > pid->maxSumError){
        y_i = MAX_F_I_TERM;
        eSum_1 = pid->maxSumError;
    }
    else if(eSum_1 < -pid->maxSumError){
        y_i = -MAX_F_I_TERM;
        eSum_1 = -pid->maxSumError;
    }
    else{
        y_i = eSum_1 + Ki * e;
    }

    /**< Calculate Dterm. */
    y_d = Kd/Ts * (e - e_1);                 ///< Without 1. order filter 
//    y_d = Kd/Ts * (e-e_1)/((e-e_1)*Tf/Ts+1);	///< 1. order filter

    /**<  sum terms. */
    y = (y_p + y_i + y_d);
    
    if(y > UPPER_SAT){
        y = UPPER_SAT;
    }
    else if(y < LOWER_SAT){
        y = LOWER_SAT;
    }

    pid->Y = y;
    pid->Y_raw = y * pid->float2raw;
    pid->E = e;
    pid->Esum = eSum_1;
    pid->P = (double)processValue * pid->raw2float;
    pid->W = (double)setPoint * pid->raw2float;
    
    return 0;
}
//#pragma O2
#endif
                
                

void calc_coeff (	struct PID_DATA *pid, 
									uint8_t integration_method) {
	float Kp = pid->Kp;
	float Ki = pid->Ki;
	float Kd = pid->Kd;
	float Tf = (float)pid->Tf / 1000000;						// Filter timeconstant [s] NOT [us]
	float Ts = (float)pid->Ts / 1000000;						// Sampletime [s] NOT [us]
	
	switch (integration_method) {
		case BACK_SQUARE:	{														// "Rueckwaerts- Rechteck" //
			pid->b0 = Kp + Ki*Ts + Kd/Ts;
			pid->b1 = -(Kp + 2*Kd/Ts);
			pid->b2 = Kd/Ts; 
		} break;
		
		case TRIANGLE:		{														// "Trapez" //
			pid->b0 = Kp + Ki*Ts/2 + Kd/Ts;
			pid->b1 = -(Kp - Ki*Ts/2 + 2*Kd/Ts);
			pid->b2 = Kd/Ts; 
		} break;
		
		default: {
			pid->b0 = 0;
			pid->b1 = 0;
			pid->b2 = 0;
		}
	}
}

#define INTERNAL_SETPOINT

/*! \brief PID control algorithm.
 *
 *  Calculates output from setpoint, process value and PID status.
 *
 *  \param setPoint  Desired value.
 *  \param processValue  Measured value.
 *  \param pid  PID status struct.
 *
 															  4096							  4096																											*/
//static int pid_Controller(int16_t setPoint, int16_t processValue, struct PID_DATA *pid) {
#pragma O0
int pid_Controller(int16_t setPoint, int16_t processValue, struct PID_DATA *pid) {

#ifdef LARGE_PID

//	int k = ((pid->ind + 1) & (PID_BUFF_LENGTH - 1));				// incremented index counter 
	volatile int k = pid->ind;																					// index counter 
    
#ifdef INTERNAL_SETPOINT
    int w = setPoint;		                    // actual setpoint value
    volatile 	int e = setPoint - processValue;													// e(k) actual error
#else
    /* under test: 
     * place the process output (pos_X) directly by DMA controller 
     * place the process output (pos_X) directly by DMA controller */ 
    
//    volatile int e = ADC_MultiConvBuff[INDEX_W] - ADC_MultiConvBuff[INDEX_Py];
    volatile int e = ADC_Buff[INDEX_W] - ADC_Buff[INDEX_Py];
#endif
    volatile int e_1 = pid->E[(k-1) & (PID_BUFF_LENGTH - 1)];					// e(k-1) last error
    volatile int eSum_1 = pid->ESUM[(k-1) & (PID_BUFF_LENGTH - 1)];		// esum(k-1) sum error
//	int p_1	= pid->P[(k-1) & (PID_BUFF_LENGTH - 1)];				// last process value P(k-1)	
	volatile int y = 0;																								// next PID output value 
	volatile int y_p, y_i, y_d;																// parallel compensator terms
	float Kp = pid->Kp;
	float Ki = pid->Ki;
	float Kd = pid->Kd;
	float Tf = (float)pid->Tf / 1000000;							// Filter timeconstant [s] NOT [us]
	float Ts = (float)pid->Ts / 1000000;							// Sampletime [s] NOT [us]

//	float P_term = 0;			// var for actual P- term calculation
//	float I_term = 0;			// var for actual I- term calculation
//	float D_term = 0;			// var for actual D- term calculation
//	
/* ---------------------------------------------------------------------------
 * Stellungsalgorithmus	("Einfuehrung in die Regelungstechnik - Heinz Mann")
 * ---------------------------------------------------------------------------
 * y(k) = y(k-1) + b0*e(k) + b1*e(k-1) + b2*e(k-2)
 *
 * Reglerkoeffizienten:  
 * --------------------
 * 		  	Integrationsvorschrift: "Rueckwaerts- Rechteck"
 *				----------------------------------------------
 *				b0 = Kp + Ki*Ts + Kd/Ts 				[ =  Kp*(1 + Ts/Tn + Tv/Ts) 			]
 *				b1 = -(Kp + 2*Kd/Ts)						[ = -Kp*(1 + 2*Tv/Ts) 						]
 *				b2 = Kd/Ts											[ =  Kp*Tv/Ts											]
 *
 * 		  	Integrationsvorschrift: "Trapez"
 *				----------------------------------------------
 *				b0 = Kp + Ki*Ts/2 + Kd/Ts 			[ =  Kp*(1 + Ts/(2*Tn) + Tv/Ts) 	]
 *				b1 = -(Kp - Ki*Ts/2 + 2*Kd/Ts)	[ = -Kp*(1 - Ts/(2*Tn) + 2*Tv/Ts)	]
 *				b2 = Kd/Ts											[ =  Kp*Tv/Ts											]
 *
 *
 *	Parallelrealisierung:
 *	y_p(k) = Kp * e(k)
 *	y_i(k) = Ki * Ts*Sk
 *
 *  ohne D- Filter (...) + Kd/Ts * (z-1)
 *  ------------------------------------
 * 	y_d(k) = Kd/Ts * (e(k) - e(k-1))
 *
 * 	mit D- Filter (...) + Kd/(Tf + Ts/(z-1))
 *	-----------------------------------------
 *  y_d(k) = Kd/Ts * delta_e/(delta_e * Tf/Ts + 1) 	
 *
 */
 
/*	y = y_1 + pid->b0*e + pid->b1*e_1 + pid->b2*e_2;
		
	if (y > ADC_RNG/2) {
			y  = ADC_RNG/2;    // limit output due to DAC
	}			
	if (y < -ADC_RNG/2) {
			y  = -ADC_RNG/2; 
	}
*/


// Calculate Pterm and limit error overflow
  if (e > pid->maxError){
    y_p = MAX_INT;
  }
  else if (e < -pid->maxError){
    y_p = -MAX_INT;
  }
  else{
    y_p = Kp * e;
  }

 // Calculate Iterm and limit integral runaway
  if(eSum_1 > pid->maxSumError){
    y_i = MAX_I_TERM;
    eSum_1 = pid->maxSumError;
  }
  else if(eSum_1 < -pid->maxSumError){
    y_i = -MAX_I_TERM;
    eSum_1 = -pid->maxSumError;
  }
  else{
    y_i = eSum_1 + Ki * e;
  }

// Calculate Dterm 
// y_d = Kd/Ts * (e - e_1); 					   // without 1. order filter
  y_d = Kd/Ts * (e-e_1)/((e-e_1)*Tf/Ts+1);	// 1. order filter

// sum all terms
  y = (y_p + y_i + y_d);
  if(y > UPPER_DAC_LIMIT){
    y = UPPER_DAC_LIMIT;
  }
  else if(y < LOWER_DAC_LIMIT){
    y = LOWER_DAC_LIMIT;
  }
	
	pid->Y[k] = y;											// actual PID out write back 
	pid->ESUM[k] = eSum_1 + Ki * e;			// integrate actual sum error ( y_i )
	pid->E[k] = e;											// actual error write back
#ifdef INTERNAL_SETPOINT
  	pid->W[k] = w;											// only for history
	pid->P[k] = processValue;						// only for history
#else
    pid->W[k] = ADC_MultiConvBuff[INDEX_W];						// only for history
	pid->P[k] = ADC_MultiConvBuff[INDEX_Py];						// only for history
#endif

    pid->ind = (k < PID_BUFF_LENGTH-1)  ?  (k+1) : 0;
//	pid->ind = (k+1) & (PID_BUFF_LENGTH - 1);		// inc circular buffer index
	
  return y;
#else


    int k=0;

    float Int = pid->ESUM[k];
    float lastE = (float)pid->E[k];
    
	float Kp = pid->Kp;
	float Ki = pid->Ki;
	float Kd = pid->Kd;
    float Tf = (float)pid->Tf / 1000000;							// Filter timeconstant [s] NOT [us]
	float Ts = (float)pid->Ts / 1000000;							// Sampletime [s] NOT [us]

    float E = processValue - setPoint;
  // Calculate Pterm and limit error overflow
	float Prop = Kp * (float)E;                             // proportional part 
//	float Dif  = Kd * (float)(E - lastE) / Ts;  				// diff. part 
	float Dif = Kd*(float) (E-lastE)/((float)(E-lastE)*Tf+Ts);  			// diff. part 1. order filtered 
	Int += Ki * (float)E / Ts;                             			// integral part 
    
    int16_t toPlant = (int)(Prop + Dif + Int);                      // summ all three 
//	GPIO_WriteBit(GPIOE, GPIO_Pin_4, Bit_RESET);
    
	pid->Y[k] = toPlant;											// actual PID out write back 
	pid->ESUM[k] = Int;			// integrate actual sum error ( y_i )
	pid->E[k] = E;											// actual error write back
  	pid->W[k] = setPoint;											// only for history
	pid->P[k] = processValue;						// only for history
    
    return toPlant;
#endif 
}
#pragma O2

/*! \brief Resets the integrator.
 *
 *  Calling this function will reset the integrator in the PID regulator.
 */
void pid_Reset_Integrator(volatile struct PID_DATA *pid, PID_struct_type type) {
//void pid_Reset_Integrator(PID_DATA *pid) {
    if (type == BUFFERED) {
        memset((int *)&pid->ESUM, 0, PID_BUFF_LENGTH*sizeof(pid->ESUM[0]));
        memset((int *)&pid->E, 0, PID_BUFF_LENGTH*sizeof(pid->E[0]));
    }
    else {
        pid->ESUM[0] = 0;
        pid->E[0] = 0;
    }
}

/*
	
//////	
//////	
//////  // Calculate Pterm and limit error overflow
//////	float Prop = Kp * (float)E;                             // proportional part 
////////	float Dif  = Kd * (float)(E - lastE) / Ts;  				// diff. part 
//////	float Dif = Kd*(float) (E-lastE)/((float)(E-lastE)*Tf+Ts);  			// diff. part 1. order filtered 
//////	Int += Ki * (float)E / Ts;                             			// integral part 

//////	ToPlant = (int)(Prop + Dif + Int);                      // summ all three 
//////	GPIO_WriteBit(GPIOC, GPIO_Pin_4, Bit_RESET);
//////	
	
	int k = PID->ind;

//		ESUM[ind] = W[ind] - Y[ind ];           // calculate error 
	PID->ESUM[k] = PID->W[k] - PID->Y[k];

//		Prop = Kp * (float)ESUM[ind];                             // proportional part 
	float Prop = PID->Kp * (float)PID->ESUM[k];

//		Dif  = Kd * (float)(ESUM[ind] - ESUM[(ind-1) & 63]) / Ts;  // diff. part 
	float Dif = PID->Kd * (float)( PID->ESUM[k] - PID->ESUM[(k-1) & 63]) / PID->Ts;

//		Int += Ki * (float)ESUM[ind];                             // integral part 
	PID->Integrator += PID->Ki * (float)PID->ESUM[k];

//		ToPlant[ind] = (int)(Prop + Dif + Int);                        // summ all three 
	PID->ToPlant[k] = (int)(Prop + Dif + PID->Integrator); 
//		GPIO_WriteBit(GPIOC, GPIO_Pin_4, Bit_RESET);

	
	if (ToPlant > 4095) {
		ToPlant  = 4095;    // limit output due to DAC
	}			
	if (ToPlant < 0) {
		ToPlant  = 0; 
	}
	
	//  // Calculate Iterm and limit integral runaway
  if(Int > pid->maxSumError){
    Int = MAX_I_TERM;
    pid->sumError = pid->maxSumError;
  }
  else if(Int < -pid->maxSumError){
    Int = -MAX_I_TERM;
    pid->sumError = -pid->maxSumError;
  }
  else{
    pid->sumError = Int;
    i_term = pid->Ki * pid->sumError;
  }
	
	
	pid->W[k] = W;
	pid->Y[k] = Y;
	pid->ESUM[k] = E;
	pid->ToPlant[k] = ToPlant;
	pid->Integrator = Int;	
	k = (k + 1) & 63;         // increment pointer to circular buffer 		
	pid->ind = k;
	
//  // Calculate Pterm and limit error overflow
//  if (E > pid->maxError){
//    p_term = MAX_INT_16;
//  }
//  else if (error < -pid->maxError){
//    p_term = -MAX_INT_16;
//  }
//  else{
//    p_term = pid->Kp * error;
//  }

//  // Calculate Iterm and limit integral runaway
//  temp = pid->sumError + error;
//  if(temp > pid->maxSumError){
//    i_term = MAX_I_TERM;
//    pid->sumError = pid->maxSumError;
//  }
//  else if(temp < -pid->maxSumError){
//    i_term = -MAX_I_TERM;
//    pid->sumError = -pid->maxSumError;
//  }
//  else{
//    pid->sumError = temp;
//    i_term = pid->Ki * pid->sumError;
//  }

//  // Calculate Dterm
//  d_term = pid->Kd * (pid->lastProcessValue - processValue);

//  pid->lastProcessValue = processValue;




//  ret = (p_term + i_term + d_term);
//  if(ret > MAX_INT_16){
//    ret = MAX_INT_16;
//  }
//  else if(ret < -MAX_INT_16){
//    ret = -MAX_INT_16;
//  }
*/

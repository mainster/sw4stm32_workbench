/**
 * @file		pid.h
 *
 * @date		16 Feb 2016
 * @author	  Manuel Del Basso (mainster)
 * @email	   manuel.delbasso@gmail.com
 *
 * @ide		 System Workbench ac6 (eclipse stm32)
 * @stdperiph   STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 *
 * @brief	   PID algorithms
 *
 * @verbatim

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

 * @endverbatim
 *
 */
#ifndef _PID_H_
#define _PID_H_

#include "stdint.h"

#if ! (defined (KEIL_IDE) || defined (__GNUC__))
	#error "Please specify an IDE or GNUC tools!"
#endif

/**
 * @addtogroup XY-Galvo scanner
 * @{
 */

/**
 * @defgroup   PID (group_title)
 * @brief      PID compensator implementation. @{
 *
 *             PID implementation is used to handle control-loop functionality.
 *
 * @par It provides functions for
 *      - pid instance initialization
 *      - pid reset internals
 *      - update compensator outputs
 */

/**
 * @defgroup   PID_Macros (group_title)
 * @brief      PID methodes related macro definitions. @{
 */

/**
 * @brief      Size of PID structure internal buffers.
 */
#define PID_BUFF_LENGTH		5

/**
 * @brief      Flag to enable/disable PID parameter data type "float".
 */
#define FLOAT_PARAMETERS   	1

/**
 * @brief      Integration methode "Reward Rectangle". FIXME: typedef me!
 */
#define	 BACK_SQUARE	20

/** 
 * @brief      Integration methode "Trapezodial". FIXME: typedef me!
 */
#define	 TRIANGLE		21

/**
 * @brief      Memory initialisation macro. Can be used for structures and
 *             primitive variables.
 *
 * @param      var    The variable or structure.
 * @param      type   The type of var.
 *
 */
#define InitStruct(var, type) type var; memset(&var, 0, sizeof(type))

/**
 * @brief      Range limits for different data types. Needed to avoid
 *             sign/overflow problems
 */
#define MAX_INT_16		INT16_MAX
#define MAX_INT			INT16_MAX
#define MAX_LONG		INT32_MAX
#define MAX_I_TERM	  	(MAX_LONG / 2)

/**
 * @brief      Range limits for different data types. Needed to avoid
 *             sign/overflow problems
 */
#define UPPER_SAT		100
#define LOWER_SAT	   -100
#define FLOAT_MAX		1000
#define MAX_F_I_TERM	(FLOAT_MAX / 2)

/**
 * @}	PID_Macros	(end)
 */

/**
 * @defgroup   PID_Variables
 * @brief      PID compensator variables. @{
 */

/**
 * @var        int16_t P_now, W_now
 * @brief      Actual plant value / set-point value. 
 */
volatile int16_t P_now, W_now;

/**
 * @var        float KP_INIT
 * @brief      Initial value of proportional gain. 
 */
static volatile float 	KP_INIT = 1;

/**
 * @var        float KI_INIT
 * @brief      Initial value of integral gain. 
 */
/**
 * @var        float KD_INIT
 * @brief      Initial value of derivative gain. 
 */
/**
 * @var        float TF_INIT
 * @brief      Initial value of D-term filter proportional gain. 
 */
static volatile float
	KI_INIT = 0,
	KD_INIT = 0,
	TF_INIT = 0;

/**
 * @} 	PID_Variables (end)
 */

/**
 * @defgroup PID_Typedefs
 * @brief    PID methode type definitions.
 * @{
 */

/* Disable compiler optimization */
#if  defined (KEIL_IDE)
	#pragma O0
#elif  defined (__GNUC__)
	#pragma GCC push_options
	#pragma GCC optimize ("O0")
#endif

/**
 * @brief      External declared PID instances.
 */
extern struct PID_DATA	 pidDataX, pidDataY;


/**
 * @brief      Runtime memory structure of single PID instance.
 *
 *             Holds compensator values, range parameters and runtime data for a
 *             sinle PID instance.
 */
typedef struct PID_DATA {

	float W_remf;					//!< Angle setpoint value [float]. (Updated via Qt interface).
	int16_t W_int;					//!< Internal setpoint (Updated via Qt interface).
	int16_t W_rem;					//!< Angle setpoint value [int16]. (Updated via Qt interface).
	int16_t W[PID_BUFF_LENGTH];		//!< Setpoint history.
	int16_t E[PID_BUFF_LENGTH];		//!< Error history: e(k)=w(k)-p(k).
	int16_t ESUM[PID_BUFF_LENGTH];	//!< Error sum history: e(k)=e(k-1)+w-p.
	int16_t P[PID_BUFF_LENGTH];		//!< Process value (sensor output).
	int16_t Y[PID_BUFF_LENGTH];		//!< PID output history.
	uint8_t remoteCmdActive;		//!< Flag. The commanding source of set-point value is stored in g.() */
	uint16_t Tf;					//!< Time constant for 1. order D-term filter [us] (see MATLAB/Simulink).
	uint16_t Ts;					//!< Sample time [us].
	uint16_t ind;					//!< Index

	/** 
	 * FIXME:	Avoid overflow! 
	 * (W - P) !!<=!! maxError.
	 */
	int16_t maxError;				//!< Max. value of error signal */ 

	/** 
	 * FIXME: 	Avoid overflow! 
	 * E(k) !!<=!! maxSumError.
	 * 
	 * TODO:	Implement failsafe anti-windup system. 
	 */
	int16_t maxSumError;			//!< Max. value of integral error.

#ifdef FLOAT_PARAMETERS
	float b0, b1, b2;				//!< PID algorithm coefficents [float].
	float Kp;						//!< PID proportional gain [float].
	float Ki;						//!< PID integral gain [float].
	float Kd;						//!< PID derivative gain [float].
#elif DOUBLE_PARAMETERS
	double b0, b1, b2;				//!< PID algorithm coefficents [double].
	double Kp;						//!< PID proportional gain [double].
	double Ki;						//!< PID integral gain [double].
	double Kd;						//!< PID derivative gain [double].
#else
	error "Select PID param. datatype FLOAT_PARAMETERS or DOUBLE_PARAMETERS"
#endif

} pidData_t;



/**
 * @brief      Runtime memory structure (NEW) of single PID instance.
 *
 *             Holds compensator values, range parameters and runtime data for a
 *             sinle PID instance.
 */
typedef struct PID_DATA_NEW {
	int16_t W;					//!< Angle set-point value [int16].
	int16_t W_int;				//!< Internal setpoint (Updated via Qt interface).
	int16_t W_rem;				//!< Angle set-point value [int16]. (Updated via Qt interface).
	int16_t Y;					//!< PID output value.
	int16_t E;					//!< Actual error: e(k)=w(k)-p(k).
	int16_t ESUM;				//!< Error sum: e(k)=e(k-1)+w-p.
	int16_t P;					//!< Process value (sensor output).
	uint16_t ind;				//!< Index.
	uint8_t remoteCmdActive;	//!< Flag. The commanding source of set-point value is stored in g.() */
	uint16_t Tf;				//!< Time constant for 1. order D-term filter [us] (see MATLAB/Simulink).
	uint16_t Ts;				//!< Sample time [us].

	/** 
	 * FIXME:	Avoid overflow! 
	 * (W - P) !!<=!! maxError.
	 */
	int16_t maxError;			//!< Max. value of error signal */ 

	/** 
	 * FIXME: 	Avoid overflow! 
	 * E(k) !!<=!! maxSumError.
	 * 
	 * TODO:	Implement failsafe anti-windup system. 
	 */
	int16_t maxSumError;		//!< Max. value of integral error.	

	float b0, b1, b2;			//!< PID algorithm coefficents [float].
	float Kp;					//!< PID proportional gain [float].
	float Ki;					//!< PID integral gain [float].
	float Kd;					//!< PID derivative gain [float].

} pidData_t_NEW;



/**
 * @brief      Runtime memory structure of single PID instance (float).
 *
 *             Holds compensator values, range parameters and runtime data for a
 *             sinle PID instance.
 */
typedef struct FPID_DATA {
	int16_t	W_raw;
	int16_t	Y_raw;
	float raw2float;
	float float2raw;
	float MAX_Y_I_TERM;
	float W;					//!< Angle set-point value [int16].
	float Y;					//!< PID output value.
	float E;					//!< Actual error: e(k)=w(k)-p(k).
	float ESUM;					//!< Error sum: e(k)=e(k-1)+w-p.
	float P;					//!< Process value (sensor output).
	uint16_t Tf;				//!< Time constant for 1. order D-term filter [us] (see MATLAB/Simulink).
	uint16_t Ts;				//!< Sample time [us].

	/** 
	 * FIXME:	Avoid overflow! 
	 * (W - P) !!<=!! maxError.
	 */
	float maxError;				//!< Max. value of error signal */ 

	/** 
	 * FIXME: 	Avoid overflow! 
	 * E(k) !!<=!! maxSumError.
	 * 
	 * TODO:	Implement failsafe anti-windup system. 
	 */
	float maxSumError;			//!< Max. value of integral error.	

	float b0, b1, b2;			//!< PID algorithm coefficents [float].
	float Kp;					//!< PID proportional gain [float].
	float Ki;					//!< PID integral gain [float].
	float Kd;					//!< PID derivative gain [float].

} pidDatafloat_t;

/**
 * @brief      PID structure type enumeration.
 */
typedef enum {
	BUFFERED,
	UNBUFFERED
} PID_struct_type;

/* Re-Enable compiler optimization */
#if defined (KEIL_IDE)
	#pragma O2
#elif defined (__GNUC__)
	#pragma GCC pop_options
#endif

/**
 * @}	PID_Typedefs (end)
 */


void pid_Init_Tp(void);

void pid_Init_Tp_arg(int arg);


void pid_Init(float Kp, float Ki, float Kd, uint16_t setTf,
			  uint16_t setTs, struct PID_DATA *pid, PID_struct_type type);

int pid_Controller(int16_t setPoint, int16_t processValue, struct PID_DATA *pid_st);

void pid_Reset_Integrator(volatile struct PID_DATA *pid_st, PID_struct_type type);

void update_pid_data(float Kp, float Ki, float Kd, uint16_t setTf, struct PID_DATA *pid);

void calc_coeff (struct PID_DATA *pid, uint8_t integration_method);

void fpid_Init( double Kp, double Ki, double Kd, uint16_t setTf, uint16_t setTs,
				struct FPID_DATA *pid);

int fpid_Controller(int16_t setPoint, int16_t processValue, struct FPID_DATA *pid);

/**
 * @}
 */
/**
 * @}
 */
#endif

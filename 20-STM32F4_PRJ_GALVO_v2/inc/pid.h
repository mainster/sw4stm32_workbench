




#ifndef _PID_H_
#define _PID_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "globals.h"

#if ! (defined (KEIL_IDE) || defined (__GNUC__))
#error "Please specify an IDE or GNUC tools!"
#endif

/**
 * @addtogroup MD_APP
 * @{
 */

/**
 * @addtogroup APP_Pid
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
 * @addtogroup APP_Pid_Macros
 * @brief      PID methods related macro definitions. @{
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
 * @brief      Memory initialization macro. Can be used for structures and
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

/** @} */ 	/* APP_Pid_Macros */

/**
 * @addtogroup APP_Pid_Variables
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

/** @} */ 	/* APP_Pid_Variables */

/**
 * @addtogroup  APP_Pid_Typedefs
 * @brief       PID method type definitions. @{
 */

/* Disable compiler optimization */
#if  defined (KEIL_IDE)
#pragma O0
#elif  defined (__GNUC__)
#pragma GCC push_options
#pragma GCC optimize ("O0")
#endif

/**
 * @brief      Integration methods enumeration.
 */
typedef enum {
	PID_IntMethode_FwdRect = 19,	//!< Use method "Forward Rectangle".
	PID_IntMethode_RwdRect = 20,	//!< Use method "Reward Rectangle".
	PID_IntMethode_Trapez = 21,		//!< Use method "Trapezoidal".
} PID_IntMethode_t;

/**
 * @brief      PID structure type enumeration.
 */
typedef enum {
	PID_StructType_Buffered,
	PID_StructType_Unbuffered,
} PID_StructType_t;

/**
 * @brief      Runtime memory structure of single PID instance.
 *
 *             Holds compensator values, range parameters and runtime data for a
 *             single PID instance.
 */
typedef struct PID_DATA {

	float W_remf;					//!< Angle set-point value [float]. (Updated via Qt interface).
	int16_t W_int;					//!< Internal set-point (Updated via Qt interface).
	int16_t W_rem;					//!< Angle set-point value [int16]. (Updated via Qt interface).
	int16_t W[PID_BUFF_LENGTH];		//!< Set-point history.
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
 *             single PID instance.
 */
typedef struct PID_DATA_NEW {
	int16_t W;					//!< Angle set-point value [int16].
	int16_t W_int;				//!< Internal set-point (Updated via Qt interface).
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
	 * TODO:	Implement fail-safe anti-windup system.
	 */
	int16_t maxSumError;		//!< Max. value of integral error.

	float b0, b1, b2;			//!< PID algorithm coefficients [float].
	float Kp;					//!< PID proportional gain [float].
	float Ki;					//!< PID integral gain [float].
	float Kd;					//!< PID derivative gain [float].

} pidData_t_NEW;



/**
 * @brief      Runtime memory structure of single PID instance (float).
 *
 *             Holds compensator values, range parameters and runtime data for a
 *             single PID instance.
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
	 * TODO:	Implement fail-safe anti-windup system.
	 */
	float maxSumError;			//!< Max. value of integral error.

	float b0, b1, b2;			//!< PID algorithm coefficients [float].
	float Kp;					//!< PID proportional gain [float].
	float Ki;					//!< PID integral gain [float].
	float Kd;					//!< PID derivative gain [float].

} pidDatafloat_t;

/**
 * @brief      External declared PID instances.
 */
extern struct PID_DATA	 pidDataX, pidDataY;


/* Re-Enable compiler optimization */
#if defined (KEIL_IDE)
#pragma O2
#elif defined (__GNUC__)
#pragma GCC pop_options
#endif

/** @} */ 	/* APP_Pid_Typedefs */

/**
 * @addtogroup APP_Pid_Functions
 * @brief    	PID functions and methods.
 * @{
 */

/**
 * @brief      Initialisation of PID controller parameters. Initialise the
 *             variables used by the PID algorithm.
 *
 */
void pid_Init_Tp (void);

/**
 * @brief      { function_description }
 *
 * @param[in]  arg   The argument
 */
void pid_Init_Tp_arg (int arg);


/**
 * @brief      Float types PID instance initialization methode.
 *
 * @param      Kp      PID proportional gain value.
 * @param      Ki      PID integral gain value.
 * @param      Kd      PID derivate gain value.
 * @param[in]  setTf   Initial value for 1.st order D-filter time constant [us].
 * @param[in]  setTs   Sample rate [us];
 * @param      pid     Pointer to structure which holds PID instance data.
 * @param[in]  type    Select buffered or unbuffered PID structure (See @ref
 *                     PID_struct_type enumeration).
 */
void pid_Init (float Kp, float Ki, float Kd, uint16_t setTf,
			   uint16_t setTs, struct PID_DATA *pid, PID_StructType_t stType);


/**
 * @brief      Double types PID instance initialization methode.
 *
 * @param      Kp      PID proportional gain value.
 * @param      Ki      PID integral gain value.
 * @param      Kd      PID derivate gain value.
 * @param[in]  setTf   Initial value for 1.st order D-filter time constant [us].
 * @param[in]  setTs   Sample rate [us];
 * @param      pid     Pointer to structure which holds PID instance data.
 */
void fpid_Init (double Kp, double Ki, double Kd, uint16_t setTf,
				uint16_t setTs, struct FPID_DATA *pid);

/**
 * @brief      Clear integral part of PID model state.
 *
 *             Resets the runtime integrator of the PID compensator. Use for
 *             anti-windup system, after over current tripping etc.
 *
 * @param      pid      Pointer to structure which holds PID instance data.
 * @param[in]  stType   Select buffered or unbuffered PID structure (See @ref
 *                      PID_struct_type enumeration).
 */
void pid_Reset_Integrator (volatile struct PID_DATA *pid,
						   PID_StructType_t stType);

/**
 * @brief      Pre-Calculates PID coeffizients.
 *
 *             Pre-Calculation of coeffizients after a parameter update can
 *             speedup the underlying algorithms.
 *
 * @param      pid          Pointer to structure which holds PID instance data.
 * @param[in]  intMethode   The integration method
 */
void PID_Calc_Coeffs (struct PID_DATA *pid, PID_IntMethode_t intMethode);

/**
 * @brief      Invoke PID controller calculation.
 *
 *             Invoke the control algorithm calculation processes for a single
 *             PID instance.
 *
 * @param[in]  setPoint       Pass next set-point value via setPoint.
 * @param[in]  processValue   Pass actual process (e.g. plunt output) value.
 * @param      pid          Pointer to structure which holds PID instance data.
 *
 * @callergraph
 */
int pid_Controller (int16_t setPoint, int16_t processValue, struct PID_DATA *pid);

/**
 * @brief      { function_description }
 *
 * @param[in]  Kp      { parameter_description }
 * @param[in]  Ki      { parameter_description }
 * @param[in]  Kd      { parameter_description }
 * @param[in]  setTf   The set tf
 * @param      pid     The pid
 */
void update_pid_data (float Kp, float Ki, float Kd,
					  uint16_t setTf, struct PID_DATA *pid);

/**
 * @brief      { function_description }
 *
 * @param[in]  setPoint       The set point
 * @param[in]  processValue   The process value
 * @param      pid            The pid
 *
 * @return     { description_of_the_return_value }
 */
int fpid_Controller (int16_t setPoint, int16_t processValue,
					 struct FPID_DATA *pid);

/** @} */ 	/* APP_Pid_Functions */

/** @} */ 	/* APP_Pid */

/** @} */ 	/* MD_APP */

#endif

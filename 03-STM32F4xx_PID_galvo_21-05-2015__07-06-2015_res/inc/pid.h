/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief Header file for pid.c.
 *
 * - File:               pid.h
 * - Compiler:           IAR EWAAVR 4.11A
 * - Supported devices:  All AVR devices can be used.
 * - AppNote:            AVR221 - Discrete PID controller
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support email: avr@atmel.com
 *
 * $Name$
 * $Revision: 456 $
 * $RCSfile$
 * $Date: 2006-02-16 12:46:13 +0100 (to, 16 feb 2006) $
 *****************************************************************************/

#ifndef __PID_H
#define __PID_H

#include "stdint.h"

#define 	BACK_SQUARE		20		// Integrationsmethode "Rueckckwaerts- Rechteck" //
#define 	TRIANGLE			21		// Integrationsmethode "Trapez" //

/*! \brief PID Status
 *
 * Setpoints and data used by the PID control algorithm
 */
 #define PID_BUFF_LENGTH    5
 #define FLOAT_PARAMETERS   1

typedef enum {
    BUFFERED,
    UNBUFFERED
} PID_struct_type;
#pragma GCC push_options
#pragma GCC optimize ("O0")

typedef struct PID_DATA{
    int16_t W[PID_BUFF_LENGTH];		/**< Setpoint history */
    int16_t Y[PID_BUFF_LENGTH];		/**< PID output history */
    int16_t ESUM[PID_BUFF_LENGTH];		/**< Error sum history: e(k)=e(k-1)+w-p */ 
    int16_t E[PID_BUFF_LENGTH];		/**< Error history: e(k)=w(k)-p(k)  */
    int16_t P[PID_BUFF_LENGTH];		/**< Process value (sensor output) */
    uint16_t ind;
#ifdef FLOAT_PARAMETERS
    float	b0, b1, b2;				    /**< PID algorithm coefficents */
    float Kp;
    float Ki;
    float Kd;
#elif   DOUBLE_PARAMETERS
    double	b0, b1, b2;				    /**< PID algorithm coefficents */
    double Kp;
    double Ki;
    double Kd;
#else 
    error "Select PID param. datatype FLOAT_PARAMETERS or DOUBLE_PARAMETERS" 
#endif
    
    float W_remf;                   /**< Setpoint float type, remote placed */    
//single instance    float Wy_remf;                   /**< Setpoint float type, remote placed */    
    int16_t W_rem;                  /**< Setpoint integral type, remote placed */
    uint16_t Tf;					 /**< [us] Time constant for 1. order D- term filtering */
    uint16_t Ts;					 /**< [us] Sampletime  */
    int16_t maxError;				 /**< Maximum allowed error, avoid overflow	 */
                                     /**< (W - P) !!<=!! maxError */
    int16_t maxSumError;			 /**< Maximum allowed sumerror, avoid overflow */
                                     /**< E(k) 	 !!<=!! maxSumError */
    int16_t W_int;					 /**< Internal Setpoint (console interface) */
    uint8_t remoteCmdActive;       /**< Indicators with information about the setpoint source are stored in--> g.() */
} pidData_t;



typedef struct PID_DATA_NEW{
    int16_t W;		/**< Setpoint history */
    int16_t Y;		/**< PID output history */
    int16_t ESUM;		/**< Error sum history: e(k)=e(k-1)+w-p */ 
    int16_t E;		/**< Error history: e(k)=w(k)-p(k)  */
    int16_t P;		/**< Process value (sensor output) */
    uint16_t ind;
    float	b0, b1, b2;				    /**< PID algorithm coefficents */
    float Kp;
    float Ki;
    float Kd;

    int16_t W_rem;
    uint16_t Tf;					    /**< [us] Time constant for 1. order D- term filtering */
    uint16_t Ts;					    /**< [us] Sampletime  */
    int16_t maxError;				    /**< Maximum allowed error, avoid overflow	 */
                                        /**< (W - P) !!<=!! maxError */
    int16_t maxSumError;				/**< Maximum allowed sumerror, avoid overflow */
                                        /**< E(k) 	 !!<=!! maxSumError */
    int16_t W_int;					    /**< Internal Setpoint (console interface) */
    uint8_t remoteCmd;
} pidData_t_NEW;




typedef struct FPID_DATA{
    int16_t    W_raw;
    int16_t    Y_raw;
    
    float       W;
    float       Y;
    float       raw2float, float2raw; 
	float	b0, b1, b2;         // PID algorithm coefficents
    float   Kp, Ki, Kd;
    float       Esum;
    float       E;
    float       maxError;
    float       maxSumError;
    float       MAX_Y_I_TERM;
    float       P;
	uint16_t Tf;					// [us] Time constant for 1. order D- term filtering
	uint16_t Ts;					// [us] Sampletime     
    
} pidDatafloat_t;

#pragma GCC pop_options


#define InitStruct(var, type) type var; memset(&var, 0, sizeof(type))

extern struct PID_DATA     pidDataX, pidDataY;

/*! \brief Maximum values
 *
 * Needed to avoid sign/overflow problems
 */
//#define MAX_INT_16      INT16_MAX
#define MAX_INT         INT16_MAX
#define MAX_LONG        INT32_MAX
#define MAX_I_TERM      (MAX_LONG / 2)

/**< Same for float-only pid. */
#define UPPER_SAT        100
#define LOWER_SAT       -100
#define FLOAT_MAX        1000
#define MAX_F_I_TERM     (FLOAT_MAX / 2)


//// Boolean values
//#define FALSE           0
//#define TRUE            1

void pid_Init(float Kp, float Ki, float Kd, uint16_t setTf, 
                    uint16_t setTs, struct PID_DATA *pid, PID_struct_type type);
int pid_Controller(int16_t setPoint, int16_t processValue, struct PID_DATA *pid_st);
void pid_Reset_Integrator(volatile struct PID_DATA *pid_st, PID_struct_type type);
void update_pid_data(float Kp, float Ki, float Kd, uint16_t setTf, struct PID_DATA *pid);
void calc_coeff (struct PID_DATA *pid, uint8_t integration_method);

void fpid_Init(	double Kp, double Ki, double Kd, uint16_t setTf, uint16_t setTs, 
                                                                struct FPID_DATA *pid);

int fpid_Controller(int16_t setPoint, int16_t processValue, struct FPID_DATA *pid);

void pid_Init_Tp(void);
void pid_Init_Tp_arg(int arg);

#endif

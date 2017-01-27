/**
 * @file        pid.h
 *
 * @date        16 Feb 2016
 * @author      Manuel Del Basso (mainster)
 * @email       manuel.delbasso@gmail.com
 *
 * @ide         System Workbench ac6 (eclipse stm32)
 * @stdperiph   STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 *
 * @brief       PID algorithms
 *
 * @verbatim

	------------------------------------------------------------------------

	Copyright (C) 2016	Manuel Del Basso

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

#ifndef __PID_H
#define __PID_H

#include "stdint.h"

#if ! (defined (KEIL_IDE) || defined (__GNUC__))
	#error "Please specify an IDE or GNUC tools!"
#endif

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

#if  defined (KEIL_IDE)
	#pragma O0
#elif  defined (__GNUC__)
	#pragma GCC push_options
	#pragma GCC optimize ("O0")
#endif

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

#if  defined (KEIL_IDE)
	#pragma O2
#elif  defined (__GNUC__)
	#pragma GCC pop_options
#endif


#define InitStruct(var, type) type var; memset(&var, 0, sizeof(type))



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


// Boolean values
#define FALSE           0
#define TRUE            1

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

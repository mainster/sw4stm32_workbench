/**
* @file        pid.h
*
* @date
* @author      Manuel Del Basso (mainster)
* @email       manuel.delbasso@gmail.com
*
* @ide         System Workbench ac6 (eclipse stm32)
* @stdperiph   STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
* @license     GNU GPL v3
*
* @brief       PID compensator implementation.
*
   @verbatim

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

   @endverbatim
*
*/
#ifndef _PID_H_
#define _PID_H_

#ifdef __cplusplus
extern "C" {
#endif

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
 * @addtogroup  MD_APP MD_Application
 * @{
 */

/**
 * @addtogroup  APP_Pid PID Compensator
 * @brief       Proportional-Integral-Derivative compensator initialization and
 *              calculation implementations. @{
 *
 *              PID implementation provides Proportional-Integral-Derivative
 *              compensation functionality and belongs to the closed loop
 *              system.
 *
 * @par It provides functions for
 *      - pid instance initialization
 *      - pid reset internals
 *      - update compensator outputs
 */

/**
 * @addtogroup  Pid_Macros
 * @brief       PID methods related macro definitions. @{
 */

/**
 * @brief      Size of PID structure internal buffers.
 */
#define PID_BUFF_LENGTH     5

/**
 * @brief      Flag to enable/disable PID parameter data type "float".
 */
#define FLOAT_PARAMETERS    1

/**
 * @brief      Memory initialization macro. Can be used for structures and
 *             primitive variables.
 *
 * @param      var    The variable or structure.
 * @param      type   The type of var.
 * @return     None
 */
#define InitStruct(var, type) type var; memset(&var, 0, sizeof(type))

/**
 * @brief      Range limits for different data types. Needed to avoid
 *             sign/overflow problems
 */
#define MAX_INT_16      INT16_MAX
#define MAX_INT         INT16_MAX
#define MAX_LONG        INT32_MAX
#define MAX_I_TERM      (MAX_LONG / 2)

/**
 * @brief      Range limits for different data types. Needed to avoid
 *             sign/overflow problems
 */
#define UPPER_SAT       100
#define LOWER_SAT      -100
#define FLOAT_MAX       1000
#define MAX_F_I_TERM    (FLOAT_MAX / 2)

/** @} */

/**
 * @addtogroup  Pid_Variables
 * @brief       PID compensator variables. @{
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
static volatile float   KP_INIT = 1;

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

/** @} */

/**
 * @addtogroup  Pid_Typedefs
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
    PID_IntMethod_FwdRect = 19,    //!< Use method "Forward Rectangle".
    PID_IntMethod_RwdRect = 20,    //!< Use method "Reward Rectangle".
    PID_IntMethod_Trapez = 21,     //!< Use method "Trapezoidal".
} PID_IntMethod_t;

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
    float W_remf;                   //!< Angle set-point value [float]. (Updated via Qt interface).
    int16_t W_int;                  //!< Internal set-point (Updated via Qt interface).
    int16_t W_rem;                  //!< Angle set-point value [int16]. (Updated via Qt interface).
    int16_t W[PID_BUFF_LENGTH];     //!< Set-point history.
    int16_t E[PID_BUFF_LENGTH];     //!< Error history: e(k)=w(k)-p(k).
    int16_t ESUM[PID_BUFF_LENGTH];  //!< Error sum history: e(k)=e(k-1)+w-p.
    int16_t P[PID_BUFF_LENGTH];     //!< Process value (sensor output).
    int16_t Y[PID_BUFF_LENGTH];     //!< PID output history.
    uint8_t remoteCmdActive;        //!< Flag. The commanding source of set-point value is stored in g.() */
    uint16_t Tf;                    //!< Time constant for 1. order D-term filter @si{us} (see MATLAB/Simulink).
    uint16_t Ts;                    //!< Sample time @si{us}.
    uint16_t ind;                   //!< Index

    /**
     * FIXME:   Avoid overflow! (W - P) !!<=!! maxError.
     */
    int16_t maxError;               //!< Max. value of error signal */

    /**
     * FIXME:   Avoid overflow! E(k) !!<=!! maxSumError.
     *
     * TODO:    Implement failsafe anti-windup system.
     */
    int16_t maxSumError;            //!< Max. value of integral error.

#ifdef FLOAT_PARAMETERS
    float b0, b1, b2;               //!< PID algorithm coefficients [float].
    float Kp;                       //!< PID proportional gain [float].
    float Ki;                       //!< PID integral gain [float].
    float Kd;                       //!< PID derivative gain [float].
#elif DOUBLE_PARAMETERS
    double b0, b1, b2;              //!< PID algorithm coefficients [double].
    double Kp;                      //!< PID proportional gain [double].
    double Ki;                      //!< PID integral gain [double].
    double Kd;                      //!< PID derivative gain [double].
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
    int16_t W;                  //!< Angle set-point value [int16].
    int16_t W_int;              //!< Internal set-point (Updated via Qt interface).
    int16_t W_rem;              //!< Angle set-point value [int16]. (Updated via Qt interface).
    int16_t Y;                  //!< PID output value.
    int16_t E;                  //!< Actual error: e(k)=w(k)-p(k).
    int16_t ESUM;               //!< Error sum: e(k)=e(k-1)+w-p.
    int16_t P;                  //!< Process value (sensor output).
    uint16_t ind;               //!< Index.
    uint8_t remoteCmdActive;    //!< Flag. The commanding source of set-point value is stored in g.() */
    uint16_t Tf;                //!< Time constant for 1. order D-term filter @si{us} (see MATLAB/Simulink).
    uint16_t Ts;                //!< Sample time @si{us}.

    /**
     * FIXME:   Avoid overflow! (W - P) !!<=!! maxError.
     */
    int16_t maxError;           //!< Max. value of error signal */

    /**
     * FIXME:   Avoid overflow! E(k) !!<=!! maxSumError.
     *
     * TODO:    Implement fail-safe anti-windup system.
     */
    int16_t maxSumError;        //!< Max. value of integral error.

    float b0, b1, b2;           //!< PID algorithm coefficients [float].
    float Kp;                   //!< PID proportional gain [float].
    float Ki;                   //!< PID integral gain [float].
    float Kd;                   //!< PID derivative gain [float].

} pidData_t_NEW;



/**
 * @brief      Runtime memory structure of single PID instance (float).
 *
 *             Holds compensator values, range parameters and runtime data for a
 *             single PID instance.
 */
typedef struct FPID_DATA {
    int16_t W_raw;
    int16_t Y_raw;
    float raw2float;
    float float2raw;
    float MAX_Y_I_TERM;
    float W;                    //!< Angle set-point value [int16].
    float Y;                    //!< PID output value.
    float E;                    //!< Actual error: e(k)=w(k)-p(k).
    float ESUM;                 //!< Error sum: e(k)=e(k-1)+w-p.
    float P;                    //!< Process value (sensor output).
    uint16_t Tf;                //!< Time constant for 1. order D-term filter @si{us} (see MATLAB/Simulink).
    uint16_t Ts;                //!< Sample time @si{us}.

    /**
     * FIXME:   Avoid overflow! (W - P) !!<=!! maxError.
     */
    float maxError;             //!< Max. value of error signal */

    /**
     * FIXME:   Avoid overflow! E(k) !!<=!! maxSumError.
     *
     * TODO:    Implement fail-safe anti-windup system.
     */
    float maxSumError;          //!< Max. value of integral error.

    float b0, b1, b2;           //!< PID algorithm coefficients [float].
    float Kp;                   //!< PID proportional gain [float].
    float Ki;                   //!< PID integral gain [float].
    float Kd;                   //!< PID derivative gain [float].

} pidDatafloat_t;

/**
* @brief      External declared PID instances.
*/
extern struct PID_DATA   pidDataX, pidDataY;


/* Re-Enable compiler optimization */
#if defined (KEIL_IDE)
#pragma O2
#elif defined (__GNUC__)
#pragma GCC pop_options
#endif

/** @} */

/**
 * @addtogroup  Pid_Functions
 * @brief       PID methods related macro definitions. @{
 */

/**
 * @brief      Initialization of PID controller parameters. Initialize the
 *             variables used by the PID algorithm.
 */
void pid_Init_Tp (void);


/**
 * @brief      Initialization method for integral type PID instance.
 *
 *             Initial load an integral type PID instances controller parameters.
 *
 * @param      Kp       PID proportional gain value.
 * @param      Ki       PID integral gain value.
 * @param      Kd       PID derivate gain value.
 * @param[in]  setTf    Initial value for 1.st order derivative term filter time
 *                      constant @si{us}.
 * @param[in]  setTs    Sample rate @si{us};
 * @param      pid      Pointer to the instances PID structure.
 * @param[in]  stType   Select buffered or unbuffered PID structure (See
 *                      PID_struct_type enumeration).
 */
void pid_Init (float Kp, float Ki, float Kd, uint16_t setTf, uint16_t setTs,
               struct PID_DATA *pid, PID_StructType_t stType);


/**
 * @brief      Initialization method for double type PID instance.
 *
 *             Initial load an double type PID instances controller parameters.
 *
 * @param      Kp      PID proportional gain value.
 * @param      Ki      PID integral gain value.
 * @param      Kd      PID derivate gain value.
 * @param[in]  setTf   Initial value for 1.st order D-filter time constant
 *                     @si{us}.
 * @param[in]  setTs   Sample rate @si{us};
 * @param      pid     Pointer to the instances PID structure.
 */
void fpid_Init (double Kp, double Ki, double Kd, uint16_t setTf,
                uint16_t setTs, struct FPID_DATA *pid);

/**
 * @brief      Clear integral part of PID model state.
 *
 *             Resets the runtime integrator of the PID compensator. Use for
 *             anti-windup system, after over current tripping etc.
 *
 * @param      pid      Pointer to the instances PID structure.
 * @param[in]  stType   Select buffered or unbuffered PID structure (See
 *                      PID_struct_type enumeration).
 */
void pid_Reset_Integrator (volatile struct PID_DATA *pid, PID_StructType_t stType);

/**
 * @brief      Pre-Calculates PID coefficients.
 *
 *             Pre-Calculation of coefficients after a parameter update can
 *             speedup the underlying algorithms.
 *
 * @param      pid         Pointer to the instances PID structure.
 * @param[in]  intMethod   The integration method
 */
void PID_Calc_Coeffs (struct PID_DATA *pid, PID_IntMethod_t intMethod);

/**
 * @brief      Update the compensator parameters of a given PID instance.
 *
 * @param[in]  Kp      New proportional gain value.
 * @param[in]  Ki      New integral gain value.
 * @param[in]  Kd      New derivative gain value.
 * @param[in]  setTf   New 1st order filter time constant value.
 * @param      pid     Pointer to the instances PID structure.
 */
void update_pid_data (float Kp, float Ki, float Kd,
                      uint16_t setTf, struct PID_DATA *pid);

/**
 * @brief      Invoke integral type PID compensator calculation.
 *
 *             Invoke the control algorithm calculation processes for a single
 *             integral type PID instance.
 *
 * @param[in]  setPoint       Pass next set-point value via setPoint.
 * @param[in]  processValue   Pass actual process (e.g. plant output) value.
 * @param      pid            Pointer to the instances PID structure.
 *
 * @return     Compensator output, next actuator control signal.
 */
int pid_Controller (int16_t setPoint, int16_t processValue,
                    struct PID_DATA *pid);

/**
 * @brief      Invoke integral type PID compensator calculation.
 *
 *             Invoke the control algorithm calculation processes for a single
 *             integral type PID instance.
 *
 * @param[in]  setPoint       Pass next set-point value via setPoint.
 * @param[in]  processValue   Pass actual process (e.g. plant output) value.
 * @param      pid            Pointer to the instances PID structure.
 *
 * @return     Compensator output, next actuator control signal.
 */
int fpid_Controller (int16_t setPoint, int16_t processValue,
                     struct FPID_DATA *pid);

/** @} */

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif


#endif

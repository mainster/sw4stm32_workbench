/*
 * File: pid4.h
 *
 * Code generated for Simulink model :pid4.
 *
 * Model version      : 1.62
 * Simulink Coder version    : 8.5 (R2013b) 08-Aug-2013
 * TLC version       : 8.5 (Aug  6 2013)
 * C/C++ source code generated on  : Mon May 18 20:24:07 2015
 *
 * Target selection: stm32F4xx.tlc
 * Embedded hardware selection: STMicroelectronics->STM32F4xx 32-bit Cortex-M4
 * Code generation objectives:
 *    1. Traceability
 *    2. Debugging
 * Validation result: Not run
 *
 *
 *
 * ******************************************************************************
 * * attention
 * *
 * * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
 * * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 * *
 * ******************************************************************************
 */

#ifndef RTW_HEADER_pid4_h_
#define RTW_HEADER_pid4_h_
#ifndef pid4_COMMON_INCLUDES_
# define pid4_COMMON_INCLUDES_
#include <stddef.h>
#include <string.h>
#include "rtwtypes.h"
#endif                                 /* pid4_COMMON_INCLUDES_ */

#include "pid4_types.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
# define rtmGetErrorStatus(rtm)        ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
# define rtmSetErrorStatus(rtm, val)   ((rtm)->errorStatus = (val))
#endif

#ifndef rtmGetStopRequested
# define rtmGetStopRequested(rtm)      ((void*) 0)
#endif

/* user code (top of header file) */
#define TS                             40.0e-6

/* Block signals and states (auto storage) for system '<Root>' */
typedef struct {
    real_T P_Gain;                     /* '<S14>/P_Gain' */
    real_T pulseGen;                   /* '<S2>/pulseGen' */
    real_T sumW;                       /* '<S2>/sumW' */
    real_T pulseGenSw;                 /* '<S2>/pulseGenSw' */
    real_T error;                      /* '<Root>/sume' */
    real_T Proportionals;              /* '<S14>/Proportionals' */
    real_T integrate;                  /* '<S5>/integrate' */
    real_T I_gain;                     /* '<S16>/I_gain' */
    real_T D_gain;                     /* '<S12>/D_gain' */
    real_T Derivative;                 /* '<S12>/Derivative' */
    real_T filter;                     /* '<S5>/filter' */
    real_T sumd;                       /* '<S5>/sumd' */
    real_T Filtercoeff;                /* '<S16>/Filtercoeff' */
    real_T sump;                       /* '<S5>/sump' */
    real_T I_gain_a;                   /* '<S13>/I_gain' */
    real_T Integral;                   /* '<S13>/Integral' */
    real_T I_gain_o;                   /* '<S15>/I_gain' */
    real_T sums;                       /* '<S5>/sums' */
    real_T antiwindup;                 /* '<S15>/antiwindup' */
    real_T sumI;                       /* '<S5>/sumI' */
    real_T _xintergrator;              /* '<S5>/integrate' */
    real_T _xfilter;                   /* '<S5>/filter' */
    real_T D;                          /* '<Root>/D_gain_Memory' */
    real_T I;                          /* '<Root>/I_gain_memory' */
    real_T N;                          /* '<Root>/N_fact_Memory' */
    real_T P;                          /* '<Root>/P_gain_Memory' */
    real_T kb;                         /* '<Root>/kb_fact_Memory' */
    int32_T clockTickCounter;          /* '<S2>/pulseGen' */
} DW_pid4;

/* External inputs (root inport signals with auto storage) */
typedef struct {
    real_T r1;                         /* '<Root>/r1' */
    real_T ym;                         /* '<Root>/ym' */
} ExtU_pid4;

/* External outputs (root outports fed by signals with auto storage) */
typedef struct {
    real_T yc;                         /* '<Root>/yc' */
} ExtY_pid4;

/* Real-time Model Data Structure */
struct tag_RTM_pid4 {
    const char_T *errorStatus;
};

/* Block signals and states (auto storage) */
extern DW_pid4 pid4_DW;

/* External inputs (root inport signals with auto storage) */
extern ExtU_pid4 pid4_U;

/* External outputs (root outports fed by signals with auto storage) */
extern ExtY_pid4 pid4_Y;

/* Model entry point functions */
extern void pid4_initialize(void);
extern void pid4_step(void);

/* Real-time Model object */
extern RT_MODEL_pid4 *const pid4_M;

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'pid4'
 * '<S1>'   : 'pid4/pidstm'
 * '<S2>'   : 'pid4/pulseGen'
 * '<S3>'   : 'pid4/pidstm/pidBlock'
 * '<S4>'   : 'pid4/pidstm/pidIdeal'
 * '<S5>'   : 'pid4/pidstm/pidParallel'
 * '<S6>'   : 'pid4/pidstm/pidBlock/PIDController'
 * '<S7>'   : 'pid4/pidstm/pidIdeal/Kd'
 * '<S8>'   : 'pid4/pidstm/pidIdeal/Ki'
 * '<S9>'   : 'pid4/pidstm/pidIdeal/Kp'
 * '<S10>'  : 'pid4/pidstm/pidIdeal/awm'
 * '<S11>'  : 'pid4/pidstm/pidIdeal/filterCoeff'
 * '<S12>'  : 'pid4/pidstm/pidParallel/Kd'
 * '<S13>'  : 'pid4/pidstm/pidParallel/Ki'
 * '<S14>'  : 'pid4/pidstm/pidParallel/Kp'
 * '<S15>'  : 'pid4/pidstm/pidParallel/awm'
 * '<S16>'  : 'pid4/pidstm/pidParallel/filterCoeff'
 */
#endif                                 /* RTW_HEADER_pid4_h_ */

/* File trailer for Real-Time Workshop generated code.
 *
 * [EOF] pid4.h
 */

/*
 * File: pid_lib_v3.h
 *
 * Code generated for Simulink model :pid_lib_v3.
 *
 * Model version      : 1.35
 * Simulink Coder version    : 8.5 (R2013b) 08-Aug-2013
 * TLC version       : 8.5 (Aug  6 2013)
 * C/C++ source code generated on  : Sun May 17 23:29:07 2015
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

#ifndef RTW_HEADER_pid_lib_v3_h_
#define RTW_HEADER_pid_lib_v3_h_
#include "rtw_modelmap.h"
#ifndef pid_lib_v3_COMMON_INCLUDES_
# define pid_lib_v3_COMMON_INCLUDES_
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "rtwtypes.h"
#include "builtin_typeid_types.h"
#endif                                 /* pid_lib_v3_COMMON_INCLUDES_ */

#include "pid_lib_v3_types.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetDataMapInfo
# define rtmGetDataMapInfo(rtm)        ((rtm)->DataMapInfo)
#endif

#ifndef rtmSetDataMapInfo
# define rtmSetDataMapInfo(rtm, val)   ((rtm)->DataMapInfo = (val))
#endif

#ifndef rtmGetDefaultParam
# define rtmGetDefaultParam(rtm)       ((rtm)->ModelData.defaultParam)
#endif

#ifndef rtmSetDefaultParam
# define rtmSetDefaultParam(rtm, val)  ((rtm)->ModelData.defaultParam = (val))
#endif

#ifndef rtmGetParamIsMalloced
# define rtmGetParamIsMalloced(rtm)    ((rtm)->ModelData.paramIsMalloced)
#endif

#ifndef rtmSetParamIsMalloced
# define rtmSetParamIsMalloced(rtm, val) ((rtm)->ModelData.paramIsMalloced = (val))
#endif

#ifndef rtmGetRootDWork
# define rtmGetRootDWork(rtm)          ((rtm)->ModelData.dwork)
#endif

#ifndef rtmSetRootDWork
# define rtmSetRootDWork(rtm, val)     ((rtm)->ModelData.dwork = (val))
#endif

#ifndef rtmGetErrorStatus
# define rtmGetErrorStatus(rtm)        ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
# define rtmSetErrorStatus(rtm, val)   ((rtm)->errorStatus = (val))
#endif

#ifndef rtmGetStopRequested
# define rtmGetStopRequested(rtm)      ((void*) 0)
#endif

#define pid_lib_v3_M_TYPE              RT_MODEL_pid_lib_v3

/* user code (top of header file) */
#define TS                             10.0e-6

/* Block signals and states (auto storage) for system '<Root>' */
typedef struct {
  real_T D_gain;                       /* '<S2>/D_gain' */
  real_T pulseGen;                     /* '<S1>/pulseGen' */
  real_T SumI1;                        /* '<S1>/SumI1' */
  real_T pulseGenEN;                   /* '<S1>/pulseGenEN' */
  real_T Sum;                          /* '<Root>/Sum' */
  real_T SumE;                         /* '<S1>/SumE' */
  real_T Derivative;                   /* '<S2>/Derivative' */
  real_T Filter;                       /* '<S1>/Filter' */
  real_T I_gain;                       /* '<S3>/I_gain' */
  real_T SumD;                         /* '<S1>/SumD' */
  real_T Filtercoeff;                  /* '<S3>/Filtercoeff' */
  real_T I_gain_p;                     /* '<S4>/I_gain' */
  real_T Integral;                     /* '<S4>/Integral' */
  real_T Integrator;                   /* '<S1>/Integrator' */
  real_T P_Gain;                       /* '<S5>/P_Gain' */
  real_T SumP;                         /* '<S1>/SumP' */
  real_T Proportionals;                /* '<S5>/Proportionals' */
  real_T I_gain_m;                     /* '<S6>/I_gain' */
  real_T SumSat;                       /* '<S1>/SumSat' */
  real_T antiwindup;                   /* '<S6>/antiwindup' */
  real_T SumI;                         /* '<S1>/SumI' */
  real_T _xfilter;                     /* '<S1>/Filter' */
  real_T _xintergrator;                /* '<S1>/Integrator' */
  real_T D;                            /* '<S1>/D_gain_Memory' */
  real_T I;                            /* '<S1>/I_gain_memory' */
  real_T N;                            /* '<S1>/N_fact_Memory' */
  real_T P;                            /* '<S1>/P_gain_Memory' */
  real_T kb;                           /* '<S1>/kb_fact_Memory' */
  int32_T clockTickCounter;            /* '<S1>/pulseGen' */
} DW_pid_lib_v3;

/* Parameters (auto storage) */
struct P_pid_lib_v3_ {
  real_T const_D_Value;                /* Expression: 0
                                        * Referenced by: '<S1>/const_D'
                                        */
  real_T const_I_Value;                /* Expression: 0
                                        * Referenced by: '<S1>/const_I'
                                        */
  real_T const_kb_Value;               /* Expression: .8
                                        * Referenced by: '<S1>/const_kb'
                                        */
  real_T const_N_Value;                /* Expression: 100
                                        * Referenced by: '<S1>/const_N'
                                        */
  real_T const_P_Value;                /* Expression: 1
                                        * Referenced by: '<S1>/const_P'
                                        */
  real_T const_D1_Value;               /* Expression: 0.25
                                        * Referenced by: '<S1>/const_D1'
                                        */
  real_T pulseGen_Amp;                 /* Expression: 0.5
                                        * Referenced by: '<S1>/pulseGen'
                                        */
  real_T pulseGen_Period;              /* Expression: 10e3
                                        * Referenced by: '<S1>/pulseGen'
                                        */
  real_T pulseGen_Duty;                /* Expression: 5e3
                                        * Referenced by: '<S1>/pulseGen'
                                        */
  real_T pulseGen_PhaseDelay;          /* Expression: 0
                                        * Referenced by: '<S1>/pulseGen'
                                        */
  real_T const_I1_Value;               /* Expression: 0
                                        * Referenced by: '<S1>/const_I1'
                                        */
  real_T Filter_gainval;               /* Computed Parameter: Filter_gainval
                                        * Referenced by: '<S1>/Filter'
                                        */
  real_T Filter_IC;                    /* Expression: 0
                                        * Referenced by: '<S1>/Filter'
                                        */
  real_T Integrator_gainval;           /* Computed Parameter: Integrator_gainval
                                        * Referenced by: '<S1>/Integrator'
                                        */
  real_T Integrator_IC;                /* Expression: 0
                                        * Referenced by: '<S1>/Integrator'
                                        */
  real_T Saturation_UpperSat;          /* Expression: 1.2
                                        * Referenced by: '<S1>/Saturation'
                                        */
  real_T Saturation_LowerSat;          /* Expression: -1.2
                                        * Referenced by: '<S1>/Saturation'
                                        */
  real_T D_gain_Memory_InitialValue;   /* Expression: 0
                                        * Referenced by: '<S1>/D_gain_Memory'
                                        */
  real_T I_gain_memory_InitialValue;   /* Expression: 0
                                        * Referenced by: '<S1>/I_gain_memory'
                                        */
  real_T N_fact_Memory_InitialValue;   /* Expression: 0
                                        * Referenced by: '<S1>/N_fact_Memory'
                                        */
  real_T P_gain_Memory_InitialValue;   /* Expression: 0
                                        * Referenced by: '<S1>/P_gain_Memory'
                                        */
  real_T kb_fact_Memory_InitialValue;  /* Expression: 0
                                        * Referenced by: '<S1>/kb_fact_Memory'
                                        */
  uint8_T pulseGenEN_CurrentSetting;   /* Computed Parameter: pulseGenEN_CurrentSetting
                                        * Referenced by: '<S1>/pulseGenEN'
                                        */
};

/* Real-time Model Data Structure */
struct tag_RTM_pid_lib_v3 {
  const char_T *errorStatus;

  /*
   * DataMapInfo:
   * The following substructure contains information regarding
   * structures generated in the model's C API.
   */
  struct {
    rtwCAPI_ModelMappingInfo mmi;
    void* dataAddress[46];
    int32_T* vardimsAddress[46];
  } DataMapInfo;

  /*
   * ModelData:
   * The following substructure contains information regarding
   * the data used in the model.
   */
  struct {
    P_pid_lib_v3 *defaultParam;
    boolean_T paramIsMalloced;
    DW_pid_lib_v3 *dwork;
  } ModelData;
};

extern P_pid_lib_v3 pid_lib_v3_P;      /* parameters */

/* External data declarations for dependent source files */
extern const char *RT_MEMORY_ALLOCATION_ERROR;

/* Model entry point functions */
extern RT_MODEL_pid_lib_v3 *pid_lib_v3(real_T *pid_lib_v3_U_error, real_T
  *pid_lib_v3_U_r, real_T *pid_lib_v3_U_ym, real_T *pid_lib_v3_Y_Ypid);
extern void pid_lib_v3_initialize(RT_MODEL_pid_lib_v3 *const pid_lib_v3_M,
  real_T *pid_lib_v3_U_error, real_T *pid_lib_v3_U_r, real_T *pid_lib_v3_U_ym,
  real_T *pid_lib_v3_Y_Ypid);
extern void pid_lib_v3_step(RT_MODEL_pid_lib_v3 *const pid_lib_v3_M, real_T
  pid_lib_v3_U_error, real_T pid_lib_v3_U_r, real_T pid_lib_v3_U_ym, real_T
  *pid_lib_v3_Y_Ypid);
extern void pid_lib_v3_terminate(RT_MODEL_pid_lib_v3 * pid_lib_v3_M);

/* Function to get C API Model Mapping Static Info */
extern const rtwCAPI_ModelMappingStaticInfo*
  pid_lib_v3_GetCAPIStaticMap(void);

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
 * '<Root>' : 'pid_lib_v3'
 * '<S1>'   : 'pid_lib_v3/PID_STM32'
 * '<S2>'   : 'pid_lib_v3/PID_STM32/Derivative'
 * '<S3>'   : 'pid_lib_v3/PID_STM32/FilterCoeff'
 * '<S4>'   : 'pid_lib_v3/PID_STM32/Integral'
 * '<S5>'   : 'pid_lib_v3/PID_STM32/Proportional'
 * '<S6>'   : 'pid_lib_v3/PID_STM32/anti_windup1'
 */

/*-
 * Requirements for '<Root>': pid_lib_v3
 */
#endif                                 /* RTW_HEADER_pid_lib_v3_h_ */

/* File trailer for Real-Time Workshop generated code.
 *
 * [EOF] pid_lib_v3.h
 */

/*
 * File: pid_lib_v3.c
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

#include "pid_lib_v3_capi.h"
#include "pid_lib_v3.h"
#include "pid_lib_v3_private.h"

/* Model step function */
void pid_lib_v3_step(RT_MODEL_pid_lib_v3 *const pid_lib_v3_M, real_T
                     pid_lib_v3_U_error, real_T pid_lib_v3_U_r, real_T
                     pid_lib_v3_U_ym, real_T *pid_lib_v3_Y_Ypid)
{
  P_pid_lib_v3 *pid_lib_v3_P = ((P_pid_lib_v3 *)
    pid_lib_v3_M->ModelData.defaultParam);
  DW_pid_lib_v3 *pid_lib_v3_DW = ((DW_pid_lib_v3 *)
    pid_lib_v3_M->ModelData.dwork);
  real_T u;
  real_T u_0;
  real_T u_1;

  /* DataStoreWrite: '<S1>/D_gain_Write' incorporates:
   *  Constant: '<S1>/const_D'
   */
  pid_lib_v3_DW->D = pid_lib_v3_P->const_D_Value;

  /* DataStoreWrite: '<S1>/I_gain_Write' incorporates:
   *  Constant: '<S1>/const_I'
   */
  pid_lib_v3_DW->I = pid_lib_v3_P->const_I_Value;

  /* DataStoreWrite: '<S1>/Kb_fact_Write' incorporates:
   *  Constant: '<S1>/const_kb'
   */
  pid_lib_v3_DW->kb = pid_lib_v3_P->const_kb_Value;

  /* DataStoreWrite: '<S1>/N_coeff_Write' incorporates:
   *  Constant: '<S1>/const_N'
   */
  pid_lib_v3_DW->N = pid_lib_v3_P->const_N_Value;

  /* DataStoreWrite: '<S1>/P_gain_Write' incorporates:
   *  Constant: '<S1>/const_P'
   */
  pid_lib_v3_DW->P = pid_lib_v3_P->const_P_Value;

  /* DataStoreRead: '<S2>/D_gain' */
  pid_lib_v3_DW->D_gain = pid_lib_v3_DW->D;

  /* DiscretePulseGenerator: '<S1>/pulseGen' */
  pid_lib_v3_DW->pulseGen = (pid_lib_v3_DW->clockTickCounter <
    pid_lib_v3_P->pulseGen_Duty) && (pid_lib_v3_DW->clockTickCounter >= 0) ?
    pid_lib_v3_P->pulseGen_Amp : 0.0;
  if (pid_lib_v3_DW->clockTickCounter >= pid_lib_v3_P->pulseGen_Period - 1.0) {
    pid_lib_v3_DW->clockTickCounter = 0;
  } else {
    pid_lib_v3_DW->clockTickCounter++;
  }

  /* End of DiscretePulseGenerator: '<S1>/pulseGen' */

  /* Sum: '<S1>/SumI1' incorporates:
   *  Constant: '<S1>/const_D1'
   */
  pid_lib_v3_DW->SumI1 = pid_lib_v3_DW->pulseGen - pid_lib_v3_P->const_D1_Value;

  /* ManualSwitch: '<S1>/pulseGenEN' incorporates:
   *  Constant: '<S1>/const_I1'
   */
  if (pid_lib_v3_P->pulseGenEN_CurrentSetting == 1) {
    pid_lib_v3_DW->pulseGenEN = pid_lib_v3_DW->SumI1;
  } else {
    pid_lib_v3_DW->pulseGenEN = pid_lib_v3_P->const_I1_Value;
  }

  /* End of ManualSwitch: '<S1>/pulseGenEN' */

  /* Sum: '<Root>/Sum' incorporates:
   *  Inport: '<Root>/ym'
   */
  pid_lib_v3_DW->Sum = pid_lib_v3_U_ym - 0.0;

  /* Sum: '<S1>/SumE' incorporates:
   *  Inport: '<Root>/error'
   *  Inport: '<Root>/r'
   */
  pid_lib_v3_DW->SumE = ((pid_lib_v3_DW->pulseGenEN + pid_lib_v3_U_error) +
    pid_lib_v3_U_r) - pid_lib_v3_DW->Sum;

  /* Product: '<S2>/Derivative' */
  pid_lib_v3_DW->Derivative = pid_lib_v3_DW->D_gain * pid_lib_v3_DW->SumE;

  /* DiscreteIntegrator: '<S1>/Filter' */
  pid_lib_v3_DW->Filter = pid_lib_v3_DW->_xfilter;

  /* DataStoreRead: '<S3>/I_gain' */
  pid_lib_v3_DW->I_gain = pid_lib_v3_DW->N;

  /* Sum: '<S1>/SumD' */
  pid_lib_v3_DW->SumD = pid_lib_v3_DW->Derivative - pid_lib_v3_DW->Filter;

  /* Product: '<S3>/Filtercoeff' */
  pid_lib_v3_DW->Filtercoeff = pid_lib_v3_DW->I_gain * pid_lib_v3_DW->SumD;

  /* DataStoreRead: '<S4>/I_gain' */
  pid_lib_v3_DW->I_gain_p = pid_lib_v3_DW->I;

  /* Product: '<S4>/Integral' */
  pid_lib_v3_DW->Integral = pid_lib_v3_DW->I_gain_p * pid_lib_v3_DW->SumE;

  /* DiscreteIntegrator: '<S1>/Integrator' */
  pid_lib_v3_DW->Integrator = pid_lib_v3_DW->_xintergrator;

  /* DataStoreRead: '<S5>/P_Gain' */
  pid_lib_v3_DW->P_Gain = pid_lib_v3_DW->P;

  /* Sum: '<S1>/SumP' */
  pid_lib_v3_DW->SumP = (pid_lib_v3_DW->SumE + pid_lib_v3_DW->Integrator) +
    pid_lib_v3_DW->Filtercoeff;

  /* Product: '<S5>/Proportionals' */
  pid_lib_v3_DW->Proportionals = pid_lib_v3_DW->P_Gain * pid_lib_v3_DW->SumP;

  /* Saturate: '<S1>/Saturation' */
  u = pid_lib_v3_DW->Proportionals;
  u_0 = pid_lib_v3_P->Saturation_LowerSat;
  u_1 = pid_lib_v3_P->Saturation_UpperSat;
  if (u >= u_1) {
    /* Outport: '<Root>/Ypid' */
    *pid_lib_v3_Y_Ypid = u_1;
  } else if (u <= u_0) {
    /* Outport: '<Root>/Ypid' */
    *pid_lib_v3_Y_Ypid = u_0;
  } else {
    /* Outport: '<Root>/Ypid' */
    *pid_lib_v3_Y_Ypid = u;
  }

  /* End of Saturate: '<S1>/Saturation' */

  /* DataStoreRead: '<S6>/I_gain' */
  pid_lib_v3_DW->I_gain_m = pid_lib_v3_DW->kb;

  /* Sum: '<S1>/SumSat' */
  pid_lib_v3_DW->SumSat = *pid_lib_v3_Y_Ypid - pid_lib_v3_DW->Proportionals;

  /* Product: '<S6>/antiwindup' */
  pid_lib_v3_DW->antiwindup = pid_lib_v3_DW->I_gain_m * pid_lib_v3_DW->SumSat;

  /* Sum: '<S1>/SumI' */
  pid_lib_v3_DW->SumI = pid_lib_v3_DW->Integral - pid_lib_v3_DW->antiwindup;

  /* Update for DiscreteIntegrator: '<S1>/Filter' */
  pid_lib_v3_DW->_xfilter += pid_lib_v3_P->Filter_gainval *
    pid_lib_v3_DW->Filtercoeff;

  /* Update for DiscreteIntegrator: '<S1>/Integrator' */
  pid_lib_v3_DW->_xintergrator += pid_lib_v3_P->Integrator_gainval *
    pid_lib_v3_DW->SumI;
}

/* Model initialize function */
void pid_lib_v3_initialize(RT_MODEL_pid_lib_v3 *const pid_lib_v3_M, real_T
  *pid_lib_v3_U_error, real_T *pid_lib_v3_U_r, real_T *pid_lib_v3_U_ym, real_T
  *pid_lib_v3_Y_Ypid)
{
  P_pid_lib_v3 *pid_lib_v3_P = ((P_pid_lib_v3 *)
    pid_lib_v3_M->ModelData.defaultParam);
  DW_pid_lib_v3 *pid_lib_v3_DW = ((DW_pid_lib_v3 *)
    pid_lib_v3_M->ModelData.dwork);

  /* Start for DiscretePulseGenerator: '<S1>/pulseGen' */
  pid_lib_v3_DW->clockTickCounter = 0;

  /* Start for DataStoreMemory: '<S1>/D_gain_Memory' */
  pid_lib_v3_DW->D = pid_lib_v3_P->D_gain_Memory_InitialValue;

  /* Start for DataStoreMemory: '<S1>/I_gain_memory' */
  pid_lib_v3_DW->I = pid_lib_v3_P->I_gain_memory_InitialValue;

  /* Start for DataStoreMemory: '<S1>/N_fact_Memory' */
  pid_lib_v3_DW->N = pid_lib_v3_P->N_fact_Memory_InitialValue;

  /* Start for DataStoreMemory: '<S1>/P_gain_Memory' */
  pid_lib_v3_DW->P = pid_lib_v3_P->P_gain_Memory_InitialValue;

  /* Start for DataStoreMemory: '<S1>/kb_fact_Memory' */
  pid_lib_v3_DW->kb = pid_lib_v3_P->kb_fact_Memory_InitialValue;

  /* InitializeConditions for DiscreteIntegrator: '<S1>/Filter' */
  pid_lib_v3_DW->_xfilter = pid_lib_v3_P->Filter_IC;

  /* InitializeConditions for DiscreteIntegrator: '<S1>/Integrator' */
  pid_lib_v3_DW->_xintergrator = pid_lib_v3_P->Integrator_IC;
}

/* Model terminate function */
void pid_lib_v3_terminate(RT_MODEL_pid_lib_v3 * pid_lib_v3_M)
{
  /* model code */
  if (pid_lib_v3_M->ModelData.paramIsMalloced) {
    rt_FREE(pid_lib_v3_M->ModelData.defaultParam);
  }

  rt_FREE(pid_lib_v3_M->ModelData.dwork);
  rt_FREE(pid_lib_v3_M);
}

/* Model data allocation function */
RT_MODEL_pid_lib_v3 *pid_lib_v3(real_T *pid_lib_v3_U_error, real_T
  *pid_lib_v3_U_r, real_T *pid_lib_v3_U_ym, real_T *pid_lib_v3_Y_Ypid)
{
  RT_MODEL_pid_lib_v3 *pid_lib_v3_M;
  pid_lib_v3_M = (RT_MODEL_pid_lib_v3 *) malloc(sizeof(RT_MODEL_pid_lib_v3));
  if (pid_lib_v3_M == NULL) {
    return NULL;
  }

  (void) memset((char *)pid_lib_v3_M, 0,
                sizeof(RT_MODEL_pid_lib_v3));

  /* parameters */
  {
    P_pid_lib_v3 *p;
    static int_T pSeen = FALSE;

    /* only malloc on multiple model instantiation */
    if (pSeen == TRUE ) {
      p = (P_pid_lib_v3 *) malloc(sizeof(P_pid_lib_v3));
      rt_VALIDATE_MEMORY(pid_lib_v3_M,p);
      (void) memcpy(p, &pid_lib_v3_P,
                    sizeof(P_pid_lib_v3));
      pid_lib_v3_M->ModelData.paramIsMalloced = (TRUE);
    } else {
      p = &pid_lib_v3_P;
      pid_lib_v3_M->ModelData.paramIsMalloced = (FALSE);
      pSeen = TRUE;
    }

    pid_lib_v3_M->ModelData.defaultParam = (p);
  }

  /* states (dwork) */
  {
    DW_pid_lib_v3 *dwork = (DW_pid_lib_v3 *) malloc(sizeof(DW_pid_lib_v3));
    rt_VALIDATE_MEMORY(pid_lib_v3_M,dwork);
    pid_lib_v3_M->ModelData.dwork = (dwork);
  }

  /* Initialize DataMapInfo substructure containing ModelMap for C API */
  pid_lib_v3_InitializeDataMapInfo(pid_lib_v3_M,
    pid_lib_v3_M->ModelData.defaultParam, pid_lib_v3_M->ModelData.dwork);

  {
    P_pid_lib_v3 *pid_lib_v3_P = ((P_pid_lib_v3 *)
      pid_lib_v3_M->ModelData.defaultParam);
    DW_pid_lib_v3 *pid_lib_v3_DW = ((DW_pid_lib_v3 *)
      pid_lib_v3_M->ModelData.dwork);

    /* states (dwork) */
    (void) memset((void *)pid_lib_v3_DW, 0,
                  sizeof(DW_pid_lib_v3));

    /* external inputs */
    (*pid_lib_v3_U_error) = 0.0;
    (*pid_lib_v3_U_r) = 0.0;
    (*pid_lib_v3_U_ym) = 0.0;

    /* external outputs */
    (*pid_lib_v3_Y_Ypid) = 0.0;
  }

  return pid_lib_v3_M;
}

/* File trailer for Real-Time Workshop generated code.
 *
 * [EOF] pid_lib_v3.c
 */

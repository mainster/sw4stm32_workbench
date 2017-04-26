/**
 * @file        adc_dac_dma.c
 *
 * @date        30 Jan 2017
 * @author      Manuel Del Basso (mainster)
 * @email       manuel.delbasso@gmail.com
 *
 * @ide         System Workbench ac6 (eclipse stm32)
 * @stdperiph   STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 * @license   GNU GPL v3
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
#include "adc_dac_dma.h"


//ActuatorSafestateGuard_t ASG;
extern ActuatorSafestateGuard_t ASG;

/**
 * @addtogroup MD_APP
 * @{
 */

/**
 * @addtogroup APP_Analog
 * @{
 */

/**
 * @addtogroup Analog_Variables
 * @{
 */

/** @} */

/**
 * @addtogroup Analog_Function_Pointers
 * @{
 */

/**
 * @brief      Definition of DAC Write Access pointer.
 *
 *             Use this function pointer to provide unprivileged, secure write
 *             access to the DAC hardware. When the ASG enters the system into
 *             fault state, the pointer reference changes to the ASG-tripped
 *             function" DAC_SetDualChanSigned_Tripped from which all actuator
 *             control signals are forced to output a predefined, safe value.
 *
 * @param[in]  Raw   DAC compensator algorithm output value for channel 2.
 * @param[in]  Raw   DAC compensator algorithm output value for channel 1.
 */
DAC_WP_t (*DAC_SecureSetDualChanSigned) (int16_t, int16_t);

/** @} */

/**
 * @addtogroup Analog_Functions
 * @{
 */

DAC_WP_t DAC_SetDualChanSigned(int16_t Data2, int16_t Data1) {
  // ===========================================================================
  //        DO NOT ACCESS THIS FUNCTION BY DIRECT FUNCTION CALL !!!
  // ===========================================================================

  /* TODO:  Implement active range checks */
  DAC_SetDualChannelData(DAC_Align_12b_R, Data2 + AN_BIAS_INT, Data1 + AN_BIAS_INT);
  return DEFAULT_WRITE_DAC;
}


/* ===========================================================================
 *  DAC_SetDualChanSigned_Tripped() defines the error condition function.
 *  Point to this function if an ASG error conditon (i.e. Tripped) ocured
 * =========================================================================== */
DAC_WP_t DAC_SetDualChanSigned_Tripped(int16_t dummy1, int16_t dummy2) {
  uint16_t data2, data1;

  data2 = decode_toUint(ASG.safeVal + VA_BIAS);
  data1 = decode_toUint(ASG.safeVal + VA_BIAS);

  DAC_SetDualChannelData( DAC_Align_12b_R, data2, data1);

  return TRIPPED_WRITE_DAC;
}

/** @} */

/** @} */

/** @} */





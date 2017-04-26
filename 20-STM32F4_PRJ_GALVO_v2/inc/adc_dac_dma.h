/**
 * @file        adc_dac_dma.h
 *
 * @date        30 Jan 2017
 * @author      Manuel Del Basso (mainster)
 * @email       manuel.delbasso@gmail.com
 *
 * @ide         System Workbench ac6 (eclipse stm32)
 * @stdperiph   STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 * @license		GNU GPL v3
 *
 * @brief       Provides DMA related configuration and access functions for ADC and DAC.
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

#ifndef _ADC_DAC_DMA_H_
#define _ADC_DAC_DMA_H_

#ifdef __cplusplus
 extern "C" {
#endif 
    
#include "globals.h"
#include "nvic_config.h"
#include "main.h"
// #include "md_stm32f4_dac.h"
#include "pid.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx_dac.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx.h"
#include "helper.h"
#include "actuators.h"

/**
 * @addtogroup MD_APP
 * @{
 */

/**
 * @addtogroup  APP_Analog Analog
 * @{
 *
 * @par Analog to digital converter:
 *     The physical unit of the process or plant outputs states to be an angular
 *     displacement, measured in @si{rad}. The position demodulator electronics
 *     encodes the angle values into a proportional voltage signal. After the
 *     signal conditioning circuits (anti-aliasing, biasing, etc), this signal
 *     is fed to the ADCs. The ideal transfer function from rotor position to
 *     the ADC input voltage, can be expressed as gain @tex{K_{PD}} with the
 *     unit @si{V/rad}.
 */


/**
 * @par Pin out
 * 
 *     ==============================================================
 *                               20-05-2015
 *     ==============================================================
 *     
 *     // Set-point (external)
 *     // Auf prototyp Signal v1.3 wurde dazu einer der
 *     // I_Sens_X/Y Kanäle benutzt
 *
 *      CHAN   ADC1    ADC2    ADC3
 *      ----------------------------
 *         0    PA0    PA0    PA0
 *         1    PA1    PA1    PA1
 *         2    PA2    PA2    PA2
 *         3    PA3    PA3    PA3
 *         4    PA4    PA4    PF6
 *         5    PA5    PA5    PF7
 *         6    PA6    PA6    PF8
 *         7    PA7    PA7    PF9
 *         8    PB0    PB0    PF10
 *         9    PB1    PB1    PF3
 *         10   PC0    PC0    PC0
 *         11   PC1    PC1    PC1
 *         12   PC2    PC2    PC2
 *         13   PC3    PC3    PC3
 *         14   PC4    PC4    PF4
 *         15   PC5    PC5    PF5
 *     
 *         Signal v1.3 proto
 *         ---------------
 *         PC3     Isens_X
 *         PA3     Isens_Y
 *         PA6     pos_X
 *     ----P-A-7---p-o-s_-Y--
 *         PB0     pos_Y   
 *     
 *         Bei ADC dual-mode:
 *         ADC1_CH06    pos_X  (PA6)
 *         ADC2_CH07    pos_Y  (PB0)
 *     
 *         Zum testen, triplemode (prototyp v1.3)
 *         ADC3_CH03    setpoint=Isens_X  (PA3)
 *     
 *         Zum testen, dualmode (prototyp v1.3)
 *         ADC2_CH03    setpoint=Isens_X  (PA3)
 *     
 */

/**
 * @par TODO:
 *     Ob multimode fuer X und Y Position sinnvoll ist, muss getestet werden.
 *     Multimode bedeutet, dass 2 oder sogar 3 Wandlungen synchron ablaufen.
 *     Wenn ein "start Conversion" trigger an ADC1 gesendet wird, starten ADC2
 *     und ggf ADC3 ebenfalls eine Wandlung. Der DMA controller kann so
 *     konfiguriert werden, dass bei einem DMA-request einer der 2(3) ADC-Werte
 *     ins SRAM geschrieben wird Oder dass bei einem DMA-request zwei ADC Werte
 *     (dual half-word) gleichzeitig ins SRAM uebertragen werden. => Sinnvoll
 *     zb. bei einem Zustandsregler, dort werden position und und Wicklungsstrom
 *     vom gleichen Zeitpunkt benoetigt.
 */

/**
 * @addtogroup Analog_Macros
 * @{
 */

/** @} */

/**
 * @addtogroup Analog_Variables
 * @{
 */

/** @} */

/**
 * @addtogroup Analog_Typedefs
 * @{
 */

/**
 * @brief      DAC output write functions return type enumeration
 *
 *             Uused as return type of function pointer @ref
 *             DAC_SecureSetDualChanSigned(). Polling the return argument of
 *             DAC_SecureSetDualChanSigned() could be used to determine, where
 *             the the write pointer points to!. 
 */
typedef enum {
    /**
     * @brief      Default return value under normal conditions when the
     *             function pointer (*DAC_SecureSetDualChanSigned) references to
     *             the unguarded DAC_SetDualChanSigned function which is used to
     *             provide write access to DAC hardware.
     */
    DEFAULT_WRITE_DAC,

    /**
     * @brief      Return value if the ASG has been tripped and
     *             (*DAC_SecureSetDualChanSigned) references to
     *             DAC_SetDualChanSigned_Tripped.
     */
    TRIPPED_WRITE_DAC
} DAC_WP_t;

/** @} */

/**
 * @addtogroup Analog_Functions
 * @{
 */

/**
 * @brief      Dual DAC set channel wrapper function.
 *
 *             This function extends the DAC_SetDualChannelData function which
 *             is a part of CMSIS driver layer. The extension provides active
 *             range checks for @b FIXME: min max values\b provided by a serial
 *             command to protect connected Galvos against algorithm failures
 *             which can raise in overcurrent states.
 *
 * @attention  DO NOT ACCESS THIS FUNCTION VIA DIRECT FUNCTION CALL !!! WRITE
 *             ACCESS TO DAC REGISTERS SHOULD BE HANDLED BY FUNCTION POINTER
 *             DAC_SecureSetDualChanSigned.
 *
 * @param[in]  Data2   Raw DAC compensator algorithm output value for channel 2.
 * @param[in]  Data1   Raw DAC compensator algorithm output value for channel 1.
 *
 * @return     "Fingerprint" to check whether the ASG system has triggered the
 *             fuse or not!
 */
DAC_WP_t DAC_SetDualChanSigned (int16_t Data2, int16_t Data1);

/**
 * @brief      Safe state function.
 *
 *             Error condition function dereferenced via guarded DAC write
 *             accessor (*DAC_SecureSetDualChanSigned).
 *
 * @param[in]  Data2   Raw DAC compensator algorithm output value for channel 2.
 * @param[in]  Data1   Raw DAC compensator algorithm output value for channel 1.
 *
 * @return     "Fingerprint" to check whether the ASG system has triggered the
 *             fuse or not!
 */
DAC_WP_t DAC_SetDualChanSigned_Tripped (int16_t Data2, int16_t Data1);

/**
 * Documented in implementation file.
 * @file adc_dac_dma.c
 */
extern  DAC_WP_t  (*DAC_SecureSetDualChanSigned) (int16_t, int16_t);

/** @} */

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

#endif

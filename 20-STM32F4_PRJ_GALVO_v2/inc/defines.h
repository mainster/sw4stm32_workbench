/**
 * @file        defines.h
 *
 * @date        28 Jan 2017
 * @author    Manuel Del Basso (mainster)
 * @email      manuel.delbasso@gmail.com
 *
 * @ide      System Workbench ac6 (eclipse stm32)
 * @stdperiph   STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 * @license     GNU GPL v3
 *
 * @brief      Global defines include file.
 *
 * @verbatim

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

    @endverbatim
 *
 */
#ifndef DEFINES_H_
#define DEFINES_H_

/**
 * @defgroup   flags Compilerflags
 */

/**
 * @def KEIL_IDE
 * @ingroup    flags
 *
 * Compiler flag to tell if KEIL uVision IDE is used for the build process. For
 * GCC, this flag has to be #undef'ed.
 */
#ifndef     KEIL_IDE
  #define     KEIL_IDE
  #undef      KEIL_IDE
#else
  #define     KEIL_IDE
#endif

/**
 * @addtogroup MD_APP
 * @{
 */

/**
 * @addtogroup APP_Global
 * @{
 */

/**
 * @addtogroup Global_Macros
 * @{
 */

/* ==============================================================
                        Global things
============================================================== */


#define hi  Bit_SET
#define lo  Bit_RESET

//#define DBG_IO        GPIOE
//#define DBG0          GPIO_Pin_0
//#define DBG1          GPIO_Pin_1
//#define DBG2          GPIO_Pin_2
//#define DBG3          GPIO_Pin_3
//#define DBG4          GPIO_Pin_4
//#define DBG5          GPIO_Pin_5
//#define 
//DBG_PID_TIMING_PE6    GPIO_Pin_6

/** 
 * @brief     Index definitions for adc_buffer[] access.
 */
//#define ADC1_CH0      0
//#define ADC1_EXT_W    1
//#define ADC1_CH2      2
//#define ADC1_CH3      3

//#define ADC_Y_VAL &adc_buff[0]
//#define ADC_U_VAL &adc_buff[1]
//#define ADC_BACK_VAL &adc_buff[2]

//#define ADC_Y_CHAN        ADC_Channel_0
//#define ADC_U_CHANNEL     ADC_Channel_6
//#define ADC_BACK_CHANNEL  ADC_Channel_3

/**
 * @brief     Index definitions for ADC_MultiConvBuff[] access. 
 */
#define INDEX_Px    0       //!< Process output of channel X.
#define INDEX_Py    1       //!< Process output of channel Y.
#define INDEX_Ix    2       //!< Actuator coil current of channel X.
#define INDEX_Iy            //!< Actuator coil current of channel Y (FIXME: Hardware).
#define INDEX_Wx    3       //!< Set-point value for channel X.
#define INDEX_Wy            //!< Set-point value for channel Y (FIXME: Hardware).

/** @} */


/**
 * @addtogroup Global_Typedefs
 * @{
 */

/**
 * @brief      TriBool enumerations.
 */
typedef enum {
    FALSE = 0,
    TRUE,
    DNI             //!< Do Not Interpret
} tribool_state_t;

/** @} */

/**
 * @addtogroup Global_Variables
 * @{
 */

/** @} */

/**
 * @addtogroup Global_Functions
 * @{
 */

/** @} */

/** @} */


/* ==============================================================
                        Analog things
============================================================== */

/**
 * @addtogroup APP_Analog
 * @{
 */

/**
 * @addtogroup Analog_Macros
 * @{
 */

/**
 * @brief      ADC code range definition. [digits]
 *
 *             Since both embedded ADC hardware cells are configured to operate
 *             at full 12-Bit resolution, the ADC result register values are
 *             ranged between 0...0x0fff.
 *
 * @note       Remember to consider the analog front-end biasing behavior. The
 *             peak-voltage resolution of a full scaled sinusoidal input signal
 *             reaches only half the ADC-resolution since output values of the
 *             angular position decoder can be positive or negative. In other
 *             words: The angular displacement of the rotor can achieve in
 *             clockwise and counterclockwise direction.
 */
#define ADC_RNG             0x0fff

/**
 * @brief      Analog reference input voltage definition. [V]
 *
 *             Both embedded analog units of the STM32F4xx can be configured to
 *             use different reference signal sources such as an external
 *             voltage signal or the internal bandgap circuits.
 *
 * @note       If both AD- and DA- converter configured for the external
 *             reference source, they has to operate on a common analog
 *             reference signal.
 */
#define AREF_IN_VOLT        (float) 2.5


/**
 * @brief      Bias level definition for the analog front-end. [V]
 * @note       See @ref ADC_RNG,
 */
#define VA_BIAS             (float) 2.5/2

/**
 * @brief      Least significant bit voltage level representation. [V]
 *
 *             Some calculations are easier to perform using the minimum
 *             quantization level for the analog subsystems.
 *
 * @note       Differences between an ADCs resolution, the ENOB (effective
 *             number of bits), THD (total harmonic distortion), SINHD
 *             (SIgnal-to-Noise-And-Distortion), DR (Dynamic Range) and SFDR
 *             (Spurious-Free Dynamic Range) must considered carefully when
 *             designing analog and/or digital filter stages. A good summary (or
 *             point of start) to that topic can be found under 
 *             @link http://www.analog.com/en/analog-dialogue/articles/adc-input-noise.html
 *             
 * @note       This evaluates to @f$2.5V/(2^12-1) = 610.5uV/dig@f$.
 */
#define VLSB                (float) ((float) AREF_IN_VOLT / ADC_RNG)

/**
 * @brief      Analog offset or bias Voltage.
 *
 *             Rounded representation of the analog offset or bias voltage as
 *             integral data type.
 *
 * @note       This evaluates to @f$(4095+1)/2 = 2048@f$ which could physically
 *             be expressed as @f$VLSB*AN_BIAS_INT = 1.2503V$ @a pprox VA_BIAS@f$
 */
#define AN_BIAS_INT             (uint16_t)  (0xfff+1)/2

/**
 * @brief      Upper DAC output limit.
 *
 *             Upper limit of digital-to-analog converter output signal,
 *             represented as integral data type.
 *             
 * @note       This evaluates to @f$(4095-1)/2 = 2046@f$ which could physically
 *             be expressed as @f$VLSB*UPPER_DAC_LIMIT_SIGNED = 1.24908V@f$.
 */
#define UPPER_DAC_LIMIT_SIGNED  (int16_t)   (0xfff-1)/2

/**
 * @brief      Lower DAC output limit.
 *
 *             Lower limit of digital-to-analog converter output signal,
 *             represented as integral data type.
 *             
 * @note       This evaluates to @f$(4095-1)/2 = 2046@f$ which could physically
 *             be expressed as @f$VLSB*LOWER_DAC_LIMIT_SIGNED = 1.2503V@f$.
 */
#define LOWER_DAC_LIMIT_SIGNED  (int16_t)  -(0xfff+1)/2

/**
 * @brief      Remote set-point amplitude division factor.
 *
 *             To mitigate the processing load for remotely (Qt console
 *             interface) performed set-point updates, they were transferred as
 *             binary coded short integers (16-Bit). The AMPL_FLOAT_DIVISOR is
 *             used to scale the binary set-point data. In other words, this
 *             parameter determines the resolution as well as the absolute
 *             reading of the set-point amplitude values.
 */
#define AMPL_FLOAT_DIVISOR      (int16_t)   1e3

/** @} */

/** @} */


/* ==============================================================
                           PID things
============================================================== */

/**
 * @addtogroup APP_Pid
 * @{
 */

/**
 * @addtogroup Pid_Macros
 * @{
 */

/**
 * @brief      Sample time for PID algorithm timing.
 *
 *             Determines the systems overall bandwidth.
 * @note       Unit: µs
 * @note       11/2016: Verify higher control throughput on Texas Instruments
 *             C2000 Series <a href="http://www.ti.com/product/TMS320F28335">
 *             TMS320F28335 "Delfino" DSC. </a>
 */
#define TS          2

#define SAMPLE_INTERVAL   (45*TS)       // 45MHz APBx timer clocks

/** @} */

/** @} */

/** @} */

#endif 

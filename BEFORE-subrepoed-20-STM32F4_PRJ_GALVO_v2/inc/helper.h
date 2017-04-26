/**
 * @file        helper.h
 *
 * @date        
 * @author      Manuel Del Basso (mainster)
 * @email       manuel.delbasso@gmail.com
 *
 * @ide         System Workbench ac6 (eclipse stm32)
 * @stdperiph   STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 * @license     GNU GPL v3
 * 
 * @brief       Provides tools and helper functions.
 * 
   @verbatim

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
#ifndef _HELPER_H_
#define _HELPER_H_

#ifdef __cplusplus
 extern "C" {
#endif 
  
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

/**
 * @addtogroup MD_APP
 * @{
 */

/**
 * @addtogroup APP_Helper Helper
 * @{
 */

/**
 * @addtogroup Helper_Macros
 * @{
 */

/** @} */

/**
 * @addtogroup Helper_Typedefs
 * @{
 */

/** @} */

/**
 * @addtogroup Helper_Variables
 * @{
 */

/** @} */

/**
 * @addtogroup Helper_Functions
 * @{
 */

/**
  * @brief      Split string at delimiter and return pointer to string array.
  *
  * @param      str     Pointer to string that should be splitted.
  * @param      delim   Set of delimiters, eg. " ," means to split str at spaces
  *                     and commas.
  * @retval     int:    Returns the number of splits, -1 if no token was found.
  */
int strsplit(char *str, const char *delim);

int16_t decode_toInt(float value);
uint16_t decode_toUint(float value);
int16_t decodeDbl_toInt(double value);
uint16_t decodeDbl_toUint(double value);

/** @} */

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

#endif

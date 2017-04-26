/**
 * @file        helper.c
 *
 * @date        
 * @author      Manuel Del Basso (mainster)
 * @email       manuel.delbasso@gmail.com
 *
 * @ide         System Workbench ac6 (eclipse stm32)
 * @stdperiph   STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 * @license     GNU GPL v3
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
#include "helper.h"
#include "globals.h"
#include <stdio.h>
#include <string.h>

int strsplit(char *str, const char *delim) {
    char *tok;
    int ret=-1;
    tok = strtok(str, delim);

    while (tok != NULL) {
        if (ret==-1)
          ret = 1;
            
        printf ("Token: %s\n", tok);
        tok = (strtok(NULL, delim));
        ret++;
    }
    
    return ret;
}

int16_t decode_toInt(float value) {    
    return (int16_t)((float)value/VLSB);
}


uint16_t decode_toUint(float value) {
    return (uint16_t)((float)value/VLSB);
}

int16_t decodeDbl_toInt(double value) {
    return (int16_t)((double)value/VLSB);
}


uint16_t decodeDbl_toUint(double value) {
    return (uint16_t)((double)value/VLSB);
}

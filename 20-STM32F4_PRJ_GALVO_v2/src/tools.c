/**
 *	Keil project for XY-GalvoScanner
 *  29-04-2015
 *
 *
 *	@author		Manuel Del Basso
 *	@email		Manuel.DelBasso@googlemail.com  
 *	@ide		Keil uVision 5
 *	@packs		STM32F4xx Keil packs version 2.2.0 or greater required
 *	@stdperiph	STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 *
 *	Additional defines in "Options for Target" > "C/C++" > "Defines"
 *		- ARM_MATH_CM4
 *		- __FPU_PRESENT = 1
 */
 
#include "tools.h"
#include "defines.h"
#include <stdio.h>
#include <string.h>
/**
  * @brief  split string at delimiter and return pointer to string array
  * @param  *str:   pointer to string that should be splitted
  * @param  *delim: set of delimiters, eg. " ," means to split str at spaces and commas  
  * @retval int:    returns the number of splits, -1 if no token was found
  */
int strsplit(char *str, const char *delim) {
    char *tok;
    int ret=-1;
    tok = strtok(str, delim);

    while (tok != NULL) {
        if (ret==-1) {          /**< first token? */
            ret = 1;
        }
            
        printf ("Token: %s\n", tok);
        tok = (strtok(NULL, delim));
        ret++;
    }
    
    return ret;
}

int16_t decode_toInt(float value) {
    
    return (int16_t)((float)value/LSB);
}


uint16_t decode_toUint(float value) {
    
    return (uint16_t)((float)value/LSB);
}

int16_t decodeDbl_toInt(double value) {
    
    return (int16_t)((double)value/LSB);
}


uint16_t decodeDbl_toUint(double value) {
    
    return (uint16_t)((double)value/LSB);
}

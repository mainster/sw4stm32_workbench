#ifndef _TOOLS_H_
#define _TOOLS_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

int strsplit(char *str, const char *delim);
int16_t decode_toInt(float value);
uint16_t decode_toUint(float value);
int16_t decodeDbl_toInt(double value);
uint16_t decodeDbl_toUint(double value);

#endif

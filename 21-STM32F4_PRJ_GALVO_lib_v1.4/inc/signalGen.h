#ifndef __SIGNALGEN_H_ 
#define __SIGNALGEN_H_ 

#include <string.h>

int16_t *pSeq = NULL; 
float    *pSeq_f = NULL;
unsigned short cSeq;

#define TABLESIZE 8


int16_t Cosine[TABLESIZE] = {
0, 889, 1601, 1997, 1997, 1601, 889, 0
};


float CosineN[TABLESIZE] = {
0, 4.338837e-01, 7.818315e-01, 9.749279e-01, 9.749279e-01, 7.818315e-01, 4.338837e-01, 
1.224647e-16
};


int16_t calcTab[TABLESIZE] = {
-2048, -1463, -878, -293, 3803, 3217, 2632, 2048
};


int16_t Quadratic[TABLESIZE] = {
-2048, -2027, -1964, -1860, -1714, -1526, -1296, -1024
};


int16_t I_Quadratic[TABLESIZE] = {
2048, 2027, 1964, 1860, 1714, 1526, 1296, 1024
};


int16_t Sawtooth[TABLESIZE] = {
-2048, -1755, -1463, -1170, -878, -585, -293, 0
};

uint8_t interrupterBuff[] = { 0, 1, 0, 1 };

float squarewaveBuff[] =     { 1, -1, '\0' };

uint8_t *pInt = &interrupterBuff[0];
//uint8_t *pInt



#define NVECTORS 13
uint16_t vectorCtr=0;

float line1[NVECTORS][3] = {
{ 1.000000, -1.250019,  0.000000},
{ 1.000000, -1.015640,  0.000000},
{ 0.000000, -0.844739,  0.000000},
{ 1.000000, -0.737316,  0.000000},
{ 0.000000, -0.368658,  0.000000},
{ 1.000000, -0.070802,  0.000000},
{ 0.000000,  0.058595,  0.000000},
{ 1.000000,  0.605478,  0.000000},
{ 0.000000,  0.717784,  0.000000},
{ 1.000000,  0.852064,  0.000000},
{ 0.000000,  0.969253,  0.000000},
{ 1.000000,  1.084001,  0.000000},
{ 0.000000,  1.188983,  0.000000}
};


#endif

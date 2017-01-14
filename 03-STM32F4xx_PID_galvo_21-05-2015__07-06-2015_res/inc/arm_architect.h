/**
 * @file					arm_architect.h
 *
 * @date				Jan 4, 2017
 *	@author			Manuel Del Basso (mainster)
 *	@email				manuel.delbasso@gmail.com
 *
 *	@ide					System Workbench ac6 (eclipse stm32)
 *	@stdperiph		STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 *
 * @brief				Table 29. Thumb architecture versions in relation to ARM architecture versions
 *
 * ARM architecture	__TARGET_ARCH_ARM	__TARGET_ARCH_THUMB
 * v4	4	0
 * v4T	4	1
 * v5T, v5TE, v5TEJ	5	2
 * v6, v6K, v6Z	6	3
 * v6T2	6	4
 * v6-M, v6S-M	0	3
 * v7-R	7	4
 * v7-M, v7E-M	0	4
 *
 * http://www.keil.com/support/man/docs/armccref/armccref_BABJFEFG.htm#BABFAHJH
 */
#ifndef ARM_ARCHITECT_H_
#define ARM_ARCHITECT_H_

const char *TARGET_ARCH_ARM[] ={
    "v7-M, v7E-M", " ", " ", " ", 
    "v4, v4T",
    "v5T, v5TE, v5TEJ",
    "v6, v6K, v6Z, v6T2",
    "v7-R"
};

#endif

/**
 * @file        arm_architect.h
 *
 * @date        29 Jan 2017
 * @author      Manuel Del Basso (mainster)
 * @email       manuel.delbasso@gmail.com
 *
 * @ide         System Workbench ac6 (eclipse stm32)
 * @stdperiph   STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 * @license		GNU GPL v3
 *
 * @brief       Table 29. Thumb architecture versions in relation to 
 * <a href="http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0491c/BABJFEFG.html">ARM architecture versions</a>
 *
 * @par
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
   @verbatim

	Copyright (C) 2016	Manuel Del Basso

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
#ifndef _ARM_ARCHITECT_H_
#define _ARM_ARCHITECT_H_

const char *TARGET_ARCH_ARM[] = {
    "v7-M, v7E-M", " ", " ", " ", 
    "v4, v4T",
    "v5T, v5TE, v5TEJ",
    "v6, v6K, v6Z, v6T2",
    "v7-R"
};

#endif

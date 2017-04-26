/**
 * @file        attributes.h
 *
 * @date        26 Jan 2017
 * @author      Manuel Del Basso (mainster)
 * @email       manuel.delbasso@gmail.com
 *
 * @ide         System Workbench ac6 (eclipse stm32)
 * @stdperiph   STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 *
 * @brief       Platform/Compiler dependent definitions
 * 
   @verbatim

	------------------------------------------------------------------------

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

	------------------------------------------------------------------------

   @endverbatim
 *
 */
#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

/**
 * Build by GNU Compiler Collection?
 */
#if defined (__GNUC__)
	/**
	 * The keyword __attribute__ allows you to specify special attributes when 
	 * making a declaration.
	 * 
	 * The weak attribute causes the declaration to be emitted as a weak symbol
	 * rather than a global. This is primarily useful in defining library
	 * functions which can be overridden in user code, though it can also be
	 * used with non-function declarations. Weak symbols are supported for ELF
	 * targets, and also for a.out targets when using the GNU assembler and
	 * linker.
	 */
	#ifndef    __weak		
		#define   __weak  __attribute__((weak))
	#endif	


	#ifndef asm
		#define asm __attribute__((__asm__))
	#endif

	/**
	 * The packed attribute specifies that a variable or structure field should
	 * have the smallest possible alignment--one byte for a variable, and one
	 * bit for a field, unless you specify a larger value with the aligned
	 * attribute. Here is a structure in which the field x is packed, so that it
	 * immediately follows a:
	 *
	 * struct foo {
	 * 	 char a;
	 * 	 int x[2] __attribute__ ((packed));
	 * };
	 * 
	 */ 
	#ifndef	__packed	
		#define 	__packed 	__attribute__((__packed__))
	#endif	
#endif

#endif

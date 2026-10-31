/*
 * Value32.hpp
 *
 *  Created on: 26 сент. 2026 г.
 *      Author: dysgn
 */

#ifndef INC_VALUE32_HPP_
#define INC_VALUE32_HPP_

/** Holder of input and output arguments
 */
union Value32
{
	/// Value is signed integer
	int32_t Int;

	/// Value is unsigned integer
	uint32_t Uint;

	/// Value is 32-bit floating point
	float Float;

	/// Value is 4-character string
	char Char[4];
};

#endif /* INC_VALUE32_HPP_ */

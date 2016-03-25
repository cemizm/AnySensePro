/*
 * Endianess.h
 *
 *  Created on: 23.03.2016
 *      Author: cem
 */

#ifndef UTILS_INCLUDE_ENDIANESS_H_
#define UTILS_INCLUDE_ENDIANESS_H_

#include <stdint.h>

namespace Utils
{

class Endianess
{
public:
	static int16_t ToBig(int16_t u)
	{
		return __builtin_bswap16(u);
	}
	static int32_t ToBig(int32_t u)
	{
		return __builtin_bswap32(u);
	}
	static int64_t ToBig(int64_t u)
	{
		return __builtin_bswap64(u);
	}
	static uint8_t ToBCD(uint8_t dec)
	{
		uint8_t result = 0;
		int shift = 0;

		while (dec)
		{
			result += (dec % 10) << shift;
			dec = dec / 10;
			shift += 4;
		}
		return result;
	}

	static uint16_t ToBCD(uint16_t dec)
	{
		uint16_t result = 0;
		int shift = 0;

		while (dec)
		{
			result += (dec % 10) << shift;
			dec = dec / 10;
			shift += 4;
		}
		return result;
	}
};

} /* namespace Utils */

#endif /* UTILS_INCLUDE_ENDIANESS_H_ */

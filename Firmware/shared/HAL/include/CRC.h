/*
 * CRC.h
 *
 *  Created on: 03.03.2016
 *      Author: cem
 */

#ifndef HAL_INCLUDE_CRC_H_
#define HAL_INCLUDE_CRC_H_

namespace HAL
{

#include <libopencm3/stm32/crc.h>
#include <libopencm3/stm32/rcc.h>

#define CRC_DR8H1				MMIO32(CRC_DR + 0x03)

class CRC
{
public:
	static void PowerUp()
	{
		rcc_periph_clock_enable(rcc_periph_clken::RCC_CRC);
	}
	static void Reset()
	{
		crc_reset();
	}
	static void ReverseOutputEnable()
	{
		crc_reverse_output_enable();
	}
	static void ReverseOutputDisable()
	{
		crc_reverse_output_disable();
	}
	static void SetReverseInput(uint32_t reverse_in)
	{
		crc_set_reverse_input(reverse_in);
	}
	static void SetPolynominalSize(uint32_t polysize)
	{
		crc_set_polysize(polysize);
	}
	static void SetPolynominal(uint32_t polynominal)
	{
		crc_set_polynomial(polynominal);
	}
	static void SetIntial(uint32_t initial)
	{
		crc_set_initial(initial);
	}
	static uint16_t Calculate16(uint16_t poly, uint16_t seed, const uint16_t size, const uint8_t* data)
	{
		SetIntial(seed << 16);
		ReverseOutputEnable();
		SetReverseInput(CRC_CR_REV_IN_WORD);

		Reset();

		SetPolynominalSize(CRC_CR_POLYSIZE_32BIT);
		SetPolynominal(poly << 16);

		return Update16(size, data);
	}


	static uint16_t Update16(const uint16_t size, const uint8_t* data)
	{
		const uint8_t *src = data;
		const uint8_t *target = src + size;

		while (((uintptr_t) src & 0x03) != 0)
		{
			CRC_DR8 = *src++; //Write 8 BIT
		}

		while (src <= target - 4)
		{
			CRC_DR = *(uint32_t *) src; //Write 32 BIT
			src += 4;
		}

		while (src < target)
		{
			CRC_DR8 = *src++; //Write 8 Bit
		}

		return CRC_DR16;
	}
};

} /* namespace App */

#endif /* HAL_INCLUDE_CRC_H_ */

/*
 * ChipSelect.h
 *
 *  Created on: 16.10.2015
 *      Author: cem
 */

#ifndef UTILS_INCLUDE_CHIPSELECT_H_
#define UTILS_INCLUDE_CHIPSELECT_H_

#include <Pin.h>

namespace Utils
{

class ChipSelect
{
private:
	HAL::Pin& m_pin;
public:
	ChipSelect(HAL::Pin& pin) :
			m_pin(pin)
	{
		m_pin.Off();
	}
	~ChipSelect()
	{
		m_pin.On();
	}
	inline void Reselect()
	{
		m_pin.On();
		m_pin.Off();
	}
};

} /* namespace Utils */

#endif /* UTILS_INCLUDE_CHIPSELECT_H_ */

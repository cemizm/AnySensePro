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
		Deselect();
	}
	inline void Reselect()
	{
		m_pin.On();
		m_pin.Off();
	}
	inline void Deselect()
	{
		m_pin.On();
	}
};

} /* namespace Utils */

#endif /* UTILS_INCLUDE_CHIPSELECT_H_ */

/*
 * SensorFrSky.h
 *
 *  Created on: 11.03.2016
 *      Author: cem
 */

#ifndef APPLICATION_INCLUDE_SENSORFRSKY_H_
#define APPLICATION_INCLUDE_SENSORFRSKY_H_

#include "USART.h"
#include "SensorAdapter.h"
#include "Interrupt.h"

namespace App
{

/*
 *
 */
class SensorFrSky: public SensorAdapter<SensorAdapterBase::Procotol::FrSky>, public HAL::InterruptHandler
{
private:
	static const uint8_t PollTime = delay_ms(10);

	HAL::USART& m_usart;
public:
	SensorFrSky(HAL::USART& usart) :
			SensorAdapter(), m_usart(usart)
	{
	}

	void Init() override;
	void Run() override;

	void ISR() override;
};

} /* namespace App */

#endif /* APPLICATION_INCLUDE_SENSORFRSKY_H_ */

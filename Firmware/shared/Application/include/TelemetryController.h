/*
 * TelemetryController.h
 *
 *  Created on: 15.10.2015
 *      Author: cem
 */

#ifndef APPLICATION_INCLUDE_TELEMETRYCONTROLLER_H_
#define APPLICATION_INCLUDE_TELEMETRYCONTROLLER_H_

#include "TelemetryAdapter.h"

#include "Configuration.h"

#include "USART.h"
#include "Timer.h"
#include "I2C.h"

namespace App
{

class TelemetryController: public ConfigurationChanged
{
private:
	uint8_t m_workspace[TELEMETRY_WORKSPACE];
	TelemetryAdapterBase* m_active;

	HAL::USART& m_usart;
	HAL::Timer& m_timer;
	HAL::I2C& m_i2c;

public:
	TelemetryController(HAL::USART& usart, HAL::Timer& timer, HAL::I2C& i2c) :
			m_workspace(), m_active(NULL), m_usart(usart), m_timer(timer), m_i2c(i2c)
	{

	}
	void Init();
	void Run();

	void UpdateConfiguration() override;
};

} /* namespace Application */

#endif /* APPLICATION_INCLUDE_TELEMETRYCONTROLLER_H_ */

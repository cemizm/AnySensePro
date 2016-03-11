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

namespace App
{


class TelemetryController: public ConfigurationChanged
{
private:
	uint8_t m_workspace[TELEMETRY_WORKSPACE];
	TelemetryAdapterBase* m_active;

	HAL::USART& m_usart;

public:
	TelemetryController(HAL::USART& usart) :
			m_workspace(), m_active(NULL), m_usart(usart)
	{

	}
	void Init();
	void Run();

	void UpdateConfiguration() override;
};

} /* namespace Application */

#endif /* APPLICATION_INCLUDE_TELEMETRYCONTROLLER_H_ */

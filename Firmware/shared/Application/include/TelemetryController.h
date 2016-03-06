/*
 * TelemetryController.h
 *
 *  Created on: 15.10.2015
 *      Author: cem
 */

#ifndef APPLICATION_INCLUDE_TELEMETRYCONTROLLER_H_
#define APPLICATION_INCLUDE_TELEMETRYCONTROLLER_H_

#include "TelemetryAdapter.h"
#include "TelemetryMAVLink.h"

#include "OSAL.h"

#include "USART.h"

#include "Configuration.h"

namespace App
{


class TelemetryController: public ConfigurationChanged
{
private:
	TelemetryMAVLink m_MAVLinkAdapter;
	TelemetryAdapter* m_available[TelemetryProtocol::Last];
	TelemetryAdapter* m_active;
	TelemetryPort port;
	uint8_t m_workspace[TELEMETRY_WORKSPACE];

	OSAL::EventFlag eventFlag;


public:
	TelemetryController(HAL::USART& usart) :
			m_active(NULL), port(usart), m_workspace(), eventFlag()
	{

	}
	void Init();
	void Run();

	void UpdateConfiguration() override;
};

} /* namespace Application */

#endif /* APPLICATION_INCLUDE_TELEMETRYCONTROLLER_H_ */

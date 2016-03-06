/*
 * TelemetryController.cpp
 *
 *  Created on: 15.10.2015
 *      Author: cem
 */

#include <TelemetryController.h>

namespace App
{

void TelemetryController::Init()
{
	m_available[TelemetryProtocol::MAVLink] = &m_MAVLinkAdapter;

	Config.AddUpdateHandler(*this);
}

void TelemetryController::Run()
{
	uint8_t protocol;
	for (;;)
	{
		protocol = (uint8_t) Config.GetProtocol();
		m_active = protocol < TelemetryProtocol::Last ? m_available[protocol] : nullptr;

		if (m_active != nullptr)
		{
			m_active->Init(m_workspace, port);
			m_active->Run();
		}
		else
			eventFlag.wait();
	}
}

void TelemetryController::UpdateConfiguration()
{
	uint8_t protocol = (uint8_t) Config.GetProtocol();

	if (protocol >= TelemetryProtocol::Last)
		return;

	if (m_active == nullptr)
	{
		eventFlag.signal();
		return;
	}

	if (m_active == m_available[protocol])
		m_active->UpdateConfiguration();
	else
		m_active->DeInit();
}

} /* namespace Application */

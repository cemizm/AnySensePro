/*
 * TelemetryController.cpp
 *
 *  Created on: 15.10.2015
 *      Author: cem
 */

#include <TelemetryController.h>

#include "TelemetryMAVLink.h"
#include "TelemetryFrSky.h"
#include "TelemetryHoTT.h"
#include "TelemetryMultiplex.h"
#include "TelemetryJeti.h"
#include "TelemetryFutaba.h"

#include <new>

namespace App
{

void TelemetryController::Init()
{
	Config.AddUpdateHandler(*this);
}

void TelemetryController::Run()
{
	TelemetryProtocol protocol;
	for (;;)
	{
		m_active = nullptr;
		memset(m_workspace, 0, TELEMETRY_WORKSPACE);

		protocol = Config.GetProtocol();

		switch (protocol)
		{
		case TelemetryProtocol::MAVLink:
			m_active = new (m_workspace) TelemetryMAVLink(m_usart);
			break;
		case TelemetryProtocol::FrSky:
			m_active = new (m_workspace) TelemetryFrSky(m_usart);
			break;
		case TelemetryProtocol::HoTT:
			m_active = new (m_workspace) TelemetryHoTT(m_usart);
			break;
		case TelemetryProtocol::Multiplex:
			m_active = new (m_workspace) TelemetryMultiplex(m_usart);
			break;
		case TelemetryProtocol::Jeti:
			m_active = new (m_workspace) TelemetryJeti(m_usart, m_timer);
			break;
		case TelemetryProtocol::Futaba:
			m_active = new (m_workspace) TelemetryFutaba(m_usart, m_timer);
			break;
		default:
			m_active = new (m_workspace) TelemetryAdapterBase();
			break;
		}

		m_active->Init();
		m_active->Run();
	}
}

void TelemetryController::UpdateConfiguration()
{
	TelemetryProtocol protocol = Config.GetProtocol();

	if (protocol >= TelemetryProtocol::Last)
		return;

	if (m_active->Handles() == protocol)
		m_active->UpdateConfiguration();
	else
		m_active->DeInit();
}

} /* namespace Application */

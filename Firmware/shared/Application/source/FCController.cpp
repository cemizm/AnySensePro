/*
 * FCController.cpp
 *
 *  Created on: 14.03.2016
 *      Author: cem
 */

#include <FCController.h>
#include <FCMAVLink.h>
#include <FCTarot.h>
#include <FCAlign.h>

#include <SensorStore.h>

#include <string.h>
#include <new>

namespace App
{

void FCController::Init()
{

}

void FCController::Run()
{

	for (;;)
	{
		m_active = nullptr;
		memset(m_workspace, 0, FCAdapter::Workspace);

		switch (m_protocol)
		{
		case FCAdapter::Protocol::MAVLink:
			m_active = new (m_workspace) FCMAVLink(m_usart);
			break;
		case FCAdapter::Protocol::Tarot:
			m_active = new (m_workspace) FCTarot(m_usart);
			break;
		case FCAdapter::Protocol::Align:
			m_active = new (m_workspace) FCAlign(m_usart);
			break;
		default:
			m_active = new (m_workspace) FCAdapter();
			break;
		}

		m_active->Init();
		m_active->Run();

		if (SensorData.GetFCType() == FCType::Naza || SensorData.GetFCType() == FCType::Phantom
				|| SensorData.GetFCType() == FCType::Wookong || SensorData.GetFCType() == FCType::A2)
			return;

		m_protocol = (FCAdapter::Protocol) ((m_protocol + 1) % FCAdapter::Protocol::Last);
	}
}

} /* namespace App */

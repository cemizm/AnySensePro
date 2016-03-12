/*
 * SensorController.cpp
 *
 *  Created on: 11.03.2016
 *      Author: cem
 */

#include <SensorController.h>
#include <string.h>

#include "SensorFrSky.h"

#include <new>

namespace App
{

void SensorController::Init()
{

}

void SensorController::Run()
{
	SensorAdapterBase::Procotol protocol = SensorAdapterBase::Procotol::FrSky;
	for (;;)
	{
		m_active = nullptr;
		memset(m_workspace, 0, SensorAdapterBase::Workspace);

		switch (protocol)
		{
		case SensorAdapterBase::Procotol::FrSky:
			m_active = new (m_workspace) SensorFrSky(m_usart);
			break;
		default:
			return;
			break;
		}

		m_active->Init();
		m_active->Run();
	}
}

} /* namespace App */

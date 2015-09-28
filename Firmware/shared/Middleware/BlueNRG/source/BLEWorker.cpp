/*
 * BLEWorker.cpp
 *
 *  Created on: 14.09.2015
 *      Author: cem
 */

#include "BLEWorker.h"
namespace Application
{

void BLEWorker::Run()
{
	m_device.Init();

	if (m_device.GATT.Init() != BlueNRG::HCIGenericStatusCode::Success)
		return;

	if (m_device.GAP.Init(BlueNRG::GAPRole::Peripheral, &m_HGAPService, &m_HDeviceNameChar, &m_HAppearanceChar)
			!= BlueNRG::HCIGenericStatusCode::Success)
		return;

	uint8_t packetType = 0;

	for (;;)
	{
		switch (packetType)
		{
		case 0:
			//m_device.Send();
			break;
		}

		packetType = (packetType + 1) % 10;

		HAL::OSAL::SleepMS(200);
	}
}

} /* namespace Application */

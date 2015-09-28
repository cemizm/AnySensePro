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

	m_device.GATT.Init();

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

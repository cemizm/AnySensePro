/*
 * LoggingController.cpp
 *
 *  Created on: 17.03.2016
 *      Author: cem
 */

#include <LoggingController.h>

#include "Configuration.h"
#include "SensorStore.h"

#include "stdlib.h"

#include "OSAL.h"

namespace App
{

static const char* label = "SD:ANYSENSEPRO";
static const char* prefix = "SD:flight_";
static const char* ext = ".bin";

void LoggingController::Init()
{
	updateFilename();
}

void LoggingController::Run()
{
	FRESULT fr = FR_TIMEOUT;
	for (;;)
	{
		if (fr != FR_OK || m_currentSession != SensorData.GetSession())
		{
			if(m_currentSession != SensorData.GetSession())
			{
				m_currentSession = SensorData.GetSession();
				updateFilename();
			}

			if (fr == FR_OK) //... flight finished
			{
				f_close(&m_file);
				m_currentData = 0;
			}

			fr = f_open(&m_file, m_fileName, FA_READ | FA_WRITE | FA_CREATE_ALWAYS);
			if (fr == FR_OK)
			{
				f_sync(&m_file);
				f_setlabel(label);
			}
		}

		if (fr == FR_OK)
		{
			switch (m_currentData)
			{
			case 0:
				m_dataLen = m_mavlink.PackGPS(&m_msg);
				break;
			case 1:
				m_dataLen = m_mavlink.PackAttitude(&m_msg);
				break;
			case 2:
				m_dataLen = m_mavlink.PackBatteryPack(&m_msg);
				break;
			case 3:
				m_dataLen = m_mavlink.PackHeartbeat(&m_msg);
				break;
			case 4:
				m_dataLen = m_mavlink.PackRCOut(&m_msg);
				break;
			case 5:
				m_dataLen = m_mavlink.PackSystemStatus(&m_msg);
				break;
			case 6:
				m_dataLen = m_mavlink.PackVFRHud(&m_msg);
				break;
			case 7:
				m_dataLen = m_mavlink.PackGPSInt(&m_msg);
				break;
			case 8:
				m_dataLen = m_mavlink.PackSystemTime(&m_msg);
				break;
			}

			m_currentData = (m_currentData + 1) % 9;

			m_dataLen = m_mavlink.FillBytes(&m_msg, m_buffer);

			fr = f_write(&m_file, m_buffer, m_dataLen, (UINT*) &m_written);

			if (fr == FR_OK)
				fr = f_sync(&m_file);
		}

		OSAL::Timer::SleepMS(fr == FR_OK ? 10 : 100);
	}
}

void LoggingController::updateFilename()
{
	memset(m_fileName, 0, sizeof(m_fileName));

	uint16_t fn = Config.GetNextFlightNumber();
	char tmp[10];
	itoa(fn, tmp, 10);

	strcat(m_fileName, prefix);
	strcat(m_fileName, tmp);
	strcat(m_fileName, ext);
}

} /* namespace App */

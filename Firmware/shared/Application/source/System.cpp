/*
 * System.cpp
 *
 *  Created on: 03.03.2016
 *      Author: cem
 */

#include <System.h>
#include <string.h>

#include <SensorStore.h>

namespace App
{

void System::log(uint8_t level, const char* msg)
{
	if (!logging.get_free_size())
		return;

	working.msgLen = (uint8_t) strlen(msg);
	if (working.msgLen > LogEntryMsgLen)
		working.msgLen = LogEntryMsgLen;

	working.level = level;

	memcpy(working.msg, msg, working.msgLen);
	logging.push(working);
}

void System::logError(const char* msg)
{
	log(LevelError, msg);
}
void System::logWarning(const char* msg)
{
	log(LevelWarning, msg);
}

void System::Run()
{
	ledActivity.Off();
	ledError.Off();

	do
	{
		ledActivity.Toggle();
	} while (loaded.wait(delay_ms(200)));

	ledActivity.Off();
	uint8_t blink = 0;
	for (;;)
	{
		if (logging.pop(working, delay_ms(20)))
		{
			//log

			if (working.level == LevelError)
				ledError.On();
		}

		if(SensorData.GetFCType() == FCType::Unknown)
		{
			ledActivity.On();
		}
		else if (blink % 15 == 0)
		{
			if (blink < 15)
			{
				if (SensorData.GetSatellites() == 0)
					ledActivity.On();
			}
			else if (blink < 30)
			{
				if (SensorData.GetSatellites() < 4)
					ledActivity.On();
			}
			else if (blink < 45)
			{
				if (SensorData.GetSatellites() < 7)
					ledActivity.On();
			}
		}
		else if (blink % 5 == 0)
			ledActivity.Off();

		blink = (blink + 1) % (15 * 5);

		//accumlate data
	}
}

} /* namespace App */

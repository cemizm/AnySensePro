/*
 * TelemetryMAVLink.cpp
 *
 *  Created on: 18.10.2015
 *      Author: cem
 */

#include <TelemetryMAVLink.h>

namespace App
{

static_assert (sizeof(TelemetryMAVLink) <= TELEMETRY_WORKSPACE, "TelemetryMAVLink Telemetry will not fit!");

void TelemetryMAVLink::Init()
{
	mav.Init();
	eventFlag.clear();
}

void TelemetryMAVLink::Run(void)
{
	uint8_t run = 1;

	while (run)
	{
		if (!eventFlag.wait(delay_ms(MAVLINK_COMM_DELAY_MS)))
		{
			mav.loop();
		}
		else
			run = 0;
	}

	mav.DeInit();
}

} /* namespace Utils */

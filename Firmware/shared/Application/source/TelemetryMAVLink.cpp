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
	do
	{
		mav.loop();
	} while (m_run);

	mav.DeInit();
}

void TelemetryMAVLink::DeInit()
{
	m_run = 0;
	eventFlag.signal();
}

} /* namespace Utils */

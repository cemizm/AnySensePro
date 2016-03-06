/*
 * TelemetryMAVLink.cpp
 *
 *  Created on: 18.10.2015
 *      Author: cem
 */

#include <TelemetryMAVLink.h>

namespace App
{

void TelemetryMAVLink::Init(uint8_t* workspace, TelemetryPort& port)
{
	WorkData.workspace = workspace;
	this->port = &port;
}

void TelemetryMAVLink::Run(void)
{
}

void TelemetryMAVLink::DeInit(void)
{
}

void TelemetryMAVLink::UpdateConfiguration(void)
{
}

} /* namespace Utils */

/*
 * TelemetryController.cpp
 *
 *  Created on: 15.10.2015
 *      Author: cem
 */

#include <TelemetryController.h>

#include <stddef.h>

namespace App
{

void TelemetryController::Init(){

}

void TelemetryController::Run()
{
	for (;;)
	{
		OSAL::Timer::SleepSeconds(5);
	}
}


} /* namespace Application */

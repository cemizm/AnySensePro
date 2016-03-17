/*
 * LoggingController.cpp
 *
 *  Created on: 17.03.2016
 *      Author: cem
 */

#include <LoggingController.h>

#include "OSAL.h"

namespace App
{

void LoggingController::Init()
{
}

void LoggingController::Run()
{
	//supress compiler warnings
	for (;;)
	{
		OSAL::Timer::SleepMS(10);
	}
}

} /* namespace App */

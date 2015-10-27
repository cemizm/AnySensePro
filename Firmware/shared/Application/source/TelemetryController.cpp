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

TelemetryController::TelemetryController(Configuration& configuration) :
		m_configuration(configuration), m_active(NULL), m_workspace()
{

}

void TelemetryController::Run()
{
	for (;;)
	{

	}
}

} /* namespace Application */

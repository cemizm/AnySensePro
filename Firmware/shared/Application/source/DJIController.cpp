/*
 * DJIController.cpp
 *
 *  Created on: 17.09.2015
 *      Author: cem
 */

#include "DJIController.h"

namespace Application
{

void DJIController::Init()
{

}

void DJIController::Run()
{
	for(;;)
	{
		HAL::OSAL::SleepMS(100);
	}
}

} /* namespace LIBS */

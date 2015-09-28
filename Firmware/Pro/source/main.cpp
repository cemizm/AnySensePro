/*
 * main.cpp
 *
 *  Created on: 09.09.2015
 *      Author: cem
 */

#include <USBDevice.h>
#include <USBInterface.h>
#include <USBCDCInterface.h>
#include <USBMSCInterface.h>

#include <DJIController.h>

#include <scmRTOS.h>
#include <OSAL.h>

#include "Board.h"

USB::USBCDCInterface iFace;
USB::USBMSCInterface iFace2;
USB::USBInterface* iFaces[] = { &iFace, &iFace2 };

USB::USBDevice usb_device(Board::USB, iFaces, 1);

Libs::SensorStore sensorStore;
Application::DJIController djiController(Board::FC::CAN, sensorStore);


// Process types
typedef OS::process<OS::pr0, 512> TProc0;
typedef OS::process<OS::pr1, 512> TProc1;
typedef OS::process<OS::pr2, 300> TProc2;
typedef OS::process<OS::pr3, 300> TProc3;

// Process objects
TProc0 Proc0;
TProc1 Proc1;
TProc2 Proc2;
TProc3 Proc3;

int main()
{
	Board::SystemInit();

	usb_device.Init();

	OS::run();
}

namespace OS
{
template<>
OS_PROCESS void TProc0::exec()
{
	djiController.Run();
}

template<>
OS_PROCESS void TProc1::exec()
{
	//blewoker.Run();
	sleep();
}

template<>
OS_PROCESS void TProc2::exec()
{
	Board::LedActivity.PowerUp();
	Board::LedActivity.ModeSetup(GPIO_MODE_OUTPUT, GPIO_PUPD_NONE);

	Board::LedError.PowerUp();
	Board::LedError.ModeSetup(GPIO_MODE_OUTPUT, GPIO_PUPD_NONE);

	Board::LedError.On();
	for (;;)
	{
		Board::LedError.Toggle();
		Board::LedActivity.Toggle();
		HAL::OSAL::SleepMS(20);

	}
}

template<>
OS_PROCESS void TProc3::exec()
{
	sleep();
}
}

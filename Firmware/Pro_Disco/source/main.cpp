/*
 * main.cpp
 *
 *  Created on: 09.09.2015
 *      Author: cem
 */

#include <DJIController.h>
#include <scmRTOS.h>
#include <OSAL.h>
#include "USBDevice.h"
#include "USBInterface.h"
#include "USBCDCInterface.h"
#include "USBMSCInterface.h"
#include "Pin.h"

#include "Board.h"

App::SensorStore sensorStore;

App::DJIController djiController(Board::FC::CAN);

USB::USBCDCInterface cdcInterface;
USB::USBMSCInterface mscInterface;

USB::USBInterface* iFaces[] = { &cdcInterface, &mscInterface };

USB::USBDevice device(Board::USB, iFaces, 1);

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

	device.Init();

	OS::run();
}

namespace OS
{
template<>
OS_PROCESS void TProc0::exec()
{
	djiController.Init();
	djiController.Run();
}

template<>
OS_PROCESS void TProc1::exec()
{
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
		OSAL::Timer::SleepMS(20);

	}
}

template<>
OS_PROCESS void TProc3::exec()
{
	sleep();
}
}

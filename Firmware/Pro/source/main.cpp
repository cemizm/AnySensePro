/*
 * main.cpp
 *
 *  Created on: 09.09.2015
 *      Author: cem
 */

#include "Board.h"

#include <StorageFlashSPI.h>

#include <TelemetryController.h>
#include <System.h>
#include <DJIController.h>
#include <MAVLinkComm.h>
#include <USBWorker.h>

#include <scmRTOS.h>
#include <OSAL.h>

#include <Configuration.h>

using namespace App;

TelemetryController telemetryController;
DJIController djiController(Board::FC::CAN);
MAVLinkComm mavLinkComm(Board::OSD::USART);
System SystemService(Board::LedError, Board::LedActivity);
USBWorker usb_worker(Board::CDCDevice);

// Process types
typedef OS::process<OS::pr0, 1024> TProc0; //Telemetry Controller
typedef OS::process<OS::pr1, 1024> TProc1; //DJI Controller
typedef OS::process<OS::pr2, 1024> TProc2; //MAVLink Out Controller
typedef OS::process<OS::pr3, 1024> TProc3; //FrSky Controller
typedef OS::process<OS::pr4, 1024> TProc4; //USB Controller
typedef OS::process<OS::pr5, 1024> TProc5; //System Service
typedef OS::process<OS::pr6, 1024> TProc6; //Logger

// Process objects
TProc0 Proc0;
TProc1 Proc1;
TProc2 Proc2;
TProc3 Proc3;
TProc4 Proc4;
TProc5 Proc5;
TProc6 Proc6;

int main()
{
	Board::Init();

	OS::run();
}

namespace OS
{
template<>
OS_PROCESS void TProc0::exec()
{
	Storage::StorageFlashSPI::Mount();
	Storage::StorageFlashSPI::GC(512 * 1024);

	Config.Init();

	SystemService.signalLoaded();

	telemetryController.Init();
	telemetryController.Run();

	//supress compiler warnings
	for (;;)
		;
}

template<>
OS_PROCESS void TProc1::exec()
{
	SystemService.isLoaded();

	djiController.Init();
	djiController.Run();

	//supress compiler warnings
	for (;;)
		;
}

template<>
OS_PROCESS void TProc2::exec()
{
	SystemService.isLoaded();

	mavLinkComm.Init();
	mavLinkComm.Run();

	//supress compiler warnings
	for (;;)
		;
}

template<>
OS_PROCESS void TProc3::exec()
{
	SystemService.isLoaded();

	//FrSky Sensor worker...

	//supress compiler warnings
	for (;;)
		OSAL::Timer::SleepSeconds(5);
}

template<>
OS_PROCESS void TProc4::exec()
{
	SystemService.isLoaded();

	usb_worker.Init();
	usb_worker.Run();

	//supress compiler warnings
	for (;;)
		;
}

template<>
OS_PROCESS void TProc5::exec()
{
	SystemService.Run();

	//supress compiler warnings
	for (;;)
		;
}
template<>
OS_PROCESS void TProc6::exec()
{
	SystemService.isLoaded();

	//Logger worker...

	//supress compiler warnings
	for (;;)
		OSAL::Timer::SleepSeconds(5);
}

}

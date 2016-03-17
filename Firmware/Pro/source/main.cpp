/*
 * main.cpp
 *
 *  Created on: 09.09.2015
 *      Author: cem
 */

#include <scmRTOS.h>
#include <OSAL.h>

#include "Board.h"

#include <Configuration.h>

#include <StorageFlashSPI.h>

#include <TelemetryController.h>
#include <DJIController.h>
#include <FCController.h>
#include <MAVLinkComm.h>
#include <SensorController.h>
#include <USBWorker.h>
#include <System.h>
#include <LoggingController.h>

using namespace App;

TelemetryController telemetryController(Board::Telemetry::USART, Board::Telemetry::TIMER);
DJIController djiController(Board::FC::CAN);
FCController fcController(Board::FC::USART);
MAVLinkComm mavLinkComm(Board::OSD::USART, MAVLINK_COMM_1);
SensorController sensorController(Board::Sensor::USART);
USBWorker usb_worker(Board::CDCDevice);
System SystemService(Board::LedError, Board::LedActivity);
LoggingController loggingController;

// Process types
typedef OS::process<OS::pr0, 1024> TProc0; //Telemetry Controller
typedef OS::process<OS::pr1, 1024> TProc1; //DJI Controller
typedef OS::process<OS::pr2, 1024> TProc2; //PixHawk, Tarot etc.. Controller
typedef OS::process<OS::pr3, 1024> TProc3; //MAVLink Out Controller
typedef OS::process<OS::pr4, 1024> TProc4; //FrSky Controller
typedef OS::process<OS::pr5, 1024> TProc5; //USB Controller
typedef OS::process<OS::pr6, 1024> TProc6; //System Service
typedef OS::process<OS::pr7, 1024> TProc7; //Logger

// Process objects
TProc0 Proc0;
TProc1 Proc1;
TProc2 Proc2;
TProc3 Proc3;
TProc4 Proc4;
TProc5 Proc5;
TProc6 Proc6;
TProc7 Proc7;

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

	fcController.Init();
	fcController.Run();

	//supress compiler warnings
	for (;;)
		OSAL::Timer::SleepSeconds(5);
}

template<>
OS_PROCESS void TProc3::exec()
{
	SystemService.isLoaded();

	mavLinkComm.Init();
	mavLinkComm.Run();

	//supress compiler warnings
	for (;;)
		;
}

template<>
OS_PROCESS void TProc4::exec()
{
	SystemService.isLoaded();

	sensorController.Init();
	sensorController.Run();

	//supress compiler warnings
	for (;;)
		OSAL::Timer::SleepSeconds(5);
}

template<>
OS_PROCESS void TProc5::exec()
{
	SystemService.isLoaded();

	usb_worker.Init();
	usb_worker.Run();

	//supress compiler warnings
	for (;;)
		;
}

template<>
OS_PROCESS void TProc6::exec()
{
	SystemService.Run();

	//supress compiler warnings
	for (;;)
		;
}
template<>
OS_PROCESS void TProc7::exec()
{
	SystemService.isLoaded();

	loggingController.Init();
	loggingController.Run();

	//supress compiler warnings
	for (;;)
		;
}

}

/*
 * main.cpp
 *
 *  Created on: 09.09.2015
 *      Author: cem
 */

#include "Board.h"

#include <StorageFlashSPI.h>
#include <System.h>
#include <DJIController.h>
#include <MAVLinkComm.h>
#include <USBWorker.h>

#include <scmRTOS.h>
#include <OSAL.h>

#include <Configuration.h>

using namespace App;

System SystemService(Board::LedError, Board::LedActivity);
DJIController djiController(Board::FC::CAN);
MAVLinkComm mavLinkComm(Board::OSD::USART);
USBWorker usb_worker(Board::CDCDevice);

// Process types
typedef OS::process<OS::pr0, 1024> TProc0;
typedef OS::process<OS::pr1, 1024> TProc1;
typedef OS::process<OS::pr2, 2048> TProc2;
typedef OS::process<OS::pr3, 1024> TProc3;
typedef OS::process<OS::pr4, 300> TProc4;

// Process objects
TProc0 Proc0;
TProc1 Proc1;
TProc2 Proc2;
TProc3 Proc3;
TProc4 Proc4;

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


	SystemService.signalLoaded();

	djiController.Init();
	djiController.Run();

	//supress compiler warnings
	for (;;)
		;
}

template<>
OS_PROCESS void TProc1::exec()
{
	SystemService.isLoaded();

	mavLinkComm.Init();
	mavLinkComm.Run();

	//supress compiler warnings
	for (;;)
		;
}

template<>
OS_PROCESS void TProc2::exec()
{
	SystemService.isLoaded();

	for (;;)
		OSAL::Timer::SleepSeconds(5);
}

template<>
OS_PROCESS void TProc3::exec()
{
	SystemService.isLoaded();

	usb_worker.Init();
	usb_worker.Run();

	//supress compiler warnings
	for (;;)
		;
}

template<>
OS_PROCESS void TProc4::exec()
{
	SystemService.Run();

	//supress compiler warnings
	for (;;)
		;
}

}

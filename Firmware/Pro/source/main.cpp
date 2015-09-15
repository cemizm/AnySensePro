/*
 * main.cpp
 *
 *  Created on: 09.09.2015
 *      Author: cem
 */

#include <BLEDevice.h>
#include <BLEWorker.h>
#include <scmRTOS.h>
#include <OSAL.h>

#include "Board.h"

BlueNRG::BLEDevice bledevice(Board::BLE::Config, Board::BLE::SPI, Board::BLE::CSN, Board::BLE::IRQ, Board::BLE::RSTN);
Application::BLEWorker blewoker(bledevice);

// Process types
typedef OS::process<OS::pr0, 300> TProc0;
typedef OS::process<OS::pr1, 300> TProc1;
typedef OS::process<OS::pr2, 300> TProc2;
typedef OS::process<OS::pr3, 300> TProc3;

// Process objects
TProc0 Proc0;
TProc1 Proc1;
TProc2 Proc2;
TProc3 Proc3;

// Event Flags to test
OS::TEventFlag ef;
OS::TEventFlag timerEvent;

int main()
{
	Board::SystemInit();

	Board::Led.PowerUp();
	Board::Led.ModeSetup(GPIO_MODE_OUTPUT, GPIO_PUPD_NONE);

	OS::run();
}

namespace OS
{
template<>
OS_PROCESS void TProc0::exec()
{
	bledevice.Run();
}

template<>
OS_PROCESS void TProc1::exec()
{
	blewoker.Run();
}

template<>
OS_PROCESS void TProc2::exec()
{
	for (;;)
	{
		Board::Led.On();
		HAL::OSAL::SleepMS(500);
		ef.signal();
		HAL::OSAL::SleepMS(500);
	}
}

template<>
OS_PROCESS void TProc3::exec()
{
	for (;;)
	{
		ef.wait();
		Board::Led.Off();
	}
}
}

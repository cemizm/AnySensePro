/*
 * main.cpp
 *
 *  Created on: 09.09.2015
 *      Author: cem
 */

#include <DJIController.h>
#include <MAVLinkComm.h>
#include <USBWorker.h>

#include <scmRTOS.h>
#include <OSAL.h>

#include <Configuration.h>
#include <SensorStore.h>

#include "Board.h"

using namespace App;

USBWorker usb_worker(CDCDevice);
DJIController djiController(Board::FC::CAN);
MAVLinkComm mavLinkComm(Board::OSD::USART);

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
	/*
	 uint8_t test[] = { 0x00, 0x55, 0x80, 0x00, 0xF4, 0x02, 0x10, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99,
	 0x99, 0x99, 0x99, 0x99, 0x99, 0x35, 0xD1, 0xD7, 0xDA, 0x97, 0x62, 0x83, 0x22, 0xCE, 0x75, 0xDF, 0xA5, 0xFA, 0x9A,
	 0x18, 0x26, 0x45, 0xF2, 0x00, 0xA2, 0x17, 0xF2, 0xD2, 0x22, 0xE3, 0xB4, 0x75, 0xA2, 0x04, 0x13, 0x55, 0xA4, 0x17,
	 0x55, 0xE7, 0xA6, 0x0D, 0xFC, 0x5A, 0x22, 0x9B, 0x63, 0xAD, 0x22, 0x2C, 0x36, 0x5C, 0xA4, 0x23, 0xAE, 0x3D, 0xA5,
	 0x99, 0x48, 0x3E, 0xA0, 0xC1, 0x29, 0xC0, 0xA4, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x45, 0x4F, 0x66,
	 0xA5, 0x50, 0x30, 0x11, 0xA3, 0x2B, 0xD6, 0x2E, 0x23, 0xF5, 0x9F, 0xFB, 0xA2, 0xA7, 0x9B, 0xE4, 0x66, 0xBA, 0x9D,
	 0x82, 0x99, 0x83, 0x99, 0x82, 0x99, 0x95, 0x99, 0x19, 0x99, 0x99, 0x99, 0xF5, 0x11, 0xA0, 0x91 };

	 HAL::CRC::PowerUp();

	 volatile uint32_t crc = HAL::CRC::Calculate16(0x1021, 0x496c, 126, test + 1);
	 */

	Board::Init();

	OS::run();
}

namespace OS
{
template<>
OS_PROCESS void TProc0::exec()
{
	Storage::StorageFlashSPI::Mount();

	djiController.Init();
	djiController.Run();

	//supress compiler warnings
	for (;;)
		;
}

template<>
OS_PROCESS void TProc1::exec()
{
	mavLinkComm.Init();
	mavLinkComm.Run();

	//supress compiler warnings
	for (;;)
		;
}

template<>
OS_PROCESS void TProc2::exec()
{
	for (;;)
		;
}

template<>
OS_PROCESS void TProc3::exec()
{
	usb_worker.Run();

	//supress compiler warnings
	for (;;)
		;
}

template<>
OS_PROCESS void TProc4::exec()
{
	Board::LedError.PowerUp();
	Board::LedError.ModeSetup(GPIO_MODE_OUTPUT, GPIO_PUPD_NONE);
	Board::LedError.SetOutputOptions(GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ);

	Board::LedActivity.PowerUp();
	Board::LedActivity.ModeSetup(GPIO_MODE_OUTPUT, GPIO_PUPD_NONE);
	Board::LedActivity.SetOutputOptions(GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ);

	for (;;)
	{
		Board::LedActivity.Toggle();
		OSAL::Timer::SleepMS(20);
	}
}

}

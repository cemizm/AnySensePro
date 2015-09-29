/*
 * main.cpp
 *
 *  Created on: 09.09.2015
 *      Author: cem
 */

#include <SensorStore.h>

#include <StorageManager.h>
#include <StorageSDSPI.h>
#include <StorageFlashSPI.h>

#include <USBDevice.h>
#include <USBInterface.h>
#include <USBCDCInterface.h>
#include <USBMSCInterface.h>

#include <DJIController.h>

#include <scmRTOS.h>
#include <OSAL.h>

#include "Board.h"

#include "ff.h"

Application::SensorStore sensorStore;

Storage::StorageSDSPI sdStorage(Board::MicroSD::SPI, Board::MicroSD::CSN, Board::MicroSD::CD);
Storage::StorageFlashSPI flashStorage(Board::Ram::SPI, Board::Ram::CSN);

USB::USBCDCInterface iFace;
USB::USBMSCInterface iFace2;
USB::USBInterface* iFaces[] = { &iFace, &iFace2 };

USB::USBDevice usb_device(Board::USB, iFaces, 1);

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

DWORD get_fattime(void)
{
	return 0;
}

FATFS fatFs;
FIL file;

int main()
{
	Board::SystemInit();
	usb_device.Init();

	Storage::Instance.RegisterStorage(Board::Storages::MicroSd, &sdStorage);
	Storage::Instance.RegisterStorage(Board::Storages::Flash, &flashStorage);
	f_mount(&fatFs, "0:", 0);
	f_mount(&fatFs, "1:", 0);

	OS::run();
}

namespace OS
{
template<>
OS_PROCESS void TProc0::exec()
{
	sleep();
}

template<>
OS_PROCESS void TProc1::exec()
{
	f_open(&file, "0:test.txt", FA_WRITE | FA_CREATE_ALWAYS);
	sleep();
}

template<>
OS_PROCESS void TProc2::exec()
{
	Board::LedError.PowerUp();
	Board::LedError.ModeSetup(GPIO_MODE_OUTPUT, GPIO_PUPD_NONE);
	Board::LedError.SetOutputOptions(GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ);

	Board::LedActivity.PowerUp();
	Board::LedActivity.ModeSetup(GPIO_MODE_OUTPUT, GPIO_PUPD_NONE);
	Board::LedActivity.SetOutputOptions(GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ);

	Board::LedError.Toggle();

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

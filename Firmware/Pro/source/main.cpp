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

#include <Stopwatch.h>
#include <scmRTOS.h>
#include <OSAL.h>

#include "Board.h"

Application::SensorStore sensorStore;

Storage::StorageSDSPI sdStorage(Board::MicroSD::SPI, Board::MicroSD::CSN, Board::MicroSD::CD);
Storage::StorageFlashSPI flashStorage(Board::Flash::SPI, Board::Flash::CSN);

USB::USBCDCInterface iFace;
USB::USBMSCInterface iFace2;
USB::USBInterface* iFaces[] = { &iFace, &iFace2 };

USB::USBDevice usb_device(Board::USB, iFaces, 1);

Application::DJIController djiController(Board::FC::CAN, sensorStore);

// Process types
typedef OS::process<OS::pr0, 512> TProc0;
typedef OS::process<OS::pr1, 2048> TProc1;
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

#define BUFFER		2048
#define F10MB		10485760/BUFFER
#define F100MB		104857600/BUFFER

FATFS fatFs;
FIL file;
char* content[BUFFER];

int main()
{
	Board::SystemInit();
	usb_device.Init();

	Storage::Instance.RegisterStorage(Board::Storages::SDStorage, &sdStorage);
	Storage::Instance.RegisterStorage(Board::Storages::FlashStorage, &flashStorage);

	f_mount(&fatFs, "SD:", 0);
	f_mount(&fatFs, "SPIFLASH:", 0);

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
	Utils::Stopwatch sp;
	uint32_t read = 0;
	uint32_t written = 0;

	volatile uint32_t stop[5] = { 0 };

	FRESULT fr = FR_TIMEOUT;
	for (;;)
	{
		//Format Disk
		sp.Reset();
		if (f_mkfs("SD:", 0, 0) == FR_OK)
			f_setlabel("SD:AnyLogger");

		if (stop[0] == 0)
			stop[0] = sp.ElapsedTime();

		stop[0] = (sp.ElapsedTime() + stop[0]) / 2;

		//Write 10.485.760 Bytes
		sp.Reset();
		fr = f_open(&file, "SD:10.bin", FA_READ | FA_WRITE | FA_CREATE_ALWAYS);
		if (fr == FR_OK)
		{
			for (uint32_t i = 0; (i < F10MB && fr == FR_OK); i++)
			{
				fr = f_write(&file, content, BUFFER, (UINT*) &written);
			}

			f_close(&file);
		}

		if (stop[1] == 0)
			stop[1] = sp.ElapsedTime();
		stop[1] = (sp.ElapsedTime() + stop[1]) / 2;

		//Write 104.857.600 Bytes
		sp.Reset();
		fr = f_open(&file, "SD:100.bin", FA_READ | FA_WRITE | FA_CREATE_ALWAYS);
		if (fr == FR_OK)
		{
			for (uint32_t i = 0; (i < F100MB && fr == FR_OK); i++)
			{
				fr = f_write(&file, content, BUFFER, (UINT*) &written);
			}

			f_close(&file);
		}
		if (stop[2] == 0)
			stop[2] = sp.ElapsedTime();
		stop[2] = (sp.ElapsedTime() + stop[2]) / 2;

		//Read 10.485.760 Bytes
		sp.Reset();
		fr = f_open(&file, "SD:10.bin", FA_READ | FA_OPEN_EXISTING);
		if (fr == FR_OK)
		{
			do
			{
				fr = f_read(&file, content, BUFFER, (UINT*) &read);
			} while (fr == FR_OK && read == BUFFER);

			f_close(&file);
		}
		if (stop[3] == 0)
			stop[3] = sp.ElapsedTime();
		stop[3] = (sp.ElapsedTime() + stop[3]) / 2;

		//Read 104.857.600 Bytes
		sp.Reset();
		fr = f_open(&file, "SD:100.bin", FA_READ | FA_OPEN_EXISTING);
		if (fr == FR_OK)
		{
			do
			{
				fr = f_read(&file, content, BUFFER, (UINT*) &read);
			} while (fr == FR_OK && read == BUFFER);

			f_close(&file);
		}
		if (stop[4] == 0)
			stop[4] = sp.ElapsedTime();
		stop[4] = (sp.ElapsedTime() + stop[4]) / 2;


		sleep(delay_ms(100));
	}
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

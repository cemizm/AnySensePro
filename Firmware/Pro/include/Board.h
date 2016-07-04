/*
 * Board.h
 *
 *  Created on: 11.09.2015
 *      Author: cem
 */

#ifndef BOARD_H_
#define BOARD_H_

#include <Pin.h>
#include <SPI.h>
#include <DMA.h>
#include <CAN.h>
#include <USB.h>
#include <USART.h>
#include <USBCDCDevice.h>
#include <Timer.h>
#include <I2C.h>

#define FIRMWARE_VERSION		0x02020100
#define HARDWARE_VERSION		0x01

namespace Board
{

enum Storages
{
	FlashStorage = 0, SDStorage = 1,
};

void Init();
void InitBootLoader();
void JumpToBootLoader();

extern HAL::Pin LedActivity;
extern HAL::Pin LedError;

extern USB::USBCDCDevice CDCDevice;

namespace MicroSD
{

extern HAL::Pin CD;
extern HAL::Pin CSN;
extern HAL::SPI SPI;

}

namespace Flash
{

}

namespace FC
{
extern HAL::CAN CAN;
extern HAL::USART USART;
}

namespace Telemetry
{
extern HAL::USART USART;
extern HAL::Timer TIMER;
extern HAL::I2C I2C;
}

namespace Sensor
{
extern HAL::USART USART;
}

namespace OSD
{
extern HAL::USART USART;
}

}

#endif /* BOARD_H_ */

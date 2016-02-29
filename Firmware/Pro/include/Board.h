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
#include <BLEDevice.h>

#include <StorageFlashSPI.h>


#define FIRMWARE_VERSION		0x00010001
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

extern HAL::USB USB;

namespace MicroSD
{

extern HAL::Pin CD;
extern HAL::Pin CSN;
extern HAL::SPI SPI;

}

namespace Flash
{

extern HAL::Pin CSN;
extern HAL::SPI SPI;

}


namespace FC
{
extern HAL::CAN CAN;
extern HAL::USART USART;

}

namespace Telemetry
{
extern HAL::USART USART;
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

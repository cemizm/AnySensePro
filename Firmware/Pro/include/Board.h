/*
 * Board.h
 *
 *  Created on: 11.09.2015
 *      Author: cem
 */

#ifndef BOARD_H_
#define BOARD_H_

#include "Pin.h"
#include "SPI.h"
#include "DMA.h"
#include "CAN.h"
#include "USB.h"
#include <BLEDevice.h>

namespace Board
{

enum Storages
{
	FlashStorage = 0, SDStorage = 1,
};

extern void SystemInit();

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

namespace Telemetry
{
}

namespace FC
{
extern HAL::CAN CAN;
}

}

#endif /* BOARD_H_ */

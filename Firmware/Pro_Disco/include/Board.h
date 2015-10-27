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

namespace Board
{

extern void SystemInit();

extern HAL::Pin LedActivity;
extern HAL::Pin LedError;

extern HAL::USB USB;

namespace Telemetry
{
}

namespace FC
{
extern HAL::CAN CAN;
}

}

#endif /* BOARD_H_ */

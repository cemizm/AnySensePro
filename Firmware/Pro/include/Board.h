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
#include <BLEDevice.h>



namespace Board
{

extern void SystemInit();

extern HAL::Pin Led;

namespace BLE
{

extern HAL::Pin IRQ;
extern HAL::Pin CSN;
extern HAL::Pin RSTN;

extern HAL::SPI SPI;

extern BlueNRG::BLEConfig Config;

}

}

#endif /* BOARD_H_ */

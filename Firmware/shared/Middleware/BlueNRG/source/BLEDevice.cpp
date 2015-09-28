/*
 * BLEDevice.cpp
 *
 *  Created on: 08.09.2015
 *      Author: cem
 */

#include "BLEDevice.h"
#include "OSAL.h"

#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/exti.h>

namespace BlueNRG
{

void BLEDevice::Init()
{
	m_bleInterface.Init();
	m_bleInterface.Enable();
}

void BLEDevice::Run()
{
	m_bleInterface.Run();
}

}


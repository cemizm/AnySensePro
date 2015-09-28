/*
 * BLEDevice.h
 *
 *  Created on: 08.09.2015
 *      Author: cem
 */

#ifndef MIDDLEWARE_BLUENRG_INCLUDE_BLEDEVICE_H_
#define MIDDLEWARE_BLUENRG_INCLUDE_BLEDEVICE_H_

#include "BLEInterface.h"

#include "HCI.h"
#include "GATT.h"
#include "GAP.h"

namespace BlueNRG
{

struct BLEConfig
{
	uint8_t DeviceAddress[6];
	const char* LocalName;
	const char* ServiceName;
};

class BLEDevice
{

private:
	BLEConfig& m_bleConfig;
	BLEInterface m_bleInterface;
public:
	BLEDevice(BLEConfig& config, HAL::SPI& spi, HAL::Pin& csn, HAL::Pin& irq, HAL::Pin& rstn) :
			m_bleConfig(config), m_bleInterface(spi, csn, irq, rstn), HCI(m_bleInterface), GATT(HCI), GAP(HCI)
	{
	}

	void Init();
	void Run();

	HCInterface HCI;
	GATTInterface GATT;
	GAPInterface GAP;

};

} /* namespace BlueNRG */

#endif /* MIDDLEWARE_BLUENRG_INCLUDE_BLEDEVICE_H_ */

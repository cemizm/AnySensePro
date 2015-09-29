/*
 * StorageSDSPI.h
 *
 *  Created on: 28.09.2015
 *      Author: cem
 */

#ifndef MIDDLEWARE_STORAGE_INCLUDE_STORAGEFLASHSPI_H_
#define MIDDLEWARE_STORAGE_INCLUDE_STORAGEFLASHSPI_H_

#include <stdint.h>

#include <Interrupt.h>
#include <SPI.h>
#include <Pin.h>

#include "StorageInterface.h"

namespace Storage
{

enum FlashCommand
{
	WriteEnable = 0x06,
	WriteDisable = 0x04,
	ReadId = 0x9F,
	ReadStatus = 0x05,
	WriteStatus = 0x01,
	ReadData = 0x03,
	FastReadData = 0x0B,
	PageProgram = 0x02,
	SectorErase = 0xD8,
	BlukErase = 0xC7,
	DeepPowerDown = 0xB9,
	ReleasePowerDown = 0xAB,
};

class StorageFlashSPI: public StorageInterface, public HAL::InterruptHandler
{
private:
	StorageStatus m_status;
	HAL::SPI& m_spi;
	HAL::Pin& m_csn;

	uint8_t SendCommand(FlashCommand cmd, uint32_t params);
public:
	StorageFlashSPI(HAL::SPI& spi, HAL::Pin& csn) :
			m_status(StorageStatus::NoInit), m_spi(spi), m_csn(csn)
	{

	}

	StorageStatus GetStatus() override;
	StorageStatus Init() override;
	StorageResult Read(uint8_t* buff, uint32_t sector, uint16_t count) override;
	StorageResult Write(const uint8_t* buff, uint32_t sector, uint16_t count) override;
	StorageResult IOCtl(StorageCommand cmd, void* buff) override;


	void ISR() override;
};

}
/* namespace Storage */

#endif /* MIDDLEWARE_STORAGE_INCLUDE_STORAGESDSPI_H_ */

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

#define PAGE_SIZE        	256UL
#define PAGES_PER_SECTORS   256UL
#define SECTORS          	32UL
#define SECTOR_SIZE			(PAGE_SIZE * PAGES_PER_SECTORS)
#define PAGES            	(SECTORS * PAGES_PER_SECTORS)
#define SIZE             	(SECTORS * PAGES_PER_SECTORS * PAGE_SIZE)

class StorageFlashSPI: public StorageInterface, public HAL::InterruptHandler
{
private:
	uint8_t m_status;
	uint8_t m_initialized;
	HAL::SPI& m_spi;
	HAL::Pin& m_csn;
	uint16_t m_workbyte[1] = { 0xffff };
	OSAL::EventFlag m_dmaRXFinished;
	OSAL::EventFlag m_dmaTXFinished;

	void InitHW();

	uint8_t waitReady(uint16_t timeout);
	uint8_t writePage(uint32_t page, const uint8_t* data);

public:
	StorageFlashSPI(HAL::SPI& spi, HAL::Pin& csn) :
			m_status(StorageStatus::NoInit), m_initialized(), m_spi(spi), m_csn(csn), m_dmaRXFinished(), m_dmaTXFinished()
	{

	}

	uint8_t GetStatus() override;
	uint8_t Init() override;
	StorageResult Read(uint8_t* buff, uint32_t sector, uint16_t count) override;
	StorageResult Write(const uint8_t* buff, uint32_t sector, uint16_t count) override;
	StorageResult IOCtl(StorageCommand cmd, void* buff) override;

	void ISR() override;
};

}
/* namespace Storage */

#endif /* MIDDLEWARE_STORAGE_INCLUDE_STORAGESDSPI_H_ */

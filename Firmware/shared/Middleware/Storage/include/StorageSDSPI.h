/*
 * StorageSDSPI.h
 *
 *  Created on: 28.09.2015
 *      Author: cem
 */

#ifndef MIDDLEWARE_STORAGE_INCLUDE_STORAGESDSPI_H_
#define MIDDLEWARE_STORAGE_INCLUDE_STORAGESDSPI_H_

#include <stdint.h>

#include <OSAL.h>

#include <Interrupt.h>
#include <SPI.h>
#include <Pin.h>

#include "StorageInterface.h"

namespace CardType
{

static const uint8_t Unknown = 0x00; /* Failure or Unknown */
static const uint8_t MMC = 0x01; /* MMC ver 3 */
static const uint8_t SD1 = 0x02; /* SD ver 1 */
static const uint8_t SD2 = 0x04; /* SD ver 2 */
static const uint8_t SDC = (SD1 | SD2); /* SD */
static const uint8_t Block = 0x08; /* Block addressing */

}

namespace Storage
{

static const uint8_t ACMD = 0x80; /* ACMD Command */
static const uint8_t CMD0 = 0; /* GO_IDLE_STATE */
static const uint8_t CMD1 = 1; /* SEND_OP_COND (MMC) */
static const uint8_t ACMD41 = ACMD + 41; /* SEND_OP_COND (SDC) */
static const uint8_t CMD8 = 8; /* SEND_IF_COND */
static const uint8_t CMD9 = 9; /* SEND_CSD */
static const uint8_t CMD10 = 10; /* SEND_CID */
static const uint8_t CMD12 = 12; /* STOP_TRANSMISSION */
static const uint8_t ACMD13 = ACMD + 13; /* SD_STATUS (SDC) */
static const uint8_t CMD16 = 16; /* SET_BLOCKLEN */
static const uint8_t CMD17 = 17; /* READ_SINGLE_BLOCK */
static const uint8_t CMD18 = 18; /* READ_MULTIPLE_BLOCK */
static const uint8_t CMD23 = 23; /* SET_BLOCK_COUNT (MMC) */
static const uint8_t ACMD23 = ACMD + 23; /* SET_WR_BLK_ERASE_COUNT (SDC) */
static const uint8_t CMD24 = 24; /* WRITE_BLOCK */
static const uint8_t CMD25 = 25; /* WRITE_MULTIPLE_BLOCK */
static const uint8_t CMD32 = 32; /* ERASE_ER_BLK_START */
static const uint8_t CMD33 = 33; /* ERASE_ER_BLK_END */
static const uint8_t CMD38 = 38; /* ERASE */
static const uint8_t CMD55 = 55; /* APP_static const uint8_t CMD */
static const uint8_t CMD58 = 58; /* READ_OCR */

class StorageSDSPI: public StorageInterface, public HAL::InterruptHandler
{
private:
	uint8_t m_status;
	uint8_t m_cardType;
	HAL::SPI& m_spi;
	HAL::Pin& m_csn;
	HAL::Pin& m_cd;
	OSAL::EventFlag m_dmaRXFinished;
	OSAL::EventFlag m_dmaTXFinished;
	uint8_t m_hwInit;
	uint16_t m_workbyte[1] = { 0xffff };

	void initHardware();
	uint8_t detect();

	uint8_t waitReady(uint16_t ms);
	void deselect();
	uint8_t select();
	void setSpeed(uint8_t high);

	uint8_t sendCommand(uint8_t cmd, uint32_t params);

	uint8_t readBlock(uint8_t* buff, uint16_t btr);
	uint8_t writeBlock(const uint8_t* buff, uint16_t token);

public:
	StorageSDSPI(HAL::SPI& spi, HAL::Pin& csn, HAL::Pin& cd) :
			m_status(StorageStatus::NoInit), m_cardType(CardType::Unknown), m_spi(spi), m_csn(csn), m_cd(cd), m_dmaRXFinished(), m_dmaTXFinished(), m_hwInit(
					0)
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

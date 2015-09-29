/*
 * StorageSDSPI.h
 *
 *  Created on: 28.09.2015
 *      Author: cem
 */

#ifndef MIDDLEWARE_STORAGE_INCLUDE_STORAGESDSPI_H_
#define MIDDLEWARE_STORAGE_INCLUDE_STORAGESDSPI_H_

#include <stdint.h>

#include <Interrupt.h>
#include <SPI.h>
#include <Pin.h>

#include "StorageInterface.h"

namespace Storage
{

enum SDCommand
{
	ACMD = 0x80,
	CMD0 = 0, /* GO_IDLE_STATE */
	CMD1 = 1, /* SEND_OP_COND (MMC) */
	ACMD41 = ACMD + 41, /* SEND_OP_COND (SDC) */
	CMD8 = 8, /* SEND_IF_COND */
	CMD9 = 9, /* SEND_CSD */
	CMD10 = 10, /* SEND_CID */
	CMD12 = 12, /* STOP_TRANSMISSION */
	ACMD13 = ACMD + 13, /* SD_STATUS (SDC) */
	CMD16 = 16, /* SET_BLOCKLEN */
	CMD17 = 17, /* READ_SINGLE_BLOCK */
	CMD18 = 18, /* READ_MULTIPLE_BLOCK */
	CMD23 = 23, /* SET_BLOCK_COUNT (MMC) */
	ACMD23 = ACMD + 23, /* SET_WR_BLK_ERASE_COUNT (SDC) */
	CMD24 = 24, /* WRITE_BLOCK */
	CMD25 = 25, /* WRITE_MULTIPLE_BLOCK */
	CMD32 = 32, /* ERASE_ER_BLK_START */
	CMD33 = 33, /* ERASE_ER_BLK_END */
	CMD38 = 38, /* ERASE */
	CMD55 = 55, /* APP_CMD */
	CMD58 = 58, /* READ_OCR */
};

inline SDCommand operator~ (SDCommand a) { return (SDCommand)~(int)a; }
inline SDCommand operator| (SDCommand a, SDCommand b) { return (SDCommand)((int)a | (int)b); }
inline SDCommand operator& (SDCommand a, SDCommand b) { return (SDCommand)((int)a & (int)b); }
inline SDCommand operator^ (SDCommand a, SDCommand b) { return (SDCommand)((int)a ^ (int)b); }
inline SDCommand& operator|= (SDCommand& a, SDCommand b) { return (SDCommand&)((int&)a |= (int)b); }
inline SDCommand& operator&= (SDCommand& a, SDCommand b) { return (SDCommand&)((int&)a &= (int)b); }
inline SDCommand& operator^= (SDCommand& a, SDCommand b) { return (SDCommand&)((int&)a ^= (int)b); }

enum CardType
{
	Unknown = 0x00, /* Failure or Unknown */
	MMC = 0x01, /* MMC ver 3 */
	SD1 = 0x02, /* SD ver 1 */
	SD2 = 0x04, /* SD ver 2 */
	SDC = (SD1 | SD2), /* SD */
	Block = 0x08, /* Block addressing */

};

inline CardType operator~ (CardType a) { return (CardType)~(int)a; }
inline CardType operator| (CardType a, CardType b) { return (CardType)((int)a | (int)b); }
inline CardType operator& (CardType a, CardType b) { return (CardType)((int)a & (int)b); }
inline CardType operator^ (CardType a, CardType b) { return (CardType)((int)a ^ (int)b); }
inline CardType& operator|= (CardType& a, CardType b) { return (CardType&)((int&)a |= (int)b); }
inline CardType& operator&= (CardType& a, CardType b) { return (CardType&)((int&)a &= (int)b); }
inline CardType& operator^= (CardType& a, CardType b) { return (CardType&)((int&)a ^= (int)b); }

class StorageSDSPI: public StorageInterface, public HAL::InterruptHandler
{
private:
	StorageStatus m_status;
	CardType m_cardType;
	HAL::SPI& m_spi;
	HAL::Pin& m_csn;
	HAL::Pin& m_cd;

	uint8_t waitReady(uint16_t ms);
	void deselect();
	uint8_t select();

	uint8_t sendCommand(SDCommand cmd, uint32_t params);

public:
	StorageSDSPI(HAL::SPI& spi, HAL::Pin& csn, HAL::Pin& cd) :
			m_status(StorageStatus::NoInit), m_cardType(CardType::Unknown), m_spi(spi), m_csn(csn), m_cd(cd)
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

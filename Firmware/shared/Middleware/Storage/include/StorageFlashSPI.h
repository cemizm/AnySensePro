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

#include <spiffs.h>

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

class StorageFlashSPI: public HAL::InterruptHandler
{
private:
	HAL::SPI& m_spi;
	HAL::Pin& m_csn;
	uint16_t m_workbyte[1] = { 0xffff };
	OSAL::EventFlag m_dmaRXFinished;
	OSAL::EventFlag m_dmaTXFinished;

	u8_t work_buf[PAGE_SIZE * 2];
	u8_t fds[32 * 4];
	u8_t cache_buf[(PAGE_SIZE + 32) * 4];
	spiffs fs;

	void InitHW();
	int32_t _Mount();

	static s32_t spiffs_read(u32_t addr, u32_t size, u8_t *dst);
	static s32_t spiffs_write(u32_t addr, u32_t size, u8_t *src);
	static s32_t spiffs_erase(u32_t addr, u32_t size);

	uint8_t waitReady(uint16_t timeout);
	uint8_t waitReadyAsync(uint16_t timeout);
	uint8_t writePage(uint32_t addr, uint32_t size, const uint8_t* data);

public:
	StorageFlashSPI(HAL::SPI& spi, HAL::Pin& csn) :
			m_spi(spi), m_csn(csn), m_dmaRXFinished(), m_dmaTXFinished()
	{

	}

	class FileAutoRelease
	{
	private:
		spiffs_file fh;
	public:
		FileAutoRelease(spiffs_file fd) :
				fh(fd)
		{
		}
		~FileAutoRelease()
		{
			Close(fh);
		}
	};

	int32_t Init();

	int32_t Read(uint32_t addr, uint32_t size, uint8_t* buff);
	int32_t Write(uint32_t addr, uint32_t size, const uint8_t* buff);
	int32_t Erase(uint32_t addr, uint32_t size);

	static int32_t Mount();
	static void Unmount();
	static int32_t Format();
	static spiffs_file Open(const char *path, spiffs_flags flags, spiffs_mode mode);
	static int32_t Write(spiffs_file fh, void *buf, int32_t len);
	static int32_t Read(spiffs_file fh, void* buf, int32_t len);
	static int32_t Close(spiffs_file fh);
	static int32_t Rename(const char* oldPath, const char* newPath);
	static int32_t Remove(const char* path);
	static int32_t FileStat(spiffs_file fh, spiffs_stat* st);
	static int32_t GC(uint32_t size);

	void ISR() override;
};

}

extern Storage::StorageFlashSPI flashStorage;

/* namespace Storage */

#endif /* MIDDLEWARE_STORAGE_INCLUDE_STORAGESDSPI_H_ */

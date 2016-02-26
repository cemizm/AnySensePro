/*
 * StorageSDSPI.cpp
 *
 *  Created on: 28.09.2015
 *      Author: cem
 */

#include <StorageFlashSPI.h>

#include <OSAL.h>
#include <TimeoutTimer.h>
#include <ChipSelect.h>

namespace Storage
{

void StorageFlashSPI::InitHW()
{
	m_csn.On();
	m_csn.PowerUp();
	m_csn.ModeSetup(GPIO_MODE_OUTPUT, GPIO_PUPD_NONE);
	m_csn.SetOutputOptions(GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ);

	m_spi.Init(SPI_CR1_BAUDRATE_FPCLK_DIV_2, SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE, SPI_CR1_CPHA_CLK_TRANSITION_1, SPI_CR2_DS_8BIT,
	SPI_CR1_MSBFIRST);

	m_spi.SetUniDirectionalMode();
	m_spi.SetFullDuplexMode();

	m_spi.EnableSoftwareSlaveManagement();
	m_spi.DisableCRC();
	m_spi.SetFifoReceptionThreshold8Bit();

	m_spi.SetupRXDMA();
	m_spi.SetupTXDMA();

	HAL::InterruptRegistry.Enable(m_spi.GetRXDMA().NVIC_IRQn, 15, this);

	m_spi.Enable();
}

int32_t StorageFlashSPI::Init()
{

	InitHW();

	Utils::ChipSelect select(m_csn);

	m_spi.Xfer8(FlashCommand::ReadId);

	m_spi.Xfer8(0xFF); //Manufacturer
	uint16_t deviceID = m_spi.Xfer8(0xFF) << 8; //Device ID
	deviceID |= m_spi.Xfer8(0xFF);

	if (deviceID != 0x2015)
		return SPIFFS_ERR_INTERNAL;

	return SPIFFS_OK;
}

int32_t StorageFlashSPI::Mount(spiffs* fs)
{
	spiffs_config cfg;
	cfg.phys_size = SIZE; // use all spi flash
	cfg.phys_addr = 0; // start spiffs at start of spi flash
	cfg.phys_erase_block = SECTOR_SIZE; // according to datasheet
	cfg.log_block_size = SECTOR_SIZE; // let us not complicate things
	cfg.log_page_size = PAGE_SIZE; // as we said

	cfg.hal_read_f = spiffs_read;
	cfg.hal_write_f = spiffs_write;
	cfg.hal_erase_f = spiffs_erase;

	return SPIFFS_mount(fs, &cfg, work_buf, fds, sizeof(fds), cache_buf, sizeof(cache_buf), 0);
}

uint8_t StorageFlashSPI::waitReady(uint16_t timeout)
{
	Utils::ChipSelect select(m_csn);

	volatile uint8_t d = 0x00;
	Utils::TimeoutTimer t(delay_ms(timeout));

	m_spi.Xfer8(FlashCommand::ReadStatus);

	do
	{
		d = m_spi.Xfer8(0xFF);
	} while (((d & 0x01) == 0x01) && !t.IsTimeout());

	return (d & 0x01) == 0x00 ? 1 : 0;
}

uint8_t StorageFlashSPI::writePage(uint32_t addr, uint32_t size, const uint8_t* buff)
{
	Utils::ChipSelect select(m_csn);

	m_spi.Xfer8(FlashCommand::WriteEnable);

	select.Reselect();

	m_spi.Xfer8(FlashCommand::PageProgram);
	m_spi.Xfer8(addr >> 16); // Address
	m_spi.Xfer8(addr >> 8);
	m_spi.Xfer8(addr);

	m_spi.InitDMATransfer(HAL::SPIDMADirection::ToDevice, (uint32_t) buff, (uint32_t) m_workbyte, size);

	m_dmaRXFinished.clear();

	m_spi.EnableRXDma();
	m_spi.EnableTXDma();

	if (!m_dmaRXFinished.wait(100))
	{
		m_spi.DisableClearAll();
		return 0;
	}
	m_spi.DisableClearAll();

	return 1;
}

int32_t StorageFlashSPI::Read(uint32_t addr, uint32_t size, uint8_t* buff)
{
	Utils::ChipSelect select(m_csn);

	m_spi.Xfer8(FlashCommand::FastReadData);
	m_spi.Xfer8(addr >> 16); // Address
	m_spi.Xfer8(addr >> 8);
	m_spi.Xfer8(addr);

	m_spi.Xfer8(0xFF); // Dummy

	m_spi.InitDMATransfer(HAL::SPIDMADirection::FromDevice, (uint32_t) m_workbyte, (uint32_t) buff, size);

	m_dmaRXFinished.clear();

	m_spi.EnableRXDma();
	m_spi.EnableTXDma();

	if (!m_dmaRXFinished.wait(delay_ms(100)))
	{
		m_spi.DisableClearAll();
		return SPIFFS_ERR_INTERNAL;
	}

	m_spi.DisableClearAll();

	return SPIFFS_OK;
}

int32_t StorageFlashSPI::Write(uint32_t addr, uint32_t size, const uint8_t* buff)
{

	uint32_t buff_size = PAGE_SIZE;
	do
	{
		buff_size = size > PAGE_SIZE ? PAGE_SIZE : size;

		if (!writePage(addr, buff_size, buff))
			break;

		if (!waitReady(10))
			break;

		addr += buff_size;
		buff += buff_size;
		size -= buff_size;

	} while (size > 0);

	return size > 0 ? SPIFFS_ERR_INTERNAL : SPIFFS_OK;
}

int32_t StorageFlashSPI::Erase(uint32_t addr, uint32_t size)
{
	(void) size;

	Utils::ChipSelect select(m_csn);

	m_spi.Xfer8(FlashCommand::WriteEnable);

	select.Reselect();

	m_spi.Xfer8(FlashCommand::SectorErase);
	m_spi.Xfer8(addr >> 16); // Address
	m_spi.Xfer8(addr >> 8);
	m_spi.Xfer8(addr);

	select.Deselect();

	if (!waitReady(3100))
		return SPIFFS_ERR_ERASE_FAIL;

	return SPIFFS_OK;
}

void StorageFlashSPI::ISR()
{
	if (m_spi.GetRXDMA().GetInterruptFlag(DMA_TCIF))
	{
		m_spi.GetRXDMA().ClearInterruptFlags(DMA_TCIF);
		m_dmaRXFinished.signal_isr();
	}
	if (m_spi.GetTXDMA().GetInterruptFlag(DMA_TCIF))
	{
		m_spi.GetTXDMA().ClearInterruptFlags(DMA_TCIF);
		m_dmaTXFinished.signal_isr();
	}
}

s32_t StorageFlashSPI::spiffs_read(u32_t addr, u32_t size, u8_t *dst)
{
	return flashStorage.Read(addr, size, dst);
}

s32_t StorageFlashSPI::spiffs_write(u32_t addr, u32_t size, u8_t *src)
{
	return flashStorage.Write(addr, size, src);
}

s32_t StorageFlashSPI::spiffs_erase(u32_t addr, u32_t size)
{
	return flashStorage.Erase(addr, size);
}

}

/* namespace Storage */

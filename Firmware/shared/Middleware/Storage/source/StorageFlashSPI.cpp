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

#define FAT_SECTOR_SIZE			512

namespace Storage
{

uint8_t StorageFlashSPI::GetStatus()
{
	return m_status;
}

void StorageFlashSPI::InitHW()
{
	if (m_initialized)
		return;

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

	m_initialized = 1;
}

uint8_t StorageFlashSPI::Init()
{
	m_status = StorageStatus::NoInit;

	InitHW();

	Utils::ChipSelect select(m_csn);

	m_spi.Xfer8(FlashCommand::ReadId);

	m_spi.Xfer8(0xFF); //Manufacturer
	uint16_t deviceID = m_spi.Xfer8(0xFF) << 8; //Device ID
	deviceID |= m_spi.Xfer8(0xFF);

	if (deviceID == 0x2015)
		m_status &= ~(StorageStatus::NoInit | StorageStatus::NoDisk | StorageStatus::WriteProtect);

	return m_status;
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

StorageResult StorageFlashSPI::Read(uint8_t* buff, uint32_t sector, uint16_t count)
{
	if (m_status & StorageStatus::NoInit)
		return StorageResult::NotReady;

	sector *= FAT_SECTOR_SIZE;

	Utils::ChipSelect select(m_csn);

	m_spi.Xfer8(FlashCommand::FastReadData);
	m_spi.Xfer8(sector >> 16); // Address
	m_spi.Xfer8(sector >> 8);
	m_spi.Xfer8(sector);

	m_spi.Xfer8(0xFF); // Dummy

	m_spi.InitDMATransfer(HAL::SPIDMADirection::FromDevice, (uint32_t) m_workbyte, (uint32_t) buff, count * FAT_SECTOR_SIZE);

	m_dmaRXFinished.clear();

	m_spi.EnableRXDma();
	m_spi.EnableTXDma();

	if (!m_dmaRXFinished.wait(delay_ms(100)))
	{
		m_spi.DisableClearAll();
		return StorageResult::Error;
	}

	m_spi.DisableClearAll();

	return StorageResult::OK;
}

StorageResult StorageFlashSPI::Write(const uint8_t* buff, uint32_t sector, uint16_t count)
{
	sector *= FAT_SECTOR_SIZE;
	count *= 2;

	do
	{
		if (!writePage(sector, buff))
			break;

		if (!waitReady(10))
			break;

		sector += PAGE_SIZE;
		buff += PAGE_SIZE;

	} while (--count);

	return count > 0 ? StorageResult::Error : StorageResult::OK;
}

uint8_t StorageFlashSPI::writePage(uint32_t sector, const uint8_t* buff)
{
	Utils::ChipSelect select(m_csn);

	m_spi.Xfer8(FlashCommand::WriteEnable);

	select.Reselect();

	m_spi.Xfer8(FlashCommand::PageProgram);
	m_spi.Xfer8(sector >> 16); // Address
	m_spi.Xfer8(sector >> 8);
	m_spi.Xfer8(sector);

	m_spi.InitDMATransfer(HAL::SPIDMADirection::ToDevice, (uint32_t) buff, (uint32_t) m_workbyte, PAGE_SIZE);

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

StorageResult StorageFlashSPI::IOCtl(StorageCommand cmd, void* buff)
{
	StorageResult res = StorageResult::Error;

	if (m_status & StorageStatus::NoInit)
		return StorageResult::NotReady;

	switch (cmd)
	{
	case StorageCommand::Sync: /* Make sure that no pending write process */
		if (waitReady(500))
			res = StorageResult::OK;
		break;
	case StorageCommand::GetSectorCount: /* Get number of sectors on the disk (DWORD) */
		*(uint32_t*) buff = SIZE / FAT_SECTOR_SIZE;
		res = StorageResult::OK;
		break;
	case StorageCommand::GetSectorSize: /* Get R/W sector size (WORD) */
		*(uint32_t*) buff = FAT_SECTOR_SIZE;
		res = StorageResult::OK;
		break;
	case StorageCommand::GetBlockSize:
		*(uint32_t*) buff = SECTOR_SIZE / FAT_SECTOR_SIZE;
		res = StorageResult::OK;
		break;

	default:
		res = StorageResult::InvalidParameter;
	}

	return res;
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

}

/* namespace Storage */

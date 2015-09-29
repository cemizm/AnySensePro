/*
 * StorageSDSPI.cpp
 *
 *  Created on: 28.09.2015
 *      Author: cem
 */

#include <StorageFlashSPI.h>

#include <OSAL.h>
#include <TimeoutTimer.h>

namespace Storage
{

StorageStatus StorageFlashSPI::GetStatus()
{
	return m_status;
}

StorageStatus StorageFlashSPI::Init()
{
	m_status = StorageStatus::NoInit;

	m_csn.On();
	m_csn.PowerUp();
	m_csn.ModeSetup(GPIO_MODE_OUTPUT, GPIO_PUPD_NONE);
	m_csn.SetOutputOptions(GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ);

	m_spi.Init(SPI_CR1_BAUDRATE_FPCLK_DIV_256, SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE, SPI_CR1_CPHA_CLK_TRANSITION_1, SPI_CR2_DS_8BIT,
	SPI_CR1_MSBFIRST);

	m_spi.SetUniDirectionalMode();
	m_spi.SetFullDuplexMode();

	m_spi.EnableSoftwareSlaveManagement();
	m_spi.DisableCRC();
	m_spi.SetFifoReceptionThreshold8Bit();

	//m_spi.SetupRXDMA();
	//m_spi.SetupTXDMA();

	HAL::InterruptRegistry.Enable(m_spi.GetRXDMA().NVIC_IRQn, 15, this);
	HAL::InterruptRegistry.Enable(m_spi.GetTXDMA().NVIC_IRQn, 15, this);

	m_spi.Enable();

	m_csn.On();
	//
	//Step 1 Select Chip
	//
	m_csn.Off();
	//
	//Step 2 Transfer intruction of "get ID"
	//
	m_spi.Xfer8(FlashCommand::ReadId);

	//
	// Get Manufacture ID
	//
	uint8_t ucManuID = m_spi.Xfer8(0xFF);
	//
	// Get Devidce ID
	//
	uint8_t ucHighDeviID = m_spi.Xfer8(0xFF);
	uint8_t ucLowDeviID = m_spi.Xfer8(0xFF);
	//ucLowDeviID = m_spi.Xfer8(0xFF);
	//
	//Step 3 Disable chip select
	//
	m_csn.On();

	uint32_t ulIDcode = ucManuID << 16;
	ulIDcode |= ucHighDeviID << 8;
	ulIDcode |= ucLowDeviID;

	if (ulIDcode == 0xFF)
	{
		m_csn.Off();
	}

	return m_status;
}

StorageResult StorageFlashSPI::Read(uint8_t* buff, uint32_t sector, uint16_t count)
{
	(void) buff;
	(void) sector;
	(void) count;
	return StorageResult::Error;
}

StorageResult StorageFlashSPI::Write(const uint8_t* buff, uint32_t sector, uint16_t count)
{
	(void) buff;
	(void) sector;
	(void) count;
	return StorageResult::Error;
}

StorageResult StorageFlashSPI::IOCtl(StorageCommand cmd, void* buff)
{
	(void) cmd;
	(void) buff;
	return StorageResult::Error;
}

uint8_t StorageFlashSPI::SendCommand(FlashCommand cmd, uint32_t params)
{

}

void StorageFlashSPI::ISR()
{

}

}

/* namespace Storage */

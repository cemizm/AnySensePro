/*
 * StorageSDSPI.cpp
 *
 *  Created on: 28.09.2015
 *      Author: cem
 */

#include <StorageSDSPI.h>

#include <OSAL.h>
#include <TimeoutTimer.h>

namespace Storage
{

StorageStatus StorageSDSPI::GetStatus()
{
	return m_status;
}

StorageStatus StorageSDSPI::Init()
{
	uint8_t n, ocr[4];
	CardType ct = CardType::Unknown;
	SDCommand cmd = SDCommand::CMD0;

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

	m_cd.PowerUp();
	m_cd.ModeSetup(GPIO_MODE_INPUT, GPIO_PUPD_PULLUP);

	m_spi.Enable();

	//while (!m_spi.GetTXEmpty())
	//	;
	//m_spi.Xfer8(0xFF);

	//HAL::OSAL::SleepMS(10);

	if (m_cd.Get())
		return StorageStatus::NoDisk;

	for (n = 10; n; n--)
		m_spi.Xfer8(0xFF);

	if (sendCommand(CMD0, 0) == 1)
	{
		TimeoutTimer t(delay_sec(1));

		if (sendCommand(CMD8, 0x1AA) == 1)
		{
			for (n = 0; n < 4; n++)
				ocr[n] = m_spi.Xfer8(0xFF);

			if (ocr[2] == 0x01 && ocr[3] == 0xAA)
			{
				while (!t.IsTimeout() && sendCommand(ACMD41, 1UL << 30))
					;

				if (!t.IsTimeout() && sendCommand(CMD58, 0) == 0)
				{
					for (n = 0; n < 4; n++)
					{
						ocr[n] = m_spi.Xfer8(0xFF);
					}
					ct = (ocr[0] & 0x40) ? CardType::SD2 | CardType::Block : CardType::SD2;
				}
			}
		}
		else
		{
			if (sendCommand(ACMD41, 0) <= 1)
			{
				ct = CardType::SD1;
				cmd = ACMD41;
			}
			else
			{
				ct = CardType::MMC;
				cmd = CMD1;
			}
			while (!t.IsTimeout() && sendCommand(cmd, 0))
				;
			if (!t.IsTimeout() || sendCommand(CMD16, 512) != 0)
			{
				ct = CardType::Unknown;
			}
		}
	}

	m_cardType = ct;
	deselect();

	if (ct != CardType::Unknown)
	{
		m_status &= ~StorageStatus::NoInit;
		m_status &= ~StorageStatus::WriteProtect;
	}
	else
	{
		m_status = StorageStatus::NoInit;
	}

	return m_status;
}

StorageResult StorageSDSPI::Read(uint8_t* buff, uint32_t sector, uint16_t count)
{
	(void) buff;
	(void) sector;
	(void) count;
	return StorageResult::Error;
}

StorageResult StorageSDSPI::Write(const uint8_t* buff, uint32_t sector, uint16_t count)
{
	(void) buff;
	(void) sector;
	(void) count;
	return StorageResult::Error;
}

StorageResult StorageSDSPI::IOCtl(StorageCommand cmd, void* buff)
{
	(void) cmd;
	(void) buff;
	return StorageResult::Error;
}

uint8_t StorageSDSPI::waitReady(uint16_t ms)
{
	uint8_t d;
	TimeoutTimer t(delay_ms(ms));

	do
	{
		d = m_spi.Xfer8(0xFF);
	} while (d != 0xFF && !t.IsTimeout());

	return (d == 0xFF) ? 1 : 0;
}

void StorageSDSPI::deselect()
{
	m_csn.On();
	m_spi.Xfer8(0xFF);
}

uint8_t StorageSDSPI::select()
{
	m_csn.Off();
	m_spi.Xfer8(0xFF);

	if (waitReady(500))
		return 1;

	deselect();
	return 0;
}

uint8_t StorageSDSPI::sendCommand(SDCommand cmd, uint32_t params)
{
	uint8_t n, res;

	if (cmd & ACMD)
	{
		cmd &= ~ACMD;
		res = sendCommand(CMD55, 0);
		if (res > 1)
			return res;
	}

	if (cmd != CMD12)
	{
		deselect();
		if (!select())
			return 0xFF;
	}

	m_spi.Xfer8(0x40 | cmd);
	m_spi.Xfer8((uint8_t) (params >> 24));
	m_spi.Xfer8((uint8_t) (params >> 16));
	m_spi.Xfer8((uint8_t) (params >> 8));
	m_spi.Xfer8((uint8_t) params);

	n = 0x01;
	if (cmd == CMD0)
		n = 0x95;
	if (cmd == CMD8)
		n = 0x87;

	m_spi.Xfer8(n);

	if (cmd == CMD12)
		m_spi.Xfer8(0xFF);

	n = 15;
	do
	{
		res = m_spi.Xfer8(0xFF);
	} while ((res & 0x80) && --n);

	return res;
}

void StorageSDSPI::ISR()
{

}

}

/* namespace Storage */

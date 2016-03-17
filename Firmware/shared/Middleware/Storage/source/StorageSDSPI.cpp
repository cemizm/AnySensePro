/*
 * StorageSDSPI.cpp
 *
 *  Created on: 28.09.2015
 *      Author: cem
 */

#include <StorageSDSPI.h>

#include <TimeoutTimer.h>
#include <Stopwatch.h>

extern uint8_t PrioritySD;

namespace Storage
{

uint8_t StorageSDSPI::GetStatus()
{
	return m_status;
}

uint8_t StorageSDSPI::Init()
{
	uint8_t n, ocr[4];
	uint8_t ct = CardType::Unknown;
	uint8_t cmd = CMD0;

	if (!m_hwInit)
		initHardware();

	setSpeed(0);

	while (!m_spi.GetTXEmpty())
		;
	m_spi.Xfer8(0xFF);

	OSAL::Timer::SleepMS(10);

	if (!detect())
		return StorageStatus::NoDisk;

	for (n = 10; n; n--)
		m_spi.Xfer8(0xFF);

	if (sendCommand(CMD0, 0) == 1)
	{
		Utils::TimeoutTimer t(delay_sec(1));

		if (sendCommand(CMD8, 0x1AA) == 1)
		{
			for (n = 0; n < 4; n++)
				ocr[n] = m_spi.Xfer8(0xFF);

			if (ocr[2] == 0x01 && ocr[3] == 0xAA)
			{
				while (!t.IsTimeout() && sendCommand(ACMD41, 1UL << 30))
					OSAL::Timer::SleepMS(100);

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

	if (m_cardType != CardType::Unknown)
	{
		m_status &= ~StorageStatus::NoInit;
		m_status &= ~StorageStatus::WriteProtect;

		setSpeed(1);
	}

	return m_status;
}

StorageResult StorageSDSPI::Read(uint8_t* buff, uint32_t sector, uint16_t count)
{
	if (!detect() || (m_status & StorageStatus::NoInit))
		return StorageResult::NotReady;

	if (!(m_cardType & CardType::Block))
		sector *= 512; /* LBA ot BA conversion (byte addressing cards) */

	if (count == 1)
	{
		if ((sendCommand(CMD17, sector) == 0) && readBlock(buff, 512))
			count = 0;
	}
	else
	{
		if (sendCommand(CMD18, sector) == 0)
		{
			do
			{
				if (!readBlock(buff, 512))
					break;

				buff += 512;
			} while (--count);

			sendCommand(CMD12, 0); /* STOP_TRANSMISSION */
		}
	}
	deselect();

	return count ? StorageResult::Error : StorageResult::OK;
}

StorageResult StorageSDSPI::Write(const uint8_t* buff, uint32_t sector, uint16_t count)
{
	if (!detect() || (m_status & StorageStatus::NoInit))
		return StorageResult::NotReady;

	if (!(m_cardType & CardType::Block))
		sector *= 512; /* LBA ot BA conversion (byte addressing cards) */

	if (count == 1)
	{
		if ((sendCommand(CMD24, sector) == 0) && writeBlock(buff, 0xFE))
			count = 0;
	}
	else
	{ /* Multiple sector write */
		if (m_cardType & CardType::SDC)
			sendCommand(ACMD23, count); /* Predefine number of sectors */
		if (sendCommand(CMD25, sector) == 0)
		{ /* WRITE_MULTIPLE_BLOCK */
			do
			{
				if (!writeBlock(buff, 0xFC))
					break;

				buff += 512;
			} while (--count);

			if (!writeBlock(0, 0xFD))
				count = 1;
		}
	}
	deselect();

	return count ? StorageResult::Error : StorageResult::OK;
}

StorageResult StorageSDSPI::IOCtl(StorageCommand cmd, void* buff)
{
	StorageResult res;
	uint8_t n, csd[16], *ptr = (uint8_t*) buff;
	uint16_t csize;

	res = StorageResult::Error;

	if (m_status & StorageStatus::NoInit)
		return StorageResult::NotReady;

	switch (cmd)
	{
	case StorageCommand::Sync: /* Make sure that no pending write process */
		if (select())
			res = StorageResult::OK;
		break;
	case StorageCommand::GetSectorCount: /* Get number of sectors on the disk (DWORD) */
		if ((sendCommand(CMD9, 0) == 0) && readBlock(csd, 16))
		{
			if ((csd[0] >> 6) == 1)
			{ /* SDC version 2.00 */
				csize = csd[9] + ((uint16_t) csd[8] << 8) + 1;
				*(uint32_t*) buff = (uint32_t) csize << 10;
			}
			else
			{ /* SDC version 1.XX or MMC*/
				n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
				csize = (csd[8] >> 6) + ((uint16_t) csd[7] << 2) + ((uint16_t) (csd[6] & 3) << 10) + 1;
				*(uint32_t*) buff = (uint32_t) csize << (n - 9);
			}
			res = StorageResult::OK;
		}
		break;
	case StorageCommand::GetSectorSize: /* Get R/W sector size (WORD) */
		*(uint32_t*) buff = 512;
		res = StorageResult::OK;
		break;

	case Storage::GetBlockSize: /* Get erase block size in unit of sector (DWORD) */
		if (m_cardType & CardType::SD2)
		{ /* SDC version 2.00 */
			if (sendCommand(ACMD13, 0) == 0)
			{ /* Read SD status */
				m_spi.Xfer8(0xFF);
				if (readBlock(csd, 16))
				{ /* Read partial block */
					for (n = 64 - 16; n; n--)
						m_spi.Xfer8(0xFF); /* Purge trailing data */
					*(uint32_t*) buff = 16UL << (csd[10] >> 4);
					res = StorageResult::OK;
				}
			}
		}
		else
		{ /* SDC version 1.XX or MMC */
			if ((sendCommand(CMD9, 0) == 0) && readBlock(csd, 16))
			{ /* Read CSD */
				if (m_cardType & CardType::SD1)
				{ /* SDC version 1.XX */
					*(uint32_t*) buff = (((csd[10] & 63) << 1) + ((uint16_t) (csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
				}
				else
				{ /* MMC */
					*(uint32_t*) buff = ((uint16_t) ((csd[10] & 124) >> 2) + 1)
							* (((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
				}
				res = StorageResult::OK;
			}
		}
		break;

	case StorageCommand::MMCGetType: /* Get card type flags (1 byte) */
		*ptr = m_cardType;
		res = StorageResult::OK;
		break;

	case StorageCommand::MMCGetCSD: /* Receive CSD as a data block (16 bytes) */
		if (sendCommand(CMD9, 0) == 0 && readBlock(ptr, 16))
			res = StorageResult::OK;
		break;

	case StorageCommand::MMCGetCID: /* Receive CID as a data block (16 bytes) */
		if (sendCommand(CMD10, 0) == 0 && readBlock(ptr, 16))
			res = StorageResult::OK;
		break;

	case StorageCommand::MMCGetOCR: /* Receive OCR as an R3 resp (4 bytes) */
		if (sendCommand(CMD58, 0) == 0)
		{ /* READ_OCR */
			for (n = 4; n; n--)
				*ptr++ = m_spi.Xfer8(0xFF);
			res = StorageResult::OK;
		}
		break;

	case StorageCommand::MMCGetSDStatus: /* Receive SD status as a data block (64 bytes) */
		if (sendCommand(ACMD13, 0) == 0)
		{ /* SD_STATUS */
			m_spi.Xfer8(0xFF);
			if (readBlock(ptr, 64))
				res = StorageResult::OK;
		}
		break;

	default:
		res = StorageResult::InvalidParameter;
	}

	deselect();

	return res;
}

void StorageSDSPI::initHardware()
{
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

	m_spi.SetupRXDMA();
	m_spi.SetupTXDMA();

	HAL::InterruptRegistry.Enable(m_spi.GetRXDMA().NVIC_IRQn, PrioritySD, this);
	//HAL::InterruptRegistry.Enable(m_spi.GetTXDMA().NVIC_IRQn, 15, this);

	m_cd.PowerUp();
	m_cd.ModeSetup(GPIO_MODE_INPUT, GPIO_PUPD_PULLUP);

	m_cd.EXTI_ResetPendingBit();
	m_cd.EXTI_SelectSource();
	m_cd.EXTI_SetTrigger(EXTI_TRIGGER_BOTH);
	m_cd.EXTI_Enable();

	HAL::InterruptRegistry.Enable(m_cd.EXTI_NVIC_IRQ, PrioritySD, this);

	m_spi.Enable();

	m_hwInit = 1;
}

uint8_t StorageSDSPI::detect()
{
	return m_cd.Get() ? 0 : 1;
}

uint8_t StorageSDSPI::waitReady(uint16_t ms)
{
	uint8_t d = 0x00;
	Utils::TimeoutTimer t(delay_ms(ms));

	do
	{
		d = m_spi.Xfer8(0xFF);
	} while (d != 0xFF && !t.IsTimeout());

	return d == 0xFF ? 1 : 0;
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

	if (waitReady(1000))
		return 1;

	deselect();
	return 0;
}

void StorageSDSPI::setSpeed(uint8_t high)
{
	if (high)
		m_spi.SetBaudratePrescaler(SPI_CR1_BR_FPCLK_DIV_2);
	else
		m_spi.SetBaudratePrescaler(SPI_CR1_BR_FPCLK_DIV_256);
}

uint8_t StorageSDSPI::sendCommand(uint8_t cmd, uint32_t params)
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

	n = 10;
	do
	{
		res = m_spi.Xfer8(0xFF);
	} while ((res & 0x80) && --n);

	return res;
}

uint8_t StorageSDSPI::readBlock(uint8_t* buff, uint16_t btr)
{
	uint8_t token = 0;

	Utils::TimeoutTimer t(delay_ms(1000));

	while (!t.IsTimeout())
	{
		token = m_spi.Xfer8(0xFF);
		if (token != 0xFF)
			break;
	}

	if (token != 0xFE)
		return 0;

//now read data via DMA

	m_spi.InitDMATransfer(HAL::SPIDMADirection::FromDevice, (uint32_t) m_workbyte, (uint32_t) buff, btr);

	m_dmaRXFinished.clear();

	m_spi.EnableRXDma();
	m_spi.EnableTXDma();

	if (!m_dmaRXFinished.wait())
	{
		m_spi.DisableClearAll();
		return 0;
	}
	m_spi.DisableClearAll();

	m_spi.Xfer8(0xFF);
	m_spi.Xfer8(0xFF);

	return 1;
}

uint8_t StorageSDSPI::writeBlock(const uint8_t* buff, uint16_t token)
{
	uint8_t resp;

	if (!waitReady(1000))/* Wait for card ready */
		return 0;

	m_spi.Xfer8(token);

	if (token != 0xFD)
	{
		/* now write data via DMA */

		m_spi.InitDMATransfer(HAL::SPIDMADirection::ToDevice, (uint32_t) buff, (uint32_t) m_workbyte, 512);

		m_dmaRXFinished.clear();

		m_spi.EnableRXDma();
		m_spi.EnableTXDma();

		if (!m_dmaRXFinished.wait())
		{
			m_spi.DisableClearAll();
			return 0;
		}
		m_spi.DisableClearAll();

		m_spi.Xfer8(0xFF);
		m_spi.Xfer8(0xFF); /* Dummy CRC */

		resp = m_spi.Xfer8(0xFF); /* Receive data resp */
		if ((resp & 0x1F) != 0x05) /* Function fails if the data packet was not accepted */
			return 0;
	}

	return 1;
}

void StorageSDSPI::ISR()
{
	if (m_cd.EXTI_IsPendingBit())
	{
		m_cd.EXTI_ResetPendingBit();

		if (!detect())
			m_status |= (StorageStatus::NoDisk | StorageStatus::NoInit);
		else
			m_status &= ~StorageStatus::NoDisk;

		m_cardType = CardType::Unknown;
	}
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

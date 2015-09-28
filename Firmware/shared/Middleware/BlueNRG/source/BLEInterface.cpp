/*
 * BLEInterface.cpp
 *
 *  Created on: 12.09.2015
 *      Author: cem
 */

#include <BLEInterface.h>

#include "OSAL.h"
#include <string.h>

namespace BlueNRG
{

RUNLOOP void BLEInterface::Run()
{
	for (;;)
	{
		gotWorkFlag.wait(delay_us(m_Timeout));
		if (m_irq.Get())
		{
			transaction.MasterHeader.State = HCISPIState::Data_Receive;
			XFerData();
			m_Timeout = ACTIVEIRQTIMEOUT;
		}
		else if (!tx_queue.IsEmpty())
		{
			TXQueueItem* item = tx_queue.Peek();

			transaction.MasterHeader.State = HCISPIState::Data_Transmit;
			transaction.Size = item->size;

			memcpy(transaction.MasterPayload, item->data, item->size);

			XFerData();

			m_Timeout = INACTIVEIRQTIMEOUT;
		}
	}
}

BLETransactionStatus BLEInterface::XFerData()
{
	//m_irq.ModeSetup(GPIO_MODE_OUTPUT, GPIO_PUPD_NONE);
	//m_irq.On();

	HAL::OSAL::Sleep(1);

	HAL::SPISelect cns(m_spi, m_csn);

	const HAL::DMA& RX = m_spi.GetRXDMA();
	const HAL::DMA& TX = m_spi.GetTXDMA();

	m_spi.DisableClearAll();

	TX.SetMemoryAddress((uint32_t) &transaction.MasterHeader);
	RX.SetMemoryAddress((uint32_t) &transaction.SlaveHeader);

	RX.EnableTransferCompleteInterrupt();

	TX.SetNumerOfData(sizeof(transaction.MasterHeader));
	RX.SetNumerOfData(sizeof(transaction.SlaveHeader));

	dmaDoneFlag.clear();

	RX.EnableChannel();
	TX.EnableChannel();

	m_spi.EnableRXDma();
	m_spi.EnableTXDma();

	if (!dmaDoneFlag.wait(delay_ms(10)))
	{
		m_irq.ModeSetup(GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN);
		return BLETransactionStatus::Timeout;
	}

	//m_irq.ModeSetup(GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN);

	m_spi.DisableClearAll();

	if (transaction.SlaveHeader.State != HCISPIState::HCI_OK)
		return BLETransactionStatus::DeviceNotReady;

	if (transaction.MasterHeader.State == HCISPIState::Data_Receive)
	{
		if (transaction.SlaveHeader.ReceiveBuffer == 0)
			return BLETransactionStatus::DeviceNotReady;

		transaction.Size = transaction.SlaveHeader.ReceiveBuffer;
	}
	else
	{
		if (transaction.SlaveHeader.TransmitBuffer == 0)
			return BLETransactionStatus::DeviceNotReady;
	}

	//TODO: Split RX/TX Packets to fit buffer

	TX.SetMemoryAddress((uint32_t) transaction.MasterPayload);
	RX.SetMemoryAddress((uint32_t) transaction.SlavePayload);

	RX.EnableTransferCompleteInterrupt();

	TX.SetNumerOfData(transaction.Size);
	RX.SetNumerOfData(transaction.Size);

	dmaDoneFlag.clear();

	RX.EnableChannel();
	TX.EnableChannel();

	m_spi.EnableRXDma();
	m_spi.EnableTXDma();

	if (!dmaDoneFlag.wait(delay_ms(10)))
		return BLETransactionStatus::Timeout;

	if (transaction.MasterHeader.State == HCISPIState::Data_Receive)
	{
		if (m_handlers.dataHandler != NULL)
		{
			m_handlers.dataHandler->OnDataReceived(transaction.SlavePayload, transaction.Size);
		}
	}
	else
	{
		tx_queue.Dequeue();
	}

	return BLETransactionStatus::TransactionDone;
}

void BLEInterface::Init()
{
	m_csn.On();
	m_csn.PowerUp();
	m_csn.ModeSetup(GPIO_MODE_OUTPUT, GPIO_PUPD_NONE);
	m_csn.SetOutputOptions(GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ);

	m_irq.PowerUp();
	m_irq.ModeSetup(GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN);

	m_rstn.Off();
	m_rstn.PowerUp();
	m_rstn.ModeSetup(GPIO_MODE_OUTPUT, GPIO_PUPD_NONE);
	m_rstn.SetOutputOptions(GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ);

	m_spi.Init(SPI_CR1_BAUDRATE_FPCLK_DIV_32, SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE, SPI_CR1_CPHA_CLK_TRANSITION_1, SPI_CR2_DS_8BIT,
	SPI_CR1_MSBFIRST);

	m_spi.SetUniDirectionalMode();
	m_spi.SetFullDuplexMode();

	m_spi.EnableSoftwareSlaveManagement();
	m_spi.DisableCRC();
	m_spi.SetFifoReceptionThreshold8Bit();

	rcc_periph_clock_enable(rcc_periph_clken::RCC_SYSCFG);

	m_irq.EXTI_ResetPendingBit();
	m_irq.EXTI_SelectSource();
	m_irq.EXTI_SetTrigger(EXTI_TRIGGER_RISING);
	m_irq.EXTI_Enable();

	m_spi.SetupRXDMA();
	m_spi.SetupTXDMA();

	HAL::InterruptRegistry.Enable(m_irq.EXTI_NVIC_IRQ, 15, this);

	HAL::InterruptRegistry.Enable(m_spi.GetRXDMA().NVIC_IRQn, 15, this);
	HAL::InterruptRegistry.Enable(m_spi.GetTXDMA().NVIC_IRQn, 15, this);

}

void BLEInterface::Enable()
{
	m_spi.Enable();
	Reset();
}

void BLEInterface::Reset()
{
	m_rstn.Off();
	HAL::OSAL::Sleep(5);
	m_rstn.On();
	HAL::OSAL::Sleep(5);
}

void BLEInterface::Disable()
{
	m_rstn.Off();
	m_spi.Disable();
}

void BLEInterface::ISR()
{
	HAL::OSALISRSupport support;

	if (m_irq.EXTI_IsPendingBit())
	{
		m_irq.EXTI_ResetPendingBit();
		gotWorkFlag.signal_isr();
	}
	if (m_spi.GetRXDMA().GetInterruptFlag(DMA_TCIF))
	{
		m_spi.GetRXDMA().ClearInterruptFlags(DMA_TCIF);
		dmaDoneFlag.signal_isr();
	}
	if (m_spi.GetTXDMA().GetInterruptFlag(DMA_TCIF))
	{
		m_spi.GetTXDMA().ClearInterruptFlags(DMA_TCIF);
		dmaDoneFlag.signal_isr();
	}
}

void BLEInterface::Send(uint8_t* data, uint16_t size)
{
	TXQueueItem* item = tx_queue.Enqueue();
	if (item != NULL)
	{
		item->size = size;
		memcpy(item->data, data, size);
	}

}

} /* namespace BlueNRG */

/*
 * MAVLinkComm.cpp
 *
 *  Created on: 28.10.2015
 *      Author: cem
 */

#include <MAVLinkComm.h>
#include "Interrupt.h"

extern uint8_t PriorityMavlink;

namespace App
{

void MAVLinkComm::Init(void)
{
	m_usart.Init(GPIO_PUPD_PULLDOWN);
	m_usart.SetBaudrate(57600);
	m_usart.SetStopbits(USART_STOPBITS_1);
	m_usart.SetDatabits(8);
	m_usart.SetFlowControl(USART_FLOWCONTROL_NONE);
	m_usart.SetParity(USART_PARITY_NONE);
	m_usart.SetMode(USART_MODE_TX_RX);
	m_usart.DisableOverrunDetection();

	HAL::InterruptRegistry.Enable(m_usart.NVIC_IRQn, PriorityMavlink, this);
	HAL::InterruptRegistry.Enable(m_usart.GetTXDMA().NVIC_IRQn, PriorityMavlink, this);

	m_usart.SetupTXDMA();
	m_usart.EnableRxInterrupt();
	m_usart.Enable();
}

void MAVLinkComm::DeInit(void)
{
	m_usart.ClearTXDma();
	m_usart.DisableRxInterrupt();
	m_usart.Disable();

	HAL::InterruptRegistry.Disable(m_usart.NVIC_IRQn);
	HAL::InterruptRegistry.Disable(m_usart.GetTXDMA().NVIC_IRQn);

	m_usart.DeInit();
}

void MAVLinkComm::Run(void)
{
	for (;;)
		loop();
}

void MAVLinkComm::loop()
{
	uint16_t dataLen = 0;

	if (gotMsg.wait(delay_ms(MAVLINK_COMM_DELAY_MS)))
	{
		//handle message??
	}
	else if (!m_isSending)
	{
		switch (m_currentGroup)
		{
		case 0:
		{
			switch (m_currentItem[0])
			{
			case 0:
				dataLen = m_mavlink.PackAttitude(&m_msg_work);
				break;
			case 1:
				dataLen = m_mavlink.PackVFRHud(&m_msg_work);
				break;
			case 2:
				dataLen = m_mavlink.PackSystemTime(&m_msg_work);
				break;
			case 3:
				dataLen = m_mavlink.PackRCOut(&m_msg_work);
				break;
			}

			m_currentItem[0] = (m_currentItem[0] + 1) % 4;
		}
			break;
		case 1:
		{
			switch (m_currentItem[1])
			{
			case 0:
				dataLen = m_mavlink.PackGPS(&m_msg_work);
				break;
			case 1:
				dataLen = m_mavlink.PackGPSInt(&m_msg_work);
				break;
			case 2:
				dataLen = m_mavlink.PackBatteryPack(&m_msg_work);
				break;
			case 3:
				dataLen = m_mavlink.PackHeartbeat(&m_msg_work);
				break;
			case 4:
				dataLen = m_mavlink.PackSystemStatus(&m_msg_work);
				break;
			}

			m_currentItem[1] = (m_currentItem[1] + 1) % 5;
		}
			break;
		case 2:
		{
			switch (m_currentItem[2])
			{
			case 0:
				dataLen = m_mavlink.PackSystemTime(&m_msg_work);
				break;
			case 1:
				dataLen = m_mavlink.PackAttitude(&m_msg_work);
				break;
			}

			m_currentItem[2] = (m_currentItem[2] + 1) % 2;
		}
			break;
		}
		m_currentGroup = (m_currentGroup + 1) % MAX_GROUPS;
	}

	if (dataLen > 0)
	{
		SendMessage();
		dataLen = 0;
	}
}

uint8_t MAVLinkComm::SendMessage()
{
	if (m_isSending == 1)
		return 0;
	m_isSending = 1;

	uint16_t dataLen = m_mavlink.FillBytes(&m_msg_work, m_msg_buffer);

	m_usart.SendDma(m_msg_buffer, dataLen);

	return 1;
}

void MAVLinkComm::ISR(void)
{
	if (m_usart.GetInterruptSource(USART_ISR_RXNE))
	{
		if (m_mavlink.Decode(m_usart.Receive(), &m_msg_in))
			gotMsg.signal_isr();
	}
	if (m_usart.GetTXDMA().GetInterruptFlag(DMA_TCIF))
	{
		m_usart.GetTXDMA().ClearInterruptFlags(DMA_TCIF);
		m_isSending = 0;
	}
}

} /* namespace HAL */

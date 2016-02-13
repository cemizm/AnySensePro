/*
 * MAVLinkComm.cpp
 *
 *  Created on: 28.10.2015
 *      Author: cem
 */

#include <MAVLinkComm.h>
#include "Interrupt.h"

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

	HAL::InterruptRegistry.Enable(m_usart.NVIC_IRQn, 15, this);
	HAL::InterruptRegistry.Enable(m_usart.GetTXDMA().NVIC_IRQn, 15, this);

	m_usart.SetupTXDMA();
	m_usart.EnableRxInterrupt();
	m_usart.Enable();
}

void MAVLinkComm::DeInit(void)
{

}

void MAVLinkComm::Run(void)
{
	uint16_t dataLen = 0;
	uint8_t currentItem = 0;

	for (;;)
	{
		if (m_messages_in.pop(m_msg_work, delay_ms(MAVLINK_COMM_DELAY_MS)))
		{
		}
		else
		{
			switch (currentItem)
			{
			case 0:
				dataLen = m_mavlink.PackGPS(&m_msg_work);
				break;
			case 1:
				dataLen = m_mavlink.PackAttitude(&m_msg_work);
				break;
			case 2:
				dataLen = m_mavlink.PackBatteryPack(&m_msg_work);
				break;
			case 3:
				dataLen = m_mavlink.PackHeartbeat(&m_msg_work);
				break;
			case 4:
				dataLen = m_mavlink.PackRCOut(&m_msg_work);
				break;
			case 5:
				dataLen = m_mavlink.PackSystemStatus(&m_msg_work);
				break;
			case 6:
				dataLen = m_mavlink.PackVFRHud(&m_msg_work);
				break;
			}

			currentItem = (currentItem + 1) % 7;
		}

		if (dataLen > 0)
		{
			m_messages_out.push(m_msg_work);
			if (!m_isSending)
				SendMessage();
			dataLen = 0;
		}
	}
}

uint8_t MAVLinkComm::SendMessage()
{
	if (m_messages_out.get_count() == 0)
		return 0;

	if (m_messages_out.pop(m_msg_out, 0))
	{
		uint16_t dataLen = m_mavlink.FillBytes(&m_msg_out, m_msg_buffer);

		m_usart.SendDma((uint32_t) m_msg_buffer, dataLen);

		m_isSending = 1;
		return 1;
	}

	return 0;
}

void MAVLinkComm::ISR(void)
{
	if (m_usart.GetInterruptSource(USART_ISR_RXNE))
	{
		if (m_mavlink.Decode(m_usart.Receive(), &m_msg_in))
		{
			if (m_messages_in.get_free_size() > 0)
				m_messages_in.push(m_msg_in);
		}
	}
	if (m_usart.GetTXDMA().GetInterruptFlag(DMA_TCIF))
	{
		m_usart.GetTXDMA().ClearInterruptFlags(DMA_TCIF);
		if (!SendMessage())
		{
			m_usart.GetTXDMA().DisableChannel();

			m_isSending = 0;
		}
	}
}

} /* namespace HAL */

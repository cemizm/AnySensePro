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
	m_usart.DisableOverrunDetection();

	HAL::InterruptRegistry.Enable(m_usart.NVIC_IRQn, 15, this);
	HAL::InterruptRegistry.Enable(m_usart.GetTXDMA().NVIC_IRQn, 15, this);

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
	{
		m_messages_in.pop(m_msg_work, delay_ms(MAVLINK_COMM_DELAY_MS));
		loop();
	}
}

void MAVLinkComm::loop()
{
	uint16_t dataLen = 0;

	if (m_messages_in.get_count() > 0)
	{
		m_messages_in.pop(m_msg_work);
		//handle message??
	}
	else if (m_messages_out.get_free_size() > 0)
		dataLen = PackNextMessage();

	if (dataLen > 0)
	{
		m_messages_out.push(m_msg_work);
		SendMessage();
		dataLen = 0;
	}
}

uint16_t MAVLinkComm::PackNextMessage()
{
	uint16_t dataLen = 0;
	switch (m_currentItem)
	{
	case 0:
		dataLen = m_mavlink.PackGPS(&m_msg_work);
		break;
	case 1:
	case 4:
	case 8:
		dataLen = m_mavlink.PackAttitude(&m_msg_work);
		break;
	case 2:
		dataLen = m_mavlink.PackVFRHud(&m_msg_work);
		break;
	case 3:
	case 7:
		dataLen = m_mavlink.PackRCOut(&m_msg_work);
		break;
	case 5:
		dataLen = m_mavlink.PackBatteryPack(&m_msg_work);
		break;
	case 6:
		dataLen = m_mavlink.PackHeartbeat(&m_msg_work);
		break;
	case 9:
		dataLen = m_mavlink.PackSystemStatus(&m_msg_work);
		break;
	}
	m_currentItem = (m_currentItem + 1) % 10;
	return dataLen;
}

uint8_t MAVLinkComm::SendMessage()
{
	if(m_isSending == 1)
		return 0;

	if (m_messages_out.get_count() == 0)
		return 0;

	if (m_messages_out.pop(m_msg_out, 0))
	{
		uint16_t dataLen = m_mavlink.FillBytes(&m_msg_out, m_msg_buffer);

		m_isSending = 1;
		m_usart.SendDma(m_msg_buffer, dataLen);

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
		m_isSending = 0;
		if (!SendMessage())
		{
			m_usart.GetTXDMA().DisableChannel();
		}
	}
}

} /* namespace HAL */

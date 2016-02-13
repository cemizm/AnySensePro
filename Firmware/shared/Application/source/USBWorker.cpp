/*
 * USBWorker.cpp
 *
 *  Created on: 17.10.2015
 *      Author: cem
 */

#include <USBWorker.h>
#include <stddef.h>

namespace App
{

void USBWorker::Run()
{
	uint16_t dataLen = 0;
	uint8_t currentItem = 0;

	for (;;)
	{
		if (m_channels.pop(m_msg, delay_ms(m_delay_ms)))
		{
			//process
			m_delay_ms = USB_WORKER_DELAY_CONNECTED;
		}
		else if ( m_CDC.IsConnected())
		{
			switch (currentItem)
			{
			case 0:
				dataLen = m_mavlink.PackGPS(&m_msg);
				break;
			case 1:
				dataLen = m_mavlink.PackAttitude(&m_msg);
				break;
			case 2:
				dataLen = m_mavlink.PackBatteryPack(&m_msg);
				break;
			case 3:
				dataLen = m_mavlink.PackHeartbeat(&m_msg);
				break;
			case 4:
				dataLen = m_mavlink.PackRCOut(&m_msg);
				break;
			case 5:
				dataLen = m_mavlink.PackSystemStatus(&m_msg);
				break;
			case 6:
				dataLen = m_mavlink.PackVFRHud(&m_msg);
				break;
			}

			currentItem = (currentItem + 1) % 7;
		}

		if (dataLen > 0)
		{
			dataLen = m_mavlink.FillBytes(&m_msg, m_buffer);
			m_CDC.SendData(m_buffer, dataLen);
			dataLen = 0;
		}
	}
}

void USBWorker::DeviceConnected()
{
	m_delay_ms = USB_WORKER_DELAY_CONNECTED;
}
void USBWorker::DeviceDisconnected()
{
	m_delay_ms = USB_WORKER_DELAY_DISCONNECTED;
}

void USBWorker::DataRX(uint8_t* data, uint8_t len)
{
	for (uint8_t i = 0; i < len; i++)
	{

		if (m_mavlink.Decode(data[i], &m_msg_rcv))
		{
			if (m_channels.get_free_size() > 0)
				m_channels.push(m_msg_rcv);
		}
	}
}

}
/* namespace Utils */

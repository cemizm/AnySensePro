/*
 * USBWorker.h
 *
 *  Created on: 17.10.2015
 *      Author: cem
 */

#ifndef APPLICATION_INCLUDE_USBWORKER_H_
#define APPLICATION_INCLUDE_USBWORKER_H_

#include <MAVLinkLayer.h>
#include <OSAL.h>
#include <USBCDCInterface.h>

#include <Pool.h>
#include <Queue.h>

#define USB_WORKER_DELAY_CONNECTED		50
#define USB_WORKER_DELAY_DISCONNECTED	1000
#define USB_WORKER_MESSAGES				5
#define USB_WORKER_BUFFER_SIZE			255

namespace App
{

class USBWorker: public USB::USBSerialHandler
{
private:
	USB::USBCDCInterface& m_CDC;
	OSAL::Channel<mavlink_message_t, USB_WORKER_MESSAGES> m_channels;
	MAVLinkLayer m_mavlink;
	uint16_t m_delay_ms;
	mavlink_message_t m_msg;
	mavlink_message_t m_msg_rcv;
	uint8_t m_buffer[USB_WORKER_BUFFER_SIZE];

public:
	USBWorker(USB::USBCDCInterface& cdc) :
			m_CDC(cdc), m_channels(), m_mavlink(MAVLINK_COMM_0), m_delay_ms(USB_WORKER_DELAY_CONNECTED), m_buffer()
	{
		m_CDC.RegisterHandler(this);
	}

	void Run();

	void Connected() override;
	void Disconnected() override;
	void DataRX(uint8_t* data, uint8_t len) override;
};

} /* namespace Utils */

#endif /* APPLICATION_INCLUDE_USBWORKER_H_ */
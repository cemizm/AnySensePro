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
#include <USBCDCDevice.h>
#include <System.h>

#include <Pool.h>
#include <Queue.h>

#define USB_WORKER_DELAY_CONNECTED		10
#define USB_WORKER_DELAY_DISCONNECTED	1000
#define USB_WORKER_MESSAGES				3

namespace App
{

class USBWorker: public USB::USBSerialHandler
{
private:
	USB::USBCDCDevice& m_CDC;
	OSAL::Channel<mavlink_message_t, USB_WORKER_MESSAGES> m_channels;
	MAVLinkLayer m_mavlink;
	uint16_t m_delay_ms;
	mavlink_message_t m_msg;
	mavlink_message_t m_msg_rcv;
	uint8_t m_buffer[MAVLINK_MAX_PACKET_LEN];
	uint8_t m_data_buffer[MAVLINK_MAX_PACKET_LEN];
	uint8_t established;
	LogEntry logEntry;

	void ReceiveUpdate(uint32_t size);

	void SendAck(MAV_CMD_ACK ack);

	enum TransferType
	{
		FW_Update = 0,
	};

public:
	USBWorker(USB::USBCDCDevice& cdc) :
			m_CDC(cdc), m_channels(), m_mavlink(MAVLINK_COMM_0), m_delay_ms(USB_WORKER_DELAY_CONNECTED), m_buffer(), established(0)
	{
		m_CDC.RegisterHandler(this);
	}

	void Init();
	void Run();

	void DeviceConnected() override;
	void DeviceDisconnected() override;
	void DataRX(uint8_t* data, uint8_t len) override;
};

} /* namespace Utils */

#endif /* APPLICATION_INCLUDE_USBWORKER_H_ */

/*
 * MAVLinkComm.h
 *
 *  Created on: 28.10.2015
 *      Author: cem
 */

#ifndef APPLICATION_INCLUDE_MAVLINKCOMM_H_
#define APPLICATION_INCLUDE_MAVLINKCOMM_H_

#include <USART.h>
#include <OSAL.h>
#include <Interrupt.h>

#include "MAVLinkLayer.h"

namespace App
{

const uint8_t MAVLINK_COMM_MESSAGES_IN = 3;
const uint8_t MAVLINK_COMM_MESSAGES_OUT = 3;
const uint16_t MAVLINK_COMM_DELAY_MS = 50;
//const uint8_t MAVLINK_COMM_DELAY_MS_CONNECTED = 100;

class MAVLinkComm: public HAL::InterruptHandler
{
private:
	HAL::USART& m_usart;
	OSAL::Channel<mavlink_message_t, MAVLINK_COMM_MESSAGES_IN> m_messages_in;
	OSAL::Channel<mavlink_message_t, MAVLINK_COMM_MESSAGES_OUT> m_messages_out;
	MAVLinkLayer m_mavlink;
	uint16_t m_delay_ms;
	mavlink_message_t m_msg_in;
	uint8_t m_msg_buffer[MAVLINK_MAX_PACKET_LEN];
	mavlink_message_t m_msg_out;
	mavlink_message_t m_msg_work;

	uint8_t m_isSending;
	uint8_t m_currentItem;

	uint8_t SendMessage();
	uint16_t PackNextMessage();

public:
	MAVLinkComm(HAL::USART& usart) :
			m_usart(usart), m_messages_in(), m_messages_out(), m_mavlink(MAVLINK_COMM_1), m_delay_ms(MAVLINK_COMM_DELAY_MS), m_msg_in(), m_msg_buffer(), m_msg_out(), m_msg_work(), m_isSending(
					0), m_currentItem(0)
	{

	}

	void loop(void);

	void Init(void);
	void DeInit(void);
	void Run(void);
	void ISR(void) override;
};

} /* namespace HAL */

#endif /* APPLICATION_INCLUDE_MAVLINKCOMM_H_ */

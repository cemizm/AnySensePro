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

const uint16_t MAVLINK_COMM_DELAY_MS = 50;
const uint8_t MAX_GROUPS = 3;

class MAVLinkComm: public HAL::InterruptHandler
{
protected:
	HAL::USART& m_usart;
	MAVLinkLayer m_mavlink;
	mavlink_message_t m_msg_in;
	mavlink_message_t m_msg_work;
	OSAL::EventFlag gotMsg;

	uint8_t m_msg_buffer[MAVLINK_MAX_PACKET_LEN];

	uint8_t m_isSending;
	uint8_t m_currentGroup;
	uint8_t m_currentItem[MAX_GROUPS];

	uint8_t SendMessage();
	uint16_t PackNextMessage();

public:
	MAVLinkComm(HAL::USART& usart, mavlink_channel_t channel) :
			m_usart(usart), m_mavlink(channel), m_msg_in(), m_msg_work(), gotMsg(), m_msg_buffer(), m_isSending(0), m_currentGroup(
					0), m_currentItem()
	{

	}

	void loop(void);

	void Init(void);
	void DeInit(void);
	virtual void Run(void);
	void ISR(void) override;
};

} /* namespace HAL */

#endif /* APPLICATION_INCLUDE_MAVLINKCOMM_H_ */

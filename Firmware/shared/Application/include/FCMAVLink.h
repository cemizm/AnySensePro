/*
 * FCMAVLink.h
 *
 *  Created on: 15.03.2016
 *      Author: cem
 */

#ifndef APPLICATION_INCLUDE_FCMAVLINK_H_
#define APPLICATION_INCLUDE_FCMAVLINK_H_

#include <FCAdapter.h>
#include "USART.h"
#include "MAVLinkComm.h"
#include "Interrupt.h"

namespace App
{

class FCMAVLink: public FCAdapter, public MAVLinkComm
{
private:
	static const uint8_t MaxMAVStreams = 6;
	const uint8_t MAVStreams[MaxMAVStreams] = { MAV_DATA_STREAM_RAW_SENSORS, MAV_DATA_STREAM_EXTENDED_STATUS,
			MAV_DATA_STREAM_RC_CHANNELS, MAV_DATA_STREAM_POSITION, MAV_DATA_STREAM_EXTRA1, MAV_DATA_STREAM_EXTRA2 };

	const uint16_t MAVRates[MaxMAVStreams] = { 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A };

	void UpdateHeartbeat(mavlink_message_t& msg);
	void UpdateSysStatus(mavlink_message_t& msg);
	void UpdateGPSRaw(mavlink_message_t& msg);
	void UpdateVFRHUD(mavlink_message_t& msg);
	void UpdateAttitude(mavlink_message_t& msg);
	void UpdateTime(mavlink_message_t& msg);
public:
	FCMAVLink(HAL::USART& usart) :
			FCAdapter(), MAVLinkComm(usart, MAVLINK_COMM_3)
	{
	}

	void Init() override;
	void Run() override;
};

} /* namespace App */

#endif /* APPLICATION_INCLUDE_FCMAVLINK_H_ */

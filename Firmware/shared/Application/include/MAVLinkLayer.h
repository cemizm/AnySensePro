/*
 * MAVLink.h
 *
 *  Created on: 18.10.2015
 *      Author: cem
 */

#ifndef APPLICATION_INCLUDE_MAVLINKLAYER_H_
#define APPLICATION_INCLUDE_MAVLINKLAYER_H_

#include <mavlink.h>

namespace App
{
class MAVLinkLayer
{
private:
	mavlink_channel_t m_channel;
	mavlink_status_t m_status;
public:
	MAVLinkLayer(mavlink_channel_t channel);
	uint8_t Decode(uint8_t data, mavlink_message_t* msg);

	uint16_t FillBytes(mavlink_message_t* msg, uint8_t* data);

	uint16_t PackHeartbeat(mavlink_message_t* msg);
	uint16_t PackSystemStatus(mavlink_message_t* msg);
	uint16_t PackGPS(mavlink_message_t* msg);
	uint16_t PackGPSInt(mavlink_message_t* msg);
	uint16_t PackVFRHud(mavlink_message_t* msg);
	uint16_t PackAttitude(mavlink_message_t* msg);
	uint16_t PackRCOut(mavlink_message_t* msg);
	uint16_t PackBatteryPack(mavlink_message_t* msg);
	uint16_t PackSystemTime(mavlink_message_t* msg);

	uint16_t PackCommandAck(mavlink_message_t* msg, MAV_CMD_ACK ack);
	uint16_t PackConfigurationVersion3(mavlink_message_t* msg, uint32_t firmware, uint8_t hardware);
	uint16_t PackLogData(mavlink_message_t* msg, uint8_t count, const uint8_t* data);
	uint16_t PackConfigurationData(mavlink_message_t* msg, const uint8_t* data);

	uint16_t PackMAVStreams(mavlink_message_t* msg, uint8_t stream, uint8_t rate);
};

} /* namespace Utils */

#endif /* APPLICATION_INCLUDE_MAVLINKLAYER_H_ */

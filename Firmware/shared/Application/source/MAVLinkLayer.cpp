/*
 * MAVLink.cpp
 *
 *  Created on: 18.10.2015
 *      Author: cem
 */

#include <MAVLinkLayer.h>
#include <SensorStore.h>

#include <stddef.h>

namespace App
{

#define MAVLINK_SYSTEM_ID						0xCE
#define MAVLINK_COMP_ID							MAV_COMP_ID_UART_BRIDGE
#define MAVLINK_TYPE							MAV_TYPE_QUADROTOR
#define MAVLINK_SENSORS							MAV_SYS_STATUS_SENSOR_3D_GYRO | MAV_SYS_STATUS_SENSOR_3D_ACCEL | \
												MAV_SYS_STATUS_SENSOR_3D_MAG | MAV_SYS_STATUS_SENSOR_ABSOLUTE_PRESSURE | \
												MAV_SYS_STATUS_SENSOR_GPS | MAV_SYS_STATUS_SENSOR_ATTITUDE_STABILIZATION | \
												MAV_SYS_STATUS_SENSOR_MOTOR_OUTPUTS | MAV_SYS_STATUS_SENSOR_RC_RECEIVER

//Manual = 0, GPS = 1, Failsafe = 2, Attitude = 3
uint8_t MAVLINK_MODE_MAP[] = { 1, 16, 6, 2 };
uint8_t MAVLINK_AP_MAP[] = { MAV_AUTOPILOT_INVALID, MAV_AUTOPILOT_NAZA, MAV_AUTOPILOT_PHANTOM, MAV_AUTOPILOT_WOOKONG,
		MAV_AUTOPILOT_A2, MAV_AUTOPILOT_PX4, MAV_AUTOPILOT_TAROT_ZYX_M };

MAVLinkLayer::MAVLinkLayer(mavlink_channel_t channel) :
		m_channel(channel), m_status()
{

}

uint8_t MAVLinkLayer::Decode(uint8_t data, mavlink_message_t* msg)
{
	return mavlink_parse_char(m_channel, data, msg, &m_status);
}
uint16_t MAVLinkLayer::PackHeartbeat(mavlink_message_t* msg)
{
	return mavlink_msg_heartbeat_pack(MAVLINK_SYSTEM_ID, MAVLINK_COMP_ID, msg, MAVLINK_TYPE,
			MAVLINK_AP_MAP[SensorData.GetFCType()], SensorData.GetArmed() ? 128 : 0, MAVLINK_MODE_MAP[SensorData.GetFlightMode()],
			SensorData.GetFlightMode() == FlightMode::Failsafe ? MAV_STATE_CRITICAL : MAV_STATE_ACTIVE);
}
uint16_t MAVLinkLayer::PackSystemStatus(mavlink_message_t* msg)
{
	return mavlink_msg_sys_status_pack(MAVLINK_SYSTEM_ID, MAVLINK_COMP_ID, msg, MAVLINK_SENSORS, MAVLINK_SENSORS, MAVLINK_SENSORS,
			0, SensorData.GetBattery(), SensorData.GetCurrent() * 100, SensorData.GetCharge(), 0, 0, 0, 0, 0, 0);
}
uint16_t MAVLinkLayer::PackGPS(mavlink_message_t* msg)
{
	const GPSPosition& gps = SensorData.GetPositionCurrent();

	return mavlink_msg_gps_raw_int_pack(MAVLINK_SYSTEM_ID, MAVLINK_COMP_ID, msg, 0, SensorData.GetFixType(),
			gps.Latitude * 10000000, gps.Longitude * 10000000, SensorData.GetRelativeAltitude() * 1000,
			SensorData.GetHdop() * 100, SensorData.GetVdop() * 100, SensorData.GetSpeed() * 100,
			SensorData.GetCourseOverGround() * 100, SensorData.GetSatellites());
}
uint16_t MAVLinkLayer::PackVFRHud(mavlink_message_t* msg)
{
	return mavlink_msg_vfr_hud_pack(MAVLINK_SYSTEM_ID, MAVLINK_COMP_ID, msg, 0, SensorData.GetSpeed(), SensorData.GetHeading(),
			(SensorData.GetThrottle() + 1000) / 20, SensorData.GetRelativeAltitude(), SensorData.GetVerticalSpeed());
}
uint16_t MAVLinkLayer::PackAttitude(mavlink_message_t* msg)
{
	return mavlink_msg_attitude_pack(MAVLINK_SYSTEM_ID, MAVLINK_COMP_ID, msg, 0, SensorData.GetRoll() * M_PI / 180,
			SensorData.GetPitch() * M_PI / 180, SensorData.GetHeading() * M_PI / 180, 0, 0, 0);
}
uint16_t MAVLinkLayer::PackRCOut(mavlink_message_t* msg)
{
	return mavlink_msg_rc_channels_raw_pack(MAVLINK_SYSTEM_ID, MAVLINK_COMP_ID, msg, 0, 10, SensorData.GetRCChannel(1),
			SensorData.GetRCChannel(2), SensorData.GetRCChannel(5), SensorData.GetRCChannel(3), SensorData.GetRCChannel(7),
			SensorData.GetRCChannel(8), SensorData.GetRCChannel(4), SensorData.GetRCChannel(9), 255);
}
uint16_t MAVLinkLayer::PackBatteryPack(mavlink_message_t* msg)
{
	return mavlink_msg_battery_status_pack(MAVLINK_SYSTEM_ID, MAVLINK_COMP_ID, msg, 1, MAV_BATTERY_FUNCTION_ALL,
			MAV_BATTERY_TYPE_LIPO, SensorData.GetTemperatur1(), SensorData.GetCells(), SensorData.GetCurrent() * 10,
			SensorData.GetCapacity(), -1, SensorData.GetCharge());
}

uint16_t MAVLinkLayer::FillBytes(mavlink_message_t* msg, uint8_t* data)
{
	return mavlink_msg_to_send_buffer(data, msg);
}

uint16_t MAVLinkLayer::PackCommandAck(mavlink_message_t* msg, MAV_CMD_ACK ack)
{
	return mavlink_msg_command_ack_pack(MAVLINK_SYSTEM_ID, MAVLINK_COMP_ID, msg, ack, 0);
}

uint16_t MAVLinkLayer::PackConfigurationVersion3(mavlink_message_t* msg, uint32_t firmware, uint8_t hardware)
{
	return mavlink_msg_configuration_version3_pack(MAVLINK_SYSTEM_ID, MAVLINK_COMP_ID, msg, firmware, hardware);
}

uint16_t MAVLinkLayer::PackLogData(mavlink_message_t* msg, uint8_t count, const uint8_t* data)
{
	return mavlink_msg_log_data_pack(MAVLINK_SYSTEM_ID, MAVLINK_COMP_ID, msg, 0, 0, count, data);
}

uint16_t MAVLinkLayer::PackConfigurationData(mavlink_message_t* msg, const uint8_t* data)
{
	return mavlink_msg_configuration_data_pack(MAVLINK_SYSTEM_ID, MAVLINK_COMP_ID, msg, data);
}

} /* namespace App */

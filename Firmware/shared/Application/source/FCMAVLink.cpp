/*
 * FCMAVLink.cpp
 *
 *  Created on: 15.03.2016
 *      Author: cem
 */

#include <FCMAVLink.h>
#include "SensorStore.h"
#include "math.h"

namespace App
{

static_assert (sizeof(FCMAVLink) <= FCAdapter::Workspace, "FCMAVLink will not fit!");

void FCMAVLink::Init()
{
	MAVLinkComm::Init();
}

void FCMAVLink::Run()
{
	m_alive = OSAL::Timer::GetTime() + delay_sec(5);

	while (m_alive > OSAL::Timer::GetTime())
	{
		if (m_messages_in.pop(m_msg_work, WaitForDataTimeout))
		{
			switch (m_msg_work.msgid)
			{
			case MAVLINK_MSG_ID_HEARTBEAT:
				m_alive = OSAL::Timer::GetTime() + delay_sec(5);
				UpdateHeartbeat(m_msg_work);
				break;
			case MAVLINK_MSG_ID_SYS_STATUS:
				UpdateSysStatus(m_msg_work);
				break;
			case MAVLINK_MSG_ID_GPS_RAW_INT:
				UpdateGPSRaw(m_msg_work);
				break;
			case MAVLINK_MSG_ID_VFR_HUD:
				UpdateVFRHUD(m_msg_work);
				break;
			case MAVLINK_MSG_ID_ATTITUDE:
				UpdateAttitude(m_msg_work);
				break;
			}
		}
		else
			RequestStreams();
	}

	DeInit();
}

void FCMAVLink::UpdateHeartbeat(mavlink_message_t& msg)
{
	uint8_t base_mode = mavlink_msg_heartbeat_get_base_mode(&msg);
	uint32_t osd_mode = mavlink_msg_heartbeat_get_custom_mode(&msg);

	SensorData.SetFCType(FCType::APM);
	SensorData.SetSensorPresent(Sensors::Arming, 1);

	SensorData.SetArmed(base_mode & (1 << 7));
	switch (osd_mode)
	{
	case 0:
	case 1:
		SensorData.SetFlightMode(FlightMode::Manual);
		break;
	case 2:
		SensorData.SetFlightMode(FlightMode::Attitude);
		break;
	case 5:
	case 16:
		SensorData.SetFlightMode(FlightMode::GPS);
		break;
	case 6:
		SensorData.SetFlightMode(FlightMode::Failsafe);
		break;
	}

}
void FCMAVLink::UpdateSysStatus(mavlink_message_t& msg)
{
	int8_t charge = mavlink_msg_sys_status_get_battery_remaining(&msg);
	int16_t current = mavlink_msg_sys_status_get_current_battery(&msg);

	SensorData.SetSensorPresent(Sensors::Charge, charge != -1);
	SensorData.SetSensorPresent(Sensors::Current, current != -1);
	SensorData.SetSensorPresent(Sensors::Voltage, 1);

	SensorData.SetBattery(mavlink_msg_sys_status_get_voltage_battery(&msg));
	SensorData.SetCharge(charge == -1 ? 0 : charge);
	SensorData.SetCurrent(current == -1 ? 0 : (current / 100.0f));
}

void FCMAVLink::UpdateGPSRaw(mavlink_message_t& msg)
{
	SensorData.SetSensorPresent(Sensors::SGPS, 1);

	double lat = mavlink_msg_gps_raw_int_get_lat(&msg);
	double lon = mavlink_msg_gps_raw_int_get_lon(&msg);
	SensorData.SetPositionCurrent(lat / 10000000, lon / 10000000);

	uint16_t hdop = mavlink_msg_gps_raw_int_get_eph(&msg);
	uint16_t vdop = mavlink_msg_gps_raw_int_get_epv(&msg);

	SensorData.SetHdop(hdop == UINT16_MAX ? 100 : hdop / 100.0f);
	SensorData.SetVdop(vdop == UINT16_MAX ? 100 : vdop / 100.0f);

	SensorData.SetSatellites(mavlink_msg_gps_raw_int_get_satellites_visible(&msg));
	SensorData.SetCourseOverGround(mavlink_msg_gps_raw_int_get_cog(&msg) / 100.0f);
	SensorData.SetSpeed(mavlink_msg_gps_raw_int_get_vel(&msg) / 100.0f);
	uint8_t fix = mavlink_msg_gps_raw_int_get_fix_type(&msg);
	if (fix == 1)
		fix = 0;
	SensorData.SetFixType((GPSFixType) fix);
}

void FCMAVLink::UpdateVFRHUD(mavlink_message_t& msg)
{
	SensorData.SetSensorPresent(Sensors::Vario, 1);

	SensorData.SetHeading(mavlink_msg_vfr_hud_get_heading(&msg));
	SensorData.SetThrottle(mavlink_msg_vfr_hud_get_throttle(&msg) * 20 - 1000);
	SensorData.SetAltitude(mavlink_msg_vfr_hud_get_alt(&msg));
	SensorData.SetVerticalSpeed(mavlink_msg_vfr_hud_get_climb(&msg));
}

void FCMAVLink::UpdateAttitude(mavlink_message_t& msg)
{
	SensorData.SetPitch(mavlink_msg_attitude_get_pitch(&msg) * (180.0f / M_PI));
	SensorData.SetRoll(mavlink_msg_attitude_get_roll(&msg) * (180.0f / M_PI));
}

void FCMAVLink::RequestStreams()
{
	for (uint8_t i = 0; i < MaxMAVStreams; i++)
	{
		m_messages_out.push(m_msg_work);
		SendMessage();
	}
}

} /* namespace App */

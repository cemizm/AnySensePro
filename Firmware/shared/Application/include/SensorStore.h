/*
 * SensorStore.h
 *
 *  Created on: 18.09.2015
 *      Author: cem
 */

#ifndef LIBS_INCLUDE_SENSORSTORE_H_
#define LIBS_INCLUDE_SENSORSTORE_H_

#include <stdint.h>
#include <math.h>
#include <DateTime.h>

const uint8_t MAXRCCHANNELS = 16;
const uint8_t MAXCELLS = 24;
const uint8_t MAXMOTOROUTS = 10;

namespace App
{

enum FCType
{
	Unknown = 0, Naza = 1, Phantom = 2, Wookong = 3, A2 = 4, APM = 5, ZYX_M = 6,
};

enum GPSFixType
{
	FixNo = 0, Fix2D = 2, Fix3D = 3, FixDGPS = 4
};

enum FlightMode
{
	Manual = 0, GPS = 1, Failsafe = 2, Attitude = 3
};

enum Sensors
{
	SGPS = 0,
	Vario = 1,
	Voltage = 2,
	GForce = 3,
	Current = 4,
	Consumption = 5,
	Cells = 6,
	Charge = 7,
	Homeing = 8,
	Arming = 9,
	RPM = 10,
};

struct GPSPosition
{
	double Latitude;
	double Longitude;
};

class SensorStore
{
private:
	FCType m_FCType = FCType::Unknown;
	GPSFixType m_FixType = GPSFixType::FixNo; //fix type (see GPSFixType)
	uint8_t m_Satellites = 0; //number of satellites
	float m_HDOP = 0; //horizontal dilution of precision
	float m_VDOP = 0; //vertical dilution of precision
	GPSPosition m_PositionCurrent;
	GPSPosition m_PositionHome;
	float m_Speed = 0; //speed in m/s
	float m_CourseOverGround = 0; //course over ground

	Utils::DateTime m_dateTime = Utils::DateTime(14, 6, 13, 18, 0, 0);

	float m_Altitude = 0; //altitude in m (from barometric sensor)
	float m_HomeAltitude = 0; // home altitude from barometric sensor
	float m_VerticalSpeed = 0; //vertical speed (barometric) in m/s (a.k.a. climb speed)

	float m_Heading = 0; //heading in degree

	float m_Pitch = 0; //pitch in degree
	float m_Roll = 0; //roll in degree

	FlightMode m_FlightMode = FlightMode::Failsafe; //flight mode (see mode_t enum)
	uint8_t m_Armed = 0;
	int16_t m_RCChannels[MAXRCCHANNELS] = { 0 }; //RC stick input (-1000~1000), use rcInChan_t enum to index the table
	int16_t m_Throttle = 0;
	uint16_t m_Motors[MAXMOTOROUTS] = { 0 };

	uint16_t m_Battery = 0; //battery voltage in mV
	uint8_t m_CellCount = 0;
	uint16_t m_Cells[MAXCELLS] = { 0 }; //battery voltage in mV
	float m_Current = 0; //current consumption in 0.1 A
	uint32_t m_Capacity = 0; // current capacity (3115mAh)

	uint8_t m_Charge = 0; // percentage of charge (62%)
	uint32_t m_Flightime = 0; //flightime in seconds

	float m_HomeDirection = 0; //home direction in degree
	uint16_t m_HomeDistance = 0; //Home distance in meters

	float m_Temperatur1 = 0;
	float m_Temperatur2 = 0;
	uint32_t m_RPM = 0;

	uint32_t m_Sensors = 0;

	uint8_t m_session = 1;

public:
	float GetAltitude() const
	{
		return m_Altitude;
	}

	void SetAltitude(float altitude = 0)
	{
		m_Altitude = altitude;
	}

	uint8_t GetArmed() const
	{
		return m_Armed;
	}

	void SetArmed(uint8_t armed = 0)
	{
		m_Armed = armed;
	}

	uint16_t GetBattery() const
	{
		return m_Battery;
	}

	void SetBattery(uint16_t battery = 0)
	{
		m_Battery = battery;
	}

	uint32_t GetCapacity() const
	{
		return m_Capacity;
	}

	void SetCapacity(uint32_t capacity = 0)
	{
		m_Capacity = capacity;
	}

	uint8_t GetCellCount() const
	{
		return m_CellCount;
	}

	void SetCellCount(uint8_t cellCount = 0)
	{
		m_CellCount = cellCount;
	}

	const uint16_t* GetCells() const
	{
		return m_Cells;
	}

	uint16_t GetCell(uint8_t cell) const
	{
		if (cell >= m_CellCount)
			return 0;

		return m_Cells[cell];
	}

	uint16_t GetCellLowest()
	{
		uint16_t cell = m_Cells[0];
		for (uint8_t i = 1; i < m_CellCount; i++)
		{
			if (m_Cells[i] < cell)
				cell = m_Cells[i];
		}
		return cell;
	}

	uint8_t GetCharge() const
	{
		return m_Charge;
	}

	void SetCharge(uint8_t charge = 0)
	{
		m_Charge = charge;
	}

	float GetCourseOverGround() const
	{
		return m_CourseOverGround;
	}

	void SetCourseOverGround(float courseOverGround = 0)
	{
		m_CourseOverGround = courseOverGround;
	}

	float GetCurrent() const
	{
		return m_Current;
	}

	void SetCurrent(float current = 0)
	{
		m_Current = current;
	}

	const Utils::DateTime& GetDateTime() const
	{
		return m_dateTime;
	}

	void SetDateTime(uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec)
	{
		m_dateTime.SetYear(year);
		m_dateTime.SetMonth(month);
		m_dateTime.SetDay(day);
		m_dateTime.SetHour(hour);
		m_dateTime.SetMinute(min);
		m_dateTime.SetSecond(sec);
	}

	void SetDateTime(Utils::DateTime& dateTime)
	{
		m_dateTime = dateTime;
	}

	FCType GetFCType() const
	{
		return m_FCType;
	}

	void SetFCType(FCType fcType = FCType::Unknown)
	{
		m_FCType = fcType;
	}

	GPSFixType GetFixType() const
	{
		return m_FixType;
	}

	void SetFixType(GPSFixType fixType = GPSFixType::FixNo)
	{
		m_FixType = fixType;
	}

	uint8_t IsGPSOK() const
	{
		return m_FixType >= GPSFixType::Fix3D;
	}

	uint32_t GetFlightime() const
	{
		return m_Flightime;
	}

	void SetFlightime(uint32_t flightime = 0)
	{
		m_Flightime = flightime;
	}

	FlightMode GetFlightMode() const
	{
		return m_FlightMode;
	}

	void SetFlightMode(FlightMode flightMode = FlightMode::Failsafe)
	{
		m_FlightMode = flightMode;
	}

	float GetHdop() const
	{
		return m_HDOP;
	}

	void SetHdop(float hdop = 0)
	{
		m_HDOP = hdop;
	}

	float GetHeading() const
	{
		return m_Heading;
	}

	void SetHeading(float heading = 0)
	{
		m_Heading = heading;
	}

	float GetHomeAltitude() const
	{
		return m_HomeAltitude;
	}

	void SetHomeAltitude(float homeAltitude = 0)
	{
		m_HomeAltitude = homeAltitude;
	}

	float GetRelativeAltitude() const
	{
		return m_Altitude - m_HomeAltitude;
	}

	float GetPitch() const
	{
		return m_Pitch;
	}

	void SetPitch(float pitch = 0)
	{
		m_Pitch = pitch;
	}

	const GPSPosition& GetPositionCurrent() const
	{
		return m_PositionCurrent;
	}

	void SetPositionCurrent(double latitude, double longitude)
	{
		m_PositionCurrent.Latitude = latitude;
		m_PositionCurrent.Longitude = longitude;
	}

	const GPSPosition& GetPositionHome() const
	{
		return m_PositionHome;
	}

	void SetPositionHome(double latitude, double longitude)
	{
		m_PositionHome.Latitude = latitude;
		m_PositionHome.Longitude = longitude;
	}
	void SetPositionHome()
	{
		m_PositionHome.Latitude = m_PositionCurrent.Latitude;
		m_PositionHome.Longitude = m_PositionCurrent.Longitude;
	}
	void SetPositionHomeClear()
	{
		m_PositionHome.Latitude = 0;
		m_PositionHome.Longitude = 0;
	}
	uint8_t IsPositionHomeSet()
	{
		return m_PositionHome.Latitude != 0 && m_PositionHome.Longitude != 0;
	}

	void SetRCChannels(int16_t* value, uint8_t count)
	{
		for (uint8_t channel = 0; channel < count; channel++)
			m_RCChannels[channel] = value[channel];
	}

	void SetMotorOuts(uint16_t* motors, uint8_t count)
	{
		for (uint8_t motor = 0; motor < count; motor++)
			m_Motors[motor] = motors[motor];
	}

	int16_t GetRCChannel(uint8_t channel) const
	{
		return m_RCChannels[channel];
	}

	float GetRoll() const
	{
		return m_Roll;
	}

	void SetRoll(float roll = 0)
	{
		m_Roll = roll;
	}

	uint32_t GetRpm() const
	{
		return m_RPM;
	}

	void SetRpm(uint32_t rpm = 0)
	{
		m_RPM = rpm;
	}

	uint8_t GetSatellites() const
	{
		return m_Satellites;
	}

	void SetSatellites(uint8_t satellites = 0)
	{
		m_Satellites = satellites;
	}

	float GetSpeed() const
	{
		return m_Speed;
	}

	void SetSpeed(float speed = 0)
	{
		m_Speed = speed;
	}

	float GetTemperatur1() const
	{
		return m_Temperatur1;
	}

	void SetTemperatur1(float temperatur1 = 0)
	{
		m_Temperatur1 = temperatur1;
	}

	float GetTemperatur2() const
	{
		return m_Temperatur2;
	}

	void SetTemperatur2(float temperatur2 = 0)
	{
		m_Temperatur2 = temperatur2;
	}

	int16_t GetThrottle() const
	{
		return m_Throttle;
	}

	void SetThrottle(int16_t throttle = 0)
	{
		m_Throttle = throttle;
	}

	float GetVdop() const
	{
		return m_VDOP;
	}

	void SetVdop(float vdop = 0)
	{
		m_VDOP = vdop;
	}

	float GetVerticalSpeed() const
	{
		return m_VerticalSpeed;
	}

	void SetVerticalSpeed(float verticalSpeed = 0)
	{
		m_VerticalSpeed = verticalSpeed;
	}

	void SetCell(uint8_t cell, uint16_t voltage)
	{
		if (cell > MAXCELLS)
			return;

		m_Cells[cell] = voltage;
	}

	uint8_t GetSensorPresent(Sensors sensor)
	{
		uint8_t s = (uint8_t) sensor;
		uint32_t flag = (1 << s);

		return (m_Sensors & flag) == flag;
	}

	void SetSensorPresent(Sensors sensor, uint8_t present)
	{
		uint8_t s = (uint8_t) sensor;
		uint32_t flag = (1 << s);

		if (present)
			m_Sensors |= flag;
		else
			m_Sensors &= ~flag;
	}

	void SetHomeDirection(float degree)
	{
		m_HomeDirection = degree;
	}

	void SetHomeDistance(uint16_t distance)
	{
		m_HomeDistance = distance;
	}

	uint16_t GetHomeDistance()
	{
		return m_HomeDistance;
	}

	float GetHomeDirection()
	{
		return m_HomeDirection;
	}

	uint8_t GetSession()
	{
		return m_session;
	}

	void UpdateSession()
	{
		m_session++;
	}
};

extern SensorStore SensorData;

}

#endif /* LIBS_INCLUDE_SENSORSTORE_H_ */

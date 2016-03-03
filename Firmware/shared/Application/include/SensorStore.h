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

struct GPSPosition
{
	double Latitude;
	double Longitude;
};

struct GPSTime
{
	uint8_t Year; //since 2000
	uint8_t Month;
	uint8_t Day;
	uint8_t Hour;
	uint8_t Minute;
	uint8_t Second;
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

	GPSTime m_dateTime;

	float m_Altitude = 0; //altitude in m (from barometric sensor)
	float m_HomeAltitude = 0; // home altitude from barometric sensor plus 20m (meters)
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
	uint16_t m_Capacity = 0; // current capacity (3115mAh)

	uint8_t m_Charge = 0; // percentage of charge (62%)
	uint32_t m_Flightime = 0; //flightime in seconds

	float m_Temperatur1 = 0;
	float m_Temperatur2 = 0;
	uint32_t m_RPM = 0;

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

	uint16_t GetCapacity() const
	{
		return m_Capacity;
	}

	void SetCapacity(uint16_t capacity = 0)
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

	const GPSTime& GetDateTime() const
	{
		return m_dateTime;
	}

	void SetDateTime(uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec)
	{
		m_dateTime.Year = year;
		m_dateTime.Month = month;
		m_dateTime.Day = day;
		m_dateTime.Hour = hour;
		m_dateTime.Minute = min;
		m_dateTime.Second = sec;
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
};

extern SensorStore SensorData;

}

#endif /* LIBS_INCLUDE_SENSORSTORE_H_ */

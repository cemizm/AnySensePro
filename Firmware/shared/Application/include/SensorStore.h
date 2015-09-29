/*
 * SensorStore.h
 *
 *  Created on: 18.09.2015
 *      Author: cem
 */

#ifndef LIBS_INCLUDE_SENSORSTORE_H_
#define LIBS_INCLUDE_SENSORSTORE_H_

#include <stdint.h>

const uint8_t MAXRCCHANNELS = 16;
const uint8_t MAXCELLS = 24;

namespace Application
{

enum GPSFixType
{
	FixNo = 0, Fix2D = 2, Fix3D = 3, FixDGPS = 4
};

enum FlightMode
{
	Manual = 0, GPS = 1, Failsafe = 2, Attitude = 3
};

class SensorStore
{
private:

	GPSFixType m_FixType = GPSFixType::FixNo; //fix type (see GPSFixType)
	uint8_t m_NumSat = 0; //number of satellites
	float m_HDOP = 0; //horizontal dilution of precision
	float m_VDOP = 0; //vertical dilution of precision
	double m_Latitude = 0; //latitude in degree decimal
	double m_Longitude = 0; //longitude in degree decimal
	double m_HomeLatitude = 0; // home lattitude in degree decimal
	double m_homeLongitude = 0; // home longitude in degree decimal
	float m_Speed = 0; //speed in m/s
	float m_CourseOverGround = 0; //course over ground

	uint8_t m_Year = 15; //year from GPS (minus 2000)
	uint8_t m_Month = 9; //month from GPS
	uint8_t m_Day = 18; //day from GPS
	uint8_t m_Hour = 20; //hour from GPS (Note that for time between 16:00 and 23:59 the hour returned from GPS module is actually 00:00 - 7:59)
	uint8_t m_Minute = 5; //minute from GPS
	uint8_t m_Second = 23; //second from GPS


	float m_Altitude = 0; //altitude in m (from barometric sensor)
	float m_HomeAltitude = 0; // home altitude from barometric sensor plus 20m (meters)
	float m_VerticalSpeed = 0; //vertical speed (barometric) in m/s (a.k.a. climb speed)

	float m_Heading = 0; //heading in degrees (titlt compensated)

	float m_Pitch = 0; //pitch in degree
	float m_Roll = 0; //roll in degree

	FlightMode m_FlightMode = FlightMode::Failsafe; //flight mode (see mode_t enum)
	uint8_t m_Armed = 0;
	int16_t m_RCChannels[MAXRCCHANNELS] = { 0 }; //RC stick input (-1000~1000), use rcInChan_t enum to index the table
	int16_t m_Throttle = 0;

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

};

}

#endif /* LIBS_INCLUDE_SENSORSTORE_H_ */

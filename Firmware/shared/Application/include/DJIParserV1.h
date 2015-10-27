/*
 * DJIParserV1.h
 *
 *  Created on: 05.10.2015
 *      Author: cem
 */

#ifndef APPLICATION_INCLUDE_DJIPARSERV1_H_
#define APPLICATION_INCLUDE_DJIPARSERV1_H_

#include "DJIParser.h"

namespace App
{

static const uint32_t MessageStart = 0xAA55AA55;
static const uint32_t MessageEnd = 0xCC66CC66;
static const uint8_t MAX_DJI_CHANNELS = 10;
static const uint8_t MAX_DJI_MOTORS = 8;

struct DJIMessageHeaderV1
{
	uint32_t Start;
	uint16_t Id;
	uint16_t Size;
}__attribute__((packed));

static const uint32_t MessageSize = (sizeof(DJIMessageHeaderV1) + sizeof(MessageEnd));

struct DJIMessageOSDV1
{
	DJIMessageHeaderV1 Header;
	struct
	{
		double Longitude;							// longitude (radians)
		double Latitude;							// lattitude (radians)
	}__attribute__((packed)) Position;
	float AltitudeGPS;								// altitude from GPS (meters)
	struct
	{
		float X;
		float Y;
		float Z;
	}__attribute__((packed)) Accelerometer;			// accelerometer X,Y,Z axis data (??)
	struct
	{
		float X;
		float Y;
		float Z;
	}__attribute__((packed)) Gyroscope;				// gyroscope X,Y,Z axis data (??)
	float AltitudeBaro;								// altitude from barometric sensor (meters)
	struct
	{
		float X;									// compensated heading X component
		float Y;									// compensated heading Y component

	} Heading;
	float unk0[2];
	float unk1[3];
	struct
	{
		float North;
		float East;
		float Down;
	}__attribute__((packed)) Velocity;				// averaged notrh,east,down velocity or 0 when less than 5 satellites locked (m/s)
	float unk2[3];
	struct
	{
		int16_t X;
		int16_t Y;
		int16_t Z;
	}__attribute__((packed)) Magnetometer;			// calibrated magnetometer X,Y,Z axis data
	uint8_t unk3[10];
	uint8_t Satellites;								// number of locked satellites
	uint8_t Mask;
	uint16_t Sequence;								// sequence number - increases with every message
	uint32_t Footer;
}__attribute__((packed));

struct DJIMessageGPSV1
{
	DJIMessageHeaderV1 Header;
	struct
	{
		uint8_t Seconds :6;
		uint8_t Minutes :6;
		uint8_t Hours :4;
		uint8_t Days :5;
		uint8_t Months :4;
		uint8_t Years :7;
	}__attribute__((packed)) DateTime;

	struct
	{
		uint32_t Longitude;							// Longitude (x10^7, degree decimal)
		uint32_t Latitude;							// Lattitude (x10^7, degree decimal)
	}__attribute__((packed)) Position;
	uint32_t Altitude;        						// Altitude from GPS (millimeters)
	struct
	{
		uint32_t Horizontal;						// Horizontal accuracy estimate (millimeters)
		uint32_t Vertical;							// Vertical accuracy estimate (millimeters)
	}__attribute__((packed)) Accuracy;
	uint8_t Unk0[4];
	struct
	{
		int32_t North; 								// Northward velocity (cm/s)
		int32_t East;								// Eastward velocity (cm/s)
		int32_t Down;  								// Downward velocity (cm/s)
	}__attribute__((packed)) Velocity;
	struct
	{
		uint16_t Position;
		uint16_t Vertical;
		uint16_t North;
		uint16_t East;
	}__attribute__((packed)) DOP; 					// DOP (see uBlox NAV-DOP message for details)
	uint8_t Satellites;								// Number of locked Satellites
	uint8_t Unk1;
	uint8_t FixType; 								// Fix Type (0 - no lock, 2 - 2D lock, 3 - 3D lock
	uint8_t Unk2;
	uint8_t FixStatus; 								// Fix Status flags (see uBlox NAV-SOL message for details)
	uint8_t Unk3[3];
	uint16_t Sequence;   							// sequence number - increases with every message
	uint32_t Footer;
}__attribute__((packed, aligned(1)));

struct DJIMessageRAWV1
{
	DJIMessageHeaderV1 Header;
	uint8_t unk1[4];
	uint16_t MotorOut[MAX_DJI_MOTORS];				// motor output (M1/M2/M3/M4/M5/M6/F1/F2)
	uint8_t unk2[4];
	int16_t RChannels[MAX_DJI_CHANNELS];			// RC controller input (order: unused/A/E/R/U/T/unused/X1/X2/unused)
	uint8_t unk3[11];
	uint8_t FlightMode;								// (0 - manual, 1 - GPS, 2 - failsafe, 3 - atti)
	uint8_t unk4[8];
	struct
	{
		double Longitude;							// home lattitude (radians)
		double Latitude;							// home longitude (radians)
	}__attribute__((packed)) HomePosition;
	float HomeAltitude; 							// home altitude from barometric sensor plus 20m (meters)
	uint16_t Sequence; 								// sequence number - increases with every message
	uint8_t unk5[2];
	struct
	{
		float Roll;
		float Pitch;
		float Throttle;
	}__attribute__((packed)) StabilizerInput;
	uint8_t unk6;
	uint8_t Armed;									//0 - disarmed, 1 - armed
	uint8_t Failsafe; 								//1 - failsafe, 0 - default
	uint8_t unk;
	struct
	{
		float Aileron;
		float Elevator;
		float Throttle;
	}__attribute__((packed)) ActualInput;
	struct
	{
		uint16_t Battery;
		uint16_t PMU;
		uint16_t CAN;
	}__attribute__((packed)) Voltage;
	uint8_t unk7[2];
	uint8_t ControlMode; 							// (0 - GPS/failsafe, 1 - waypoint mode?, 3 - manual, 6 - atti)
	uint8_t unk8[5];
	struct
	{
		int16_t X;     								// ???
		int16_t Y;     								// ???
		int16_t Z;     								// ???
	}__attribute__((packed)) GyroscopeScale;
	uint8_t unk9[32];
	float DownVelocity; 							// downward velocity (m/s)
	float Altitude;      							// altitude from barometric sensor (meters)
	float Roll;        								// roll angle (radians)
	float Pitch;									// pitch angle (radians)
	uint32_t Footer;
}__attribute__((packed));

union DJIMessageV1
{
	uint8_t Data[DJIChannelSize];
	DJIMessageHeaderV1 Header;
	DJIMessageOSDV1 OSD;
	DJIMessageGPSV1 GPS;
	DJIMessageRAWV1 RAW;
}__attribute__((packed));

class DJIParserV1: public DJIParser
{
private:
	DJIMessageOSDV1 OSD;
	DJIMessageGPSV1 GPS;
	DJIMessageRAWV1 RAW;
	void findNext(DJIChannel* channel);
	void moveToSyncPos(DJIChannel* channel);
	void process(DJIMessageV1* msg);
	void process(DJIMessageOSDV1* msg);
	void process(DJIMessageGPSV1* msg);
	void process(DJIMessageRAWV1* msg);
public:
	void Parse(App::DJIChannel* channel, HAL::CANRxMessage* msg) override;
};

} /* namespace Application */

#endif /* APPLICATION_INCLUDE_DJIPARSERV1_H_ */

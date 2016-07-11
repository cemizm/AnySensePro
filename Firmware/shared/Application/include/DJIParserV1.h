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

class DJIParserV1: public DJIParser
{
private:

	struct DJIHeader
	{
		uint32_t Start;
		uint16_t Id;
		uint16_t Size;
	}__attribute__((packed));

	struct DJIMessageOSD
	{
		DJIHeader Header;
		DJIOSDData Data;
		uint32_t Footer;
	}__attribute__((packed, aligned(1)));

	struct DJIMessageGPS
	{
		DJIHeader Header;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpacked-bitfield-compat"
		struct
		{
			uint8_t Seconds :6;
			uint8_t Minutes :6;
			uint8_t Hours :4;
			uint8_t Days :5;
			uint8_t Months :4;
			uint8_t Years :7;
		}__attribute__((packed)) DateTime;
#pragma GCC diagnostic pop

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

	struct DJIMessageRAW
	{
		DJIHeader Header;
		uint32_t Sequenz;								// sequence number - increases with every message
		uint16_t MotorOut[MaxMotorOuts];				// motor output (M1/M2/M3/M4/M5/M6/F1/F2)
		uint8_t unk2[4];
		int16_t RChannels[MaxRCChannels];				// RC controller input (order: unused/A/E/R/U/T/unused/X1/X2/unused)
		uint8_t unk3[11];
		uint8_t FlightMode;								// (0 - manual, 1 - GPS, 2 - failsafe, 3 - atti)
		uint8_t unk4[8];
		struct
		{
			double Latitude;							// home longitude (radians)
			double Longitude;							// home lattitude (radians)
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

	struct DJIMessageBAT
	{
		DJIHeader Header;									//
		uint16_t capacity_design; 		// design capacity (5200mAh)		2
		uint16_t capacity_full; 		// full capacity (5003mAH)			4
		uint16_t capacity_current;		// current capacity (3115mAh)		6
		uint16_t voltage; 				// voltage (11725mV)				8
		int16_t current;				// current (-371mA)					10
		uint8_t percentage_life;		// percentage of life (93%)			11
		uint8_t percentage_charge;		// percentage of charge (62%)		12
		uint16_t temperature;			// (230 * 0.1 degrees Celsius)		14
		uint16_t dischargedCount;		// discharging times (21)			16
		uint16_t serialNumber;			// serial number (6585)				18
		uint16_t cell1;					//cell 3 (3934mV)					20
		uint16_t cell2;					//cell 2 (3934mV)					22
		uint16_t cell3;					//cell 1 (3911mV)					24
		uint8_t unk2[11];											//		26
	}__attribute__((packed));

	union DJIMessage
	{
		uint8_t Data[DJIChannelSize];
		DJIHeader Header;
		DJIMessageOSD OSD;
		DJIMessageGPS GPS;
		DJIMessageRAW RAW;
		DJIMessageBAT BAT;
	}__attribute__((packed));

	static const uint32_t MessageStart = 0xAA55AA55;
	static const uint32_t MessageEnd = 0xCC66CC66;
	static const uint32_t MessageSize = (sizeof(DJIParserV1::DJIHeader) + sizeof(DJIParserV1::MessageEnd));

	static const uint16_t channelIdWookong = 0x388;

	static const uint16_t packetIdRAW = 0x1009;
	static const uint16_t packetIdBAT = 0x0926;

	void process(DJIMessage* msg);
	void process(DJIMessageGPS* msg);
	void process(DJIMessageRAW* msg);
	void process(DJIMessageBAT* msg);
public:
	void Parse(App::DJIChannel* channel, HAL::CANRxMessage* msg) override;
};

} /* namespace Application */

#endif /* APPLICATION_INCLUDE_DJIPARSERV1_H_ */

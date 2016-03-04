/*
 * DJIParserV2.h
 *
 *  Created on: 03.03.2016
 *      Author: cem
 */

#ifndef APPLICATION_INCLUDE_DJIPARSERV2_H_
#define APPLICATION_INCLUDE_DJIPARSERV2_H_

#include <DJIParser.h>
#include <math.h>

namespace App
{

class DJIParserV2: public DJIParser
{

private:

	struct DJIHeader
	{
		uint8_t StartByte;
		uint16_t Size;
		uint8_t unk1;
	}__attribute__((packed));

	struct DJIHeaderV1
	{
		DJIHeader Header;
		uint16_t Id;
	}__attribute__((packed));

	struct DJIHeaderV2
	{
		DJIHeader Header;
		uint32_t Id;
	}__attribute__((packed));

	struct DJIMessageOSD
	{
		DJIHeaderV1 Header;
		DJIOSDData Data;
		uint16_t Checksum;
	}__attribute__((packed, aligned(1)));

	struct DJIMessageGPS
	{
		DJIHeaderV1 Header;

		struct
		{
			uint32_t Date;          						// GPS date (year * 10000 + month * 100 + day)
			uint32_t Time;          						// GPS time (hour * 10000 + minute * 100 + second)

			uint8_t getDay()
			{
				return Date;
			}

			uint8_t getMonth()
			{
				return Date / 100;
			}

			uint16_t getYear()
			{
				return Date / 10000;
			}

			uint8_t getSeconds()
			{
				return Time;
			}
			uint8_t getMinutes()
			{
				return Time / 100;
			}
			uint8_t getHours()
			{
				return Time / 10000;
			}

		} DateTime;

		struct
		{
			uint32_t Longitude;							// Longitude (x10^7, degree decimal)
			uint32_t Latitude;							// Lattitude (x10^7, degree decimal)
		}__attribute__((packed)) Position;
		uint32_t Altitude;        						// Altitude from GPS (millimeters)
		struct
		{
			float North;
			float East;
			float Down;
			float getSpeed()
			{
				return sqrtf(North * North + East * East);
			}
			float getCOG()
			{
				return (-(atan2f(East, North) / M_PI * 180)) + 180;
			}
		}__attribute__((packed)) Velocity;		// averaged notrh,east,down velocity or 0 when less than 5 satellites locked (m/s)
		float unk1[6];
		struct
		{
			uint16_t Position;
			uint16_t Vertical;
			uint16_t North;
			uint16_t East;
		}__attribute__((packed)) DOP; 					// DOP (see uBlox NAV-DOP message for details)
		uint8_t Satellites;        						// number of locked satellites
		uint8_t unk3;
		uint16_t Sequence;        						// sequence number - increases with every message

		uint16_t Checksum;
	}__attribute__((packed, aligned(1)));

	struct DJIMessageRAW
	{
		DJIHeaderV2 Header;

		uint32_t Sequenz;								// sequence number - increases with every message
		int16_t RChannels[MaxRCChannels];				// RC controller input (order: unused/A/E/R/U/T/unused/X1/X2/unused)
		int16_t unk1[27];
		uint32_t MotorOut[MaxMotorOuts];   				// motor output (M1/M2/M3/M4/M5/M6/M7/M8)
		uint8_t unk2[6];
		uint8_t FlightMode;								// (0 - manual, 1 - ATTI, 2 - GPS, 4 - FAILSAFE)
		uint8_t unk3[41];
		uint8_t Armed;
		uint8_t unk4[6];
		struct
		{
			uint16_t Battery;
			uint16_t PMU;
			uint16_t CAN;
		}__attribute__((packed)) Voltage;

		uint16_t Checksum;
	}__attribute__((packed, aligned(1)));

	union DJIMessage
	{
		uint8_t Data[DJIChannelSize];
		DJIHeader Header;
		DJIHeaderV1 HeaderV1;
		DJIHeaderV2 HeaderV2;
		DJIMessageOSD OSD;
		DJIMessageGPS GPS;
		DJIMessageRAW RAW;
	}__attribute__((packed));

	static const uint8_t MessageStart = 0x55;

	static const uint16_t packetIdA2 = 0x4A44;
	static const uint32_t packetIdA2Raw = 0xA2494A44;

	void process(DJIMessage* msg);
	void process(DJIMessageGPS* msg);
	void process(DJIMessageRAW* msg);

public:
	void Parse(App::DJIChannel* channel, HAL::CANRxMessage* msg) override;
};

} /* namespace App */

#endif /* APPLICATION_INCLUDE_DJIPARSERV2_H_ */

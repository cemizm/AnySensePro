/*
 * DJIParser.h
 *
 *  Created on: 05.10.2015
 *      Author: cem
 */

#ifndef APPLICATION_INCLUDE_DJIPARSER_H_
#define APPLICATION_INCLUDE_DJIPARSER_H_

#include <stddef.h>
#include <string.h>
#include <math.h>
#include <CAN.h>
#include <SensorStore.h>

namespace App
{

static const uint16_t DJIChannelSize = 256;

struct DJIChannel
{
	uint8_t Data[DJIChannelSize];
	uint16_t ChannelId;
	uint8_t IsSnyc;
	uint16_t SyncPosition;
	uint16_t Count;
};

class DJIParser
{
private:
protected:
	struct DJIOSDData
	{
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
			float X;
			float Y;
			float Z;
			float W;
			float getRoll()
			{
				return (atan2f((2.0 * (X * Y + W * Z)), W * W + X * X - Y * Y - Z * Z)) / M_PI * 180;
			}
			float getPitch()
			{
				return (-asin(-2.0 * (X * Z - W * Y))) / M_PI * 180;
			}
			float getHeading()
			{
				return (-(atan2f((2.0 * (Y * Z + W * X)), W * W - X * X - Y * Y + Z * Z)) / M_PI * 180) + 180;
			}
		}__attribute__((packed, aligned(1))) Quaternation;
		float unk1[3];
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
	}__attribute__((packed));

	static const uint8_t MaxRCChannels = 10;
	static const uint8_t MaxMotorOuts = 8;

	static const uint16_t packetIdOSD = 0x1002;
	static const uint16_t packetIdGPS = 0x1003;

	template<typename T>
	void FindNext(DJIChannel* channel, T sign)
	{
		if (channel->Count < sizeof(T))
			return;

		for (; channel->SyncPosition < (channel->Count - (sizeof(T) - 1)); channel->SyncPosition++)
		{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
			if (*((T*) &channel->Data[channel->SyncPosition]) == sign)
			{
				MoveToSyncPos(channel);
				channel->IsSnyc = 1;
				return;
			}
		}
#pragma GCC diagnostic pop

		if (channel->IsSnyc == 0 && (channel->Count + 8) > DJIChannelSize)
			MoveToSyncPos(channel);
	}

	void MoveToSyncPos(DJIChannel* channel)
	{
		channel->Count -= channel->SyncPosition;
		memmove(channel->Data, &channel->Data[channel->SyncPosition], channel->Count);

		channel->SyncPosition = 0;
	}

	void process(DJIOSDData* msg)
	{
		if (msg->Mask != 0)
		{
			uint8_t* data = (uint8_t*) &msg->Position;
			for (uint8_t i = 0; i < sizeof(DJIOSDData); i++)
				data[i] ^= msg->Mask;
		}

		SensorData.SetPositionCurrent(msg->Position.Latitude / M_PI * 180.0, msg->Position.Longitude / M_PI * 180.0);
		SensorData.SetAltitude(msg->AltitudeBaro);

		SensorData.SetHeading(msg->Quaternation.getHeading());
		SensorData.SetRoll(msg->Quaternation.getRoll());
		SensorData.SetPitch(msg->Quaternation.getPitch());

		SensorData.SetSpeed(msg->Velocity.getSpeed());
		SensorData.SetVerticalSpeed(-msg->Velocity.Down);
		SensorData.SetCourseOverGround(msg->Velocity.getCOG());
		SensorData.SetSatellites(msg->Satellites);
	}

public:

	virtual void Parse(DJIChannel* channel, HAL::CANRxMessage* msg)
	{
		(void) channel;
		(void) msg;
	}

	virtual ~DJIParser()
	{
	}
};

} /* namespace Application */

#endif /* APPLICATION_INCLUDE_DJIPARSER_H_ */

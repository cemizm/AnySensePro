/*
 * DJIParserV1.cpp
 *
 *  Created on: 05.10.2015
 *      Author: cem
 */

#include <SensorStore.h>
#include <DJIParserV1.h>
#include <string.h>
#include <math.h>

namespace App
{

void DJIParserV1::Parse(DJIChannel* channel, HAL::CANRxMessage* msg)
{
	memcpy(&channel->Data[channel->Count], msg->data, msg->DataLength);
	channel->Count += msg->DataLength;

	if (channel->IsSnyc)
	{
		if (channel->Count > sizeof(DJIMessageHeaderV1))
		{
			DJIMessageV1* djiMessage = (DJIMessageV1*) channel->Data;

			if ((djiMessage->Header.Size + MessageSize) > DJIChannelSize)
			{
				channel->IsSnyc = 0;
				channel->SyncPosition = 1;
			}
			else if (channel->Count >= (djiMessage->Header.Size + MessageSize))
			{
				if (*((uint32_t*) &channel->Data[djiMessage->Header.Size + sizeof(DJIMessageHeaderV1)]) == MessageEnd)
				{
					process(djiMessage);

					channel->IsSnyc = 0;
					channel->SyncPosition = djiMessage->Header.Size + MessageSize;
				}
				else
				{
					channel->IsSnyc = 0;
					channel->SyncPosition = 1;
				}
			}
		}
	}

	if (!channel->IsSnyc)
		FindNext<uint32_t>(channel, MessageStart);
}

void DJIParserV1::process(DJIMessageV1* msg)
{
	switch (msg->Header.Id)
	{
	case 0x1002:
		process(&msg->OSD);
		break;
	case 0x1003:
		process(&msg->GPS);
		break;
	case 0x1009:
		process(&msg->RAW);
		break;
	default:
		break; //discard...
	}
}

void DJIParserV1::process(DJIMessageOSDV1* msg)
{
	if (msg->Mask != 0)
	{
		uint8_t* data = (uint8_t*) &msg->Position;
		for (uint8_t i = 0; i < msg->Header.Size; i++)
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

void DJIParserV1::process(DJIMessageGPSV1* msg)
{
	SensorData.SetDateTime(msg->DateTime.Years, msg->DateTime.Months, msg->DateTime.Days, msg->DateTime.Hours,
			msg->DateTime.Minutes, msg->DateTime.Seconds);

	if ((msg->FixStatus & 0x02) && msg->FixType > 0)
		SensorData.SetFixType(GPSFixType::FixDGPS);
	else
		SensorData.SetFixType((GPSFixType) msg->FixType);

	SensorData.SetVdop((float) msg->DOP.Vertical / 100);

	float ndop = (float) msg->DOP.North / 100;
	float edop = (float) msg->DOP.East / 100;
	SensorData.SetHdop(sqrtf(ndop * ndop + edop * edop));
}

void DJIParserV1::process(DJIMessageRAWV1* msg)
{
	SensorData.SetBattery(msg->Voltage.Battery);
	SensorData.SetArmed(msg->Armed);
	SensorData.SetThrottle(msg->ActualInput.Throttle);
	SensorData.SetPositionHome(msg->HomePosition.Latitude / M_PI * 180, msg->HomePosition.Longitude / M_PI * 180);
	SensorData.SetHomeAltitude(msg->HomeAltitude - 20);
	SensorData.SetFlightMode((FlightMode) msg->FlightMode);
	SensorData.SetRCChannels(msg->RChannels, MAX_DJI_CHANNELS);
	SensorData.SetMotorOuts(msg->MotorOut, MAX_DJI_MOTORS);
}

} /* namespace Application */

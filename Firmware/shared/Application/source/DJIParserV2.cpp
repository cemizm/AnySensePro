/*
 * DJIParserV2.cpp
 *
 *  Created on: 03.03.2016
 *      Author: cem
 */

#include <DJIParserV2.h>

#include <CRC.h>

namespace App
{

void DJIParserV2::Parse(DJIChannel* channel, HAL::CANRxMessage* msg)
{
	memcpy(&channel->Data[channel->Count], msg->data, msg->DataLength);
	channel->Count += msg->DataLength;

	if (channel->IsSnyc)
	{
		if (channel->Count > sizeof(DJIHeader))
		{
			DJIMessage* djiMessage = (DJIMessage*) channel->Data;

			if (djiMessage->Header.Size > DJIChannelSize)
			{
				channel->IsSnyc = 0;
				channel->SyncPosition = 1;
			}
			else if (channel->Count >= djiMessage->Header.Size)
			{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
				volatile uint16_t checksum = *(((uint16_t*) &channel->Data[djiMessage->Header.Size]) - 1);
#pragma GCC diagnostic pop

				volatile uint16_t checkCalc = HAL::CRC::Calculate16(0x1021, 0x496C, djiMessage->Header.Size - 2,
						djiMessage->Data);

				if (checksum == checkCalc)
				{
					process(djiMessage);

					channel->IsSnyc = 0;
					channel->SyncPosition = djiMessage->Header.Size;
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
		FindNext<uint8_t>(channel, MessageStart);

}

void DJIParserV2::process(DJIMessage* msg)
{
	switch (msg->HeaderV1.Id)
	{
	case packetIdOSD:
		DJIParser::process(&msg->OSD.Data);
		break;
	case packetIdGPS:
		process(&msg->GPS);
		break;
	case packetIdA2:
		switch (msg->HeaderV2.Id)
		{
		case packetIdA2Raw:
			process(&msg->RAW);
			break;
		default:
			break;
		}
		break;
	default:
		break; //discard...
	}
}

void DJIParserV2::process(DJIMessageGPS* msg)
{
	SensorData.SetDateTime(msg->DateTime.getYear(), msg->DateTime.getMonth(), msg->DateTime.getDay(), msg->DateTime.getHours(),
			msg->DateTime.getMinutes(), msg->DateTime.getSeconds());

	SensorData.SetFixType(msg->Satellites == 0 ? GPSFixType::FixNo : msg->Satellites < 4 ? GPSFixType::Fix2D : GPSFixType::Fix3D);

	SensorData.SetVdop(msg->DOP.Vertical / 100.0f);

	float ndop = msg->DOP.North / 100.0f;
	float edop = msg->DOP.East / 100.0f;

	SensorData.SetHdop(sqrtf(ndop * ndop + edop * edop));
}

void DJIParserV2::process(DJIMessageRAW* msg)
{
	SensorData.SetBattery(msg->Voltage.Battery);

	switch (msg->FlightMode)
	{
	case 0:
		SensorData.SetFlightMode(FlightMode::Manual);
		break;
	case 1:
		SensorData.SetFlightMode(FlightMode::Attitude);
		break;
	case 2:
		SensorData.SetFlightMode(FlightMode::GPS);
		break;
	default:
		SensorData.SetFlightMode(FlightMode::Failsafe);
		break;
	}

	SensorData.SetArmed(msg->Armed);
	//SensorData.SetThrottle(msg->ActualInput.Throttle);
	SensorData.SetRCChannels(msg->RChannels, MaxRCChannels);
	//SensorData.SetMotorOuts(msg->MotorOut, MaxMotorOuts);
}

} /* namespace App */

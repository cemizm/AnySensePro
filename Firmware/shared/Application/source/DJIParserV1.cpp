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
#include <System.h>

namespace App
{

void DJIParserV1::Parse(DJIChannel* channel, HAL::CANRxMessage* msg)
{
	memcpy(&channel->Data[channel->Count], msg->data, msg->DataLength);
	channel->Count += msg->DataLength;

	if (channel->IsSnyc)
	{
		if (channel->Count > sizeof(DJIHeader))
		{
			DJIMessage* djiMessage = (DJIMessage*) channel->Data;

			if ((djiMessage->Header.Size + MessageSize) > DJIChannelSize)
			{
				channel->IsSnyc = 0;
				channel->SyncPosition = 1;
			}
			else if (channel->Count >= (djiMessage->Header.Size + MessageSize))
			{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
				if (*((uint32_t*) &channel->Data[djiMessage->Header.Size + sizeof(DJIHeader)]) == MessageEnd)
				{
					if (SensorData.GetFCType() == FCType::Naza)
					{
						if (channel->ChannelId == channelIdWookong)
							SensorData.SetFCType(FCType::Wookong);
					}

					process(djiMessage);

					channel->IsSnyc = 0;
					channel->SyncPosition = djiMessage->Header.Size + MessageSize;
				}
				else
				{
					channel->IsSnyc = 0;
					channel->SyncPosition = 1;
				}
#pragma GCC diagnostic pop
			}
		}
	}

	if (!channel->IsSnyc)
		FindNext<uint32_t>(channel, MessageStart);
}

void DJIParserV1::process(DJIMessage* msg)
{
	switch (msg->Header.Id)
	{
	case packetIdOSD:
		DJIParser::process(&msg->OSD.Data);
		break;
	case packetIdGPS:
		process(&msg->GPS);
		break;
	case packetIdRAW:
		process(&msg->RAW);
		break;
	case packetIdBAT:
		process(&msg->BAT);
	default:
		break; //discard...
	}
}

void DJIParserV1::process(DJIMessageGPS* msg)
{
	SensorData.SetDateTime(msg->DateTime.Years, msg->DateTime.Months, msg->DateTime.Days, msg->DateTime.Hours,
			msg->DateTime.Minutes, msg->DateTime.Seconds);

	if ((msg->FixStatus & 0x02) && msg->FixType > 0)
		SensorData.SetFixType(GPSFixType::FixDGPS);
	else
		SensorData.SetFixType((GPSFixType) msg->FixType);

	SensorData.SetVdop(msg->DOP.Vertical / 100.0f);

	float ndop = msg->DOP.North / 100.0f;
	float edop = msg->DOP.East / 100.0f;
	SensorData.SetHdop(sqrtf(ndop * ndop + edop * edop));
}

void DJIParserV1::process(DJIMessageRAW* msg)
{
	if (SensorData.GetFCType() != FCType::Phantom)
		SensorData.SetBattery(msg->Voltage.Battery);

	if (SensorData.GetFCType() == FCType::Wookong)
	{
	}
	else
	{
		SensorData.SetSensorPresent(Sensors::Homeing, 1);

		SensorData.SetPositionHome(msg->HomePosition.Latitude / M_PI * 180, msg->HomePosition.Longitude / M_PI * 180);
		SensorData.SetHomeAltitude(msg->HomeAltitude - 20);
		SensorData.SetFlightMode((FlightMode) msg->FlightMode);
	}

	SensorData.SetArmed(msg->Armed);
	SensorData.SetThrottle(msg->ActualInput.Throttle);
	SensorData.SetRCChannels(msg->RChannels, MaxRCChannels);
	SensorData.SetMotorOuts(msg->MotorOut, MaxMotorOuts);
}

void DJIParserV1::process(DJIMessageBAT* msg)
{
	SensorData.SetFCType(FCType::Phantom);

	SensorData.SetSensorPresent(Sensors::Consumption, 1);
	SensorData.SetSensorPresent(Sensors::Current, 1);
	SensorData.SetSensorPresent(Sensors::Cells, 1);
	SensorData.SetSensorPresent(Sensors::Charge, 1);

	SensorData.SetBattery(msg->voltage);
	SensorData.SetCharge(msg->percentage_charge);
	SensorData.SetCurrent(-(msg->current / 1000.0f));
	SensorData.SetCellCount(3);
	SensorData.SetCell(0, msg->cell1);
	SensorData.SetCell(1, msg->cell2);
	SensorData.SetCell(2, msg->cell3);
	SensorData.SetTemperatur1(msg->temperature / 10.0f);
	SensorData.SetCapacity(msg->capacity_current);
}

} /* namespace Application */

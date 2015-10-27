/*
 DJI Naza (v1/V2 + PMU, Phantom) CAN data decoder library
 (c) Pawelsky 20150211
 Not for commercial use

 Requires FlexCan library v1.0 (or newer if compatible)
 https://github.com/teachop/FlexCAN_Library/releases/tag/v1.0

 Requires Teensy 3.1 board and CAN transceiver
 Complie with "CPU speed" set to "96MHz (overclock)"
 Refer to naza_can_decoder_wiring.jpg diagram for proper connection
 Connections can be greatly simplified using CAN bus and MicroSD or AllInOne shields by Pawelsky
 (see teensy_shields.jpg or teensy_aio_shield.jpg for installation and naza_can_decoder_wiring_shields.jpg or naza_can_decoder_wiring_aio_shield.jpg for wiring)
 */

#include "NazaCanDecoderLib.h"

NazaCanDecoderLib NazaCanDecoder;
FlexCAN CAN(1000000);

const CAN_message_t NazaCanDecoderLib::HEARTBEAT_1 = { 0x108, 0, 8, 0, { 0x55, 0xAA, 0x55, 0xAA, 0x07, 0x10, 0x00, 0x00 } };
const CAN_message_t NazaCanDecoderLib::HEARTBEAT_2 = { 0x108, 0, 4, 0, { 0x66, 0xCC, 0x66, 0xCC } };
const CAN_filter_t NazaCanDecoderLib::FILTER_MASK = { 0, 0, 0x7FF };
const CAN_filter_t NazaCanDecoderLib::FILTER_090 = { 0, 0, 0x090 };
const CAN_filter_t NazaCanDecoderLib::FILTER_108 = { 0, 0, 0x108 };
const CAN_filter_t NazaCanDecoderLib::FILTER_7F8 = { 0, 0, 0x7F8 };

NazaCanDecoderLib::NazaCanDecoderLib()
{
	// Empty
}

void NazaCanDecoderLib::begin()
{
	CAN.begin(FILTER_MASK);
	for (int i = 0; i < 3; i++)
		CAN.setFilter(FILTER_090, i);
	for (int i = 3; i < 6; i++)
		CAN.setFilter(FILTER_108, i);
	for (int i = 6; i < 8; i++)
		CAN.setFilter(FILTER_7F8, i);
}

double NazaCanDecoderLib::getLat()
{
	return lat;
}
double NazaCanDecoderLib::getLon()
{
	return lon;
}
double NazaCanDecoderLib::getAlt()
{
	return alt;
}
double NazaCanDecoderLib::getGpsAlt()
{
	return gpsAlt;
}
double NazaCanDecoderLib::getSpeed()
{
	return spd;
}
NazaCanDecoderLib::fixType_t NazaCanDecoderLib::getFixType()
{
	return fix;
}
uint8_t NazaCanDecoderLib::getNumSat()
{
	return sat;
}
double NazaCanDecoderLib::getHeading()
{
	return heading;
}
double NazaCanDecoderLib::getHeadingNc()
{
	return headingNc;
}
double NazaCanDecoderLib::getCog()
{
	return cog;
}
double NazaCanDecoderLib::getVsi()
{
	return vsi;
}
double NazaCanDecoderLib::getGpsVsi()
{
	return gpsVsi;
}
double NazaCanDecoderLib::getHdop()
{
	return hdop;
}
double NazaCanDecoderLib::getVdop()
{
	return vdop;
}
int16_t NazaCanDecoderLib::getRoll()
{
	return roll;
}
int8_t NazaCanDecoderLib::getPitch()
{
	return pitch;
}
uint8_t NazaCanDecoderLib::getYear()
{
	return year;
}
uint8_t NazaCanDecoderLib::getMonth()
{
	return month;
}
uint8_t NazaCanDecoderLib::getDay()
{
	return day;
}
uint8_t NazaCanDecoderLib::getHour()
{
	return hour;
}
uint8_t NazaCanDecoderLib::getMinute()
{
	return minute;
}
uint8_t NazaCanDecoderLib::getSecond()
{
	return second;
}
uint16_t NazaCanDecoderLib::getBattery()
{
	return battery;
}
int16_t NazaCanDecoderLib::getRcIn(NazaCanDecoderLib::rcInChan_t chan)
{
	return rcIn[chan];
}
NazaCanDecoderLib::mode_t NazaCanDecoderLib::getMode()
{
	return mode;
}
#ifdef GET_SMART_BATTERY_DATA
uint8_t NazaCanDecoderLib::getBatteryPercent()
{	return batteryPercent;}
uint16_t NazaCanDecoderLib::getBatteryCell(NazaCanDecoderLib::smartBatteryCell_t cell)
{	return batteryCell[cell];}
#endif

uint16_t NazaCanDecoderLib::decode()
{
	uint16_t msgId = NAZA_MESSAGE_NONE;
	if (CAN.available())
	{
		CAN.read(canMsg);
		if (canMsg.id == 0x090)
			canMsgIdIdx = 0;
		else if (canMsg.id == 0x108)
			canMsgIdIdx = 1;
		else if (canMsg.id == 0x7F8)
			canMsgIdIdx = 2;
		else
			return msgId; // we don't care about other CAN messages

		for (uint8_t i = 0; i < canMsg.len; i++)
		{
			canMsgByte = canMsg.buf[i];
			if (collectData[canMsgIdIdx] == 1)
			{
				msgBuf[canMsgIdIdx].bytes[msgIdx[canMsgIdIdx]] = canMsgByte;
				if (msgIdx[canMsgIdIdx] == 3)
				{
					msgLen[canMsgIdIdx] = msgBuf[canMsgIdIdx].header.len;
				}
				msgIdx[canMsgIdIdx] += 1;
				if ((msgIdx[canMsgIdIdx] > (msgLen[canMsgIdIdx] + 8)) || (msgIdx[canMsgIdIdx] > 256))
					collectData[canMsgIdIdx] = 0;
			}

			// Look fo header
			if (canMsgByte == 0x55)
			{
				if (header[canMsgIdIdx] == 0)
					header[canMsgIdIdx] = 1;
				else if (header[canMsgIdIdx] == 2)
					header[canMsgIdIdx] = 3;
				else
					header[canMsgIdIdx] = 0;
			}
			else if (canMsgByte == 0xAA)
			{
				if (header[canMsgIdIdx] == 1)
					header[canMsgIdIdx] = 2;
				else if (header[canMsgIdIdx] == 3)
				{
					header[canMsgIdIdx] = 0;
					collectData[canMsgIdIdx] = 1;
					msgIdx[canMsgIdIdx] = 0;
				}
				else
					header[canMsgIdIdx] = 0;
			}
			else
				header[canMsgIdIdx] = 0;

			// Look fo footer
			if (canMsgByte == 0x66)
			{
				if (footer[canMsgIdIdx] == 0)
					footer[canMsgIdIdx] = 1;
				else if (footer[canMsgIdIdx] == 2)
					footer[canMsgIdIdx] = 3;
				else
					footer[canMsgIdIdx] = 0;
			}
			else if (canMsgByte == 0xCC)
			{
				if (footer[canMsgIdIdx] == 1)
					footer[canMsgIdIdx] = 2;
				else if (footer[canMsgIdIdx] == 3)
				{
					footer[canMsgIdIdx] = 0;
					if (collectData[canMsgIdIdx] != 0)
						collectData[canMsgIdIdx] = 2;
				}
				else
					footer[canMsgIdIdx] = 0;
			}
			else
				footer[canMsgIdIdx] = 0;

			if (collectData[canMsgIdIdx] == 2)
			{
				if (msgIdx[canMsgIdIdx] == (msgLen[canMsgIdIdx] + 8))
				{
					if (msgBuf[canMsgIdIdx].header.id == NAZA_MESSAGE_MSG1002)
					{
						float magCalX = msgBuf[canMsgIdIdx].msg1002.magCalX;
						float magCalY = msgBuf[canMsgIdIdx].msg1002.magCalY;
						headingNc = -atan2(magCalY, magCalX) / M_PI * 180.0;
						if (headingNc < 0)
							headingNc += 360.0;
						float headCompX = msgBuf[canMsgIdIdx].msg1002.headCompX;
						float headCompY = msgBuf[canMsgIdIdx].msg1002.headCompY;
						heading = atan2(headCompY, headCompX) / M_PI * 180.0;
						if (heading < 0)
							heading += 360.0;
						sat = msgBuf[canMsgIdIdx].msg1002.numSat;
						gpsAlt = msgBuf[canMsgIdIdx].msg1002.altGps;
						lat = msgBuf[canMsgIdIdx].msg1002.lat / M_PI * 180.0;
						lon = msgBuf[canMsgIdIdx].msg1002.lon / M_PI * 180.0;
						alt = msgBuf[canMsgIdIdx].msg1002.altBaro;
						float nVel = msgBuf[canMsgIdIdx].msg1002.northVelocity;
						float eVel = msgBuf[canMsgIdIdx].msg1002.eastVelocity;
						spd = sqrt(nVel * nVel + eVel * eVel);
						cog = atan2(eVel, nVel) / M_PI * 180;
						if (cog < 0)
							cog += 360.0;
						vsi = -msgBuf[canMsgIdIdx].msg1002.downVelocity;
						msgId = NAZA_MESSAGE_MSG1002;
					}
					else if (msgBuf[canMsgIdIdx].header.id == NAZA_MESSAGE_MSG1003)
					{
						uint32_t dateTime = msgBuf[canMsgIdIdx].msg1003.dateTime;
						second = dateTime & 0b00111111;
						dateTime >>= 6;
						minute = dateTime & 0b00111111;
						dateTime >>= 6;
						hour = dateTime & 0b00001111;
						dateTime >>= 4;
						day = dateTime & 0b00011111;
						dateTime >>= 5;
						if (hour > 7)
							day++;
						month = dateTime & 0b00001111;
						dateTime >>= 4;
						year = dateTime & 0b01111111;
						gpsVsi = -msgBuf[canMsgIdIdx].msg1003.downVelocity;
						vdop = (double) msgBuf[canMsgIdIdx].msg1003.vdop / 100;
						double ndop = (double) msgBuf[canMsgIdIdx].msg1003.ndop / 100;
						double edop = (double) msgBuf[canMsgIdIdx].msg1003.edop / 100;
						hdop = sqrt(ndop * ndop + edop * edop);
						uint8_t fixType = msgBuf[canMsgIdIdx].msg1003.fixType;
						uint8_t fixFlags = msgBuf[canMsgIdIdx].msg1003.fixStatus;
						switch (fixType)
						{
						case 2:
							fix = FIX_2D;
							break;
						case 3:
							fix = FIX_3D;
							break;
						default:
							fix = NO_FIX;
							break;
						}
						if ((fix != NO_FIX) && (fixFlags & 0x02))
							fix = FIX_DGPS;
						msgId = NAZA_MESSAGE_MSG1003;
					}
					else if (msgBuf[canMsgIdIdx].header.id == NAZA_MESSAGE_MSG1009)
					{
						for (uint8_t j = 0; j < 10; j++)
						{
							rcIn[j] = msgBuf[canMsgIdIdx].msg1009.rcIn[j];
						}
#ifndef GET_SMART_BATTERY_DATA
						battery = msgBuf[canMsgIdIdx].msg1009.batVolt;
#endif
						rollRad = msgBuf[canMsgIdIdx].msg1009.roll;
						pitchRad = msgBuf[canMsgIdIdx].msg1009.pitch;
						roll = (int16_t) (rollRad * 180.0 / M_PI);
						pitch = (int8_t) (pitchRad * 180.0 / M_PI);
						mode = (NazaCanDecoderLib::mode_t) msgBuf[canMsgIdIdx].msg1009.flightMode;
						msgId = NAZA_MESSAGE_MSG1009;
					}
#ifdef GET_SMART_BATTERY_DATA
					else if(msgBuf[canMsgIdIdx].header.id == NAZA_MESSAGE_MSG0926)
					{
						battery = msgBuf[canMsgIdIdx].msg0926.voltage;
						batteryPercent = msgBuf[canMsgIdIdx].msg0926.chargePercent;
						for(uint8_t j = 0; j < 3; j++)
						{
							batteryCell[j] = msgBuf[canMsgIdIdx].msg0926.cellVoltage[j];
						}
						msgId = NAZA_MESSAGE_MSG0926;
					}
#endif
				}
				collectData[canMsgIdIdx] = 0;
			}
		}
	}
	return msgId;
}

void NazaCanDecoderLib::heartbeat()
{
	uint32_t currentTime = millis();
	if (currentTime > heartbeatTime)
	{
		heartbeatTime = currentTime + 2000;  // 0.5Hz so every 2000 milliseconds
		CAN.write(HEARTBEAT_1);
		CAN.write(HEARTBEAT_2);
	}
}

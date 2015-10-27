/*
 DJI A2 CAN data decoder library
 (c) Pawelsky 20150213_beta
 Not for commercial use

 Requires FlexCan library v1.0 (or newer if compatible)
 https://github.com/teachop/FlexCAN_Library/releases/tag/v1.0

 Requires FastCRC library v1.0 (or newer if compatible)
 https://github.com/FrankBoesing/FastCRC/releases/tag/1.0

 Requires Teensy 3.1 board and CAN transceiver
 Complie with "CPU speed" set to "96MHz (overclock)"
 Refer to a2_can_decoder_wiring.jpg diagram for proper connection
 Connections can be greatly simplified using CAN bus and MicroSD or AllInOne shields by Pawelsky
 (see teensy_shields.jpg or teensy_aio_shield.jpg for installation and a2_can_decoder_wiring_shields.jpg or a2_can_decoder_wiring_aio_shield.jpg for wiring)

 Big thanks to Mis for help in decoding the A2 messages
 */

#include "A2CanDecoderLib.h"
#include "FastCRC.h"

A2CanDecoderLib A2CanDecoder;
FlexCAN CAN(1000000);

const CAN_filter_t A2CanDecoderLib::FILTER_MASK = { 0, 0, 0x7FF };
const CAN_filter_t A2CanDecoderLib::FILTER_092 = { 0, 0, 0x092 };
const CAN_filter_t A2CanDecoderLib::FILTER_388 = { 0, 0, 0x388 };
const CAN_filter_t A2CanDecoderLib::FILTER_7F8 = { 0, 0, 0x7F8 };

FastCRC16 CRC16;

A2CanDecoderLib::A2CanDecoderLib()
{
	// Empty
}

void A2CanDecoderLib::begin()
{
	CAN.begin(FILTER_MASK);
	for (int i = 0; i < 3; i++)
		CAN.setFilter(FILTER_092, i);
	for (int i = 3; i < 6; i++)
		CAN.setFilter(FILTER_388, i);
	for (int i = 6; i < 8; i++)
		CAN.setFilter(FILTER_7F8, i);
}

double A2CanDecoderLib::getLat()
{
	return lat;
}
double A2CanDecoderLib::getLon()
{
	return lon;
}
double A2CanDecoderLib::getAlt()
{
	return alt;
}
double A2CanDecoderLib::getGpsAlt()
{
	return gpsAlt;
}
double A2CanDecoderLib::getSpeed()
{
	return spd;
}
uint8_t A2CanDecoderLib::getNumSat()
{
	return sat;
}
double A2CanDecoderLib::getHeading()
{
	return heading;
}
double A2CanDecoderLib::getHeadingNc()
{
	return headingNc;
}
double A2CanDecoderLib::getCog()
{
	return cog;
}
double A2CanDecoderLib::getVsi()
{
	return vsi;
}
double A2CanDecoderLib::getGpsVsi()
{
	return gpsVsi;
}
uint8_t A2CanDecoderLib::getYear()
{
	return year;
}
uint8_t A2CanDecoderLib::getMonth()
{
	return month;
}
uint8_t A2CanDecoderLib::getDay()
{
	return day;
}
uint8_t A2CanDecoderLib::getHour()
{
	return hour;
}
uint8_t A2CanDecoderLib::getMinute()
{
	return minute;
}
uint8_t A2CanDecoderLib::getSecond()
{
	return second;
}
uint16_t A2CanDecoderLib::getBattery()
{
	return battery;
}
int16_t A2CanDecoderLib::getRcIn(A2CanDecoderLib::rcInChan_t chan)
{
	return rcIn[chan];
}
A2CanDecoderLib::mode_t A2CanDecoderLib::getMode()
{
	return mode;
}

uint16_t A2CanDecoderLib::decode()
{
	uint32_t msgId = A2_MESSAGE_NONE;
	if (CAN.available())
	{
		CAN.read(canMsg);
		if (canMsg.id == 0x092)
			canMsgIdIdx = 0;
		else if (canMsg.id == 0x388)
			canMsgIdIdx = 1;
		else if (canMsg.id == 0x7F8)
			canMsgIdIdx = 2;
		else
			return msgId; // we don't care about other CAN messages

		for (uint8_t i = 0; i < canMsg.len; i++)
		{
			canMsgByte = canMsg.buf[i];
			if ((decodingState[canMsgIdIdx] == 0) && (canMsgByte == 0x55))
			{
				decodingState[canMsgIdIdx] = 1;
				msgIdx[canMsgIdIdx] = 0;
				msgLen[canMsgIdIdx] = 8;
			}
			else if ((decodingState[canMsgIdIdx] == 1) && ((canMsgByte == 0x4C) || (canMsgByte == 0x80) || (canMsgByte == 0xB5)))
				decodingState[canMsgIdIdx] = 2;
			else if ((decodingState[canMsgIdIdx] == 2) && (canMsgByte == 0x00))
				decodingState[canMsgIdIdx] = 3;
			else if ((decodingState[canMsgIdIdx] == 3) && ((canMsgByte == 0x0D) || (canMsgByte == 0x26) || (canMsgByte == 0xF4)))
				decodingState[canMsgIdIdx] = 4;
			else if ((decodingState[canMsgIdIdx] == 4) && ((canMsgByte == 0x02) || (canMsgByte == 0x03) || (canMsgByte == 0x44)))
				decodingState[canMsgIdIdx] = 5;
			else if (decodingState[canMsgIdIdx] == 5)
			{
				if (canMsgByte == 0x10)
					decodingState[canMsgIdIdx] = 6;
				else if (canMsgByte == 0x4A)
					decodingState[canMsgIdIdx] = 7;
				else
					decodingState[canMsgIdIdx] = 0;
			}
			else if ((decodingState[canMsgIdIdx] == 7) && (canMsgByte == 0x49))
				decodingState[canMsgIdIdx] = 8;
			else if ((decodingState[canMsgIdIdx] == 8) && (canMsgByte == 0xA2))
				decodingState[canMsgIdIdx] = 9;
			else if ((decodingState[canMsgIdIdx] == 6) || (decodingState[canMsgIdIdx] == 9))
			{ /* keep collecting data */
			}
			else
				decodingState[canMsgIdIdx] = 0;

			if (decodingState[canMsgIdIdx] > 0)
			{
				msgBuf[canMsgIdIdx].bytes[msgIdx[canMsgIdIdx]] = canMsgByte;
				if (msgIdx[canMsgIdIdx] == 2)
				{
					msgLen[canMsgIdIdx] = msgBuf[canMsgIdIdx].header.len;
				}
				msgIdx[canMsgIdIdx] += 1;
				if (msgIdx[canMsgIdIdx] > 255)
					decodingState[canMsgIdIdx] = 0;
			}

			if ((decodingState[canMsgIdIdx] > 2) && (msgIdx[canMsgIdIdx] == msgLen[canMsgIdIdx]))
			{
				if (decodingState[canMsgIdIdx] == 6)
				{
					if ((msgBuf[canMsgIdIdx].header1.id == A2_MESSAGE_MSG1002)
							&& (msgBuf[canMsgIdIdx].msg1002.checksum
									== CRC16.generic(0x1021, 0X496C, CRC_FLAG_REFLECT, msgBuf[canMsgIdIdx].bytes,
											msgBuf[canMsgIdIdx].header.len - 2)))
					{
						// Apply XOR mast to payload bytes
						uint8_t mask = msgBuf[canMsgIdIdx].msg1002.unk5;
						for (uint8_t j = 6; j < msgBuf[canMsgIdIdx].header1.header.len - 2; j++)
							msgBuf[canMsgIdIdx].bytes[j] ^= mask;
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
						msgId = A2_MESSAGE_MSG1002;
					}
					if ((msgBuf[canMsgIdIdx].header1.id == A2_MESSAGE_MSG1003)
							&& (msgBuf[canMsgIdIdx].msg1003.checksum
									== CRC16.generic(0x1021, 0X496C, CRC_FLAG_REFLECT, msgBuf[canMsgIdIdx].bytes,
											msgBuf[canMsgIdIdx].header.len - 2)))
					{
						uint32_t dateTime = msgBuf[canMsgIdIdx].msg1003.date;
						year = (uint8_t) (dateTime / 10000 - 2000);
						dateTime -= (year + 2000) * 10000;
						month = (uint8_t) (dateTime / 100);
						day = (uint8_t) (dateTime - month * 100);
						dateTime = msgBuf[canMsgIdIdx].msg1003.time;
						hour = (uint8_t) (dateTime / 10000);
						dateTime -= hour * 10000;
						minute = (uint8_t) (dateTime / 100);
						second = (uint8_t) (dateTime - minute * 100);
						gpsVsi = -msgBuf[canMsgIdIdx].msg1003.downVelocity / 100.0;
						msgId = A2_MESSAGE_MSG1003;
					}
				}
				else if (decodingState[canMsgIdIdx] == 9)
				{
					if ((msgBuf[canMsgIdIdx].header2.id == A2_MESSAGE_MSGDJIA2)
							&& (msgBuf[canMsgIdIdx].msgDjiA2.checksum
									== CRC16.generic(0x1021, 0X496C, CRC_FLAG_REFLECT, msgBuf[canMsgIdIdx].bytes,
											msgBuf[canMsgIdIdx].header.len - 2)))
					{
						for (uint8_t j = 0; j < 8; j++)
						{
							rcIn[j] = msgBuf[canMsgIdIdx].msgDjiA2.rcIn[j] / 10;
						}
						mode = (A2CanDecoderLib::mode_t) msgBuf[canMsgIdIdx].msgDjiA2.flightMode;
						battery = msgBuf[canMsgIdIdx].msgDjiA2.batVolt;
						msgId = A2_MESSAGE_MSGDJIA2;
					}
				}
				decodingState[canMsgIdIdx] = 0;
			}
		}
	}
	return msgId;
}

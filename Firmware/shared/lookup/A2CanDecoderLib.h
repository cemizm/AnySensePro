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
 */

#ifndef __A2_CAN_DECODER_LIB_H__
#define __A2_CAN_DECODER_LIB_H__

#include "Arduino.h"
#include "FlexCAN.h"

#define A2_MESSAGE_COUNT    3
#define A2_MESSAGE_NONE     0x0000
#define A2_MESSAGE_MSG1002  0x1002
#define A2_MESSAGE_MSG1003  0x1003
#define A2_MESSAGE_MSGDJIA2 0xA2494A44

// IO ports (for AllInOne Board v0.2)
#define IO_1 A9
#define IO_2 A8
#define IO_3 A7
#define IO_4 A6

class A2CanDecoderLib
{
public:
	typedef enum
	{
		MANUAL = 0, ATTI = 1, GPS = 2, FAILSAFE = 4
	} mode_t;  // Flying mode
	typedef enum
	{
		RC_A = 0, RC_E = 1, RC_T = 2, RC_R = 3, RC_U = 4, RC_IOC = 5, RC_HOME = 6, RC_D4 = 7
	} rcInChan_t;  // RC channel index

	A2CanDecoderLib();       // Class constructor
	void begin();            // Start the A2CanDecoder
	uint16_t decode();       // Decode incoming CAN message if any (shall be called in a loop)
	double getLat();        // Returns latitude in degree decimal
	double getLon();        // Returns longitude in degree decimal
	double getAlt();        // Returns altitude in m (from barometric sensor)
	double getGpsAlt();     // Returns altitude in m (from GPS)
	double getSpeed();      // Returns speed in m/s
	uint8_t getNumSat();     // Returns number of satellites
	double getHeading();    // Returns heading in degrees (titlt compensated)
	double getHeadingNc();  // Returns heading in degrees (not titlt compensated)
	double getCog();        // Returns course over ground
	double getVsi();        // Returns vertical speed (from barometric sensor) in m/s (a.k.a. climb speed)
	double getGpsVsi();     // Returns vertical speed (from GPS) in m/s (a.k.a. climb speed)
	uint8_t getYear();       // Returns year from GPS (minus 2000)
	uint8_t getMonth();      // Returns month from GPS
	uint8_t getDay();        // Returns day from GPS
	uint8_t getHour();       // Returns hour from GPS (GMT+7)
	uint8_t getMinute();     // Returns minute from GPS
	uint8_t getSecond();     // Returns second from GPS
	uint16_t getBattery();   // Returns battery voltage in mV
	int16_t getRcIn(rcInChan_t chan); // Returns RC stick input (-1000~1000), use rcInChan_t enum to index the table
	mode_t getMode();        // Returns flight mode (see mode_t enum)

private:
	typedef struct
		__attribute__((packed))
		{
			uint8_t startByte;
			uint16_t len;
			uint8_t unk1;
		} a2_msg_header_t;

		typedef struct
			__attribute__((packed))
			{
				a2_msg_header_t header;
				uint16_t id;
			} a2_msg_header_1_t;

			typedef struct
				__attribute__((packed))
				{
					a2_msg_header_t header;
					uint32_t id;
				} a2_msg_header_2_t;

				typedef struct
					__attribute__((packed))
					{
						a2_msg_header_1_t header;
						double lon;           // longitude (radians)
						double lat;           // lattitude (radians)
						float altGps;        // altitude from GPS (meters)
						float accX;          // accelerometer X axis data (??)
						float accY;          // accelerometer Y axis data (??)
						float accZ;          // accelerometer Z axis data (??)
						float unk0[3];
						float altBaro;       // altitude from barometric sensor (meters)
						float headCompX;     // compensated heading X component
						float unk1[2];
						float headCompY;     // compensated heading Y component
						float unk2[3];
						float northVelocity; // averaged northward velocity or 0 when less than 5 satellites locked (m/s)
						float eastVelocity;  // averaged eastward velocity or 0 when less than 5 satellites locked (m/s)
						float downVelocity;  // downward velocity (barometric) (m/s)
						float unk3[3];
						int16_t magCalX;       // calibrated magnetometer X axis data
						int16_t magCalY;       // calibrated magnetometer Y axis data
						int16_t magCalZ;       // calibrated magnetometer Y axis data
						uint8_t unk4[10];
						uint8_t numSat;        // number of locked satellites
						uint8_t unk5;
						uint16_t seqNum;        // sequence number - increases with every message
						uint16_t checksum;      // checksum
					} a2_msg1002_t;

					typedef struct
						__attribute__((packed))
						{
							a2_msg_header_1_t header;
							uint32_t date;          // GPS date (year * 10000 + month * 100 + day)
							uint32_t time;          // GPS time (hour * 10000 + minute * 100 + second)
							uint32_t lon;           // longitude (x10^7, degree decimal)
							uint32_t lat;           // lattitude (x10^7, degree decimal)
							uint32_t altGps;        // altitude from GPS (millimeters)
							float northVelocity; // northward velocity (cm/s)
							float eastVelocity;  // eastward velocity (cm/s)
							float downVelocity;  // downward velocity (cm/s)
							float unk1[6];
							uint8_t unk2[8];
							uint8_t numSat;        // number of locked satellites
							uint8_t unk3;
							uint16_t seqNum;        // sequence number - increases with every message
							uint16_t checksum;      // checksum
						} a2_msg1003_t;

						typedef struct
							__attribute__((packed))
							{
								a2_msg_header_2_t header;
								uint32_t seqNum;        // sequence number - increases with every message
								uint16_t rcIn[8];       // RC controller input (order: A/E/R/T/U/IOC/GO_HOME/D4)
								uint8_t unk1[58];
								uint32_t motorOut[8];   // motor output (M1/M2/M3/M4/M5/M6/M7/M8)
								uint8_t unk2[6];
								uint8_t flightMode;    // (0 - manual, 1 - ATTI, 2 - GPS, 4 - FAILSAFE)
								uint8_t unk3[48];
								uint16_t batVolt;       // main battery voltage (milivolts)
								uint16_t canVolt;       // CAN voltage (milivolts)
								uint16_t becVolt;       // BEC voltage (milivolts)
								uint16_t checksum;      // checksum
							} a2_msgDjiA2_t;

							typedef union
							{
								uint8_t bytes[256]; // Max message size = 181
								a2_msg_header_t header;
								a2_msg_header_1_t header1;
								a2_msg_header_2_t header2;
								a2_msg1002_t msg1002;
								a2_msg1003_t msg1003;
								a2_msgDjiA2_t msgDjiA2;
							} a2_msg_t;

							CAN_message_t canMsg;
							static const CAN_filter_t FILTER_MASK;
							static const CAN_filter_t FILTER_092;
							static const CAN_filter_t FILTER_388;
							static const CAN_filter_t FILTER_7F8;
							uint8_t canMsgIdIdx;
							char canMsgByte = 0;

							a2_msg_t msgBuf[A2_MESSAGE_COUNT];
							uint16_t msgLen[A2_MESSAGE_COUNT];
							uint16_t msgIdx[A2_MESSAGE_COUNT];
							uint8_t decodingState[A2_MESSAGE_COUNT];

							double lon;       // longitude in degree decimal
							double lat;       // latitude in degree decimal
							double alt;       // altitude in m (from barometric sensor)
							double gpsAlt;    // altitude in m (from GPS)
							double spd;       // speed in m/s
							uint8_t sat;       // number of satellites
							double heading;   // heading in degrees (titlt compensated)
							double headingNc; // heading in degrees (not titlt compensated)
							double cog;       // course over ground
							double vsi;       // vertical speed indicator (barometric) in m/s (a.k.a. climb speed)
							double gpsVsi;    // vertical speed indicator (GPS based) in m/s (a.k.a. climb speed)
							uint8_t year;
							uint8_t month;
							uint8_t day;
							uint8_t hour;
							uint8_t minute;
							uint8_t second;
							uint16_t battery; // battery voltage in mV
							int16_t rcIn[8];  // RC stick input (-1000~1000), use rcInChan_t enum to index the table
							mode_t mode;      // flight mode (see mode_t enum)
						};

						extern A2CanDecoderLib A2CanDecoder;  // The A2CanDecoder instance

#endif // __A2_CAN_DECODER_LIB_H__

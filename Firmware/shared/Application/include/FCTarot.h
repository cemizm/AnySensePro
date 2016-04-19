/*
 * FCTarot.h
 *
 *  Created on: 16.04.2016
 *      Author: cem
 */

#ifndef APPLICATION_SOURCE_FCTAROT_H_
#define APPLICATION_SOURCE_FCTAROT_H_

#include "USART.h"
#include "Interrupt.h"

#include "FCAdapter.h"

namespace App
{

/*
 *
 */
class FCTarot: public FCAdapter, public HAL::InterruptHandler
{
private:
	static const uint8_t TmpReceiveBufferSize = 32;
	static const uint8_t ReceiveBufferSize = 128;
	static const uint8_t PaketSize = 64;
	static const uint8_t PacketContentSize = 38;
	static const uint16_t StartSign = 0x6BD2;

	enum PaketType
	{
		GPS = 1,
		RC = 2,
		HomeData = 3,
	};

	struct GPSType
	{
		int32_t Longitude;
		int32_t Latitude;
		int16_t Altitude;
		int16_t Speed;
		int16_t Speed2;
		int16_t Climb;
		uint8_t unk1[2];
		uint16_t Voltage;
		uint8_t unk2[9];
		uint8_t Failsafe;
		uint8_t Flightmode;
		uint8_t unk3[2];
		uint8_t Satellites;
		uint8_t unk4[4];
	}__attribute__((packed, aligned(1)));

	struct RCType
	{
		int16_t GyroX;
		int16_t GyroY;
		int16_t GyroZ;
		int16_t AccX;
		int16_t AccY;
		int16_t AccZ;
		int16_t Altitude;
		int8_t Aileron;
		int8_t Elevator;
		int8_t Throttle;
		int8_t Rudder;
		int8_t Mode;
		int8_t unk1;
		int8_t GoHome;
		int8_t IOC;
		int8_t Gear;
		int8_t GimbalTilt;
		int8_t CamSwitch;
		int8_t InfoSwitch;
		int8_t unk2[4];
		uint8_t RCConnected;
		uint8_t Armed;
		uint8_t unk3[6];
	}__attribute__((packed, aligned(1)));

	struct HomeDataType
	{
		int32_t Longitude;
		int32_t Latitude;
		int16_t Altitude;
		uint8_t unk1[28];
	}__attribute__((packed, aligned(1)));

	struct Paket
	{
		uint16_t StartByte;
		uint8_t ColorR;
		uint8_t ColorG;
		uint8_t ColorB;
		int16_t Roll;
		int16_t Pitch;
		uint16_t Heading;
		int16_t Unk1[3];
		int16_t MagX;
		int16_t MagY;
		int16_t MagZ;
		PaketType Type;
		union
		{
			uint8_t ContentData[PacketContentSize];
			GPSType GPS;
			RCType RC;
			HomeDataType HomeData;
		};
		uint16_t CRC;
	}__attribute__((packed, aligned(1)));

	HAL::USART& m_usart;
	OSAL::EventFlag gotMsg;
	uint8_t m_tmpReceiveBuffer[TmpReceiveBufferSize];
	uint8_t m_tmpReceiveIndex;
	uint8_t m_receiveBuffer[ReceiveBufferSize];
	uint8_t m_receiveIndex;
	uint8_t m_processIndex;

	void Process(Paket& paket);
	void Process(GPSType& data);
	void Process(RCType& data);
	void Process(HomeDataType& data);

	uint16_t calculateChecksum(const uint8_t* data, uint8_t size);

public:
	FCTarot(HAL::USART& usart) :
			FCAdapter(), m_usart(usart), m_tmpReceiveBuffer(), m_tmpReceiveIndex(0), m_receiveBuffer(), m_receiveIndex(0), m_processIndex(
					0)
	{
	}

	void Init() override;
	void Run() override;
	void ISR() override;

	void DeInit();
};

} /* namespace App */

#endif /* APPLICATION_SOURCE_FCTAROT_H_ */

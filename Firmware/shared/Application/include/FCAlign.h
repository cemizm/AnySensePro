/*
 * FCAlign.h
 *
 *  Created on: 13.06.2016
 *      Author: cem
 */

#ifndef APPLICATION_SOURCE_FCALIGN_H_
#define APPLICATION_SOURCE_FCALIGN_H_

#include "USART.h"
#include "Interrupt.h"

#include "FCAdapter.h"

namespace App
{

/*
 *
 */
class FCAlign: public FCAdapter, public HAL::InterruptHandler
{

private:
	static const uint8_t TmpReceiveBufferSize = 16;
	static const uint8_t ReceiveBufferSize = 128;
	static const uint8_t StartSign = 0x41;

	enum StatusFlag
	{
		None = 0,
		Feets = 1,
		Flag2 = 2,
		AlarmBattery = 4,
		AlarmDistance = 8,
		AlarmAltitudeHigh = 16,
		AlarmAltitudeLow = 32,
		AlarmSatellites = 64,
		GPSFound = 128
	};

	enum AlignMode
	{
		AlignModeManual = 1,
		AlignModeAttitude = 2,
		AlignModeGPSVelocity = 3,
		AlignModeIOC = 4,
		AlignModeGPSAngular = 5,
		AlignModeFailsafeVelocity = 19,
		AlignModeFailsafeAngular = 21,
	};

	enum PaketType {
		PaketTypeOSD = 0x01,
		PaketTypeUnknown1 = 0x02,
	};

	struct Paket
	{
		uint8_t StartByte;
		PaketType Type;
		uint8_t PaketSize;

		StatusFlag Status;
		AlignMode Mode;
		uint8_t Satellites;
		uint8_t Voltage;
		int16_t Course;
		int16_t Pitch;
		int16_t Roll;
		int16_t Altitude;
		int32_t Longitude;
		int32_t Latitude;
		int16_t Speed;

		uint8_t unk2[4];

		uint8_t VerticalVelocity;
		uint16_t Time;
		uint16_t Distance;
		uint8_t Checksum;
	}__attribute__((packed, aligned(1)));


	HAL::USART& m_usart;
	OSAL::EventFlag gotMsg;
	uint8_t m_tmpReceiveBuffer[TmpReceiveBufferSize];
	uint8_t m_tmpReceiveIndex;
	uint8_t m_receiveBuffer[ReceiveBufferSize];
	uint8_t m_receiveIndex;
	uint8_t m_processIndex;

	void Process(Paket& paket);


	uint8_t calculateChecksum(const uint8_t* data, uint8_t size);

public:
	FCAlign(HAL::USART& usart) :
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

#endif /* APPLICATION_SOURCE_FCALIGN_H_ */

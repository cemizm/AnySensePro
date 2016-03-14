/*
 * SensorFrSky.h
 *
 *  Created on: 11.03.2016
 *      Author: cem
 */

#ifndef APPLICATION_INCLUDE_SENSORFRSKY_H_
#define APPLICATION_INCLUDE_SENSORFRSKY_H_

#include "USART.h"
#include "SensorAdapter.h"
#include "Interrupt.h"
#include "Queue.h"

namespace App
{

/*
 *
 */
class SensorFrSky: public SensorAdapter<SensorAdapterBase::Procotol::FrSky>, public HAL::InterruptHandler
{
private:
	static const uint8_t PollTime = delay_ms(12);
	static const uint8_t MaxSensors = 28;
	static const uint8_t RXPacketSize = 8;
	static const uint8_t TXPacketSize = 2;

	static const uint8_t HeaderDataFrame = 0x10;
	static const uint8_t HeaderPollFrame = 0x7E;

	const uint8_t SensorIds[MaxSensors] = { 0x00, 0xA1, 0x22, 0x83, 0xE4, 0x45, 0xC6, 0x67, 0x48, 0xE9, 0x6A, 0xCB, 0xAC, 0x0D,
			0x8E, 0x2F, 0xD0, 0x71, 0xF2, 0x53, 0x34, 0x95, 0x16, 0xB7, 0x98, 0x39, 0xBA, 0x1B };

	enum SensorValue
	{
		ALT = 0x0100,
		VARIO = 0x0110,
		CURR = 0x0200,
		VFAS = 0x0210,
		CELLS = 0x0300,
		T1 = 0x0400,
		T2 = 0x0410,
		RPM = 0x0500,
		FUEL = 0x0600,
		ACCX = 0x0700,
		ACCY = 0x0710,
		ACCZ = 0x0720,
		GPS_LONG_LATI = 0x0800,
		GPS_ALT = 0x0820,
		GPS_SPEED = 0x0830,
		GPS_COURS = 0x0840,
		GPS_TIME_DATE = 0x0850,
		A3 = 0x0900,
		A4 = 0x0910,
	};

	struct LipoData
	{
		uint8_t StartCell :4;
		uint8_t TotalCells :4;
		uint16_t Cell1 :12;
		uint16_t Cell2 :12;
	}__attribute__((packed, aligned(1)));

	struct Sensor
	{
		uint8_t Static;
		uint8_t SensorId;
		uint8_t Timeout;
		uint16_t ValueId;
		union
		{
			uint32_t Value;
			LipoData Lipo;
		};

		uint8_t IsActive()
		{
			return !Static && Timeout > 0;
		}
		void ResetTimeout()
		{
			if (Static)
				return;

			Timeout = 15;
		}
		void UpdateTimeout()
		{
			if (Static)
				return;

			Timeout--;
		}
	};

	union
	{
		uint8_t Data[TXPacketSize];
		struct
		{
			uint8_t Header;
			uint8_t Sensor;
		}__attribute__((packed, aligned(1)));
	} m_tx;

	union
	{
		uint8_t Data[RXPacketSize];
		struct
		{
			uint8_t Header;
			uint16_t Id;
			uint32_t Value;
			uint8_t crc;
		}__attribute__((packed, aligned(1)));
		uint8_t IsCRCValid()
		{
			uint16_t calc = 0;
			for (uint8_t i = 0; i < RXPacketSize; i++)
			{
				calc += Data[i]; //0-1FF
				calc += calc >> 8; //0-100
				calc &= 0x00FF;
			}

			return calc == 0x00FF;
		}
		uint8_t IsValid()
		{
			return Header == HeaderDataFrame && IsCRCValid();
		}
	} m_rx;

	uint8_t SendNextByte();
	uint8_t ReceiveNextByte();
	void UpdateSensor();
	void UpdateData();
	void UpdateCells();
	void UpdateCurrent();

	HAL::USART& m_usart;
	uint8_t m_rxLen;
	uint8_t m_txLen;

	uint8_t m_index;

	Sensor m_currentSensor;
	Utils::Queue<Sensor, 15> m_polling;
public:
	SensorFrSky(HAL::USART& usart);

	void Init() override;
	void Run() override;

	void ISR() override;
};

} /* namespace App */

#endif /* APPLICATION_INCLUDE_SENSORFRSKY_H_ */

/*
 * TelemetryJeti.h
 *
 *  Created on: 17.03.2016
 *      Author: cem
 */

#ifndef APPLICATION_INCLUDE_TELEMETRYJETI_H_
#define APPLICATION_INCLUDE_TELEMETRYJETI_H_

#include "TelemetryAdapter.h"

#include "stdlib.h"

#include "Timer.h"
#include "USART.h"
#include "Pin.h"
#include "Interrupt.h"

namespace App
{

/*
 *
 */
class TelemetryJeti: public HAL::InterruptHandler, public TelemetryAdapter<TelemetryProtocol::Jeti>
{
private:
	static const uint16_t Prescaler = 8;
	static const uint16_t Baudrate = 9800;

	static const uint8_t DataInterval = 19;
	static const uint8_t TextInterval = 2;

	static const uint8_t StartByte = 0x7E;
	static const uint8_t CRCPoly = 0x07;

	static const uint8_t TelemetryValues = 19;
	static const uint8_t MaxTelemetryCount = 15;
	static const uint8_t MaxLabelCount = 16;

	static const uint8_t MaxPacketSize = 29;
	static const uint8_t MaxHeaderSize = 8;
	static const uint8_t MaxCRCSize = 1;

	static const uint8_t MaxSimpleTextSize = 34;

	static const uint8_t MaxBufferSize = MaxPacketSize + MaxSimpleTextSize;

	static const uint8_t MaxLabelName = 10;
	static const uint8_t MaxLabelUnit = 3;

	static const uint8_t MaxContentSize = MaxPacketSize - MaxHeaderSize - MaxCRCSize;

	struct LabelType
	{
		uint8_t Name[MaxLabelName + 1];
		uint8_t Unit[MaxLabelUnit + 1];
	};

	static const LabelType Labels[TelemetryValues + 1];

	enum TelemetryValue
	{
		None = 0,
		Latitude = 1,
		Longitude = 2,
		Satellite = 3,
		GPS_Fix = 4,
		Flightmode = 5,
		Speed = 6,
		Distance = 7,
		Altitude = 8,
		VSpeed = 9,
		Compass = 10,
		COG = 11,
		HomeDirection = 12,
		Voltage = 13,
		Current = 14,
		Capacity = 15,
		LipoVoltage = 16,
		LipoTemp = 17,
		HomeLatitude = 18,
		HomeLongitude = 19,

	};

	struct ProtocolConfig
	{
		uint16_t IsValid;
		TelemetryValue Mapping[MaxTelemetryCount];
	};

	struct Type6
	{
		uint8_t Data :5;
		uint8_t Decimalpoint :2;
		uint8_t Sign :1;
	}__attribute__((packed, aligned(1)));

	struct Type14
	{
		uint16_t Data :13;
		uint8_t Decimalpoint :2;
		uint8_t Sign :1;
	}__attribute__((packed, aligned(1)));

	struct Type22
	{
		uint32_t Data :21;
		uint8_t Decimalpoint :2;
		uint8_t Sign :1;
	}__attribute__((packed, aligned(1)));

	struct Type30
	{
		uint32_t Data :29;
		uint8_t Decimalpoint :2;
		uint8_t Sign :1;
	}__attribute__((packed, aligned(1)));

	struct TypeGPS
	{
		uint16_t Minutes;
		uint16_t Degree :13;
		uint8_t IsLongitutde :1; //lat=0; lon=1;
		uint8_t IsSouthWest :1; //North/East = 0; South/West = 1
		uint8_t Sign :1; //unused??
	}__attribute__((packed, aligned(1)));

	struct DataPacket
	{
		uint8_t DataType :4;
		uint8_t Id :4;
		union
		{
			uint8_t Data[4];
			Type6 U6;
			Type14 U14;
			Type22 U22;
			Type30 U30;
			TypeGPS GPS;
		};

		uint8_t UpdateU6(uint8_t decimal, int8_t val)
		{
			DataType = 0;
			U6.Sign = val < 0;
			U6.Decimalpoint = decimal;
			U6.Data = abs(val);

			return 2;
		}
		uint8_t UpdateU14(uint8_t decimal, int16_t val)
		{
			DataType = 1;
			U14.Sign = val < 0;
			U14.Decimalpoint = decimal;
			U14.Data = abs(val);

			return 3;
		}
		uint8_t UpdateU22(uint8_t decimal, int32_t val)
		{
			DataType = 4;
			U22.Sign = val < 0;
			U22.Decimalpoint = decimal;
			U22.Data = abs(val);

			return 4;
		}
		uint8_t UpdateU30(uint8_t decimal, int32_t val)
		{
			DataType = 8;
			U30.Sign = val < 0;
			U30.Decimalpoint = decimal;
			U30.Data = abs(val);

			return 5;
		}

		uint8_t UpdateGPS(double degree, uint8_t isLon)
		{

			DataType = 9;

			GPS.IsLongitutde = isLon;
			GPS.IsSouthWest = degree < 0 ? 1 : 0;

			if (degree < 0)
				degree = -degree;

			uint16_t tmp = degree;

			GPS.Degree = tmp;
			tmp = (degree - tmp) * 60000;
			GPS.Minutes = tmp;

			return 5;
		}

	}__attribute__((packed, aligned(1)));

	struct TextPacket
	{
		uint8_t Id;
		uint8_t UnitLength :3;
		uint8_t DescriptionLength :5;
		uint8_t Label[MaxContentSize - 2];
	}__attribute__((packed, aligned(1)));

	struct
	{
		uint16_t SendBuffer[MaxBufferSize];
		uint16_t CurrentData;
		uint8_t BufferLength;
		uint8_t BufferPos;
		union
		{
			uint8_t Data[MaxPacketSize];
			struct
			{
				uint8_t Seperator; 			//FIX: 0x7E
				uint8_t ExId; 			//FIX: 0x9F Distinct identification of an EX packet, N could be an arbitrary number.
				uint8_t Length :6; 			//Length of a packet (number of bytes following)
				uint8_t Type :2; 			//Packet type; 1 – Data protocol, 0 - Text protocol
				uint16_t ManufactureId; 			//Upper part of a serial number, Manufacturer ID (Little Endian)
				uint16_t DeviceId; 			//Lower part of a serial number, Device ID (Little Endian)
				uint8_t Reserved; 			//FIX: 0x00 reserved
				uint8_t Content[MaxContentSize];
			}__attribute__((packed, aligned(1)));
		};
	} m_tx;

	HAL::USART& m_usart;
	const HAL::Pin& m_pin;
	HAL::Timer& m_timer;
	ProtocolConfig* m_config;
	uint8_t m_run;

	uint8_t m_bitPos;
	uint8_t m_parity;

	uint_fast32_t m_startTick;
	uint_fast32_t m_nextText;
	uint_fast32_t m_tick;

	uint8_t m_currentLabel;

	uint8_t m_currentValue;
	uint8_t m_tmpValue;

	void Reset();
	void UpdateBuffer();
protected:
	void Init() override;
	void Run(void) override;
	void ISR() override;
	void DeInit() override;
public:
	TelemetryJeti(HAL::USART& usart, HAL::Timer& timer) :
			m_usart(usart), m_pin(usart.GetTXPin()), m_timer(timer), m_config(nullptr), m_run(1), m_bitPos(0), m_parity(0), m_startTick(
					0), m_nextText(0), m_tick(0), m_currentLabel(0), m_currentValue(0), m_tmpValue(0)
	{
	}
}
;

} /* namespace App */

#endif /* APPLICATION_INCLUDE_TELEMETRYJETI_H_ */

/*
 * TelemetryFutaba.h
 *
 *  Created on: 10.03.2016
 *      Author: cem
 */

#ifndef APPLICATION_SOURCE_TELEMETRYFUTABA_H_
#define APPLICATION_SOURCE_TELEMETRYFUTABA_H_

#include <USART.h>
#include <Timer.h>

#include <string.h>

#include "TelemetryAdapter.h"
#include "OSAL.h"
#include "Interrupt.h"

namespace App
{

/*
 *
 */
class TelemetryFutaba: public HAL::InterruptHandler, public TelemetryAdapter<TelemetryProtocol::Futaba>
{
private:
	static const uint16_t Prescaler = 2;
	static const uint16_t Interval = 700;

	static const uint32_t Buadrate = 100000;
	static const uint8_t Timeout = 200;

	static const uint8_t FrameCount = 4;
	static const uint8_t SlotPerFrameCount = 8;
	static const uint8_t SlotCount = FrameCount * SlotPerFrameCount;
	static const uint8_t SlotSize = 2;
	static const uint8_t SlotDataSize = 3;

	static const uint8_t FrameSize = 25;
	static const uint8_t ChannelDataSize = 22;
	static const uint8_t ChannelSize = 11;

	static const uint8_t StartByte = 0x0F;
	static const uint8_t EndByte = 0x04;

	enum SlotValue
	{
		None = 0, Vario = 1, Kompass = 2, Current = 3, GPS = 4, Cell = 5, Sats = 6, Flightmode = 7, Temperatur = 8,
	};

	struct ProtocolConfig
	{
		uint16_t IsValid;
		SlotValue Slots[SlotCount];
	};

	struct GPSType
	{
		uint32_t UTC :17;
		uint32_t Latitude :26;
		uint8_t LatitudeSign :1;
		uint32_t Longitude :27;
		uint8_t LongitudeSign :1;
		uint16_t Speed :9;
		uint8_t GPSFix :1;
		uint8_t GPSStrength :2; //84
		uint16_t Unknown :12;
		uint32_t AltitudeU :6;
		uint32_t Altitude :13;
		uint8_t VarioU :3;
		uint16_t Vario :9;
		uint8_t BaroStatus :1;
	}__attribute__((packed, aligned(1)));

	struct
	{
		uint8_t Position;
		union
		{
			uint8_t Data[FrameSize];
			struct
			{
				uint8_t Start;
				union
				{
					uint8_t ChannelData[ChannelDataSize];
					struct
					{
						uint16_t Ch1 :ChannelSize;
						uint16_t Ch2 :ChannelSize;
						uint16_t Ch3 :ChannelSize;
						uint16_t Ch4 :ChannelSize;
						uint16_t Ch5 :ChannelSize;
						uint16_t Ch6 :ChannelSize;
						uint16_t Ch7 :ChannelSize;
						uint16_t Ch8 :ChannelSize;
						uint16_t Ch9 :ChannelSize;
						uint16_t Ch10 :ChannelSize;
						uint16_t Ch11 :ChannelSize;
						uint16_t Ch12 :ChannelSize;
						uint16_t Ch13 :ChannelSize;
						uint16_t Ch14 :ChannelSize;
						uint16_t Ch15 :ChannelSize;
						uint16_t Ch16 :ChannelSize;
					}__attribute__((packed, aligned(1))) Channels;
				};
				uint8_t Flags;
				uint8_t End;
			}__attribute__((packed, aligned(1)));
		};
		uint8_t IsValid()
		{
			return Start == StartByte && ((End & EndByte) == EndByte);
		}
		void Write(uint8_t data)
		{
			if (Position == FrameSize)
			{
				memmove(Data, &Data[1], FrameSize - 1);
				Position--;
			}

			Data[Position] = data;
			Position++;
		}
		void Clear()
		{
			Position = 0;
			memset(Data, 0, FrameSize);
		}
		uint8_t GetFrame()
		{
			return End >> 4;
		}
	} m_rx;

	struct
	{
		uint8_t CurrentFrame;
		uint8_t CurrentSlot;

		uint8_t CurrentData[SlotDataSize];

		uint32_t SlotData;
		uint8_t DataFrame[FrameCount][SlotPerFrameCount][SlotSize];

		uint32_t HasData()
		{
			uint32_t hasData = SlotData & GetFlag(CurrentFrame, CurrentSlot);
			if (hasData)
				UpdateCurrentSlot();

			return hasData;
		}
		uint8_t HasMoreSlots()
		{
			return CurrentSlot < SlotPerFrameCount;
		}
		void UpdateCurrentSlot()
		{
			static uint8_t SlotNumbers[SlotCount] = { 0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3,
					0x33, 0xB3, 0x73, 0xF3, 0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB,
					0x7B, 0xFB };

			CurrentData[0] = SlotNumbers[CurrentFrame * SlotPerFrameCount + CurrentSlot];
			memcpy(&CurrentData[1], DataFrame[CurrentFrame][CurrentSlot], SlotSize);
		}
		void SetHasData(uint8_t frame, uint8_t slot, uint8_t hasData, uint8_t size = 1)
		{
			for (uint8_t start = 0; start < size; start++)
			{
				SlotData &= ~GetFlag(frame, slot + start);
				SlotData |= GetFlag(frame, slot + start, hasData);
			}
		}
		inline uint32_t GetFlag(uint8_t frame, uint8_t slot, uint8_t hasData = 1)
		{
			return hasData << (frame * SlotPerFrameCount + slot);
		}
	} m_tx;

	HAL::USART& m_usart;
	HAL::Timer& m_timer;
	ProtocolConfig* m_config;
	uint8_t m_run;
	void Reset();

	void UpdateFrame(uint8_t frame);
	void UpdateVario(uint8_t* vario);
	void UpdateCurrent(uint8_t* current);
	void UpdateTemperatur(uint8_t* temp, uint8_t value);
	void UpdateRPM(uint8_t* rpm, uint16_t value);
	void UpdateGPS(GPSType* gps);
	void UpdateVoltage(uint8_t* voltage);
	void SendNextSlot();

protected:
	void Init() override;
	void Run(void) override;
	void ISR() override;
	void DeInit() override;
public:
	TelemetryFutaba(HAL::USART& usart, HAL::Timer& timer) :
			m_usart(usart), m_timer(timer), m_config(nullptr), m_run(1)
	{
	}
}
;

} /* namespace App */

#endif /* APPLICATION_SOURCE_TELEMETRYFUTABA_H_ */

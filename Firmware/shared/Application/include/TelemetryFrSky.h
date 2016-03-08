/*
 * TelemetryFrSky.h
 *
 *  Created on: 07.03.2016
 *      Author: cem
 */

#ifndef APPLICATION_INCLUDE_TELEMETRYFRSKY_H_
#define APPLICATION_INCLUDE_TELEMETRYFRSKY_H_

#include <USART.h>

#include "TelemetryAdapter.h"
#include "OSAL.h"
#include "Interrupt.h"
#include "Configuration.h"

namespace App
{

class TelemetryFrSky: public HAL::InterruptHandler, public TelemetryAdapterImpl<TelemetryProtocol::FrSky>
{
private:
	static const uint8_t PacketSize = 8;
	static const uint8_t DataStart = 0x7E;
	static const uint8_t DataFrame = 0x10;

	enum RXState
	{
		Start, Sensor, Frame
	};

	enum Sensors
	{
		Vario = 0x00, //Physical 01 - Vario
		FLVSS = 0xA1, //Physical 02 - FLVSS
		FCS40A = 0x22, //Physical 03 - FCS 40A
		GPS = 0x83, //Physical 04 - GPS
		RPM = 0xE4, //Physical 05 - RPM
		SP2UARTH = 0x45, //Physical 06 - SP2UART (Host)
		SP2UARTR = 0xC6, //Physical 07 - SP2UART (Remote)
		FCS150A = 0x67, //Physical 08 - FCS 150A
	};

	enum SensorValue
	{
		ALT = 0x0100, //-
		VARIO = 0x0110, //-
		CURR = 0x0200, //-
		VFAS = 0x0210, //-
		CELLS = 0x0300,
		T1 = 0x0400, //-
		T2 = 0x0410, //-
		SRPM = 0x0500, //-
		FUEL = 0x0600, //-
		ACCX = 0x0700, //-
		ACCY = 0x0710, //-
		ACCZ = 0x0720,
		GPS_LONG_LATI = 0x0800, //-
		GPS_ALT = 0x0820, //-
		GPS_SPEED = 0x0830,
		GPS_COURS = 0x0840,
		GPS_TIME_DATE = 0x0850,
		A3 = 0x0900,
		A4 = 0x0910,
		AIR_SPEED = 0x0a00,
		ANY_FLIGHTMODE = 0xCB00, //-
		ANY_SATS = 0xCB01, //-
		ANY_FIX = 0xCB02, //-
		ANY_ARMED = 0xCB03, //-
	};

	enum SensorValueMapping
	{
		None = 0, Satellites = 1, GPSFix = 2, FlightMode = 3, Combined = 4,
	};

	enum SensorMapping
	{
		MapAnySense = 0, MapVario = 1, MapFLVSS = 2, MapFCS = 3, MapGPS = 4, MapRPM = 5, Last = 6
	};

	struct ProtocolConfig
	{
		uint16_t IsValid;
		uint8_t SensorId;

		uint8_t VarioEnable;
		uint8_t FLVSSEnable;
		uint8_t FCSEnable;
		uint8_t GPSEnable;
		uint8_t RPMEnable;

		SensorValueMapping T1;
		SensorValueMapping T2;
		SensorValueMapping Fuel;
	};

	union
	{
		uint8_t Data[PacketSize];
		struct
		{
			uint8_t Header;
			uint16_t Id;
			int32_t Value;
			uint8_t crc;
		}__attribute__((packed, aligned(1)));
		void UpdateCRC()
		{
			uint16_t calc = 0;
			for (uint8_t i = 0; i < 7; i++)
			{
				calc += Data[i]; //0-1FF
				calc += calc >> 8; //0-100
				calc &= 0x00FF;
			}
			crc = 0xFF - calc;
		}
	} m_packet;

	HAL::USART& m_usart;
	uint8_t m_run;
	RXState m_state;
	uint8_t m_currentSensor;
	uint8_t m_sensorValue[SensorMapping::Last];
	ProtocolConfig* m_config;

	uint8_t UpdateValue();
	uint8_t UpdateAnySense();
	uint8_t UpdateVario();
	uint8_t UpdateFLVSS();
	uint8_t UpdateFCS();
	uint8_t UpdateGPS();
	uint8_t UpdateRPM();

	uint8_t UpdateSensorValueMapping(SensorValueMapping mapping);

	void Reset();

protected:
	void Init() override;
	void Run(void) override;
	void DeInit() override;
public:
	TelemetryFrSky(HAL::USART& usart) :
			TelemetryAdapterImpl(), m_usart(usart), m_run(1), m_state(RXState::Start), m_currentSensor(0), m_sensorValue(), m_config(
					nullptr)
	{
	}

	void ISR() override;
};

} /* namespace App */

#endif /* APPLICATION_INCLUDE_TELEMETRYFRSKY_H_ */

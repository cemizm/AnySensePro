/*
 * TelemetryMultiplex.h
 *
 *  Created on: 09.03.2016
 *      Author: cem
 */

#ifndef APPLICATION_INCLUDE_TELEMETRYMULTIPLEX_H_
#define APPLICATION_INCLUDE_TELEMETRYMULTIPLEX_H_

#include "USART.h"

#include "TelemetryAdapter.h"
#include "Interrupt.h"

namespace App
{

class TelemetryMultiplex: public TelemetryAdapter<TelemetryProtocol::Multiplex>, public HAL::InterruptHandler
{
private:
	static const uint8_t MaxTelemetryValues = 16;

	enum TelemetryValue
	{
		None = 0,
		Satellite = 1,
		GPSFix = 2,
		Flightmode = 3,
		Speed = 4,
		Distance = 5,
		Altitude = 6,
		VSpeed = 7,
		Compass = 8,
		COG = 9,
		HomeDirection = 10,
		Voltage = 11,
		Current = 12,
		Capacity = 13,
		LipoVoltage = 14,
		LipoTemp = 15,
		Charge=16,
	};

	enum UnitType
	{
		UnitVoltage = 1,		//0.1V -600 +600
		UnitCurrent = 2,		//0.1A -1000 +1000
		UnitVSpeed = 3, 		//0,1m/s -500 +500
		UnitSpeed = 4, 			//0,1km/h 0 +6000
		UnitRPM = 5, 			//100 1/min 0	+500
		UnitTemp = 6, 			//0,1°C -250 7000
		UnitDir = 7, 			//0,1 Grad (°) 0 3600
		UnitMeter = 8, 			//1m -500 2000
		UnitPercent = 9,		//1% Tank 0 +100
		UnitLQI = 10, 			//1% LQI 0 +100
		UnitCapacity = 11, 		//1mAh -16000 16000
		UnitLiquid = 12, 		//1mL 0 16000
		UnitDistance = 13, 		//0,1km 0 16000
	};

	struct ProtocolConfig
	{
		uint16_t IsValid;
		TelemetryValue Mapping[MaxTelemetryValues];
	};

	struct
	{
		uint8_t Count;
		uint8_t Address;
	} m_rx;

	union
	{
		uint8_t Data[3];
		struct
		{
			UnitType Unit :4;
			uint8_t Address :4;
			uint8_t Alarm :1;
			int16_t Value :15;

		}__attribute__((packed, aligned(1)));

		void SetValue(UnitType unit, int16_t value)
		{
			Unit = unit;
			Value = value;
		}

		void SetDistance(uint16_t value)
		{
			if (value > 2000)
				SetValue(UnitType::UnitDistance, value / 100);
			else
				SetValue(UnitType::UnitMeter, value);
		}
	} m_tx;

	HAL::USART& m_usart;
	ProtocolConfig* m_config;
	uint8_t m_run;

	void Reset();
protected:
	void Init() override;
	void Run(void) override;
	void ISR() override;
	void DeInit() override;
public:
	TelemetryMultiplex(HAL::USART& usart) :
		TelemetryAdapter(), m_usart(usart), m_config(), m_run(1)
	{
	}
};

}
/* namespace App */

#endif /* APPLICATION_INCLUDE_TELEMETRYMULTIPLEX_H_ */

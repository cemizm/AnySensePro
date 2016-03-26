/*
 * TelemetrySpektrum.h
 *
 *  Created on: 23.03.2016
 *      Author: cem
 */

#ifndef APPLICATION_INCLUDE_TELEMETRYSPEKTRUM_H_
#define APPLICATION_INCLUDE_TELEMETRYSPEKTRUM_H_

#include <math.h>

#include "I2C.h"

#include "TelemetryAdapter.h"
#include "Interrupt.h"
#include "Endianess.h"

namespace App
{

class TelemetrySpektrum: public TelemetryAdapter<TelemetryProtocol::Spektrum>, public HAL::InterruptHandler
{
private:
	static const uint8_t FrameSize = 16;
	static const uint8_t SensorDataSize = FrameSize - 2;
	static const uint8_t SensorCount = 11;

	static const uint8_t LipoCellCount = 6;
	const float CurrentResolutionFactor = 0.196791f;

	enum Sensor
	{
		HighCurrent = 0x03, 		//SPMA9590 SPMA9605
		PowerBox = 0x0A, 			//PBS4850
		Speed = 0x11, 				//SPMA9574
		Altitude = 0x12, 			//SPMA9575
		GpsLocation = 0x16,			//SPMA9587
		GpsStatus = 0x17,			//SPMA9587
		DualEnergy = 0x18, 			//SPMA9591
		BatteryGauge = 0x34,
		Lipo = 0x3A,
		Vario = 0x40, 				//SPMA9589
		Voltage = 0x7E,
	};

	struct ProtocolConfig
	{
		uint16_t IsValid;

		uint8_t CurrentEnable;
		uint8_t SpeedEnable;
		uint8_t AltitudeEnable;
		uint8_t GPSEnable;
		uint8_t DualEnergyEnable;
		uint8_t BatteryGaugeEnable;
		uint8_t LipoEnable;
		uint8_t VarioEnable;
		uint8_t PowerboxEnable;
		uint8_t VoltageEnable;
	};

	struct GPSDegree
	{
		uint16_t MinuteFraction;
		uint8_t Minute;
		uint8_t Degree;
		void Set(double dec)
		{
			if (dec < 0)
				dec *= -1;

			double degree = 0;
			double minute = modf(dec, &degree) * 60;
			double second = modf(minute, &minute) * 10000;
			modf(second, &second);

			Degree = Utils::Endianess::ToBCD((uint8_t) degree);
			Minute = Utils::Endianess::ToBCD((uint8_t) minute);
			MinuteFraction = Utils::Endianess::ToBCD((uint16_t) second);
		}
	}__attribute__((packed, aligned(1)));

	union SensorType
	{
		uint8_t Data[FrameSize];
		struct
		{
			Sensor Identifier;
			Sensor SIdentifier;
			union
			{
				uint8_t SensorData[SensorDataSize];
				struct
				{
					int16_t Current;					// Range: +/- 150A Resolution: 300A/2048 = 0.196791 A/tick
				}__attribute__((packed, aligned(1))) CurrentSensor;
				struct
				{
					uint16_t Speed;						// 1 km/h increments
					uint16_t MaxSpeed;					// 1 km/h increments
				}__attribute__((packed, aligned(1))) SpeedSensor;
				struct
				{
					int16_t Altitude;					// .1m increments
					int16_t MaxAltitude;				// .1m increments
				}__attribute__((packed, aligned(1))) AltitudeSensor;
				struct
				{
					uint16_t AltitudeLow;				// BCD, meters, format 3.1 (Low bits of alt)
					GPSDegree Latitude;					// BCD, format 4.4,
														// Degrees * 100 + minutes, < 100 degrees
					GPSDegree Longitude;					// BCD, format 4.4,
					// Degrees * 100 + minutes, flag --> > 99deg
					uint16_t Course;					// BCD, 3.1
					uint8_t HDOP;						// BCD, 1.1

					uint8_t IsNorth :1;
					uint8_t IsEast :1;
					uint8_t LongExceeds :1;
					uint8_t GPSFixValid :1;
					uint8_t GPSDataReceived :1;
					uint8_t GPSFix3D :1;
					uint8_t Reserved :1;
					uint8_t NegativeAlt :1;
				}__attribute__((packed, aligned(1))) GpsLocationSensor;
				struct
				{
					uint16_t Speed;						// BCD, knots, format 3.1
					uint8_t Seconds100th;
					uint8_t Seconds;
					uint8_t Minutes;
					uint8_t Hours;
					uint8_t Satellites;					// BCD, 0-99
					uint8_t AltitudeHigh;				// BCD, meters, format 2.0 (High bits alt)
				}__attribute__((packed, aligned(1))) GpsStatusSensor;
				struct
				{
					int16_t CurrentA;					// Instantaneous current, 0.01A (0-328.7A)
					int16_t CapacityA;					// Integrated mAh used, 0.1mAh (0-3276.6mAh)
					uint16_t VoltA;						// Voltage, 0.01VC (0-16.00V)
					int16_t CurrentB;					// Instantaneous current, 0.1A (0-3276.8A)
					int16_t CapacityB;					// Integrated mAh used, 1mAh (0-32.766Ah)
					uint16_t VoltB;						// Voltage, 0.01VC (0-16.00V)
				}__attribute__((packed, aligned(1))) DualEnergySensor;
				struct
				{
					int16_t CurrentA;					// Instantaneous current, 0.1A (0-3276.8A)
					int16_t CapacityA;					// Integrated mAh used, 1mAh (0-32.766Ah)
					uint16_t TempA;						// Temperature, 0.1C (0-150.0C,
														// 0x7FFF indicates not populated)
					int16_t CurrentB;					// Instantaneous current, 0.1A (0-6553.4A)
					int16_t CapacityB;					// Integrated mAh used, 1mAh (0-65.534Ah)
					uint16_t TempB;						// Temperature, 0.1C (0-150.0C,
														// 0x7FFF indicates not populated)
				}__attribute__((packed, aligned(1))) BatteryGaugeSensor;
				struct
				{
					uint8_t Cells[LipoCellCount];			// Voltage across cells
					uint16_t Temperatur;					// Temeprature 0.1C (0-655.34C)
				}__attribute__((packed, aligned(1))) LipoSensor;
				struct
				{
					int16_t Altitude;						// .1m increments
					int16_t Delta0250ms;					// delta last 250ms, 0.1m/s increments
					int16_t Delta0500ms;					// delta last 500ms, 0.1m/s increments
					int16_t Delta1000ms;					// delta last 1000ms
					int16_t Delta1500ms;					// delta last 1500ms
					int16_t Delta2000ms;					// delta last 2000ms
					int16_t Delta3000ms;					// delta last 3000ms
				}__attribute__((packed, aligned(1))) VarioSensor;
				struct
				{
					uint16_t Volt1;							// Volts, 0.01V
					uint16_t Volt2;							// Volts, 0.01V
					uint16_t Capacity1;						// Capacity in 1 mAh
					uint16_t Capacity2;						// Capacity in 1 mAh
					uint8_t Unused[5];
					uint8_t AlarmVoltage1 :1;
					uint8_t AlarmVoltage2 :1;
					uint8_t AlarmCapacity1 :1;
					uint8_t AlarmCapacity2 :1;
					uint8_t AlarmRPM :1;
					uint8_t AlarmTemperatur :1;
					uint8_t AlarmReserved :2;
				}__attribute__((packed, aligned(1))) PowerboxSensor;
				struct
				{
					uint16_t Pulse;							//Pulse leading edges in microseconds
					uint16_t Volt;							//0.01 increments
					int16_t Temperature;					//degree F
				}__attribute__((packed, aligned(1))) VoltageSensor;
			};
		}__attribute__((packed, aligned(1)));
	};
	SensorType SensorPool[SensorCount];

	HAL::I2C& m_i2c;
	ProtocolConfig* m_config;
	uint8_t m_run;

	void Reset();
	SensorType* SensorEnabled(Sensor id);

	void UpdateData(Sensor id, SensorType* sensor);

	void UpdateCurrentSensor(SensorType* sensor);
	void UpdateSpeedSensor(SensorType* sensor);
	void UpdateAltitudeSensor(SensorType* sensor);
	void UpdateGPSSensor(SensorType* sensor);
	void UpdateDualEnergySensor(SensorType* sensor);
	void UpdateBatteryGaugeSensor(SensorType* sensor);
	void UpdateLipoSensor(SensorType* sensor);
	void UpdateVarioSensor(SensorType* sensor);
	void UpdatePowerboxSensor(SensorType* sensor);
	void UpdateVoltageSensor(SensorType* sensor);
protected:
	void Init() override;
	void Run(void) override;
	void ISR() override;
	void DeInit() override;
public:
	TelemetrySpektrum(HAL::I2C& i2c) :
			TelemetryAdapter(), m_i2c(i2c), m_config(), m_run(1)
	{
	}
}
;

} /* namespace App */

#endif /* APPLICATION_INCLUDE_TELEMETRYSPEKTRUM_H_ */

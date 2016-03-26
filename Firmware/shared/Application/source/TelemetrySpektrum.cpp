/*
 * TelemetrySpektrum.cpp
 *
 *  Created on: 23.03.2016
 *      Author: cem
 */

#include "TelemetrySpektrum.h"
#include "string.h"

#include "SensorStore.h"
#include "System.h"
#include "DateTime.h"

namespace App
{

static_assert (sizeof(TelemetrySpektrum) <= TELEMETRY_WORKSPACE, "TelemetrySpektrum will not fit!");

void TelemetrySpektrum::Init()
{
	static_assert (sizeof(SensorPool) == FrameSize*SensorCount, "Tx Data exceeds FrameSize!");

	m_config = (ProtocolConfig*) Config.GetConfiguration().ProtocolSettings;

	if (m_config->IsValid != ConfigKey)
	{
		m_config->IsValid = ConfigKey;
		m_config->CurrentEnable = 1;
		m_config->SpeedEnable = 1;
		m_config->AltitudeEnable = 1;
		m_config->GPSEnable = 1;
		m_config->DualEnergyEnable = 1;
		m_config->BatteryGaugeEnable = 1;
		m_config->LipoEnable = 1;
		m_config->VarioEnable = 1;
		m_config->PowerboxEnable = 1;
		m_config->VoltageEnable = 1;
		Config.Save();
	}

	m_i2c.Reset();

	m_i2c.Init(GPIO_PUPD_PULLUP);

	m_i2c.SetOwnAddress1(0x03, 0);
	m_i2c.EnableOwnAddress1();

	m_i2c.SetOwnAddress2(0x00, I2C_OAR2_OA2MSK_NO_CMP);
	m_i2c.EnableOwnAddress2();

	m_i2c.DisableAckGeneration();
	m_i2c.EnableAnalogFilter();
	m_i2c.SetDigitalFilter(0);

	m_i2c.EnableStrechClock();

	HAL::InterruptRegistry.Enable(m_i2c.NVIC_IRQn, PriorityTelemetry, this);
	HAL::InterruptRegistry.Enable(m_i2c.GetTXDMA().NVIC_IRQn, PriorityTelemetry, this);

	m_i2c.EnableAddrMatchInterrupt();

	m_i2c.SetupTXDMA();

	m_i2c.Enable();
}
void TelemetrySpektrum::Run(void)
{
	do
	{
		eventFlag.wait();
	} while (m_run);

	Reset();
}

void TelemetrySpektrum::UpdateData(Sensor id, SensorType* sensor)
{
	if (sensor == nullptr)
		return;

	sensor->Identifier = id;

	switch (id)
	{
	case Sensor::Altitude:
		UpdateAltitudeSensor(sensor);
		break;
	case Sensor::BatteryGauge:
		UpdateBatteryGaugeSensor(sensor);
		break;
	case Sensor::DualEnergy:
		UpdateDualEnergySensor(sensor);
		break;
	case Sensor::GpsLocation:
	case Sensor::GpsStatus:
		UpdateGPSSensor(sensor);
		break;
	case Sensor::HighCurrent:
		UpdateCurrentSensor(sensor);
		break;
	case Sensor::Lipo:
		UpdateLipoSensor(sensor);
		break;
	case Sensor::PowerBox:
		UpdatePowerboxSensor(sensor);
		break;
	case Sensor::Speed:
		UpdateSpeedSensor(sensor);
		break;
	case Sensor::Vario:
		UpdateVarioSensor(sensor);
		break;
	case Sensor::Voltage:
		UpdateVoltageSensor(sensor);
		break;
	}
}

void TelemetrySpektrum::UpdateCurrentSensor(SensorType* sensor)
{

	sensor->CurrentSensor.Current = Utils::Endianess::ToBig((int16_t) (SensorData.GetCurrent() / CurrentResolutionFactor));
}
void TelemetrySpektrum::UpdateSpeedSensor(SensorType* sensor)
{
	sensor->SpeedSensor.Speed = Utils::Endianess::ToBig((int16_t) SensorData.GetSpeed());
	sensor->SpeedSensor.MaxSpeed = 0xFFFF;
}
void TelemetrySpektrum::UpdateAltitudeSensor(SensorType* sensor)
{
	sensor->AltitudeSensor.Altitude = Utils::Endianess::ToBig((int16_t) (SensorData.GetRelativeAltitude() * 10));
	sensor->AltitudeSensor.MaxAltitude = 0xFFFF;
}
void TelemetrySpektrum::UpdateGPSSensor(SensorType* sensor)
{
	if (sensor->Identifier == Sensor::GpsLocation)
	{
		const GPSPosition& position = SensorData.GetPositionCurrent();
		sensor->GpsLocationSensor.AltitudeLow = Utils::Endianess::ToBCD((uint16_t) (SensorData.GetRelativeAltitude() * 10));
		sensor->GpsLocationSensor.HDOP = Utils::Endianess::ToBCD((uint8_t) (SensorData.GetHdop() * 10));
		sensor->GpsLocationSensor.Latitude.Set(position.Latitude);
		sensor->GpsLocationSensor.Longitude.Set(position.Longitude);
		sensor->GpsLocationSensor.Course = Utils::Endianess::ToBCD((uint16_t) (SensorData.GetHeading() * 10));

		sensor->GpsLocationSensor.IsNorth = position.Latitude > 0;
		sensor->GpsLocationSensor.IsEast = position.Longitude > 0;
		sensor->GpsLocationSensor.LongExceeds = position.Longitude > 100;
		sensor->GpsLocationSensor.GPSFixValid = SensorData.GetFixType() > 0;
		sensor->GpsLocationSensor.GPSDataReceived = SensorData.GetSensorPresent(Sensors::SGPS);
		sensor->GpsLocationSensor.GPSFix3D = SensorData.GetFixType() >= GPSFixType::Fix3D;
		sensor->GpsLocationSensor.NegativeAlt = SensorData.GetRelativeAltitude() < 0;
	}
	else
	{
		const Utils::DateTime& dt = SensorData.GetDateTime();
		sensor->GpsStatusSensor.Speed = Utils::Endianess::ToBCD((uint16_t) (SensorData.GetSpeed() * 19.438));
		sensor->GpsStatusSensor.Satellites = Utils::Endianess::ToBCD(SensorData.GetSatellites());
		sensor->GpsStatusSensor.AltitudeHigh = Utils::Endianess::ToBCD((uint16_t) (SensorData.GetRelativeAltitude() / 1000.0f));
		sensor->GpsStatusSensor.Hours = Utils::Endianess::ToBCD(dt.GetHour());
		sensor->GpsStatusSensor.Minutes = Utils::Endianess::ToBCD(dt.GetMinute());
		sensor->GpsStatusSensor.Seconds = Utils::Endianess::ToBCD(dt.GetSecond());
	}
}
void TelemetrySpektrum::UpdateDualEnergySensor(SensorType* sensor)
{
	sensor->DualEnergySensor.VoltA = Utils::Endianess::ToBig((int16_t) (SensorData.GetBattery() / 10.0f));
	sensor->DualEnergySensor.CurrentA = Utils::Endianess::ToBig((int16_t) (SensorData.GetCurrent() / 100.0f));
	sensor->DualEnergySensor.CapacityA = Utils::Endianess::ToBig((int16_t) (SensorData.GetCapacity() / 10.0f));

	sensor->DualEnergySensor.VoltB = Utils::Endianess::ToBig((int16_t) (SensorData.GetCellLowest() / 10.0f));
	sensor->DualEnergySensor.CurrentB = 0xFFFF;
	sensor->DualEnergySensor.CapacityB = 0xFFFF;
}
void TelemetrySpektrum::UpdateBatteryGaugeSensor(SensorType* sensor)
{
	sensor->BatteryGaugeSensor.TempA = Utils::Endianess::ToBig((int16_t) (SensorData.GetTemperatur1() * 10));
	sensor->BatteryGaugeSensor.CurrentA = Utils::Endianess::ToBig((int16_t) (SensorData.GetCurrent() / 100.0f));
	sensor->BatteryGaugeSensor.CapacityA = Utils::Endianess::ToBig((int16_t) (SensorData.GetCapacity() / 10.0f));

	sensor->BatteryGaugeSensor.TempB = Utils::Endianess::ToBig((int16_t) (SensorData.GetTemperatur2() * 10));
	sensor->BatteryGaugeSensor.CurrentB = 0xFFFF;
	sensor->BatteryGaugeSensor.CapacityB = 0xFFFF;
}
void TelemetrySpektrum::UpdateLipoSensor(SensorType* sensor)
{
	for (uint8_t i = 0; i < LipoCellCount; i++)
		sensor->LipoSensor.Cells[i] = Utils::Endianess::ToBig((int16_t) (SensorData.GetCell(i) / 20.0f));

	sensor->LipoSensor.Temperatur = Utils::Endianess::ToBig((int16_t) (SensorData.GetTemperatur1() * 10));
}
void TelemetrySpektrum::UpdateVarioSensor(SensorType* sensor)
{
	sensor->VarioSensor.Altitude = Utils::Endianess::ToBig((int16_t) (SensorData.GetAltitude() * 10));
	sensor->VarioSensor.Delta0250ms = Utils::Endianess::ToBig((int16_t) (SensorData.GetVerticalSpeed() * 10));
	sensor->VarioSensor.Delta0500ms = 0xFFFF;
	sensor->VarioSensor.Delta1000ms = 0xFFFF;
	sensor->VarioSensor.Delta1500ms = 0xFFFF;
	sensor->VarioSensor.Delta2000ms = 0xFFFF;
	sensor->VarioSensor.Delta3000ms = 0xFFFF;
}
void TelemetrySpektrum::UpdatePowerboxSensor(SensorType* sensor)
{
	sensor->PowerboxSensor.Volt1 = Utils::Endianess::ToBig((int16_t) (SensorData.GetBattery() / 10.0f));
	sensor->PowerboxSensor.Capacity1 = Utils::Endianess::ToBig((int16_t) SensorData.GetCapacity());

	sensor->PowerboxSensor.Volt2 = Utils::Endianess::ToBig((int16_t) (SensorData.GetCellLowest() / 10.0f));
	sensor->PowerboxSensor.Capacity2 = 0xFFFF;
}
void TelemetrySpektrum::UpdateVoltageSensor(SensorType* sensor)
{
	sensor->VoltageSensor.Pulse = 0xFFFF;
	sensor->VoltageSensor.Temperature = Utils::Endianess::ToBig((int16_t) (SensorData.GetTemperatur1() * 10));
	sensor->VoltageSensor.Volt = Utils::Endianess::ToBig((int16_t) (SensorData.GetBattery() / 10.0f));
}

TelemetrySpektrum::SensorType* TelemetrySpektrum::SensorEnabled(Sensor id)
{
	if (id == Sensor::Altitude && m_config->AltitudeEnable)
		return &SensorPool[0];
	else if (id == Sensor::BatteryGauge && m_config->BatteryGaugeEnable)
		return &SensorPool[1];
	else if (id == Sensor::DualEnergy && m_config->DualEnergyEnable)
		return &SensorPool[2];
	else if (id == Sensor::GpsLocation && m_config->GPSEnable)
		return &SensorPool[3];
	else if (id == Sensor::GpsStatus && m_config->GPSEnable)
		return &SensorPool[4];
	else if (id == Sensor::HighCurrent && m_config->CurrentEnable)
		return &SensorPool[5];
	else if (id == Sensor::Lipo && m_config->LipoEnable)
		return &SensorPool[6];
	else if (id == Sensor::PowerBox && m_config->PowerboxEnable)
		return &SensorPool[7];
	else if (id == Sensor::Speed && m_config->SpeedEnable)
		return &SensorPool[8];
	else if (id == Sensor::Vario && m_config->VarioEnable)
		return &SensorPool[9];
	else if (id == Sensor::Voltage && m_config->VoltageEnable)
		return &SensorPool[10];

	return nullptr;
}

void TelemetrySpektrum::Reset()
{
	m_i2c.ClearTXDma();
	m_i2c.Disable();

	m_i2c.Reset();

	HAL::InterruptRegistry.Disable(m_i2c.NVIC_IRQn);
	HAL::InterruptRegistry.Disable(m_i2c.GetTXDMA().NVIC_IRQn);
}

void TelemetrySpektrum::ISR()
{
	if (m_i2c.GetInterruptSource(I2C_ISR_ADDR))
	{
		if (m_i2c.IsReadTransferDirection())
		{
			Sensor id = (Sensor) m_i2c.GetAddressMatched();
			SensorType* sensor = SensorEnabled(id);
			if (sensor != nullptr)
			{
				UpdateData(id, sensor);
				m_i2c.SendDma(sensor->Data, FrameSize);
				m_i2c.ClearInterruptFlag(I2C_ICR_ADDRCF);
			}
			else
				m_i2c.SoftwareReset();
		}
	}

	if (m_i2c.GetTXDMA().GetInterruptFlag(DMA_TCIF))
	{
		m_i2c.GetTXDMA().ClearInterruptFlags(DMA_TCIF);
	}
}

void TelemetrySpektrum::DeInit()
{
	m_run = 0;
	eventFlag.signal();
}

} /* namespace App */

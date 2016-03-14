/*
 * TelemetryFrSky.cpp
 *
 *  Created on: 07.03.2016
 *      Author: cem
 */

#include <TelemetryFrSky.h>
#include "Configuration.h"
#include "SensorStore.h"
#include "stdlib.h"

namespace App
{

static_assert (sizeof(TelemetryFrSky) <= TELEMETRY_WORKSPACE, "TelemetryFrSky will not fit!");

void TelemetryFrSky::Init()
{
	m_config = (ProtocolConfig*) Config.GetConfiguration().ProtocolSettings;

	if (m_config->IsValid != ConfigKey)
	{
		m_config->IsValid = ConfigKey;
		m_config->SensorId = 0xCB;
		m_config->VarioEnable = 1;
		m_config->FLVSSEnable = 1;
		m_config->FCSEnable = 1;
		m_config->GPSEnable = 1;
		m_config->RPMEnable = 1;

		m_config->T1 = SensorValueMapping::Satellites;
		m_config->T2 = SensorValueMapping::GPSFix;
		m_config->Fuel = SensorValueMapping::Combined;
		Config.Save();
	}

	m_packet.Header = DataFrame;

	eventFlag.clear();

	m_usart.Init(GPIO_PUPD_PULLDOWN);
	m_usart.SetBaudrate(57600);
	m_usart.SetStopbits(USART_STOPBITS_1);
	m_usart.SetDatabits(8);
	m_usart.SetFlowControl(USART_FLOWCONTROL_NONE);
	m_usart.SetParity(USART_PARITY_NONE);
	m_usart.SetMode(USART_MODE_TX_RX);

	m_usart.DisableOverrunDetection();

	HAL::InterruptRegistry.Enable(m_usart.NVIC_IRQn, 15, this);
	HAL::InterruptRegistry.Enable(m_usart.GetTXDMA().NVIC_IRQn, 15, this);

	m_usart.EnableTXInversion();
	m_usart.EnableRXInversion();
	m_usart.EnableHalfduplex();

	m_usart.SetupTXDMA();
	m_usart.EnableRxInterrupt();
	m_usart.Enable();
}

void TelemetryFrSky::Run(void)
{
	while (m_run)
	{
		eventFlag.wait();
		if (m_run)
		{
			if (UpdateValue())
			{
				m_packet.UpdateCRC();
				m_usart.SendDma(m_packet.Data, PacketSize);
			}
		}
	}
	Reset();
}

uint8_t TelemetryFrSky::UpdateValue()
{

	if (m_currentSensor == m_config->SensorId)
	{
		return UpdateAnySense();
	}
	else
	{
		switch (m_currentSensor)
		{
		case Sensors::Vario:
			return UpdateVario();
		case Sensors::FLVSS:
			return UpdateFLVSS();
		case Sensors::FCS40A:
			return UpdateFCS();
		case Sensors::GPS:
			return UpdateGPS();
		case Sensors::RPM:
			return UpdateRPM();
		}
	}

	return 0;
}
uint8_t TelemetryFrSky::UpdateAnySense()
{
	switch (m_sensorValue[SensorMapping::MapAnySense])
	{
	case 0:
		m_packet.Id = SensorValue::ANY_FLIGHTMODE;
		m_packet.Value = SensorData.GetFlightMode();
		break;
	case 1:
		m_packet.Id = SensorValue::ANY_FIX;
		m_packet.Value = SensorData.GetFCType();
		break;
	case 2:
		m_packet.Id = SensorValue::ANY_SATS;
		m_packet.Value = SensorData.GetSatellites();
		break;
	case 3:
		m_packet.Id = SensorValue::ANY_ARMED;
		m_packet.Value = SensorData.GetArmed();
		break;
	case 4:
		m_packet.Id = SensorValue::ACCX;
		m_packet.Value = SensorData.GetRoll() * 100;
		break;
	case 5:
		m_packet.Id = SensorValue::ACCY;
		m_packet.Value = SensorData.GetPitch() * 100;
		break;
	case 6:
		m_packet.Id = SensorValue::FUEL;
		UpdateSensorValueMapping(m_config->Fuel);
		break;
	}

	m_sensorValue[SensorMapping::MapAnySense] = (m_sensorValue[SensorMapping::MapAnySense] + 1) % 7;

	return 1;
}
uint8_t TelemetryFrSky::UpdateVario()
{
	if (!m_config->VarioEnable)
		return 0;

	switch (m_sensorValue[SensorMapping::MapVario])
	{
	case 0:
		m_packet.Id = SensorValue::ALT;
		m_packet.Value = SensorData.GetAltitude() * 100;
		break;
	case 1:
		m_packet.Id = SensorValue::VARIO;
		m_packet.Value = SensorData.GetVerticalSpeed() * 100;
		break;
	}

	m_sensorValue[SensorMapping::MapVario] = (m_sensorValue[SensorMapping::MapVario] + 1) % 2;

	return 1;
}
uint8_t TelemetryFrSky::UpdateFLVSS()
{
	if (!m_config->FLVSSEnable)
		return 0;

	m_packet.Id = SensorValue::CELLS;

	uint8_t cellCount = SensorData.GetCellCount();
	uint8_t cs = (cellCount / 2) + (cellCount % 2);
	uint8_t cell = m_sensorValue[SensorMapping::MapFLVSS] * 2;

	m_packet.Lipo.StartCell = cell;
	m_packet.Lipo.TotalCells = cellCount;
	m_packet.Lipo.Cell1 = SensorData.GetCell(cell) * 5 / 10;
	m_packet.Lipo.Cell2 = SensorData.GetCell(cell + 1) * 5 / 10;

	m_sensorValue[SensorMapping::MapFLVSS] = (m_sensorValue[SensorMapping::MapFLVSS] + 1) % cs;

	return 1;
}
uint8_t TelemetryFrSky::UpdateFCS()
{
	if (!m_config->FCSEnable)
		return 0;

	switch (m_sensorValue[SensorMapping::MapFCS])
	{
	case 0:
		m_packet.Id = SensorValue::CURR;
		m_packet.Value = SensorData.GetCurrent() * 10;
		break;
	case 1:
		m_packet.Id = SensorValue::VFAS;
		m_packet.Value = SensorData.GetBattery() / 10;
		break;
	}

	m_sensorValue[SensorMapping::MapFCS] = (m_sensorValue[SensorMapping::MapFCS] + 1) % 2;

	return 1;
}
uint8_t TelemetryFrSky::UpdateGPS()
{
	if (!m_config->GPSEnable)
		return 0;

	if (SensorData.GetFixType() < GPSFixType::Fix3D)
		return 0;

	GPSPosition pos = SensorData.GetPositionCurrent();
	GPSTime time = SensorData.GetDateTime();
	switch (m_sensorValue[SensorMapping::MapGPS])
	{
	case 0:
		m_packet.Id = SensorValue::GPS_LONG_LATI;
		m_packet.Value = (((abs(pos.Longitude * 10000000)) / 100) * 6);
		m_packet.Value |= pos.Longitude < 0 ? 0xC0000000 : 0x80000000;
		break;
	case 1:
		m_packet.Id = SensorValue::GPS_LONG_LATI;
		m_packet.Value = (((abs(pos.Latitude * 10000000)) / 100) * 6);
		m_packet.Value |= pos.Latitude < 0 ? 0x40000000 : 0x00000000;
		break;
	case 2:
		m_packet.Id = SensorValue::GPS_ALT;
		m_packet.Value = SensorData.GetAltitude() * 100;
		break;
	case 3:
		m_packet.Id = SensorValue::GPS_SPEED;
		m_packet.Value = SensorData.GetSpeed() * 1943;
		break;
	case 4:
		m_packet.Id = SensorValue::GPS_COURS;
		m_packet.Value = SensorData.GetHeading() * 100;
		break;
	case 5:
		m_packet.Id = SensorValue::GPS_TIME_DATE;
		m_packet.Value = time.Year << 24 | time.Month << 16 | time.Day << 8 | 0xff;
		break;
	case 6:
		m_packet.Id = SensorValue::GPS_TIME_DATE;
		m_packet.Value = time.Hour << 24 | time.Minute << 16 | time.Second << 8;
		break;
	}

	m_sensorValue[SensorMapping::MapGPS] = (m_sensorValue[SensorMapping::MapGPS] + 1) % 7;

	return 1;
}
uint8_t TelemetryFrSky::UpdateRPM()
{
	if (!m_config->RPMEnable)
		return 0;

	switch (m_sensorValue[SensorMapping::MapRPM])
	{
	case 0:
		m_packet.Id = SensorValue::SRPM;
		m_packet.Value = SensorData.GetRpm() * 10;
		break;
	case 1:
		m_packet.Id = SensorValue::T1;
		if (!UpdateSensorValueMapping(m_config->T1))
			m_packet.Value = SensorData.GetTemperatur1();
		break;
	case 2:
		m_packet.Id = SensorValue::T2;
		if (!UpdateSensorValueMapping(m_config->T2))
			m_packet.Value = SensorData.GetTemperatur2();
		break;
	}

	m_sensorValue[SensorMapping::MapRPM] = (m_sensorValue[SensorMapping::MapRPM] + 1) % 3;

	return 1;
}
uint8_t TelemetryFrSky::UpdateSensorValueMapping(SensorValueMapping mapping)
{
	switch (mapping)
	{
	case SensorValueMapping::Combined:
	{
		m_packet.Value = SensorData.GetSatellites();
		m_packet.Value += SensorData.GetFixType() * 100;
		m_packet.Value += SensorData.GetFlightMode() * 1000;
		m_packet.Value += SensorData.GetArmed() ? 10000 : 0;
		m_packet.Value += SensorData.IsPositionHomeSet() ? 20000 : 0;
	}
		return 1;
	case SensorValueMapping::FlightMode:
		m_packet.Value = SensorData.GetFlightMode();
		return 1;
	case SensorValueMapping::GPSFix:
		m_packet.Value = SensorData.GetFixType();
		return 1;
	case SensorValueMapping::Satellites:
		m_packet.Value = SensorData.GetSatellites();
		return 1;
	default:
		return 0;
	}
}

void TelemetryFrSky::ISR()
{
	if (m_usart.GetInterruptSource(USART_ISR_RXNE))
	{
		uint8_t read = m_usart.Receive();
		switch (m_state)
		{
		case RXState::Start:
			if (read == DataStart)
				m_state = RXState::Sensor;
			break;
		case RXState::Sensor:
			m_currentSensor = read;
			m_state = RXState::Start;
			eventFlag.signal_isr();
			break;
		default:
			m_state = RXState::Sensor;
			break;
		}
	}
	if (m_usart.GetTXDMA().GetInterruptFlag(DMA_TCIF))
	{
		m_usart.GetTXDMA().ClearInterruptFlags(DMA_TCIF);
		m_usart.GetTXDMA().DisableChannel();
	}
}

void TelemetryFrSky::Reset()
{
	m_usart.Disable();
	m_usart.ClearTXDma();

	m_usart.DeInit();

	HAL::InterruptRegistry.Disable(m_usart.NVIC_IRQn);
	HAL::InterruptRegistry.Disable(m_usart.GetTXDMA().NVIC_IRQn);
}

void TelemetryFrSky::DeInit()
{
	m_run = 0;
	eventFlag.signal();
}

} /* namespace App */

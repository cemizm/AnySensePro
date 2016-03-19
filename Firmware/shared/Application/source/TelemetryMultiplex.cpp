/*
 * TelemetryMultiplex.cpp
 *
 *  Created on: 09.03.2016
 *      Author: cem
 */

#include "TelemetryMultiplex.h"
#include "SensorStore.h"
#include "string.h"

namespace App
{

static_assert (sizeof(TelemetryMultiplex) <= TELEMETRY_WORKSPACE, "TelemetryMultiplex will not fit!");

void TelemetryMultiplex::Init()
{
	m_config = (ProtocolConfig*) Config.GetConfiguration().ProtocolSettings;

	if (m_config->IsValid != ConfigKey)
	{
		m_config->IsValid = ConfigKey;
		m_config->Mapping[0] = TelemetryValue::None;
		m_config->Mapping[1] = TelemetryValue::None;
		m_config->Mapping[2] = TelemetryValue::Distance;
		m_config->Mapping[3] = TelemetryValue::Altitude;
		m_config->Mapping[4] = TelemetryValue::Speed;
		m_config->Mapping[5] = TelemetryValue::VSpeed;
		m_config->Mapping[6] = TelemetryValue::Compass;
		m_config->Mapping[7] = TelemetryValue::COG;
		m_config->Mapping[8] = TelemetryValue::HomeDirection;
		m_config->Mapping[9] = TelemetryValue::Flightmode;
		m_config->Mapping[10] = TelemetryValue::Satellite;
		m_config->Mapping[11] = TelemetryValue::GPSFix;
		m_config->Mapping[12] = TelemetryValue::Voltage;
		m_config->Mapping[13] = TelemetryValue::LipoVoltage;
		m_config->Mapping[14] = TelemetryValue::Current;
		m_config->Mapping[15] = TelemetryValue::Capacity;

		Config.Save();
	}

	m_usart.Init(GPIO_PUPD_PULLUP);
	m_usart.SetBaudrate(38400);
	m_usart.SetStopbits(USART_STOPBITS_1);
	m_usart.SetDatabits(8);
	m_usart.SetFlowControl(USART_FLOWCONTROL_NONE);
	m_usart.SetParity(USART_PARITY_NONE);
	m_usart.SetMode(USART_MODE_TX_RX);
	m_usart.SetReceiveTimeout(15);

	HAL::InterruptRegistry.Enable(m_usart.NVIC_IRQn, PriorityTelemetry, this);
	HAL::InterruptRegistry.Enable(m_usart.GetTXDMA().NVIC_IRQn, PriorityTelemetry, this);

	m_usart.EnableHalfduplex();
	m_usart.DisableOverrunDetection();
	m_usart.EnableRxTimeout();

	m_usart.SetupTXDMA();
	m_usart.EnableRxInterrupt();
	m_usart.EnableRxTimeoutInterrupt();
	m_usart.Enable();
}
void TelemetryMultiplex::Run(void)
{
	while (m_run)
	{
		eventFlag.wait();
		if (m_run)
		{
			memset(m_tx.Data, 0, sizeof(m_tx.Data));
			m_tx.Address = m_rx.Address;
			switch (m_config->Mapping[m_rx.Address])
			{
			case TelemetryValue::Altitude:
				m_tx.SetValue(UnitType::UnitMeter, SensorData.GetRelativeAltitude());
				break;
			case TelemetryValue::Speed:
				m_tx.SetValue(UnitType::UnitSpeed, SensorData.GetSpeed() * 36);
				break;
			case TelemetryValue::Capacity:
				m_tx.SetValue(UnitType::UnitCapacity, SensorData.GetCapacity());
				break;
			case TelemetryValue::Distance:
				m_tx.SetDistance(SensorData.GetHomeDistance());
				break;
			case TelemetryValue::COG:
				m_tx.SetValue(UnitType::UnitDir, SensorData.GetCourseOverGround() * 10);
				break;
			case TelemetryValue::Compass:
				m_tx.SetValue(UnitType::UnitDir, SensorData.GetHeading() * 10);
				break;
			case TelemetryValue::Current:
				m_tx.SetValue(UnitType::UnitCurrent, SensorData.GetCurrent() * 10);
				break;
			case TelemetryValue::Flightmode:
				m_tx.SetValue(UnitType::UnitLiquid, SensorData.GetFlightMode());
				break;
			case TelemetryValue::GPSFix:
				m_tx.SetValue(UnitType::UnitLiquid, SensorData.GetFixType());
				break;
			case TelemetryValue::HomeDirection:
				m_tx.SetValue(UnitType::UnitDir, SensorData.GetHomeDirection() * 10);
				break;
			case TelemetryValue::LipoVoltage:
				m_tx.SetValue(UnitType::UnitVoltage, SensorData.GetCellLowest() / 100);
				break;
			case TelemetryValue::Satellite:
				m_tx.SetValue(UnitType::UnitLiquid, SensorData.GetSatellites());
				break;
			case TelemetryValue::VSpeed:
				m_tx.SetValue(UnitType::UnitVSpeed, SensorData.GetVerticalSpeed() * 10);
				break;
			case TelemetryValue::Voltage:
				m_tx.SetValue(UnitType::UnitVoltage, SensorData.GetBattery() / 100);
				break;
			case TelemetryValue::Charge:
				m_tx.SetValue(UnitType::UnitPercent, SensorData.GetCharge());
				break;
			case TelemetryValue::Temperatur1:
				m_tx.SetValue(UnitType::UnitTemp, SensorData.GetTemperatur1() * 10);
				break;
			case TelemetryValue::Temperatur2:
				m_tx.SetValue(UnitType::UnitTemp, SensorData.GetTemperatur2() * 10);
				break;
			case TelemetryValue::RPM:
				m_tx.SetValue(UnitType::UnitRPM, (SensorData.GetRpm() / 10) * -1);
				break;
			case TelemetryValue::None:
				break;
			}

			if (m_tx.Unit != 0)
				m_usart.SendDma(m_tx.Data, sizeof(m_tx.Data));
		}
	}
	Reset();
}
void TelemetryMultiplex::Reset()
{
	m_usart.Disable();
	m_usart.ClearTXDma();

	m_usart.DeInit();

	HAL::InterruptRegistry.Disable(m_usart.NVIC_IRQn);
	HAL::InterruptRegistry.Disable(m_usart.GetTXDMA().NVIC_IRQn);
}

void TelemetryMultiplex::ISR()
{
	if (m_usart.GetInterruptSource(USART_ISR_RXNE))
	{
		m_rx.Address = m_usart.Receive();
		m_rx.Count++;
	}
	if (m_usart.GetInterruptSource(USART_ISR_RTOF))
	{
		m_usart.ClearInterruptFlag(USART_ICR_RTOCF);
		if (m_rx.Count == 1 && (m_rx.Address & 0xF0) == 0x00 && m_config->Mapping[m_rx.Address] != TelemetryValue::None)
			eventFlag.signal_isr();
		else
			m_rx.Count = 0;
	}
	if (m_usart.GetTXDMA().GetInterruptFlag(DMA_TCIF))
	{
		m_usart.GetTXDMA().ClearInterruptFlags(DMA_TCIF);
		m_usart.GetTXDMA().DisableChannel();
		m_rx.Count = 0;
	}
}
void TelemetryMultiplex::DeInit()
{
	m_run = 0;
	eventFlag.signal();
}

} /* namespace App */

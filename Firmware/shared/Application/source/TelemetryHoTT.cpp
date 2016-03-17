/*
 * TelemetryHoTT.cpp
 *
 *  Created on: 08.03.2016
 *      Author: cem
 */

#include "TelemetryHoTT.h"
#include "Configuration.h"

#include "string.h"

namespace App
{

static_assert (sizeof(TelemetryHoTT) <= TELEMETRY_WORKSPACE, "TelemetryHoTT will not fit!");

//Manual = 0, GPS = 1, Failsafe = 2, Attitude = 3
//FixNo = 0, Fix2D = 2, Fix3D = 3, FixDGPS = 4

#define FMLABELLENGTH		12
#define FMLENGTH			9

static const char* FlightModeLabel = "Flightmode: "; //12 char,
static const char FlightModeLabels[][FMLENGTH] = { "Manual", "GPS", "Failsafe", "Attitude" }; //cannot exceed 9 chars

static const char GPSFix[] = { '-', ' ', '2', '3', 'D' };

void TelemetryHoTT::Init()
{
	m_config = (ProtocolConfig*) Config.GetConfiguration().ProtocolSettings;

	if (m_config->IsValid != ConfigKey)
	{
		m_config->IsValid = ConfigKey;

		m_config->EAMEnable = 1;
		m_config->GAMEnable = 1;
		m_config->GPSEnable = 1;
		m_config->VarioEnable = 1;

		Config.Save();
	}

	m_usart.Init(GPIO_PUPD_PULLUP);
	m_usart.SetBaudrate(19200);
	m_usart.SetStopbits(USART_STOPBITS_2);
	m_usart.SetDatabits(9);
	m_usart.SetFlowControl(USART_FLOWCONTROL_NONE);
	m_usart.SetParity(USART_PARITY_NONE);
	m_usart.SetMode(USART_MODE_TX_RX);
	m_usart.SetReceiveTimouet(92);

	HAL::InterruptRegistry.Enable(m_usart.NVIC_IRQn, PriorityTelemetry, this);
	HAL::InterruptRegistry.Enable(m_usart.GetTXDMA().NVIC_IRQn, PriorityTelemetry, this);

	m_usart.EnableHalfduplex();
	m_usart.DisableOverrunDetection();
	m_usart.EnableRxTimeout();

	m_usart.SetupTXDMA();
	m_usart.EnableRxInterrupt();
	m_usart.Enable();
}

void TelemetryHoTT::Run(void)
{
	while (m_run)
	{
		eventFlag.wait(0);
		if (m_run)
		{
			memset(m_tx.SensorData, 0, sizeof(m_tx.SensorData));

			switch (m_rx.Sensor)
			{
			case SensorType::SensorEAM:
				UpdateEAM();
				break;
			case SensorType::SensorGPS:
				UpdateGPS();
				break;
			case SensorType::SensorGAM:
				UpdateGAM();
				break;
			case SensorType::SensorVario:
				UpdateVario();
				break;
			}
		}
	}

	Reset();
}

void TelemetryHoTT::UpdateGPS()
{
	if (!m_config->GPSEnable)
		return;

	m_tx.Sensor = SensorType::SensorGPS;
	m_tx.SensorResponse = SensorResponseType::ResponseGPS;

	m_tx.GPS.AlarmDisplay = AlarmDisplayType::AlarmDisplayNone;
	m_tx.GPS.FlightDirection = SensorData.GetHeading() / 2;
	m_tx.GPS.Speed = SensorData.GetSpeed() * 3.6;
	const GPSPosition& pos = SensorData.GetPositionCurrent();
	m_tx.GPS.Latitude.Set(pos.Latitude);
	m_tx.GPS.Longitude.Set(pos.Longitude);
	m_tx.GPS.Distance = SensorData.GetHomeDistance();
	m_tx.GPS.Altitude = 500 + SensorData.GetRelativeAltitude();
	m_tx.GPS.Climb1s = 30000 + SensorData.GetVerticalSpeed() * 100;
	m_tx.GPS.Climb3s = 120;
	m_tx.GPS.Satellites = SensorData.GetSatellites();
	m_tx.GPS.FixChar = GPSFix[SensorData.GetFixType()];
	m_tx.GPS.HomeDirection = SensorData.GetHomeDirection() / 2;
	m_tx.GPS.GPSTime.Set(SensorData.GetDateTime());
	m_tx.GPS.Fix = 'x';
	m_tx.GPS.Version = 0xCE;

	if(!SensorData.IsPositionHomeSet())
		m_tx.GPS.AlarmDisplay = AlarmDisplayMainCurr;

	SendData();
}
void TelemetryHoTT::UpdateGAM()
{
	if (!m_config->GAMEnable)
		return;

	m_tx.Sensor = SensorType::SensorGAM;
	m_tx.SensorResponse = SensorResponseType::ResponseGAM;

	uint8_t cells = SensorData.GetCellCount();
	if (cells > GAMCells)
		cells = GAMCells;

	for (uint8_t cell = 0; cell < cells; cell++)
		m_tx.GAM.Cells[cell] = SensorData.GetCell(cell) / 20;

	m_tx.GAM.Temp1 = 20 + SensorData.GetTemperatur1();
	m_tx.GAM.Temp2 = 20 + SensorData.GetTemperatur2();
	m_tx.GAM.FuelPercent = SensorData.GetCharge();
	m_tx.GAM.RPM = SensorData.GetRpm() / 10;
	m_tx.GAM.Altitude = 500 + SensorData.GetRelativeAltitude();
	m_tx.GAM.Climb = 30000 + SensorData.GetVerticalSpeed() * 100;
	m_tx.GAM.Climb3s = 120;
	m_tx.GAM.Current = SensorData.GetCurrent() * 10;
	m_tx.GAM.Voltage = SensorData.GetBattery() / 100;
	m_tx.GAM.Capacity = SensorData.GetCapacity() / 10;
	m_tx.GAM.Speed = SensorData.GetSpeed() * 3.6;
	m_tx.GAM.LowestCell = SensorData.GetCellLowest() / 20;

	m_tx.GAM.Version = 0xCE;

	SendData();
}
void TelemetryHoTT::UpdateEAM()
{
	if (!m_config->EAMEnable)
		return;

	m_tx.Sensor = SensorType::SensorEAM;
	m_tx.SensorResponse = SensorResponseType::ResponseEAM;

	uint8_t cells = SensorData.GetCellCount();
	if (cells > EAMCells)
		cells = EAMCells;

	for (uint8_t cell = 0; cell < cells; cell++)
		m_tx.EAM.Cells[cell] = SensorData.GetCell(cell) / 20;

	m_tx.EAM.AlarmDisplay = AlarmDisplayType::AlarmDisplayNone;
	m_tx.EAM.Temp1 = 20 + SensorData.GetTemperatur1();
	m_tx.EAM.Temp2 = 20 + SensorData.GetTemperatur2();
	m_tx.EAM.Altitude = 500 + SensorData.GetRelativeAltitude();
	m_tx.EAM.Current = SensorData.GetCurrent() * 10;
	m_tx.EAM.Voltage = SensorData.GetBattery() / 100;
	m_tx.EAM.Capacity = SensorData.GetCapacity() / 10;
	m_tx.EAM.Climb2s = 120;
	m_tx.EAM.Climb3s = 120;
	m_tx.EAM.RPM = SensorData.GetRpm() / 10;
	m_tx.EAM.Minutes = SensorData.GetFlightime() / 60;
	m_tx.EAM.Seconds = SensorData.GetFlightime() - (m_tx.EAM.Minutes * 60);
	m_tx.EAM.Speed = SensorData.GetSpeed() * 3.6;

	SendData();
}
void TelemetryHoTT::UpdateVario()
{
	if (!m_config->VarioEnable)
		return;

	m_tx.Sensor = SensorType::SensorVario;
	m_tx.SensorResponse = SensorResponseType::ResponseVario;

	m_tx.Vario.Altitude = 500 + SensorData.GetRelativeAltitude();
	m_tx.Vario.AltitudeMax = 500;
	m_tx.Vario.AltitudeMin = 500;
	m_tx.Vario.Climb = 120 + SensorData.GetVerticalSpeed() * 10;
	m_tx.Vario.Climb3s = 30000;
	m_tx.Vario.Climb10s = 30000;

	FlightMode fm = SensorData.GetFlightMode();
	memcpy(m_tx.Vario.Message, FlightModeLabel, FMLABELLENGTH);
	memcpy(m_tx.Vario.Message + FMLABELLENGTH, FlightModeLabels[fm], FMLENGTH);

	m_tx.Vario.FlightDirection = (SensorData.GetHeading() / 2);
	m_tx.Vario.Version = 0xCE;

	SendData();
}

void TelemetryHoTT::SendData()
{
	m_tx.Parity = 0;
	for (uint8_t i = 0; i < sizeof(m_tx.Data) - 1; i++)
		m_tx.Parity += m_tx.Data[i];

	m_usart.SendDma(m_tx.Data, sizeof(m_tx.Data));
}

void TelemetryHoTT::ISR()
{
	if (m_usart.GetInterruptSource(USART_ISR_RXNE))
	{
		uint8_t read = m_usart.Receive();
		m_usart.DisableRxTimeoutInterrupt();

		switch (m_rx.State)
		{
		case m_rx.ReadState::StateIdle:
			if (read == RequestType::Binary)
				m_rx.State = m_rx.ReadState::StateSensor;
			break;
		case m_rx.ReadState::StateSensor:
			m_rx.Sensor = (SensorType) read;
			m_rx.State = m_rx.ReadState::StateIdle;
			m_usart.ClearInterruptFlag(USART_ICR_RTOCF);
			m_usart.EnableRxTimeoutInterrupt();
			break;
		default:
			m_rx.State = m_rx.ReadState::StateIdle;
			break;
		}
	}
	if (m_usart.GetInterruptSource(USART_ISR_RTOF))
	{
		m_usart.ClearInterruptFlag(USART_ICR_RTOCF);
		m_usart.DisableRxTimeoutInterrupt();
		eventFlag.signal_isr();
	}
	if (m_usart.GetTXDMA().GetInterruptFlag(DMA_TCIF))
	{
		m_usart.GetTXDMA().ClearInterruptFlags(DMA_TCIF);
		m_usart.GetTXDMA().DisableChannel();
	}
}

void TelemetryHoTT::Reset()
{
	m_usart.Disable();
	m_usart.ClearTXDma();

	m_usart.DeInit();

	HAL::InterruptRegistry.Disable(m_usart.NVIC_IRQn);
	HAL::InterruptRegistry.Disable(m_usart.GetTXDMA().NVIC_IRQn);
}

void TelemetryHoTT::DeInit()
{
	m_run = 0;
	eventFlag.signal();
}

}

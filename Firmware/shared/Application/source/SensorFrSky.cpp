/*
 * SensorFrSky.cpp
 *
 *  Created on: 11.03.2016
 *      Author: cem
 */

#include <SensorFrSky.h>
#include <string.h>
#include <SensorStore.h>

extern uint8_t PrioritySensor;

namespace App
{

#define DATA_U8(packet)   (((uint8_t*)packet)[0]])
#define DATA_S32(packet)  (int32_t)packet
#define DATA_U32(packet)  packet

static_assert (sizeof(SensorFrSky) <= SensorAdapterBase::Workspace, "SensorFrSky will not fit!");

SensorFrSky::SensorFrSky(HAL::USART& usart) :
		SensorAdapter(), m_usart(usart), m_rxLen(0), m_txLen(0), m_index(0), m_currentSensor(), m_polling()
{
	m_tx.Header = HeaderPollFrame;
	m_currentSensor.Static = 1;
}

void SensorFrSky::Init()
{
	eventFlag.clear();

	m_usart.Init(GPIO_PUPD_PULLDOWN);
	m_usart.SetBaudrate(57600);
	m_usart.SetStopbits(USART_STOPBITS_1);
	m_usart.SetDatabits(8);
	m_usart.SetFlowControl(USART_FLOWCONTROL_NONE);
	m_usart.SetParity(USART_PARITY_NONE);
	m_usart.SetMode(USART_MODE_TX);

	m_usart.DisableOverrunDetection();

	HAL::InterruptRegistry.Enable(m_usart.NVIC_IRQn, PrioritySensor, this);

	m_usart.EnableTXInversion();
	m_usart.EnableRXInversion();
	m_usart.EnableHalfduplex();

	m_usart.Enable();
}

void SensorFrSky::Run()
{
	for (;;)
	{
		if (!eventFlag.wait(PollTime))
		{
			m_usart.SetMode(USART_MODE_TX);

			m_usart.DisableTxInterrupt();
			m_usart.DisableTcInterrupt();
			m_usart.DisableRxInterrupt();

			UpdateData();

			memset(m_rx.Data, 0, RXPacketSize);
			m_rxLen = RXPacketSize;

			UpdateSensor();

			m_tx.Sensor = m_currentSensor.SensorId;
			m_txLen = TXPacketSize;

			m_usart.EnableTxInterrupt();
			SendNextByte();
		}
	}
}

void SensorFrSky::UpdateSensor()
{
	m_polling.pop(m_currentSensor);

	if (m_currentSensor.Static)
	{
		uint8_t startSensor = m_index;
		uint8_t found = 1;
		Sensor tmp;
		do
		{
			m_currentSensor.SensorId = SensorIds[m_index];
			m_index = (m_index + 1) % MaxSensors;
			found = 1;

			if (m_polling.getSize() > 0)
			{
				for (uint8_t i = 0; i < m_polling.getSize(); i++)
				{
					m_polling.getItem(i, tmp);
					if (!tmp.Static && tmp.SensorId == m_currentSensor.SensorId)
					{
						found = 0;
						break;
					}
				}
			}

		} while (found == 0 && startSensor != m_index);

		if (found == 0)
		{
			m_polling.push(m_currentSensor);
			m_polling.pop(m_currentSensor);
		}
	}

	if (!m_currentSensor.Static)
		m_currentSensor.UpdateTimeout();
}

uint8_t SensorFrSky::SendNextByte()
{
	m_usart.ClearInterruptFlag(USART_ISR_TXE);

	if (m_txLen == 0)
		return 0;

	m_usart.Send(m_tx.Data[TXPacketSize - m_txLen]);
	m_txLen--;

	return m_txLen;
}

uint8_t SensorFrSky::ReceiveNextByte()
{
	if (m_rxLen == 0)
		return 0;

	m_rx.Data[RXPacketSize - m_rxLen] = m_usart.Receive();
	m_rxLen--;

	return m_rxLen;
}

void SensorFrSky::UpdateData()
{
	if (m_currentSensor.Static)
		m_polling.push(m_currentSensor);

	if (!m_rx.IsValid())
	{
		if (m_currentSensor.IsActive())
			m_polling.push(m_currentSensor);

		switch (m_currentSensor.ValueId)
		{
		case SensorValue::CELLS:
			UpdateCells();
			break;
		case SensorValue::CURR:
			UpdateCurrent();
			break;
		}

		return;
	}

	m_currentSensor.Static = 0;
	m_currentSensor.Value = m_rx.Value;
	m_currentSensor.ValueId = m_rx.Id;
	m_currentSensor.ResetTimeout();
	m_polling.push(m_currentSensor);

	switch (m_rx.Id)
	{
	case SensorValue::CELLS:
		UpdateCells();
		break;
	case SensorValue::CURR:
		UpdateCurrent();
		break;
	case SensorValue::T1:
		SensorData.SetTemperatur1(DATA_S32(m_rx.Value));
		break;
	case SensorValue::T2:
		SensorData.SetTemperatur2(DATA_S32(m_rx.Value));
		break;
	case SensorValue::RPM:
		SensorData.SetSensorPresent(Sensors::RPM, 1);
		SensorData.SetRpm(DATA_U32(m_rx.Value));
		break;
	case SensorValue::ALT:
		SensorData.SetSensorPresent(Sensors::Vario, 1);
		SensorData.SetAltitude(DATA_S32(m_rx.Value) / 100.0f);
		break;
	case SensorValue::VARIO:
		SensorData.SetSensorPresent(Sensors::Vario, 1);
		SensorData.SetVerticalSpeed(DATA_S32(m_rx.Value) / 100.0f);
		break;
	case SensorValue::VFAS:
		SensorData.SetSensorPresent(Sensors::Voltage, 1);
		SensorData.SetBattery(DATA_U32(m_rx.Value) * 10);
		break;
	case SensorValue::GPS_SPEED:
		SensorData.SetSpeed(DATA_U32(m_rx.Value) / 1943.0f);
		break;
	case SensorValue::GPS_ALT:
		SensorData.SetSensorPresent(Sensors::Vario, 1);
		SensorData.SetAltitude(DATA_S32(m_rx.Value) / 100.0f);
		break;
	case SensorValue::GPS_LONG_LATI:
	{
		SensorData.SetSensorPresent(Sensors::SGPS, 1);
		GPSPosition pos = GPSPosition(SensorData.GetPositionCurrent());

		uint8_t sign = (m_rx.Value & 0xc0000000) >> 30;
		float lonlat = m_rx.Value & 0x0FFFFFFF;
		lonlat = ((lonlat / 6) * 100) / 10000000;

		if (sign & 0x01)
			lonlat = -lonlat;

		if (sign & 0x02)
			pos.Longitude = lonlat;
		else
			pos.Latitude = lonlat;

		SensorData.SetPositionCurrent(pos.Latitude, pos.Longitude);
		break;
	}
	case SensorValue::GPS_COURS:
		SensorData.SetCourseOverGround(DATA_U32(m_rx.Value) / 100.0f);
		break;
	case SensorValue::GPS_TIME_DATE:
		Utils::DateTime time = SensorData.GetDateTime();
		uint32_t gps_time_date = DATA_U32(m_rx.Value);
		if (gps_time_date & 0x000000ff)
		{
			time.SetYear(gps_time_date >> 24);
			time.SetMonth(gps_time_date >> 16);
			time.SetDay(gps_time_date >> 8);
		}
		else
		{
			time.SetHour(gps_time_date >> 24);
			time.SetMinute(gps_time_date >> 16);
			time.SetSecond(gps_time_date >> 8);
		}
		SensorData.SetDateTime(time);
		break;
	}
}

void SensorFrSky::UpdateCurrent()
{
	Sensor tmp;
	float current = 0;
	for (uint8_t i = 0; i < m_polling.getSize(); i++)
	{
		m_polling.getItem(i, tmp);
		if (tmp.IsActive() && tmp.ValueId == SensorValue::CURR)
		{
			current += ((float) DATA_U32(tmp.Value)) / 10.0f;
		}
	}

	SensorData.SetCurrent(current);
	SensorData.SetSensorPresent(Sensors::Current, 1);
}

void SensorFrSky::UpdateCells()
{

	Sensor tmp;
	uint8_t cells = 0;
	for (uint8_t i = 0; i < m_polling.getSize(); i++)
	{
		m_polling.getItem(i, tmp);
		if (tmp.IsActive() && tmp.ValueId == SensorValue::CELLS)
		{
			if (tmp.Lipo.TotalCells > 0)
			{
				SensorData.SetCell(cells + tmp.Lipo.StartCell, (tmp.Lipo.Cell1 / 5.0f) * 10);
				if ((tmp.Lipo.StartCell + 1) < tmp.Lipo.TotalCells)
					SensorData.SetCell(cells + tmp.Lipo.StartCell + 1, (tmp.Lipo.Cell2 / 5.0f) * 10);
			}
			cells += tmp.Lipo.TotalCells;
		}
	}

	SensorData.SetCellCount(cells);
	SensorData.SetSensorPresent(Sensors::Cells, 1);
}

void SensorFrSky::ISR()
{
	if (m_usart.GetInterruptSource(USART_ISR_TXE))
	{
		if (!SendNextByte())
		{
			m_usart.DisableTxInterrupt();
			m_usart.EnableTcInterrupt();
		}
	}
	else if (m_usart.GetInterruptSource(USART_ISR_TC))
	{
		m_usart.DisableTcInterrupt();
		m_usart.SetMode(USART_MODE_TX_RX);

		m_usart.ClearInterruptFlag(USART_ISR_RXNE);
		m_usart.EnableRxInterrupt();
	}
	else if (m_usart.GetInterruptSource(USART_ISR_RXNE))
	{
		if (!ReceiveNextByte())
		{
			m_usart.DisableRxInterrupt();
			m_usart.SetMode(USART_MODE_TX);
		}
	}
}

} /* namespace App */

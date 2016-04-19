/*
 * FCTarot.cpp
 *
 *  Created on: 16.04.2016
 *      Author: cem
 */

#include "FCTarot.h"
#include <string.h>
#include "Endianess.h"
#include "SensorStore.h"

extern uint8_t PriorityFC;

namespace App
{

static_assert (sizeof(FCTarot) <= FCAdapter::Workspace, "FCTarot will not fit!");

void FCTarot::Init()
{
	static_assert (sizeof(FCTarot::GPSType) == FCTarot::PacketContentSize, "Tarot GPS PaketSize does not match!");
	static_assert (sizeof(FCTarot::RCType) == FCTarot::PacketContentSize, "Tarot RC PaketSize does not match!");
	static_assert (sizeof(FCTarot::HomeDataType) == FCTarot::PacketContentSize, "Tarot HomeData PaketSize does not match!");

	static_assert (sizeof(FCTarot::Paket) == FCTarot::PaketSize, "Tarot PaketSize does not match!");

	m_usart.Init(GPIO_PUPD_PULLDOWN);
	m_usart.SetBaudrate(115200);
	m_usart.SetStopbits(USART_STOPBITS_1);
	m_usart.SetDatabits(8);
	m_usart.SetFlowControl(USART_FLOWCONTROL_NONE);
	m_usart.SetParity(USART_PARITY_NONE);
	m_usart.SetMode(USART_MODE_RX);
	m_usart.DisableOverrunDetection();

	HAL::InterruptRegistry.Enable(m_usart.NVIC_IRQn, PriorityFC, this);

	m_usart.EnableRxInterrupt();
	m_usart.Enable();
}

void FCTarot::Run()
{
	Paket* tmpPaket;
	volatile uint16_t crc = 0;

	while (IsAlive())
	{
		if (gotMsg.wait(WaitForDataTimeout))
		{
			tmpPaket = (Paket*) &m_receiveBuffer[m_processIndex];

			if (tmpPaket->StartByte != StartSign)
			{
				//Find next Start sign
				for (; m_processIndex < m_receiveIndex; m_processIndex++)
				{
					tmpPaket = (Paket*) &m_receiveBuffer[m_processIndex];
					if (tmpPaket->StartByte == StartSign)
						break;
				}
			}

			if (tmpPaket->StartByte == StartSign)
			{
				if ((m_receiveIndex - m_processIndex) >= PaketSize)
				{
					crc = calculateChecksum((const uint8_t*) tmpPaket, PaketSize - 2);
					if (crc == tmpPaket->CRC)
					{
						Process(*tmpPaket);
						m_processIndex += PaketSize;
					}
					else
						m_processIndex += 1;

				}
			}

			if ((m_receiveIndex + TmpReceiveBufferSize) > ReceiveBufferSize)
			{
				m_receiveIndex = m_receiveIndex - m_processIndex;
				memmove(m_receiveBuffer, &m_receiveBuffer[m_processIndex], m_receiveIndex);
				m_processIndex = 0;
				memset(&m_receiveBuffer[m_receiveIndex], 0, ReceiveBufferSize - m_receiveIndex);
			}

		}
	}

	DeInit();
}

void FCTarot::Process(Paket& paket)
{
	SetHeartbeat();

	SensorData.SetFCType(FCType::ZYX_M);

	SensorData.SetSensorPresent(Sensors::Arming, 1);
	SensorData.SetSensorPresent(Sensors::GForce, 1);
	SensorData.SetSensorPresent(Sensors::Homeing, 1);
	SensorData.SetSensorPresent(Sensors::SGPS, 1);
	SensorData.SetSensorPresent(Sensors::Vario, 1);
	SensorData.SetSensorPresent(Sensors::Voltage, 1);

	SensorData.SetHeading(paket.Heading / 100.f);
	SensorData.SetRoll(paket.Roll / 100.f);
	SensorData.SetPitch(paket.Pitch / 100.f);

	switch (paket.Type)
	{
	case PaketType::GPS:
		Process(paket.GPS);
		break;
	case PaketType::RC:
		Process(paket.RC);
		break;
	case PaketType::HomeData:
		Process(paket.HomeData);
	}
}

void FCTarot::Process(GPSType& data)
{
	SensorData.SetPositionCurrent(data.Latitude / 10000000.f, data.Longitude / 10000000.f);
	SensorData.SetAltitude(data.Altitude / 10.f);
	SensorData.SetSpeed(data.Speed / 10.f);
	SensorData.SetVerticalSpeed(data.Climb / 10.f);
	SensorData.SetBattery(data.Voltage * 10);

	if (data.Failsafe != 0)
		SensorData.SetFlightMode(FlightMode::Failsafe);
	else if (data.Flightmode == 1)
		SensorData.SetFlightMode(FlightMode::Manual);
	else if (data.Flightmode == 2)
		SensorData.SetFlightMode(FlightMode::Attitude);
	else if (data.Flightmode == 3)
		SensorData.SetFlightMode(FlightMode::GPS);

	SensorData.SetSatellites(data.Satellites);
	SensorData.SetFixType(data.Satellites == 0 ? GPSFixType::FixNo : data.Satellites < 4 ? GPSFixType::Fix2D : GPSFixType::Fix3D);

}

void FCTarot::Process(RCType& data)
{
	SensorData.SetArmed(data.Armed);
	//TODO: Set RC Channels
}

void FCTarot::Process(HomeDataType& data)
{
	SensorData.SetHomeAltitude(data.Altitude / 10.f);
	SensorData.SetPositionHome(data.Latitude / 10000000.f, data.Longitude / 10000000.f);
}

uint16_t FCTarot::calculateChecksum(const uint8_t* data, uint8_t size)
{
	uint16_t crc = 0x0000;
	uint8_t b = 0;

	for (uint8_t j = 0; j < size; j++)
	{
		b = data[j];
		for (int i = 0; i < 8; i++)
		{
			bool bit = ((b >> (7 - i) & 1) == 1);
			bool c15 = ((crc >> 15 & 1) == 1);
			crc <<= 1;
			// If coefficient of bit and remainder polynomial = 1 xor crc with polynomial
			if (c15 ^ bit)
				crc ^= 0x1021;
		}
	}

	crc &= 0xffff;
	return Utils::Endianess::ToBig((int16_t) crc);
}

void FCTarot::DeInit(void)
{
	m_usart.DisableRxInterrupt();
	m_usart.Disable();

	HAL::InterruptRegistry.Disable(m_usart.NVIC_IRQn);

	m_usart.DeInit();
}

void FCTarot::ISR(void)
{
	if (m_usart.GetInterruptSource(USART_ISR_RXNE))
	{
		m_tmpReceiveBuffer[m_tmpReceiveIndex++] = m_usart.Receive();
		if (m_tmpReceiveIndex == TmpReceiveBufferSize)
		{
			memcpy(&m_receiveBuffer[m_receiveIndex], m_tmpReceiveBuffer, TmpReceiveBufferSize);
			m_receiveIndex += TmpReceiveBufferSize;
			m_tmpReceiveIndex = 0;
			gotMsg.signal_isr();
		}
	}
}

} /* namespace App */

/*
 * FCAlign.cpp
 *
 *  Created on: 13.06.2016
 *      Author: cem
 */

#include "FCAlign.h"
#include <string.h>
#include "Endianess.h"
#include "SensorStore.h"

extern uint8_t PriorityFC;

namespace App
{

static_assert (sizeof(FCAlign) <= FCAdapter::Workspace, "FCAlign will not fit!");

void FCAlign::Init()
{

	static_assert (sizeof(FCAlign::Paket) == 35, "Align PaketSize does not match!");

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

void FCAlign::Run()
{
	Paket* tmpPaket;
	volatile uint16_t checksum = 0;

	while (IsAlive())
	{
		if (gotMsg.wait(WaitForDataTimeout))
		{
			tmpPaket = (Paket*) &m_receiveBuffer[m_processIndex];

			if (tmpPaket->StartByte != StartSign || tmpPaket->Type != PaketTypeOSD)
			{
				//Find next Start sign
				for (; m_processIndex < m_receiveIndex; m_processIndex++)
				{
					tmpPaket = (Paket*) &m_receiveBuffer[m_processIndex];
					if (tmpPaket->StartByte == StartSign && tmpPaket->Type == PaketTypeOSD)
						break;
				}
			}

			if (tmpPaket->StartByte == StartSign && tmpPaket->Type == PaketTypeOSD)
			{
				if ((m_receiveIndex - m_processIndex) >= (tmpPaket->PaketSize + 4))
				{
					checksum = calculateChecksum((const uint8_t*) tmpPaket, tmpPaket->PaketSize + 3);
					if (checksum == tmpPaket->Checksum)
					{
						Process(*tmpPaket);
						m_processIndex += tmpPaket->PaketSize + 4;
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

void FCAlign::Process(Paket& paket)
{
	SetHeartbeat();

	SensorData.SetFCType(FCType::Align);

	SensorData.SetSensorPresent(Sensors::SGPS, 1);
	SensorData.SetSensorPresent(Sensors::GForce, 1);
	SensorData.SetSensorPresent(Sensors::Vario, 1);
	SensorData.SetSensorPresent(Sensors::Voltage, 1);

	SensorData.SetRoll(paket.Roll / 10.f);
	SensorData.SetPitch(paket.Pitch / 10.f);

	SensorData.SetPositionCurrent(paket.Latitude / 10000000.f, paket.Longitude / 10000000.f);
	SensorData.SetAltitude(paket.Altitude / 10.f);
	SensorData.SetVerticalSpeed(paket.VerticalVelocity / 10.f);
	SensorData.SetSpeed(paket.Speed / 10.f);
	SensorData.SetBattery(paket.Voltage * 100);

	if (paket.Mode == AlignModeManual)
		SensorData.SetFlightMode(FlightMode::Manual);
	else if (paket.Mode == AlignModeAttitude)
		SensorData.SetFlightMode(FlightMode::Attitude);
	else if (paket.Mode == AlignModeGPSVelocity || paket.Mode == AlignModeIOC || paket.Mode == AlignModeGPSAngular)
		SensorData.SetFlightMode(FlightMode::GPS);
	else if (paket.Mode == AlignModeFailsafeVelocity || paket.Mode == AlignModeFailsafeAngular)
		SensorData.SetFlightMode(FlightMode::Failsafe);

	SensorData.SetSatellites(paket.Satellites);
	SensorData.SetFixType(
			paket.Satellites == 0 ? GPSFixType::FixNo : paket.Satellites < 4 ? GPSFixType::Fix2D : GPSFixType::Fix3D);
}

uint8_t FCAlign::calculateChecksum(const uint8_t* data, uint8_t size)
{
	uint8_t sum = 0x00;

	for (uint8_t j = 0; j < size; j++)
		sum += data[j];

	return sum;
}

void FCAlign::DeInit(void)
{
	m_usart.DisableRxInterrupt();
	m_usart.Disable();

	HAL::InterruptRegistry.Disable(m_usart.NVIC_IRQn);

	m_usart.DeInit();
}

void FCAlign::ISR(void)
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

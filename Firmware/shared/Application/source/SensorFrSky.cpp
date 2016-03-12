/*
 * SensorFrSky.cpp
 *
 *  Created on: 11.03.2016
 *      Author: cem
 */

#include <SensorFrSky.h>

namespace App
{

void SensorFrSky::Init()
{
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

void SensorFrSky::Run()
{
	for (;;)
	{
		if(eventFlag.wait(PollTime))
		{

		}
	}
}

void SensorFrSky::ISR()
{

}

} /* namespace App */

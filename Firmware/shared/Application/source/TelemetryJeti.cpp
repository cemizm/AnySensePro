/*
 * TelemetryJeti.cpp
 *
 *  Created on: 17.03.2016
 *      Author: cem
 */

#include <TelemetryJeti.h>

#include "Pin.h"

namespace App
{

static_assert (sizeof(TelemetryJeti) <= TELEMETRY_WORKSPACE, "TelemetryJeti will not fit!");

void TelemetryJeti::Init()
{
	m_config = (ProtocolConfig*) Config.GetConfiguration().ProtocolSettings;

	if (m_config->IsValid != ConfigKey)
	{
		m_config->IsValid = ConfigKey;

		Config.Save();
	}

	m_pin.PowerUp();
	m_pin.On();
	m_pin.ModeSetup(GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP);
	m_pin.SetOutputOptions(GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ);

	m_timer.PowerUp();
	m_timer.SetClockDivision(TIM_CR1_CKD_CK_INT);
	m_timer.DirectionUp();
	m_timer.SetPrescaler(prescaler - 1);
	m_timer.SetPeriod((m_timer.clock_frequency / prescaler / baudrate) - 1);

	m_timer.ClearFlag(TIM_SR_UIF);
	m_timer.EnableIRQ(TIM_DIER_UIE);

	HAL::InterruptRegistry.Enable(m_timer.NVIC_IRQn, PriorityTelemetry, this);
}
void TelemetryJeti::Run(void)
{
	while (m_run)
	{
		eventFlag.wait(delay_ms(100));
		if (m_run)
		{
			m_counter = 0;
			m_timer.Enable();
		}
	}
	Reset();
}
void TelemetryJeti::Reset()
{
	m_timer.DisableIRQ(TIM_DIER_UIE);
	m_timer.Disable();

	HAL::InterruptRegistry.Disable(m_timer.NVIC_IRQn);
}

void TelemetryJeti::ISR()
{
	if (m_timer.GetInterruptSource(TIM_SR_UIF))
	{
		m_timer.ClearFlag(TIM_SR_UIF);

		if (m_counter == 0) //Start Bit
			m_pin.Off();
		else if (m_counter < 10) //Data Bits
		{
			m_counter % 2 == 0 ? m_pin.On() : m_pin.Off();
		}
		else if (m_counter < 11) //Parity Bit
			m_pin.Off();
		else if (m_counter < 13) //Stop Bits
			m_pin.On();

		if (m_counter == 12)
			m_timer.Disable();

		m_counter++;
	}
}
void TelemetryJeti::DeInit()
{
	m_run = 0;
	eventFlag.signal();
}
} /* namespace App */

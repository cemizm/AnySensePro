/*
 * TelemetryFutaba.cpp
 *
 *  Created on: 10.03.2016
 *      Author: cem
 */

#include "TelemetryFutaba.h"

#include "SensorStore.h"

namespace App
{

static_assert (sizeof(TelemetryFutaba) <= TELEMETRY_WORKSPACE, "TelemetryFutaba will not fit!");
void TelemetryFutaba::Init()
{
	m_config = (ProtocolConfig*) Config.GetConfiguration().ProtocolSettings;

	if (m_config->IsValid != ConfigKey)
	{
		m_config->IsValid = ConfigKey;
		m_config->Slots[1] = SlotValue::Vario;
		m_config->Slots[3] = SlotValue::Cell;
		m_config->Slots[5] = SlotValue::Current;
		m_config->Slots[8] = SlotValue::GPS;

		Config.Save();
	}

	eventFlag.clear();

	m_usart.Init(GPIO_PUPD_PULLDOWN);
	m_usart.SetBaudrate(Buadrate);
	m_usart.SetStopbits(USART_STOPBITS_2);
	m_usart.SetDatabits(9);
	m_usart.SetFlowControl(USART_FLOWCONTROL_NONE);
	m_usart.SetParity(USART_PARITY_EVEN);
	m_usart.SetMode(USART_MODE_TX_RX);
	m_usart.SetReceiveTimeout(Timeout);

	m_usart.DisableOverrunDetection();

	m_timer.PowerUp();
	m_timer.SetClockDivision(TIM_CR1_CKD_CK_INT);
	m_timer.DirectionUp();
	m_timer.SetPrescaler(Prescaler - 1);
	m_timer.SetPeriod(((m_timer.clock_frequency / Prescaler) / ((1000 * 1000) / Interval)) - 1);

	HAL::InterruptRegistry.Enable(m_usart.NVIC_IRQn, PriorityTelemetry, this);
	HAL::InterruptRegistry.Enable(m_usart.GetTXDMA().NVIC_IRQn, PriorityTelemetry, this);
	HAL::InterruptRegistry.Enable(m_timer.NVIC_IRQn, PriorityTelemetry, this);

	m_usart.EnableTXInversion();
	m_usart.EnableRXInversion();
	m_usart.EnableHalfduplex();

	m_usart.SetupTXDMA();
	m_usart.EnableRxInterrupt();
	m_usart.EnableRxTimeoutInterrupt();
	m_usart.Enable();

	m_timer.ClearFlag(TIM_SR_UIF);
	m_timer.EnableIRQ(TIM_DIER_UIE);
}

void TelemetryFutaba::Run(void)
{
	while (m_run)
	{
		eventFlag.wait();
		if (m_run)
		{
			UpdateFrame(m_rx.GetFrame());
		}
	}
	Reset();
}

void TelemetryFutaba::UpdateFrame(uint8_t frame)
{
	//None = 0, Vario = 1, Kompass = 2, Current = 3, GPS = 4, Cell = 5, Sats = 6, Flightmode = 7, Temperatur = 8,
	for (uint8_t slot = 0; slot < SlotPerFrameCount;)
	{
		switch (m_config->Slots[SlotPerFrameCount * frame + slot])
		{
		case SlotValue::Vario:
			UpdateVario(m_tx.DataFrame[frame][slot]);
			m_tx.SetHasData(frame, slot, 1, 2);
			slot += 2;
			break;
		case SlotValue::Kompass:
			UpdateRPM(m_tx.DataFrame[frame][slot], SensorData.GetHeading());
			m_tx.SetHasData(frame, slot, 1, 1);
			slot += 1;
			break;
		case SlotValue::Current:
			UpdateCurrent(m_tx.DataFrame[frame][slot]);
			m_tx.SetHasData(frame, slot, 1, 3);
			slot += 3;
			break;
		case SlotValue::GPS:
			UpdateGPS((GPSType*) m_tx.DataFrame[frame][slot]);
			m_tx.SetHasData(frame, slot, 1, 8);
			slot += 8;
			break;
		case SlotValue::Cell:
			UpdateVoltage(m_tx.DataFrame[frame][slot]);
			m_tx.SetHasData(frame, slot, 1, 2);
			slot += 2;
			break;
		case SlotValue::Sats:
			UpdateTemperatur(m_tx.DataFrame[frame][slot], SensorData.GetSatellites());
			m_tx.SetHasData(frame, slot, 1, 1);
			slot += 1;
			break;
		case SlotValue::Flightmode:
			UpdateTemperatur(m_tx.DataFrame[frame][slot], SensorData.GetFlightMode());
			m_tx.SetHasData(frame, slot, 1, 1);
			slot += 1;
			break;
		case SlotValue::Temperatur:
			UpdateTemperatur(m_tx.DataFrame[frame][slot], SensorData.GetTemperatur1());
			m_tx.SetHasData(frame, slot, 1, 1);
			slot += 1;
			break;
		default:
			m_tx.SetHasData(frame, slot, 0);
			slot++;
		}
	}

	m_tx.CurrentFrame = frame;
	m_tx.CurrentSlot = 0;
}

void TelemetryFutaba::UpdateVario(uint8_t* vario)
{
	int16_t i16tmp = SensorData.GetVerticalSpeed() * 100;
	uint16_t u16tmp = SensorData.GetAltitude() + 30000;

	vario[0] = i16tmp >> 8;
	vario[1] = i16tmp;
	vario[2] = (((u16tmp >> 8) & 0x7F)) | 0b1 << 7;
	vario[3] = (u16tmp);
}

void TelemetryFutaba::UpdateCurrent(uint8_t* current)
{
	int16_t i16tmp = SensorData.GetCurrent() * 100;

	if (i16tmp > 0x3FFF)
		i16tmp = 0x3FFF;
	else if (i16tmp < -16383)
		i16tmp = -16383;

	uint16_t u16tmp = i16tmp >= 0 ? i16tmp : (i16tmp * -1 ^ 0xFFFF);

	u16tmp = u16tmp & 0x3FFF;

	current[0] = u16tmp >> 8 | 0x40;

	if (SensorData.GetCurrent() < 0)
		current[0] |= 0x80;

	current[1] = u16tmp;

	i16tmp = SensorData.GetCapacity();
	u16tmp = SensorData.GetBattery() / 10;

	current[2] = u16tmp >> 8;
	current[3] = u16tmp;
	current[4] = i16tmp >> 8;
	current[5] = i16tmp;
}

void TelemetryFutaba::UpdateTemperatur(uint8_t* temp, uint8_t value)
{
	temp[0] = 100 + value;
	temp[1] = 0x80;
}
void TelemetryFutaba::UpdateRPM(uint8_t* rpm, uint16_t value)
{
	rpm[0] = value;
	rpm[1] = value >> 8;
}

void TelemetryFutaba::UpdateGPS(GPSType* gps)
{
	if (SensorData.IsGPSOK())
	{
		const GPSPosition& pos = SensorData.GetPositionCurrent();
		const GPSTime& time = SensorData.GetDateTime();

		gps->LongitudeSign = pos.Longitude < 0 ? 1 : 0;
		gps->Longitude = ((pos.Longitude < 0 ? pos.Longitude * -1 : pos.Longitude) * 600000);

		gps->LatitudeSign = pos.Latitude < 0 ? 1 : 0;
		gps->Latitude = ((pos.Latitude < 0 ? pos.Latitude * -1 : pos.Latitude) * 600000);

		gps->GPSFix = 1;
		gps->Speed = SensorData.GetSpeed() * 3.6;

		gps->UTC = time.Hour * 3600 + time.Minute * 60 + time.Second;
	}
	else
		gps->GPSFix = 0;

	gps->GPSStrength = (SensorData.GetFixType() == GPSFixType::FixNo) ? 0 : SensorData.GetFixType() - 1;

	gps->Vario = rintf((150 + SensorData.GetVerticalSpeed()) / 0.8f);
	gps->Altitude = rintf((SensorData.GetAltitude() + 1500) / 0.8f);

	gps->BaroStatus = 1;
}

void TelemetryFutaba::UpdateVoltage(uint8_t* voltage)
{
	uint16_t u16tmp = SensorData.GetBattery() / 100;
	voltage[0] = (u16tmp >> 8 & 0x1) | 0x80;
	voltage[1] = u16tmp;

	u16tmp = SensorData.GetCellLowest() / 100;
	voltage[2] = u16tmp >> 8 & 0x1F;
	voltage[3] = u16tmp;
}

void TelemetryFutaba::SendNextSlot()
{
	if (m_tx.HasData())
		m_usart.SendDma(m_tx.CurrentData, SlotDataSize);

	m_tx.CurrentSlot++;
}

void TelemetryFutaba::ISR()
{
	if (m_usart.GetInterruptSource(USART_ISR_RXNE)) //Receive
	{
		m_rx.Write(m_usart.Receive());
		if (m_rx.IsValid())
		{
			m_usart.EnableRxTimeout();
			eventFlag.signal_isr();
		}
	}

	if (m_usart.GetInterruptSource(USART_ISR_RTOF)) //Receive Timeout
	{
		m_usart.ClearInterruptFlag(USART_ICR_RTOCF);
		m_usart.DisableRxTimeout();

		m_usart.SetMode(USART_MODE_TX);
		m_timer.Enable();

		SendNextSlot();
	}

	if (m_timer.GetInterruptSource(TIM_SR_UIF)) //Timer
	{
		m_timer.ClearFlag(TIM_SR_UIF);

		if (!m_tx.HasMoreSlots())
		{
			m_timer.Disable();
			m_usart.SetMode(USART_MODE_TX_RX);
		}
		else
			SendNextSlot();
	}

	if (m_usart.GetTXDMA().GetInterruptFlag(DMA_TCIF)) //DMA Complete
	{
		m_usart.GetTXDMA().ClearInterruptFlags(DMA_TCIF);
		m_usart.GetTXDMA().DisableChannel();
	}
}

void TelemetryFutaba::Reset()
{
	m_usart.Disable();
	m_usart.ClearTXDma();

	m_usart.DeInit();

	m_timer.Disable();

	HAL::InterruptRegistry.Disable(m_usart.NVIC_IRQn);
	HAL::InterruptRegistry.Disable(m_usart.GetTXDMA().NVIC_IRQn);
	HAL::InterruptRegistry.Disable(m_timer.NVIC_IRQn);
}

void TelemetryFutaba::DeInit()
{
	m_run = 0;
	eventFlag.signal();
}

} /* namespace App */

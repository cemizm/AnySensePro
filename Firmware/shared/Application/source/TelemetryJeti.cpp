/*
 * TelemetryJeti.cpp
 *
 *  Created on: 17.03.2016
 *      Author: cem
 */

#include <TelemetryJeti.h>
#include "SensorStore.h"

#include "Pin.h"

#include "string.h"

namespace App
{

static_assert (sizeof(TelemetryJeti) <= TELEMETRY_WORKSPACE, "TelemetryJeti will not fit!");

const TelemetryJeti::LabelType TelemetryJeti::Labels[TelemetryJeti::TelemetryValues + 1] = { { "AnySense", "" }, { "Lat.", "" }, {
		"Lon.", "" }, { "Num Sat.", "" }, { "GPS Fix", "" }, { "F. Mode", "" }, { "Speed", "kmh" }, { "Dist.", "m" }, { "Alt.",
		"m" }, { "Climb", "m/s" }, { "Comp.", "\xB0" }, { "F. Dir", "\xB0" }, { "H. Dir", "\xB0" }, { "Bat.", "V" }, { "Curr.",
		"A" }, { "Capa.", "Ah" }, { "Cell", "V" }, { "Temp.", "\xB0" "C" }, { "H. Lat", "" }, { "H. Long", "" }, {"RPM", "rpm"} };

void TelemetryJeti::Init()
{
	m_config = (ProtocolConfig*) Config.GetConfiguration().ProtocolSettings;

	if (m_config->IsValid != ConfigKey)
	{
		m_config->IsValid = ConfigKey;

		m_config->Mapping[0] = TelemetryValue::Latitude;
		m_config->Mapping[1] = TelemetryValue::Longitude;
		m_config->Mapping[2] = TelemetryValue::Satellite;
		m_config->Mapping[3] = TelemetryValue::GPS_Fix;
		m_config->Mapping[4] = TelemetryValue::Speed;
		m_config->Mapping[5] = TelemetryValue::Distance;
		m_config->Mapping[6] = TelemetryValue::Altitude;
		m_config->Mapping[7] = TelemetryValue::VSpeed;
		m_config->Mapping[8] = TelemetryValue::Compass;
		m_config->Mapping[9] = TelemetryValue::HomeDirection;
		m_config->Mapping[10] = TelemetryValue::Voltage;
		m_config->Mapping[11] = TelemetryValue::Current;
		m_config->Mapping[12] = TelemetryValue::Capacity;
		m_config->Mapping[13] = TelemetryValue::LipoVoltage;
		m_config->Mapping[14] = TelemetryValue::LipoTemp;

		Config.Save();
	}

	m_pin.PowerUp();
	m_pin.ModeSetup(GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP);
	m_pin.SetOutputOptions(GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ);
	m_pin.On();

	m_timer.PowerUp();
	m_timer.SetClockDivision(TIM_CR1_CKD_CK_INT);
	m_timer.DirectionUp();
	m_timer.SetPrescaler(Prescaler - 1);
	m_timer.SetPeriod((m_timer.clock_frequency / Prescaler / Baudrate) - 1);

	m_timer.ClearFlag(TIM_SR_UIF);
	m_timer.EnableIRQ(TIM_DIER_UIE);

	HAL::InterruptRegistry.Enable(m_timer.NVIC_IRQn, PriorityTelemetry, this);
}

void TelemetryJeti::Run(void)
{

	m_startTick = OSAL::Timer::GetTime() + delay_sec(10);
	m_nextText = OSAL::Timer::GetTime();

	m_tx.Seperator = StartByte;
	m_tx.ExId = 0x9F;
	m_tx.ManufactureId = 0xA4CB;
	m_tx.DeviceId = 0x24;

	DataPacket* packet = nullptr;
	uint8_t packetLen = 0;
	uint8_t totalLen = 0;
	do
	{
		eventFlag.wait(delay_ms(DataInterval));

		m_tick = OSAL::Timer::GetTime();

		m_tx.Length = 6;
		memset(m_tx.Content, 0x00, MaxContentSize);

		if (m_tick > m_nextText || m_startTick > m_tick)
		{
			// EX Text
			m_tx.Type = 0;

			const LabelType text = Labels[m_currentLabel == 0 ? 0 : m_config->Mapping[m_currentLabel - 1]];

			TextPacket* label = (TextPacket*) m_tx.Content;
			label->Id = m_currentLabel;
			label->DescriptionLength = strlen((const char*) text.Name);
			label->UnitLength = strlen((const char*) text.Unit);

			memcpy(label->Label, text.Name, label->DescriptionLength);
			memcpy(&label->Label[label->DescriptionLength], text.Unit, label->UnitLength);

			m_tx.Length += label->DescriptionLength + label->UnitLength + 2;

			m_tmpValue = m_currentLabel;
			do
			{
				m_currentLabel = (m_currentLabel + 1) % MaxLabelCount;
			} while (m_config->Mapping[m_currentLabel - 1] == TelemetryValue::None && m_tmpValue != m_currentLabel
					&& m_currentLabel != 0);

			m_nextText = m_tick + delay_sec(TextInterval);
		}
		else
		{
			//EX Data
			m_tx.Type = 1;

			m_tmpValue = m_currentValue;
			totalLen = 0;

			do
			{
				packet = (DataPacket*) &m_tx.Content[totalLen];
				packetLen = 0;

				switch (m_config->Mapping[m_currentValue])
				{
				case TelemetryValue::Latitude: //Lat
					if (SensorData.IsGPSOK())
						packetLen = packet->UpdateGPS(SensorData.GetPositionCurrent().Latitude, 0);
					break;
				case TelemetryValue::Longitude: //lon
					if (SensorData.IsGPSOK())
						packetLen = packet->UpdateGPS(SensorData.GetPositionCurrent().Longitude, 1);
					break;
				case TelemetryValue::Satellite: //sat
					packetLen = packet->UpdateU6(0, SensorData.GetSatellites());
					break;
				case TelemetryValue::GPS_Fix: //gps fix
					packetLen = packet->UpdateU6(0, SensorData.GetFixType());
					break;
				case TelemetryValue::Flightmode: //mode
					packetLen = packet->UpdateU6(0, SensorData.GetFlightMode());
					break;
				case TelemetryValue::Speed: //speed
					if (SensorData.IsGPSOK())
						packetLen = packet->UpdateU14(1, SensorData.GetSpeed() * 36);
					break;
				case TelemetryValue::Distance: //distance
					if (SensorData.IsGPSOK())
						packetLen = packet->UpdateU22(2, SensorData.GetHomeDistance() * 100);
					break;
				case TelemetryValue::Altitude: //alt
					packetLen = packet->UpdateU22(2, SensorData.GetRelativeAltitude() * 100);
					break;
				case TelemetryValue::VSpeed: //vsi
					packetLen = packet->UpdateU22(2, SensorData.GetVerticalSpeed() * 100);
					break;
				case TelemetryValue::Compass: //heading
					packetLen = packet->UpdateU14(1, SensorData.GetHeading() * 10);
					break;
				case TelemetryValue::COG: //cog
					if (SensorData.IsGPSOK())
						packetLen = packet->UpdateU14(1, SensorData.GetCourseOverGround() * 10);
					break;
				case TelemetryValue::HomeDirection: //home dir
					if (SensorData.IsPositionHomeSet())
						packetLen = packet->UpdateU14(1, SensorData.GetHomeDirection() * 10);
					break;
				case TelemetryValue::Voltage: //voltage
					packetLen = packet->UpdateU14(2, SensorData.GetBattery() / 10.0f);
					break;
				case TelemetryValue::Current: //current
					packetLen = packet->UpdateU14(1, SensorData.GetCurrent() * 10);
					break;
				case TelemetryValue::Capacity: //capa
					packetLen = packet->UpdateU14(2, SensorData.GetCapacity() / 10.0f);
					break;
				case TelemetryValue::LipoVoltage: //cell
					packetLen = packet->UpdateU14(2, SensorData.GetCellLowest() / 10.0f);
					break;
				case TelemetryValue::LipoTemp: //cell temp.
					packetLen = packet->UpdateU14(1, SensorData.GetTemperatur1() * 10);
					break;
				case TelemetryValue::HomeLatitude: //Lat
					if (SensorData.IsPositionHomeSet())
						packetLen = packet->UpdateGPS(SensorData.GetPositionHome().Latitude, 0);
					break;
				case TelemetryValue::HomeLongitude: //lon
					if (SensorData.IsPositionHomeSet())
						packetLen = packet->UpdateGPS(SensorData.GetPositionHome().Longitude, 1);
					break;
				case TelemetryValue::RPM: //RPM
					packetLen = packet->UpdateU30(0, SensorData.GetRpm());
					break;
				default:
					packetLen = 0;
					break;
				}

				if ((totalLen + packetLen) > MaxContentSize)
					break;

				totalLen += packetLen;
				packet->Id = m_currentValue + 1;

				m_currentValue = (m_currentValue + 1) % MaxTelemetryCount;
			} while (m_tmpValue != m_currentValue && (totalLen + 5) <= MaxContentSize);

			m_tx.Length += totalLen;
		}

		UpdateBuffer();

		//Bang them out
		m_timer.Enable();
		eventFlag.wait();

	} while (m_run);

	Reset();
}

void TelemetryJeti::Reset()
{
	m_timer.DisableIRQ(TIM_DIER_UIE);
	m_timer.Disable();

	HAL::InterruptRegistry.Disable(m_timer.NVIC_IRQn);
}

void TelemetryJeti::UpdateBuffer()
{
	memset(m_tx.SendBuffer, 0x00, MaxBufferSize);

	uint8_t crc = 0;
	uint8_t i = 0;
	uint8_t j = 0;

	for (; i < (m_tx.Length + 2); i++)
	{
		m_tx.SendBuffer[i] = m_tx.Data[i];

		if (i >= 1)
			m_tx.SendBuffer[i] |= 0x100;

		if (i >= 2)
		{
			crc ^= m_tx.Data[i];
			for (j = 0; j < 8; j++)
			{
				crc = (crc & 0x80) ? CRCPoly ^ (crc << 1) : (crc << 1);
			}
		}
	}

	m_tx.SendBuffer[i++] = crc | 0x100;

	//SimpleText part
	m_tx.SendBuffer[i++] = 0xFE;

	for (j = 0; j < MaxSimpleTextSize - 2; j++)
		m_tx.SendBuffer[i++] = 0x00 | 0x100;

	m_tx.SendBuffer[i++] = 0xFF;
	m_tx.BufferLength = i;

	m_tx.BufferPos = 0;
	m_bitPos = 0;
	m_parity = 0;
}

void TelemetryJeti::ISR()
{
	if (m_timer.GetInterruptSource(TIM_SR_UIF))
	{
		m_timer.ClearFlag(TIM_SR_UIF);

		if (m_bitPos == 0) //Start Bit
		{
			m_pin.Off();
			m_tx.CurrentData = m_tx.SendBuffer[m_tx.BufferPos];
		}
		else if (m_bitPos <= 9) //Data Bits
		{
			m_parity ^= (m_tx.CurrentData & 0x01);

			if ((m_tx.CurrentData & 0x01) == 0x01)
				m_pin.On();
			else
				m_pin.Off();

			m_tx.CurrentData >>= 1;
		}
		else if (m_bitPos <= 10) //Parity Bit
			(m_parity ^ 0x01) == 0 ? m_pin.Off() : m_pin.On();
		else if (m_bitPos <= 12) //Stop Bits
			m_pin.On();

		if (m_bitPos == 12)
		{
			m_bitPos = 0;
			m_parity = 0;

			m_tx.BufferPos++;
			if (m_tx.BufferLength == m_tx.BufferPos)
			{
				m_timer.Disable();
				eventFlag.signal_isr();
			}
		}
		else
			m_bitPos++;
	}
}

void TelemetryJeti::DeInit()
{
	m_run = 0;
	eventFlag.signal();
}

} /* namespace App */

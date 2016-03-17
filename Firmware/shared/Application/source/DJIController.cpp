/*
 * DJIController.cpp
 *
 *  Created on: 17.09.2015
 *      Author: cem
 */

#include "DJIController.h"
#include "System.h"
#include "CRC.h"

extern uint8_t PriorityFC;

namespace App
{

void DJIController::Init()
{
	m_can.Init(0, 1, 0, 1, 0, 0, CAN_BTR_SJW_1TQ, CAN_BTR_TS1_8TQ, CAN_BTR_TS2_3TQ, 3, 0, 0);

	m_can.FilterIdList16BitInit(0, 0x90 << 5, 0x108 << 5, 0x7F8 << 5, 0x388 << 5, 0, 1);
	m_can.FilterIdList16BitInit(1, 0x308 << 5, 0x92 << 5, 0x00, 0x00, 0, 1);
	//m_can.FilterIdMask32BitInit(0, 0, 0, 0, 1);

	HAL::CRC::PowerUp();

	HAL::InterruptRegistry.Enable(m_can.NVIC_RX0_IRQn, PriorityFC, this);

	m_can.EnableIRQ(CAN_IER_FMPIE0);
}

void DJIController::Run()
{
	DJIParser* parser = NULL;
	DJIChannel* channel = NULL;
	uint_fast32_t time = 0;

	uint8_t dataReceived = 0;
	uint8_t errorLogged = 0;

	uint8_t newData = 0;

	HAL::CANTxMessage txMessage1 = { 0x108, 0, 0, 8, { 0x55, 0xAA, 0x55, 0xAA, 0x07, 0x10, 0x00, 0x00 } };
	HAL::CANTxMessage txMessage2 = { 0x108, 0, 0, 4, { 0x66, 0xCC, 0x66, 0xCC } };
	HAL::CANRxMessage msg;

	m_nextHeartbeat = OSAL::Timer::GetTime() + delay_sec(2);
	m_nextTimeout = OSAL::Timer::GetTime() + delay_sec(3);

	for (;;)
	{
		newData = m_channel.pop(msg, delay_ms(150));

		time = OSAL::Timer::GetTime();

		if (newData)
		{
			dataReceived = 1;
			m_nextTimeout = time + delay_sec(3);

			//Process CAN Bus Messages
			if (SensorData.GetFCType() == FCType::Unknown)
			{
				switch (msg.Id)
				{
				case 0x90:
					SensorData.SetFCType(FCType::Naza);
					break;
				case 0x92:
					SensorData.SetFCType(FCType::A2);
					break;
				default:
					break;
				}
			}

			switch (SensorData.GetFCType())
			{
			case FCType::Naza:
			case FCType::Phantom:
			case FCType::Wookong:
				parser = &v1Parser;
				break;
			case FCType::A2:
				parser = &v2Parser;
				break;
			default:
				parser = NULL;
				break;
			}

			if (parser != NULL)
			{
				channel = findChannel(msg.Id);
				if (channel != NULL)
					parser->Parse(channel, &msg);
			}
		}

		if (m_nextTimeout > time)
		{
			if (SensorData.GetFCType() == FCType::Naza || SensorData.GetFCType() == FCType::Phantom
					|| SensorData.GetFCType() == FCType::Wookong || SensorData.GetFCType() == FCType::Unknown)
			{
				if (m_nextHeartbeat < time)
				{
					m_can.Transmit(&txMessage1);
					m_can.Transmit(&txMessage2);
					m_nextHeartbeat = time + delay_sec(2);
				}
			}
		}
		else if(dataReceived && !errorLogged)
		{
			SystemService.logError("No response from Controller!");
			errorLogged = 1;
		}
	}
}

DJIChannel* DJIController::findChannel(uint16_t id)
{
	for (uint8_t i = 0; i < DJIChannels; i++)
	{
		if (channels[i].ChannelId == id)
			return &channels[i];

		if (channels[i].ChannelId == 0)
		{
			channels[i].ChannelId = id;
			return &channels[i];
		}
	}

	return NULL;
}

void DJIController::ISR()
{
	if (m_can.FifoPendig(0))
	{
		if (m_channel.get_free_size() == 0)
		{
			m_can.FifoRelease(0);
		}
		else
		{
			m_can.Receive(0, 1, &m_tmp_msg);
			m_channel.push(m_tmp_msg);
		}
	}
}

}

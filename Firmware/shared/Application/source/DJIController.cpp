/*
 * DJIController.cpp
 *
 *  Created on: 17.09.2015
 *      Author: cem
 */

#include "DJIController.h"

namespace App
{

void DJIController::Init()
{
	m_can.Init(0, 0, 0, 1, 0, 0, CAN_BTR_SJW_1TQ, CAN_BTR_TS1_8TQ, CAN_BTR_TS2_3TQ, 3, 0, 0);
	/*
	 CAN_FilterInitStructure.CAN_FilterIdHigh = 0x090 << 5;
	 CAN_FilterInitStructure.CAN_FilterIdHigh = 0x108 << 5;
	 CAN_FilterInitStructure.CAN_FilterIdHigh = 0x7F8 << 5;
	 CAN_FilterInitStructure.CAN_FilterIdHigh = 0x388 << 5;
	 CAN_FilterInitStructure.CAN_FilterIdHigh = 0x308 << 5;
	 */

	m_can.FilterIdList16BitInit(0, 0x90 << 5, 0x108 << 5, 0x7F8 << 5, 0x388 << 5, 0, 1);
	m_can.FilterIdList16BitInit(1, 0x308 << 5, 0x00, 0x00, 0x00, 0, 1);
	//m_can.FilterIdMask32BitInit(0, 0, 0, 0, 1);

	HAL::InterruptRegistry.Enable(m_can.NVIC_RX0_IRQn, 15, this);

	m_can.EnableIRQ(CAN_IER_FMPIE0);
}

void DJIController::Run()
{
	DJIParser* parser = NULL;
	DJIChannel* channel = NULL;
	uint_fast32_t time = 0;
	uint8_t newData = 0;

	HAL::CANTxMessage txMessage1 = { 0x108, 0, 0, 8, { 0x55, 0xAA, 0x55, 0xAA, 0x07, 0x10, 0x00, 0x00 } };
	HAL::CANTxMessage txMessage2 = { 0x108, 0, 0, 4, { 0x66, 0xCC, 0x66, 0xCC } };

	m_nextHeartbeat = OSAL::Timer::GetTime() + delay_sec(2);
	m_nextTimeout = OSAL::Timer::GetTime()+ delay_sec(3); //first collect some other messages to avoid osd heartbeat if present

	for (;;)
	{
		newData = m_PacketArrived.wait(delay_ms(150));

		time = OSAL::Timer::GetTime();

		if (newData)
		{
			m_nextTimeout = time + delay_sec(3);

			HAL::CANRxMessage* msg = m_queue.Peek();

			//Process CAN Bus Messages
			if (SensorData.GetFCType() == FCType::Unknown)
			{
				switch (msg->Id)
				{
				case 0x90:
					SensorData.SetFCType(FCType::Naza);
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
				parser = NULL;
				break;
			default:
				parser = NULL;
				break;
			}

			if (parser != NULL)
			{
				channel = findChannel(msg->Id);
				if (channel != NULL)
					parser->Parse(channel, msg);
			}

			m_queue.Dequeue();
		}

		if (m_nextTimeout > time)
		{
			if (SensorData.GetFCType() != FCType::A2)
			{
				if (m_nextHeartbeat < time)
				{
					m_can.Transmit(&txMessage1);
					m_can.Transmit(&txMessage2);
					m_nextHeartbeat = time + delay_sec(2);
				}
			}
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
		HAL::CANRxMessage* msg = m_queue.Enqueue();
		if (msg != NULL)
		{
			m_can.Receive(0, 1, msg);
			m_PacketArrived.signal_isr();
		}
		else
		{
			err = 1;
			m_can.FifoRelease(0);
		}
	}
}

}

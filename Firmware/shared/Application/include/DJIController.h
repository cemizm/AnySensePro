/*
 * DJIController.h
 *
 *  Created on: 17.09.2015
 *      Author: cem
 */

#ifndef APPLICATION_INCLUDE_DJICONTROLLER_H_
#define APPLICATION_INCLUDE_DJICONTROLLER_H_

#include <OSAL.h>

#include <Interrupt.h>

#include <CAN.h>
#include <SensorStore.h>

#include <Queue.h>

#include "DJIParser.h"
#include "DJIParserV1.h"

namespace App
{

static const uint8_t DJIChannels = 6;

class DJIController: HAL::InterruptHandler
{
private:
	HAL::CAN& m_can;
	Utils::Queue<HAL::CANRxMessage, 10> m_queue;
	OSAL::EventFlag m_PacketArrived;
	uint_fast32_t m_nextHeartbeat;
	uint_fast32_t m_nextTimeout;
	DJIChannel channels[DJIChannels];
	DJIParserV1 v1Parser;


	DJIChannel* findChannel(uint16_t id);
public:
	DJIController(HAL::CAN& can) :
			m_can(can), m_queue(), m_PacketArrived(), m_nextHeartbeat(), m_nextTimeout(), v1Parser(), err(0)
	{

	}
	uint8_t err;
	void Init();
	void Run();
	void ISR() override;
};

} /* namespace Application */

#endif /* LIBS_INCLUDE_DJICONTROLLER_H_ */

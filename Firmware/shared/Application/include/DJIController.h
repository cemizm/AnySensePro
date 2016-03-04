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
#include "DJIParserV2.h"

namespace App
{

static const uint8_t DJIChannels = 6;

class DJIController: HAL::InterruptHandler
{
private:
	HAL::CAN& m_can;
	OSAL::Channel<HAL::CANRxMessage, 5> m_channel;
	HAL::CANRxMessage m_tmp_msg;
	uint_fast32_t m_nextHeartbeat;
	uint_fast32_t m_nextTimeout;
	DJIChannel channels[DJIChannels];
	DJIParserV1 v1Parser;
	DJIParserV2 v2Parser;

	DJIChannel* findChannel(uint16_t id);
public:
	DJIController(HAL::CAN& can) :
			m_can(can), m_channel(), m_tmp_msg(), m_nextHeartbeat(), m_nextTimeout(), v1Parser(), v2Parser()
	{

	}

	void Init();
	void Run();
	void ISR() override;
};

} /* namespace Application */

#endif /* LIBS_INCLUDE_DJICONTROLLER_H_ */

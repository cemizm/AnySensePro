/*
 * FCAlign.h
 *
 *  Created on: 13.06.2016
 *      Author: cem
 */

#ifndef APPLICATION_SOURCE_FCALIGN_H_
#define APPLICATION_SOURCE_FCALIGN_H_

#include "USART.h"
#include "Interrupt.h"

#include "FCAdapter.h"

namespace App
{

/*
 *
 */
class FCAlign: public FCAdapter, public HAL::InterruptHandler
{

private:
	static const uint8_t TmpReceiveBufferSize = 32;
	static const uint8_t ReceiveBufferSize = 128;
	static const uint8_t PaketSize = 64;
	static const uint8_t PacketContentSize = 38;
	static const uint16_t StartSign = 0x6BD2;

	HAL::USART& m_usart;
	OSAL::EventFlag gotMsg;
	uint8_t m_tmpReceiveBuffer[TmpReceiveBufferSize];
	uint8_t m_tmpReceiveIndex;
	uint8_t m_receiveBuffer[ReceiveBufferSize];
	uint8_t m_receiveIndex;
	uint8_t m_processIndex;

public:
	FCAlign(HAL::USART& usart) :
			FCAdapter(), m_usart(usart), m_tmpReceiveBuffer(), m_tmpReceiveIndex(0), m_receiveBuffer(), m_receiveIndex(0), m_processIndex(
					0)
	{
	}

	void Init() override;
	void Run() override;
	void ISR() override;

	void DeInit();
};

} /* namespace App */

#endif /* APPLICATION_SOURCE_FCALIGN_H_ */

/*
 * USART.h
 *
 *  Created on: 27.10.2015
 *      Author: cem
 */

#ifndef HAL_INCLUDE_USART_H_
#define HAL_INCLUDE_USART_H_

#include "Pin.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>

namespace HAL
{

class USART
{
private:
	uint32_t m_usart;
	rcc_periph_clken m_rcc_clock;
	Pin& m_tx;
	Pin& m_rx;
public:
	USART(uint32_t usart, rcc_periph_clken rcc_clock, Pin& tx, Pin& rx) :
			m_usart(usart), m_rcc_clock(rcc_clock), m_tx(tx), m_rx(rx)
	{
	}

	inline void Init()
	{
		rcc_periph_clock_enable(m_rcc_clock);
	}

	inline void SetBaudrate(uint32_t baud);
	inline void SetDatabits(uint32_t bits);
	inline void SetStopbits(uint32_t stopbits);
	inline void SetParity(uint32_t parity);
	inline void SetMode(uint32_t mode);
	inline void SetFlowControl(uint32_t flowcontrol);
	inline void Enable();
	inline void Disable();
	inline void Send(uint16_t data);
	inline uint16_t Receive();
	inline void WaitSendReady();
	inline void WaitReceiveReady();
	inline void SendBlocking(uint16_t data);
	inline uint16_t ReceiveBlocking();
	inline void EnableRxDma();
	inline void DisableRxDma();
	inline void EnableTxDma();
	inline void DisableTxDma();
	inline void EnableRxInterrupt();
	inline void DisableRxInterrupt();
	inline void EnableTxInterrupt();
	inline void DisableTxInterrupt();
	inline void EnableErrorInterrupt();
	inline void DisableErrorInterrupt();
	inline bool GetFlag(uint32_t flag);
	inline bool GetInterruptSource(uint32_t flag);
};

} /* namespace HAL */

#endif /* HAL_INCLUDE_USART_H_ */

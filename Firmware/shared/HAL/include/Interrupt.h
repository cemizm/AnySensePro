/*
 * Interupt.h
 *
 *  Created on: 14.09.2015
 *      Author: cem
 */

#ifndef HAL_INCLUDE_INTERRUPT_H_
#define HAL_INCLUDE_INTERRUPT_H_

#include <libopencm3/cm3/nvic.h>

namespace HAL
{

class InterruptHandler
{
public:
	virtual void ISR()
	{
	}
	virtual ~InterruptHandler()
	{
	}
};

class InterruptManager
{
private:
	InterruptHandler* m_handler[NVIC_IRQ_COUNT];

public:
	void HandleISR(uint8_t irqn);
	void Enable(uint8_t irqn, uint8_t priority, InterruptHandler* handler);
	void Disable(uint8_t irqn);
};

extern InterruptManager InterruptRegistry;

}

#endif /* HAL_INCLUDE_INTERRUPT_H_ */

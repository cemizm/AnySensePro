/*
 * Interrupt.c
 *
 *  Created on: 14.09.2015
 *      Author: cem
 */

#include "Interrupt.h"
#include <stddef.h>

namespace HAL
{

InterruptManager InterruptRegistry;

void InterruptManager::HandleISR(uint8_t irqn)
{
	if (m_handler[irqn] != NULL)
		m_handler[irqn]->ISR();
}

void InterruptManager::Enable(uint8_t irqn, uint8_t priority, InterruptHandler* handler)
{
	m_handler[irqn] = handler;
	nvic_set_priority(irqn, priority);
	nvic_enable_irq(irqn);
}

void InterruptManager::Disable(uint8_t irqn)
{
	nvic_disable_irq(irqn);
	m_handler[irqn] = NULL;
}



}

/*
 * Timer.h
 *
 *  Created on: 17.03.2016
 *      Author: cem
 */

#ifndef HAL_INCLUDE_TIMER_H_
#define HAL_INCLUDE_TIMER_H_

#include "libopencm3/stm32/rcc.h"
#include "libopencm3/stm32/timer.h"

namespace HAL
{

/*
 *
 */
class Timer
{
private:
	const uint32_t m_timer;
	const rcc_periph_clken m_periph_clock;

public:
	const uint8_t NVIC_IRQn;
	const uint32_t clock_frequency;

	Timer(uint32_t timer, rcc_periph_clken periph_clock, uint8_t nvic_irqn, uint32_t clock_freq) :
			m_timer(timer), m_periph_clock(periph_clock), NVIC_IRQn(nvic_irqn), clock_frequency(clock_freq)
	{

	}

	inline void PowerUp() const
	{
		rcc_periph_clock_enable(m_periph_clock);
	}

	inline void DirectionDown() const
	{
		timer_direction_down(m_timer);
	}

	inline void DirectionUp() const
	{
		timer_direction_up(m_timer);
	}

	inline void SetClockDivision(uint32_t clock_div) const
	{
		timer_set_clock_division(m_timer, clock_div);
	}

	inline void SetPrescaler(uint32_t prescale) const
	{
		timer_set_prescaler(m_timer, prescale);
	}

	inline void SetPeriod(uint32_t period) const
	{
		timer_set_period(m_timer, period);
	}
	inline void EnableIRQ(uint32_t irq) const
	{
		timer_enable_irq(m_timer, irq);
	}
	inline void DisableIRQ(uint32_t irq) const
	{
		timer_disable_irq(m_timer, irq);
	}
	inline uint8_t GetInterruptSource(uint32_t flag) const
	{
		return timer_interrupt_source(m_timer, flag);
	}
	inline void ClearFlag(uint32_t flag) const
	{
		timer_clear_flag(m_timer, flag);
	}
	inline void Enable()
	{
		timer_enable_counter(m_timer);
	}
	inline void Disable()
	{
		timer_disable_counter(m_timer);
	}
};

} /* namespace App */

#endif /* HAL_INCLUDE_TIMER_H_ */

/*
 * Pin.h
 *
 *  Created on: 10.09.2015
 *      Author: cem
 */

#ifndef HAL_INCLUDE_PIN_H_
#define HAL_INCLUDE_PIN_H_

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/exti.h>

#include <cstdint>
#include <cstddef>

namespace HAL
{

class Pin
{
private:
	uint32_t m_gpio_port;
	rcc_periph_clken m_gpio_clock;
	uint16_t m_pin_no;
	uint32_t m_extis;
public:
	const uint8_t EXTI_NVIC_IRQ;

	Pin(uint32_t gpio_port, rcc_periph_clken port_clock, uint16_t pin_no) :
			m_gpio_port(gpio_port), m_gpio_clock(port_clock), m_pin_no(pin_no), m_extis(0), EXTI_NVIC_IRQ(0)
	{
	}

	Pin(uint32_t gpio_port, rcc_periph_clken port_clock, uint16_t pin_no, uint32_t extis, uint8_t irqn) :
			m_gpio_port(gpio_port), m_gpio_clock(port_clock), m_pin_no(pin_no), m_extis(extis), EXTI_NVIC_IRQ(irqn)
	{
	}
	inline void PowerUp() const
	{
		rcc_periph_clock_enable(m_gpio_clock);
	}

	inline void On() const
	{
		gpio_set(m_gpio_port, m_pin_no);
	}

	inline void Off() const
	{
		gpio_clear(m_gpio_port, m_pin_no);
	}

	inline void Toggle() const
	{
		gpio_toggle(m_gpio_port, m_pin_no);
	}

	inline void ModeSetup(uint8_t mode, uint8_t pull_up_down) const
	{
		gpio_mode_setup(m_gpio_port, mode, pull_up_down, m_pin_no);
	}

	inline void SetOutputOptions(uint8_t otype, uint8_t speed) const
	{
		gpio_set_output_options(m_gpio_port, otype, speed, m_pin_no);
	}

	inline void Alternate(uint8_t alt_func_num) const
	{
		gpio_set_af(m_gpio_port, alt_func_num, m_pin_no);
	}

	inline uint8_t Get() const
	{
		return gpio_get(m_gpio_port, m_pin_no);
	}

	inline uint8_t EXTI_IsPendingBit()
	{
		return exti_get_flag_status(m_extis) & m_extis ? 1 : 0;
	}
	inline void EXTI_ResetPendingBit()
	{
		exti_reset_request(m_extis);
	}
	inline void EXTI_SelectSource()
	{
		exti_select_source(m_extis, m_gpio_port);
	}
	inline void EXTI_SetTrigger(exti_trigger_type trigger)
	{
		exti_set_trigger(m_extis, trigger);
	}
	inline void EXTI_Enable()
	{
		exti_enable_request(m_extis);
	}
}
;

}

#endif /* HAL_INCLUDE_PIN_H_ */

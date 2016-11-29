/*
 * Timer.h
 *
 *  Created on: 16.10.2015
 *      Author: cem
 */

#ifndef OSAL_INCLUDE_TIMER_H_
#define OSAL_INCLUDE_TIMER_H_

#include "scmRTOS.h"

namespace OSAL
{

class Timer
{
public:
	static void Sleep(uint16_t timeout)
	{
		OS::TBaseProcess::sleep(delay_us(timeout));
	}
	static void SleepMS(uint16_t ms)
	{
		OS::TBaseProcess::sleep(delay_ms(ms));
	}
	static void SleepSeconds(uint16_t second)
	{
		OS::TBaseProcess::sleep(delay_sec(second));
	}

	static uint_fast32_t GetTime()
	{
		return OS::get_tick_count();
	}

	static uint_fast32_t GetBootMS() {
		return OS::get_tick_count() / delay_ms(1);
	}

	static uint_fast32_t GetBootUS() {
		return OS::get_tick_count() / delay_us(1);
	}
};

} /* namespace OSAL */

#endif /* OSAL_INCLUDE_TIMER_H_ */

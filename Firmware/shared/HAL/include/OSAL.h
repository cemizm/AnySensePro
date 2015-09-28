/*
 * OSAL.h
 *
 *  Created on: 10.09.2015
 *      Author: cem
 */

#ifndef HAL_INCLUDE_OSAL_H_
#define HAL_INCLUDE_OSAL_H_

#include "scmRTOS.h"

#define RUNLOOP __attribute__((__noreturn__))

namespace HAL
{


using OSALPriority = OS::TPriority;
using OSALISRSupport = OS::TISRW;
using OSALEventFlag = OS::TEventFlag;

class OSAL
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
};

}

#endif /* HAL_INCLUDE_OSAL_H_ */

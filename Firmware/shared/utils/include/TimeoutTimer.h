/*
 * TimeoutTimer.h
 *
 *  Created on: 28.09.2015
 *      Author: cem
 */

#ifndef UTILS_INCLUDE_TIMEOUTTIMER_H_
#define UTILS_INCLUDE_TIMEOUTTIMER_H_

#include <stdint.h>
#include <OSAL.h>

namespace Utils
{
class TimeoutTimer
{
private:
	uint_fast32_t time;
	uint_fast32_t started;
public:
	TimeoutTimer(uint16_t delay)
	{
		Reset(delay);
	}

	void Reset(uint16_t delay)
	{
		started = HAL::OSAL::GetTime();
		time = started + delay;
	}

	uint8_t IsTimeout()
	{
		return started > 0 && time < HAL::OSAL::GetTime() ? 1 : 0;
	}
};

} /* namespace Storage */

#endif /* UTILS_INCLUDE_TIMEOUTTIMER_H_ */

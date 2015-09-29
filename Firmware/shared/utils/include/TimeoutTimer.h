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

namespace Storage
{
class TimeoutTimer
{
private:
	uint_fast32_t time;
public:
	TimeoutTimer(uint16_t delay)
	{
		Reset(delay);
	}

	void Reset(uint16_t delay)
	{
		time = HAL::OSAL::GetTime() + delay;
	}

	uint8_t IsTimeout()
	{
		return time < HAL::OSAL::GetTime() ? 1 : 0;
	}
};

} /* namespace Storage */

#endif /* UTILS_INCLUDE_TIMEOUTTIMER_H_ */

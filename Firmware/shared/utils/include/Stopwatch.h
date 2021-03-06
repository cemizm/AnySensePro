/*
 * Stopwatch.h
 *
 *  Created on: 01.10.2015
 *      Author: cem
 */

#ifndef UTILS_INCLUDE_STOPWATCH_H_
#define UTILS_INCLUDE_STOPWATCH_H_

#include <OSAL.h>

namespace Utils
{

#ifdef DEBUG

class Stopwatch
{
private:
	volatile uint_fast32_t start;
public:
	Stopwatch()
	{
		Reset();
	}

	void Reset()
	{
		start = OSAL::Timer::GetTime();
	}

	uint32_t ElapsedTime()
	{
		return OSAL::Timer::GetTime() - start;
	}

};

#endif

} /* namespace Storage */

#endif /* UTILS_INCLUDE_STOPWATCH_H_ */

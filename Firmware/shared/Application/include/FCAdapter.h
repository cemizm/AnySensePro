/*
 * FCAdapter.h
 *
 *  Created on: 14.03.2016
 *      Author: cem
 */

#ifndef APPLICATION_INCLUDE_FCADAPTER_H_
#define APPLICATION_INCLUDE_FCADAPTER_H_

#include "OSAL.h"

namespace App
{

class FCAdapter
{
private:
	uint_fast32_t m_alive;
protected:
public:
	static const uint16_t Workspace = 1024 * 1;
	static const uint16_t WaitForDataTimeout = delay_ms(800);

	enum Protocol
	{
		MAVLink = 0, Tarot = 1, Last = 2,
	};

	FCAdapter() :
			m_alive(OSAL::Timer::GetTime() + delay_sec(5))
	{

	}

	virtual void Init()
	{
	}
	virtual void Run(void)
	{
	}
	virtual ~FCAdapter(void)
	{
	}

	void SetHeartbeat()
	{
		m_alive = OSAL::Timer::GetTime() + delay_sec(5);
	}

	uint8_t IsAlive()
	{
		return (m_alive > OSAL::Timer::GetTime()) ? 1 : 0;
	}
};

}

#endif /* APPLICATION_INCLUDE_FCADAPTER_H_ */

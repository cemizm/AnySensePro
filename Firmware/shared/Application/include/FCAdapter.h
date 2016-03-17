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
protected:
public:
	static const uint16_t Workspace = 1024 * 3;

	enum Protocol
	{
		MAVLink = 0, Last = 1,
	};

	virtual void Init()
	{
	}
	virtual void Run(void)
	{
	}
	virtual ~FCAdapter(void)
	{
	}
};

}

#endif /* APPLICATION_INCLUDE_FCADAPTER_H_ */

/*
 * SensorAdapter.h
 *
 *  Created on: 11.03.2016
 *      Author: cem
 */

#ifndef APPLICATION_SOURCE_SENSORADAPTER_H_
#define APPLICATION_SOURCE_SENSORADAPTER_H_

#include "OSAL.h"

namespace App
{

class SensorAdapterBase
{
protected:
	OSAL::EventFlag eventFlag;
public:
	static const uint16_t Workspace = 512;

	enum Procotol
	{
		None = 0, FrSky = 1,
	};

	SensorAdapterBase() :
			eventFlag()
	{
	}

	virtual Procotol Handles()
	{
		return Procotol::None;
	}
	virtual void Init()
	{
		eventFlag.clear();
	}
	virtual void Run(void)
	{
		eventFlag.wait();
	}
	virtual void DeInit()
	{
		eventFlag.signal();
	}
	virtual void UpdateConfiguration(void)
	{

	}
	virtual ~SensorAdapterBase(void)
	{
	}
};

template<SensorAdapterBase::Procotol protocol>
class SensorAdapter: public SensorAdapterBase
{
protected:
	static const uint16_t ConfigKey = 0xCBA0 + protocol;
public:
	SensorAdapter() :
			SensorAdapterBase()
	{
	}
	Procotol Handles() override
	{
		return protocol;
	}
	virtual ~SensorAdapter(void)
	{
	}
};

} /* namespace App */

#endif /* APPLICATION_SOURCE_SENSORADAPTER_H_ */

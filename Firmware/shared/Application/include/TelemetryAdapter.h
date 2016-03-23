/*
 * TelemetryAdapter.h
 *
 *  Created on: 15.10.2015
 *      Author: cem
 */

#ifndef APPLICATION_INCLUDE_TELEMETRYADAPTER_H_
#define APPLICATION_INCLUDE_TELEMETRYADAPTER_H_

#include "Configuration.h"
#include "OSAL.h"

#define TELEMETRY_WORKSPACE		1024

extern uint8_t PriorityTelemetry;

namespace App
{

class TelemetryAdapterBase
{
protected:
	OSAL::EventFlag eventFlag;
public:
	TelemetryAdapterBase() :
			eventFlag()
	{
	}

	virtual TelemetryProtocol Handles()
	{
		return TelemetryProtocol::None;
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
	virtual ~TelemetryAdapterBase(void)
	{
	}
};

template<TelemetryProtocol protocol>
class TelemetryAdapter: public TelemetryAdapterBase
{
protected:
	static const uint16_t ConfigKey = 0xCB00 + protocol;
public:
	TelemetryAdapter() :
			TelemetryAdapterBase()
	{
	}
	TelemetryProtocol Handles() override
	{
		return protocol;
	}
	virtual ~TelemetryAdapter(void)
	{
	}
};

} /* namespace Application */

#endif /* APPLICATION_INCLUDE_TELEMETRYADAPTER_H_ */

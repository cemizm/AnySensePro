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

#define TELEMETRY_WORKSPACE		1024*3

namespace App
{

class TelemetryAdapter
{
protected:
	OSAL::EventFlag eventFlag;
public:
	TelemetryAdapter() :
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
	virtual ~TelemetryAdapter(void)
	{
	}
};

} /* namespace Application */

#endif /* APPLICATION_INCLUDE_TELEMETRYADAPTER_H_ */

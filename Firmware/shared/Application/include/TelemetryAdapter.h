/*
 * TelemetryAdapter.h
 *
 *  Created on: 15.10.2015
 *      Author: cem
 */

#ifndef APPLICATION_INCLUDE_TELEMETRYADAPTER_H_
#define APPLICATION_INCLUDE_TELEMETRYADAPTER_H_

#include <stdint.h>
#include <USART.h>

#define TELEMETRY_WORKSPACE		512

struct TelemetryPort
{
	TelemetryPort(HAL::USART& usart) :
			USART(usart)
	{
	}

	HAL::USART& USART;
};

namespace App
{

class TelemetryAdapter
{
public:
	virtual void Init(uint8_t* workspace, TelemetryPort& port)
	{
		(void) workspace;
		(void) port;
	}

	virtual void Run(void)
	{
	}

	virtual void DeInit()
	{
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

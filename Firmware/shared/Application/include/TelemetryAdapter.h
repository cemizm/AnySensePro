/*
 * TelemetryAdapter.h
 *
 *  Created on: 15.10.2015
 *      Author: cem
 */

#ifndef APPLICATION_INCLUDE_TELEMETRYADAPTER_H_
#define APPLICATION_INCLUDE_TELEMETRYADAPTER_H_

#include <stdint.h>
#include <stddef.h>

#define TELEMETRY_WORKSPACE		512

namespace App
{

class TelemetryAdapter
{
private:
public:
	void Init(uint8_t* workspace)
	{
		m_workspace = workspace;
		AdapterInit();
	}

	virtual void Run(void)
	{
	}

	void DeInit()
	{
		m_workspace = NULL;
		AdapterDeInit();
	}

	virtual ~TelemetryAdapter(void)
	{
	}
protected:
	uint8_t* m_workspace;

	virtual void AdapterInit(void)
	{

	}

	virtual void AdapterDeInit(void)
	{
	}
};

} /* namespace Application */

#endif /* APPLICATION_INCLUDE_TELEMETRYADAPTER_H_ */

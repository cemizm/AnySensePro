/*
 * TelemetryMAVLink.h
 *
 *  Created on: 18.10.2015
 *      Author: cem
 */

#ifndef APPLICATION_INCLUDE_TELEMETRYMAVLINK_H_
#define APPLICATION_INCLUDE_TELEMETRYMAVLINK_H_

#include "TelemetryAdapter.h"
#include "MAVLinkLayer.h"

namespace App
{

class TelemetryMAVLink: public TelemetryAdapter
{
protected:
	virtual void AdapterInit(void) override;
	virtual void AdapterDeInit(void) override;
	virtual void Run(void) override;
};


} /* namespace Application*/

#endif /* APPLICATION_INCLUDE_TELEMETRYMAVLINK_H_ */

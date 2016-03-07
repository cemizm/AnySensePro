/*
 * TelemetryMAVLink.h
 *
 *  Created on: 18.10.2015
 *      Author: cem
 */

#ifndef APPLICATION_INCLUDE_TELEMETRYMAVLINK_H_
#define APPLICATION_INCLUDE_TELEMETRYMAVLINK_H_

#include <USART.h>

#include "TelemetryAdapter.h"
#include "MAVLinkLayer.h"
#include "MAVLinkComm.h"
#include "OSAL.h"

namespace App
{

class TelemetryMAVLink: public TelemetryAdapter
{
private:
	MAVLinkComm mav;
protected:
	void Init() override;
	void Run(void) override;
public:
	TelemetryMAVLink(HAL::USART& usart) :
			TelemetryAdapter(), mav(usart)
	{
	}

	TelemetryProtocol Handles() override
	{
		return TelemetryProtocol::MAVLink;
	}
};

} /* namespace Application*/

#endif /* APPLICATION_INCLUDE_TELEMETRYMAVLINK_H_ */

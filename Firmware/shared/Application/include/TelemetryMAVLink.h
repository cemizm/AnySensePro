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

namespace App
{

class TelemetryMAVLink: public TelemetryAdapter<TelemetryProtocol::MAVLink>
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
};

} /* namespace Application*/

#endif /* APPLICATION_INCLUDE_TELEMETRYMAVLINK_H_ */

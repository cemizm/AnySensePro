/*
 * FCController.h
 *
 *  Created on: 14.03.2016
 *      Author: cem
 */

#ifndef APPLICATION_INCLUDE_FCCONTROLLER_H_
#define APPLICATION_INCLUDE_FCCONTROLLER_H_

#include "FCAdapter.h"
#include "USART.h"

namespace App
{

class FCController
{
private:
	HAL::USART& m_usart;
	uint8_t m_workspace[FCAdapter::Workspace];
	FCAdapter* m_active;
	FCAdapter::Protocol m_protocol;
public:
	FCController(HAL::USART& usart) :
			m_usart(usart), m_workspace(), m_active(nullptr), m_protocol(FCAdapter::Protocol::MAVLink)
	{
	}

	void Init();
	void Run();
};

} /* namespace App */

#endif /* APPLICATION_INCLUDE_FCCONTROLLER_H_ */

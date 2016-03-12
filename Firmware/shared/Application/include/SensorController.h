/*
 * SensorController.h
 *
 *  Created on: 11.03.2016
 *      Author: cem
 */

#ifndef APPLICATION_INCLUDE_SENSORCONTROLLER_H_
#define APPLICATION_INCLUDE_SENSORCONTROLLER_H_

#include "USART.h"
#include <SensorAdapter.h>

namespace App
{

/*
 *
 */
class SensorController
{
private:
	uint8_t m_workspace[SensorAdapterBase::Workspace];
	SensorAdapterBase* m_active;

	HAL::USART& m_usart;

public:
	SensorController(HAL::USART& usart) :
			m_workspace(), m_active(NULL), m_usart(usart)
	{

	}
	void Init();
	void Run();
};

} /* namespace App */

#endif /* APPLICATION_INCLUDE_SENSORCONTROLLER_H_ */

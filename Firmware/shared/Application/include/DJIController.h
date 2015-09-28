/*
 * DJIController.h
 *
 *  Created on: 17.09.2015
 *      Author: cem
 */

#ifndef APPLICATION_INCLUDE_DJICONTROLLER_H_
#define APPLICATION_INCLUDE_DJICONTROLLER_H_

#include <OSAL.h>

#include <CAN.h>
#include <SensorStore.h>

namespace Application
{

class DJIController
{
private:
	HAL::CAN& m_can;
	Libs::SensorStore& m_SensorStore;
public:
	DJIController(HAL::CAN& can, Libs::SensorStore& sensorStore) :
			m_can(can), m_SensorStore(sensorStore)
	{

	}
	void Init();
	void Run();
};

} /* namespace Application */

#endif /* LIBS_INCLUDE_DJICONTROLLER_H_ */

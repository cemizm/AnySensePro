/*
 * BLEWorker.h
 *
 *  Created on: 14.09.2015
 *      Author: cem
 */

#ifndef APPLICATION_INCLUDE_BLEWORKER_H_
#define APPLICATION_INCLUDE_BLEWORKER_H_

#include <BLEDevice.h>

namespace Application
{

class BLEWorker
{
	BlueNRG::BLEDevice& m_device;
public:
	BLEWorker(BlueNRG::BLEDevice& device) :
			m_device(device)
	{
	}
	void Run();
private:
};

} /* namespace Application */

#endif /* APPLICATION_INCLUDE_BLEWORKER_H_ */

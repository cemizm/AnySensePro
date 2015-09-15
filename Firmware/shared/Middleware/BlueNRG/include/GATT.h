/*
 * GATT.h
 *
 *  Created on: 15.09.2015
 *      Author: cem
 */

#ifndef MIDDLEWARE_BLUENRG_INCLUDE_GATT_H_
#define MIDDLEWARE_BLUENRG_INCLUDE_GATT_H_

#include "HCI.h"

namespace BlueNRG
{

class GATTInterface
{
private:
	HCInterface& m_HCInterface;

public:
	GATTInterface(HCInterface& hcInterface) :
			m_HCInterface(hcInterface)
	{
	}

	void Init();
};

} /* namespace BlueNRG */

#endif /* MIDDLEWARE_BLUENRG_INCLUDE_GATT_H_ */

/*
 * GAP.h
 *
 *  Created on: 17.09.2015
 *      Author: cem
 */

#ifndef MIDDLEWARE_BLUENRG_INCLUDE_GAP_H_
#define MIDDLEWARE_BLUENRG_INCLUDE_GAP_H_

#include "HCI.h"

namespace BlueNRG
{

enum GAPRole
{
	Peripheral = 0x01, Broadcaster = 0x02, Central = 0x03, Observer = 0x04,
};

class GAPInterface
{
private:
	HCInterface& m_HCInterface;

public:
	GAPInterface(HCInterface& hcInterface) :
			m_HCInterface(hcInterface)
	{
	}

	HCIGenericStatusCode Init(GAPRole role, uint16_t* hGapService, uint16_t* hDeviceNameChar, uint16_t* hAppearanceChar);
};

} /* namespace BlueNRG */

#endif /* MIDDLEWARE_BLUENRG_INCLUDE_GAP_H_ */

/*
 * GATT.cpp
 *
 *  Created on: 15.09.2015
 *      Author: cem
 */

#include "GATT.h"

const uint16_t OCF_GATT_INIT = 0x00;

enum GATTCommand
{
	Init = 0x0101,
};

namespace BlueNRG
{


HCIGenericStatusCode GATTInterface::Init()
{
	HCIEvent event;
	if (m_HCInterface.SendCommand(OpCodeOGF::VendorSpecific, GATTCommand::Init, NULL, 0, &event) < 0)
		return HCIGenericStatusCode::Timeout;

	return (HCIGenericStatusCode) event.Data.Status.StatusCode;
}

} /* namespace BlueNRG */

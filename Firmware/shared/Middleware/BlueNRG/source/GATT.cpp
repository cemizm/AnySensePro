/*
 * GATT.cpp
 *
 *  Created on: 15.09.2015
 *      Author: cem
 */

#include <GATT.h>

namespace BlueNRG
{

enum GATTCommand
{
	Init = 0x0101,
};

const uint16_t OCF_GATT_INIT = 0x00;

void GATTInterface::Init()
{
	m_HCInterface.SendCommand(OpCodeOGF::VendorSpecific, GATTCommand::Init, NULL, 0);
}

} /* namespace BlueNRG */

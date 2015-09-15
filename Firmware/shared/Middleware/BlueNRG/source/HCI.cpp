/*
 * HCI.cpp
 *
 *  Created on: 15.09.2015
 *      Author: cem
 */

#include "HCI.h"
#include <string.h>

namespace BlueNRG
{

void HCInterface::SendCommand(OpCodeOGF ogf, uint16_t ocf, uint8_t parameters[], uint16_t size)
{
	packet.Type = HCIPacketType::Command;
	packet.data.Command.OGF = ogf;
	packet.data.Command.OCF = ocf;
	packet.data.Command.Length = size;

	if (parameters != NULL && size > 0)
		memcpy(packet.data.Command.Parameters, parameters, size);

	m_bleInterface.Send((uint8_t*) &packet, size + 4);
}

}

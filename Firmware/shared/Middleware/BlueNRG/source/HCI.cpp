/*
 * HCI.cpp
 *
 *  Created on: 15.09.2015
 *      Author: cem
 */

#include <string.h>

#include "HCI.h"

namespace BlueNRG
{

int16_t HCInterface::SendCommand(OpCodeOGF ogf, uint16_t ocf, uint8_t parameters[], uint16_t size, HCIEvent* event)
{
	HCIRequest* request = m_pendingRequests.alloc();
	if (request == NULL)
		return -1;


	HCIAutoRelease release = HCIAutoRelease(m_pendingRequests, request);

	request->Packet.Type = HCIPacketType::Command;
	request->Packet.Data.Command.OpCode.OGF = ogf;
	request->Packet.Data.Command.OpCode.OCF = ocf;
	request->Packet.Data.Command.Length = size;

	if (parameters != NULL && size > 0)
		memcpy(request->Packet.Data.Command.Parameters, parameters, size);

	request->Clear();

	m_bleInterface.Send((uint8_t*) &request->Packet, size + 4);

	if (!request->EventFlag.wait(delay_ms(100)))
		return -1;

	memcpy(event, &request->Event, request->Event.Length + 3);

	return request->EventSize;
}

void HCInterface::OnDataReceived(uint8_t data[], uint16_t size)
{
	HCIPacket* packet = (HCIPacket*) data;

	if (!VerifyPacket(packet, size))
		return;

	HCIRequest* requests[MAX_HCI_REQUESTS];
	uint8_t pending = m_pendingRequests.getItems(requests, MAX_HCI_REQUESTS);
	uint8_t eventSize = size - offsetof(HCIPacket, Data);

	if (pending > 0)
	{
		switch (packet->Data.Event.EventCode)
		{
		case HCIEventCode::CommandStatus:
		case HCIEventCode::CommandComplete:
			for (uint8_t i = 0; i < pending; i++)
			{
				if (requests[i]->Packet.Data.Command.OpCode == packet->Data.Event.Data.Status.OpCode)
				{
					memcpy(&requests[i]->Event, &packet->Data, eventSize);
					requests[i]->EventSize = eventSize;
					requests[i]->EventFlag.signal();
					return;
				}
			}
			break;
		default:
			break;
		}
	}

	//TODO: relay unhandled Events...
}

uint8_t HCInterface::VerifyPacket(HCIPacket* packet, uint16_t size)
{
	static const uint8_t t_size = offsetof(HCIPacket, Data) + offsetof(HCIEvent, Length);

	if (size < t_size)
		return 0;

	if (packet->Type != HCIPacketType::Event)
		return 0;

	if (packet->Data.Event.Length != (size - (t_size + 1)))
		return 0;

	return 1;
}

}

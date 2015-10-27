/*
 * DJIParser.h
 *
 *  Created on: 05.10.2015
 *      Author: cem
 */

#ifndef APPLICATION_INCLUDE_DJIPARSER_H_
#define APPLICATION_INCLUDE_DJIPARSER_H_

#include <stddef.h>
#include <string.h>

#include <CAN.h>

namespace App
{

static const uint16_t DJIChannelSize = 256;

struct DJIChannel
{
	uint8_t Data[DJIChannelSize];
	uint16_t ChannelId;
	uint8_t IsSnyc;
	uint16_t SyncPosition;
	uint16_t Count;
};

class DJIParser
{
private:
public:
	virtual void Parse(DJIChannel* channel, HAL::CANRxMessage* msg)
	{
		(void) channel;
		(void) msg;
	}

	template<typename T>
	void FindNext(DJIChannel* channel, T sign)
	{
		if (channel->Count < sizeof(T))
			return;

		for (; channel->SyncPosition < (channel->Count - (sizeof(T) - 1)); channel->SyncPosition++)
		{
			if (*((T*) &channel->Data[channel->SyncPosition]) == sign)
			{
				MoveToSyncPos(channel);
				channel->IsSnyc = 1;
				return;
			}
		}

		if (channel->IsSnyc == 0 && (channel->Count + 8) > DJIChannelSize)
			MoveToSyncPos(channel);
	}

	void MoveToSyncPos(DJIChannel* channel)
	{
		channel->Count -= channel->SyncPosition;
		memmove(channel->Data, &channel->Data[channel->SyncPosition], channel->Count);

		channel->SyncPosition = 0;
	}

	virtual ~DJIParser()
	{
	}
};

} /* namespace Application */

#endif /* APPLICATION_INCLUDE_DJIPARSER_H_ */

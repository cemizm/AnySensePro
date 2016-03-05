/*
 * System.h
 *
 *  Created on: 03.03.2016
 *      Author: cem
 */

#ifndef APPLICATION_INCLUDE_SYSTEM_H_
#define APPLICATION_INCLUDE_SYSTEM_H_

#include <stdint.h>
#include <OSAL.h>

namespace App
{

const uint8_t LogEntries = 3;
const uint8_t LogEntryMsgLen = 25;

struct LogEntry
{
	uint8_t msgLen;
	unsigned char msg[LogEntryMsgLen];
};

class System
{
private:
	OSAL::Channel<LogEntry, LogEntries> logging;
	LogEntry working;
	void pushLogEntry(const char* msg);
	uint8_t popLogEntry(LogEntry& entry);
public:
};

extern System SystemService;

} /* namespace App */

#endif /* APPLICATION_INCLUDE_SYSTEM_H_ */

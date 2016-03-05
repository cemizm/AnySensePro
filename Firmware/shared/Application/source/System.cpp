/*
 * System.cpp
 *
 *  Created on: 03.03.2016
 *      Author: cem
 */

#include <System.h>
#include <string.h>

namespace App
{

System SystemService;

void System::pushLogEntry(const char* msg)
{
	if (!logging.get_free_size())
		return;

	working.msgLen = (uint8_t) strlen(msg);
	if (working.msgLen > LogEntryMsgLen)
		working.msgLen = LogEntryMsgLen;

	memcpy(working.msg, msg, working.msgLen);
	logging.push(working);
}

uint8_t System::popLogEntry(LogEntry& entry)
{
	if (logging.get_count() == 0)
		return 0;

	logging.pop(entry, 0);

	return 1;
}

} /* namespace App */

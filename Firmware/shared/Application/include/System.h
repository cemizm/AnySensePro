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
#include <Pin.h>

namespace App
{

const uint8_t LogEntries = 3;
const uint8_t LogEntryMsgLen = 25;

struct LogEntry
{
	uint8_t level;
	uint8_t msgLen;
	unsigned char msg[LogEntryMsgLen];
};

class System
{
private:
	HAL::Pin& ledError;
	HAL::Pin& ledActivity;
	OSAL::Channel<LogEntry, LogEntries> logging;
	OSAL::EventFlag loaded;
	LogEntry working;

	const uint8_t LevelWarning = 1;
	const uint8_t LevelError = 2;

	void log(uint8_t level, const char* msg);

public:
	System(HAL::Pin& error, HAL::Pin& activity) :
			ledError(error), ledActivity(activity)
	{
	}

	void isLoaded(timeout_t t = 0)
	{
		loaded.wait(t);
	}
	void signalLoaded()
	{
		loaded.signal();
	}

	void logWarning(const char* msg);
	void logError(const char* msg);

	void Run();
};

} /* namespace App */

extern App::System SystemService;

#endif /* APPLICATION_INCLUDE_SYSTEM_H_ */

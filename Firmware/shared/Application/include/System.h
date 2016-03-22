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

	uint_fast32_t ft_nextCalc;
	uint_fast32_t ft_measured;
	uint16_t ft_elapsed;

	uint_fast32_t amp_nextCalc;
	uint32_t amp_elapsed;

	uint_fast32_t cells_nextCalc;
	uint_fast32_t cells_measure_end;

	uint_fast32_t home_nextCalc;
	uint_fast32_t home_measure_end;
	uint_fast32_t home_gps_measure_end;

	uint_fast32_t gps_nextCalc;

	const uint8_t LevelWarning = 1;
	const uint8_t LevelError = 2;

	const uint8_t amp_elapsed_interval = 50;
	const uint16_t amp_elapsed_limit = (360000 / amp_elapsed_interval);

	void log(uint8_t level, const char* msg);

	void calcFlightime();
	void calcConsumption();
	void calcCells();
	void calcHome();
	void calcGPSData();

public:
	System(HAL::Pin& error, HAL::Pin& activity) :
			ledError(error), ledActivity(activity), ft_nextCalc(0), ft_measured(0), ft_elapsed(0), amp_nextCalc(0), amp_elapsed(
					0), cells_nextCalc(0), cells_measure_end(0), home_nextCalc(0), home_measure_end(0), home_gps_measure_end(0), gps_nextCalc(
					0)
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

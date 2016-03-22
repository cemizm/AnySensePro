/*
 * LoggingController.h
 *
 *  Created on: 17.03.2016
 *      Author: cem
 */

#ifndef APPLICATION_INCLUDE_LOGGINGCONTROLLER_H_
#define APPLICATION_INCLUDE_LOGGINGCONTROLLER_H_

#include "MAVLinkLayer.h"
#include "ff.h"

namespace App
{

class LoggingController
{
private:
	MAVLinkLayer m_mavlink;
	mavlink_message_t m_msg;
	uint16_t m_dataLen;
	uint32_t m_written;
	uint8_t m_currentData;

	uint8_t m_buffer[MAVLINK_MAX_PACKET_LEN];

	char m_fileName[25];
	FIL m_file;

	uint8_t m_currentSession;

	void updateFilename();
public:
	LoggingController() :
			m_mavlink(MAVLINK_COMM_0), m_msg(), m_dataLen(0), m_written(0), m_currentData(0), m_fileName(), m_file(), m_currentSession(
					0)
	{
	}

	void Init();
	void Run();
};

} /* namespace App */

#endif /* APPLICATION_INCLUDE_LOGGINGCONTROLLER_H_ */

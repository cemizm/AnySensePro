/*
 * DateTime.h
 *
 *  Created on: 22.03.2016
 *      Author: cem
 */

#ifndef UTILS_INCLUDE_DATETIME_H_
#define UTILS_INCLUDE_DATETIME_H_

#include <stdint.h>

namespace Utils
{

class DateTime
{
private:
	uint8_t m_Year;   // offset from 1970;
	uint8_t m_Month;
	uint8_t m_Day;
	uint8_t m_Hour;
	uint8_t m_Minute;
	uint8_t m_Second;
public:
	DateTime(uint32_t seconds);

	DateTime(uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second) :
			m_Year(year), m_Month(month), m_Day(day), m_Hour(hour), m_Minute(minute), m_Second(second)
	{
	}

	DateTime(const DateTime& dateTime) :
			DateTime(dateTime.GetYear(), dateTime.GetMonth(), dateTime.GetDay(), dateTime.GetHour(), dateTime.GetMinute(),
					dateTime.GetSecond())
	{

	}

	void SetSecond(uint8_t second){ m_Second = second; }
	void SetMinute(uint8_t minute){ m_Minute = minute; }
	void SetHour(uint8_t hour){ m_Hour = hour; }
	void SetDay(uint8_t day){ m_Day = day; }
	void SetMonth(uint8_t month){ m_Month = month; }
	void SetYearUnix(uint8_t year){ m_Year = year; }
	void SetYear(uint16_t year){ m_Year = (year - 1970); }

	uint8_t GetSecond() const { return m_Second; }
	uint8_t GetMinute() const { return m_Minute; }
	uint8_t GetHour() const { return m_Hour; }
	uint8_t GetDay() const { return m_Day; }
	uint8_t GetMonth() const { return m_Month; }
	uint8_t GetYear() const { return m_Year; }

	uint32_t ToUnixTimestamp() const;
	uint32_t ToFatTime() const;
	uint32_t GetTimeTotalSeconds() const;
	uint32_t GetDate() const;
	uint32_t GetTime() const;
};

} /* namespace Utils */

#endif /* UTILS_INCLUDE_DATETIME_H_ */

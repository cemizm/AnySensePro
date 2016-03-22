/*
 * DateTime.cpp
 *
 *  Created on: 22.03.2016
 *      Author: cem
 */

#include <DateTime.h>

namespace Utils
{

#define LEAP_YEAR(Y)     ( ((1970+Y)>0) && !((1970+Y)%4) && ( ((1970+Y)%100) || !((1970+Y)%400) ) )
#define SECS_PER_MIN  (60UL)
#define SECS_PER_HOUR (3600UL)
#define SECS_PER_DAY  (SECS_PER_HOUR * 24UL)
#define DAYS_PER_WEEK (7UL)
#define SECS_PER_WEEK (SECS_PER_DAY * DAYS_PER_WEEK)
#define SECS_PER_YEAR (SECS_PER_WEEK * 52UL)
#define SECS_YR_2000  (946684800UL) // the time at the start of y2k

static const uint8_t monthDays[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

DateTime::DateTime(uint32_t seconds)
{
	uint8_t year;
	uint8_t month, monthLength;
	uint32_t time;
	unsigned long days;

	time = seconds;
	m_Second = time % 60;
	time /= 60; // now it is minutes
	m_Minute = time % 60;
	time /= 60; // now it is hours
	m_Hour = time % 24;
	time /= 24; // now it is days

	year = 0;
	days = 0;

	while ((unsigned) (days += (LEAP_YEAR(year) ? 366 : 365)) <= time)
		year++;

	m_Year = year; // year is offset from 1970

	days -= LEAP_YEAR(year) ? 366 : 365;
	time -= days; // now it is days in this year, starting at 0

	days = 0;
	month = 0;
	monthLength = 0;
	for (month = 0; month < 12; month++)
	{
		if (month == 1)
		{ // february
			if (LEAP_YEAR(year))
				monthLength = 29;
			else
				monthLength = 28;
		}
		else
			monthLength = monthDays[month];

		if (time >= monthLength)
			time -= monthLength;
		else
			break;
	}
	m_Month = month + 1;  // jan is month 1
	m_Day = time + 1;     // day of month
}

uint32_t DateTime::ToUnixTimestamp() const
{
	int i;
	uint32_t seconds;

	// seconds from 1970 till 1 jan 00:00:00 of the given year
	seconds = m_Year * (SECS_PER_DAY * 365);
	for (i = 0; i < m_Year; i++)
	{
		if (LEAP_YEAR(i))
			seconds += SECS_PER_DAY;   // add extra days for leap years
	}

	// add days for this year, months start from 1
	for (i = 1; i < m_Month; i++)
	{
		if ((i == 2) && LEAP_YEAR(m_Year))
			seconds += SECS_PER_DAY * 29;
		else
			seconds += SECS_PER_DAY * monthDays[i - 1];  //monthDay array starts from 0
	}

	seconds += (m_Day - 1) * SECS_PER_DAY;
	seconds += m_Hour * SECS_PER_HOUR;
	seconds += m_Minute * SECS_PER_MIN;
	seconds += m_Second;

	return seconds;
}

uint32_t DateTime::ToFatTime() const
{
	//        	   Year 1980 +	  Month		Day		   Hours	  Minute   Second
	//	uint32_t time = 4 << 25 | 6 << 21 | 13 << 16 | 18 << 11 | 0 << 5 | 0 << 0;

	uint32_t time = (m_Year - 10) << 25;
	time |= m_Month << 21;
	time |= m_Day << 16;
	time |= m_Hour << 11;
	time |= m_Minute << 5;
	time |= (uint8_t) (m_Second / 2.0f) << 0;

	return time;
}
uint32_t DateTime::GetTimeTotalSeconds() const
{
	return m_Hour * 3600 + m_Minute * 60 + m_Second;
}

uint32_t DateTime::GetDate() const
{
	return m_Year << 16 | m_Month << 8 | m_Minute;
}

uint32_t DateTime::GetTime() const
{
	return m_Hour << 16 | m_Minute << 8 | m_Second;
}

} /* namespace Utils */

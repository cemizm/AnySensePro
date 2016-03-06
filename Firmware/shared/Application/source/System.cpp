/*
 * System.cpp
 *
 *  Created on: 03.03.2016
 *      Author: cem
 */

#include <System.h>
#include <string.h>
#include <math.h>

#include <SensorStore.h>

namespace App
{

#define MATH_MAP(A,B,X,C,D)   				((uint8_t) (((float)(X-A)/(B-A)) * (D-C) + C))

void System::log(uint8_t level, const char* msg)
{
	if (!logging.get_free_size())
		return;

	working.msgLen = (uint8_t) strlen(msg);
	if (working.msgLen > LogEntryMsgLen)
		working.msgLen = LogEntryMsgLen;

	working.level = level;

	memcpy(working.msg, msg, working.msgLen);
	logging.push(working);
}

void System::logError(const char* msg)
{
	log(LevelError, msg);
}
void System::logWarning(const char* msg)
{
	log(LevelWarning, msg);
}

void System::Run()
{
	ledActivity.Off();
	ledError.Off();

	while (loaded.wait(delay_ms(200)))
		ledActivity.Toggle();

	ledActivity.Off();
	OSAL::Timer::SleepSeconds(2);

	uint8_t blink = 0;
	for (;;)
	{
		if (logging.pop(working, delay_ms(20)))
		{
			//log

			if (working.level == LevelError)
				ledError.On();
		}

		if (SensorData.GetFCType() == FCType::Unknown)
		{
			ledActivity.On();
		}
		else if (blink % 15 == 0)
		{
			if (blink < 15)
			{
				if (SensorData.GetSatellites() == 0)
					ledActivity.On();
			}
			else if (blink < 30)
			{
				if (SensorData.GetSatellites() < 4)
					ledActivity.On();
			}
			else if (blink < 45)
			{
				if (SensorData.GetSatellites() < 7)
					ledActivity.On();
			}
		}
		else if (blink % 5 == 0)
			ledActivity.Off();

		blink = (blink + 1) % (15 * 5);

		//calculate data
		calcFlightime();
		calcConsumption();
		calcCells();
		calcHome();
		calcGPSData();
	}
}

void System::calcFlightime()
{
	if (!SensorData.GetSensorPresent(Sensors::Arming))
		return;

	uint_fast32_t tick = OSAL::Timer::GetTime();
	if (ft_nextCalc > tick)
		return;

	if (SensorData.GetArmed())
	{
		if (ft_measured == 0)
			ft_measured = tick;

		ft_elapsed += (tick - ft_measured);

		uint32_t ft = SensorData.GetFlightime();
		while (ft_elapsed >= delay_sec(1))
		{
			ft++;
			ft_elapsed -= delay_sec(1);
		}
		SensorData.SetFlightime(ft);
	}
	else
		ft_measured = 0;

	ft_nextCalc = tick + delay_ms(300);
}

void System::calcConsumption()
{
	if (!SensorData.GetSensorPresent(Sensors::Current))
		return;

	if (SensorData.GetSensorPresent(Sensors::Consumption))
		return;

	uint_fast32_t tick = OSAL::Timer::GetTime();
	if (amp_nextCalc > tick)
		return;

	amp_elapsed += (SensorData.GetCurrent() * 100);

	uint16_t capa = SensorData.GetCapacity();
	while (amp_elapsed > amp_elapsed_limit)
	{
		capa++;
		amp_elapsed -= amp_elapsed_limit;
	}
	SensorData.SetCapacity(capa);

	amp_nextCalc = tick + delay_ms(amp_elapsed_interval);
}

void System::calcCells()
{
	if (!SensorData.GetSensorPresent(Sensors::Voltage))
		return;

	uint_fast32_t tick = OSAL::Timer::GetTime();
	if (cells_nextCalc > tick)
		return;

	if (!SensorData.GetSensorPresent(Sensors::Cells))
	{
		if (cells_measure_end == 0)
			cells_measure_end = tick + delay_sec(20);

		if (cells_measure_end > tick)
		{
			uint8_t cells = (uint8_t) ((SensorData.GetBattery() / 3900.0f) + 0.5f);
			SensorData.SetCellCount(cells);
		}

		uint16_t cellV = SensorData.GetBattery() / SensorData.GetCellCount();
		for (uint8_t i = 0; i < SensorData.GetCellCount(); i++)
			SensorData.SetCell(i, cellV);
	}
	if (!SensorData.GetSensorPresent(Sensors::Charge))
	{
		uint16_t cellVoltage = SensorData.GetCellLowest();

		uint8_t charge = 0;
		if (cellVoltage > 4150) // 4,15V 100%
			charge = 100;
		else if (cellVoltage > 4100) // 4,10V 90%
			charge = MATH_MAP(4100, 4150, cellVoltage, 90, 100);
		else if (cellVoltage > 3970) //3,97V 80% - 90%
			charge = MATH_MAP(3970, 4100, cellVoltage, 80, 90);
		else if (cellVoltage > 3920) //3,92V 70% - 80%,
			charge = MATH_MAP(3920, 3970, cellVoltage, 70, 80);
		else if (cellVoltage > 3870) //3,87 60% - 70%
			charge = MATH_MAP(3870, 3920, cellVoltage, 60, 70);
		else if (cellVoltage > 3830) //3,83 50% - 60%
			charge = MATH_MAP(3830, 3870, cellVoltage, 50, 60);
		else if (cellVoltage > 3790) //3,79V 40%-50%
			charge = MATH_MAP(3790, 3830, cellVoltage, 40, 50);
		else if (cellVoltage > 3750) //3,75V 30%-40%
			charge = MATH_MAP(3750, 3790, cellVoltage, 30, 40);
		else if (cellVoltage > 3700) //3,70V 20%-30%
			charge = MATH_MAP(3700, 3750, cellVoltage, 20, 30);
		else if (cellVoltage > 3600) //3,60V 10%-20%
			charge = MATH_MAP(3600, 3700, cellVoltage, 10, 20);
		else if (cellVoltage > 3300) //3,30V 5% - 10%
			charge = MATH_MAP(3300, 3600, cellVoltage, 5, 10);
		else if (cellVoltage > 3000) //3,00V 0% - 5%
			charge = MATH_MAP(3000, 3300, cellVoltage, 0, 5);

		SensorData.SetCharge(charge);
	}

	cells_nextCalc = tick + delay_ms(500);
}

void System::calcHome()
{
	if (SensorData.GetSensorPresent(Sensors::Homeing))
		return;

	uint_fast32_t tick = OSAL::Timer::GetTime();
	if (home_nextCalc > tick)
		return;

	if (SensorData.GetSensorPresent(Sensors::Arming) && !SensorData.GetArmed())
	{
		home_measure_end = SensorData.GetArmed() ? 1 : 0;
		home_gps_measure_end = SensorData.GetArmed() ? 1 : 0;
	}

	if (SensorData.GetSensorPresent(Sensors::Vario))
	{
		if (home_measure_end == 0)
			home_measure_end = tick + delay_sec(10);

		if (home_measure_end > tick)
			SensorData.SetHomeAltitude(SensorData.GetAltitude());
	}

	if (SensorData.GetSensorPresent(Sensors::SGPS))
	{
		if (home_gps_measure_end == 0)
			home_gps_measure_end = tick + delay_sec(10);

		if (home_gps_measure_end > tick)
			SensorData.GetSatellites() > 5 ? SensorData.SetPositionHome() : SensorData.SetPositionHomeClear();
	}

	home_nextCalc = tick + delay_ms(200);
}

void System::calcGPSData()
{
	if (!SensorData.GetSensorPresent(Sensors::SGPS))
		return;

	uint_fast32_t tick = OSAL::Timer::GetTime();
	if (gps_nextCalc > tick)
		return;

	if (SensorData.GetSensorPresent(Sensors::Arming) && !SensorData.GetArmed())
	{
		SensorData.SetHomeDistance(0);
		SensorData.SetHomeDirection(0);
	}
	else if (SensorData.IsPositionHomeSet())
	{
		const GPSPosition& pos = SensorData.GetPositionCurrent();
		const GPSPosition& home = SensorData.GetPositionHome();

		double off_y = home.Longitude - pos.Longitude;
		double off_x = cos(M_PI / 180 * pos.Longitude) * (home.Latitude - pos.Latitude);
		double bearing = atan2(off_y, off_x) / M_PI * 180;

		if (bearing < 0)
			bearing += 360;

		SensorData.SetHomeDirection(bearing);

		double delta = (pos.Longitude - home.Longitude) * (M_PI / 180);
		double sdlong = sin(delta);
		double cdlong = cos(delta);
		double lat1 = pos.Latitude * (M_PI / 180);
		double lat2 = home.Latitude * (M_PI / 180);
		double slat1 = sin(lat1);
		double clat1 = cos(lat1);
		double slat2 = sin(lat2);
		double clat2 = cos(lat2);
		delta = (clat1 * slat2) - (slat1 * clat2 * cdlong);
		delta = pow(delta, 2);
		delta += pow(clat2 * sdlong, 2);
		delta = sqrt(delta);
		double denom = (slat1 * slat2) + (clat1 * clat2 * cdlong);
		delta = atan2(delta, denom);

		delta *= 6372795;

		SensorData.SetHomeDistance(delta);

	}

	gps_nextCalc = tick + delay_ms(300);
}

} /* namespace App */

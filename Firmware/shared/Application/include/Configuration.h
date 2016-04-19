/*
 * Configuration.h
 *
 *  Created on: 15.10.2015
 *      Author: cem
 */

#ifndef APPLICATION_INCLUDE_CONFIGURATION_H_
#define APPLICATION_INCLUDE_CONFIGURATION_H_

#include <stdint.h>

namespace App
{

enum TelemetryProtocol
{
	None = 0, FrSky = 1, MAVLink = 2, HoTT = 3, Jeti = 4, Futaba = 5, Spektrum = 6, Multiplex = 7, Last = 8
};

union ConfigurationData
{
	uint8_t Data[240];
	struct
	{
		union
		{
			uint8_t CommonData[32];
			struct
			{
				uint8_t Version;
				TelemetryProtocol Protocol;
				uint8_t AlarmVoltage;
				uint16_t AlarmDistance;
			}__attribute__((packed));
		};
		uint8_t ProtocolSettings[128];
	};
}__attribute__((packed));

class ConfigurationChanged
{
public:

	virtual void UpdateConfiguration()
	{

	}

	virtual ~ConfigurationChanged()
	{

	}
};

struct SystemData
{
	uint32_t FlightNumber;
}__attribute__((packed));

class Configuration
{
private:
	const char* cfgname = "system.cfg";
	const char* intname = "intern.bin";
	const uint8_t ConfigurationVersion = 1;
	static const uint8_t MaxHandlers = 5;

	ConfigurationData m_data;
	SystemData m_SystemData;

	ConfigurationChanged* m_handlers[MaxHandlers];

	void NotifyHandler();

	uint8_t LoadFile(void* data, uint16_t size, const char* file);
	uint8_t SaveFile(void* data, uint16_t size, const char* file);

	uint8_t LoadSystem();
	uint8_t SaveSystem();

public:
	void Init();

	uint8_t Load();
	uint8_t Save();

	void AddUpdateHandler(ConfigurationChanged& handler);
	void RemoveUpdateHandler(ConfigurationChanged& handler);

	ConfigurationData& GetConfiguration();
	void SetConfiguration(ConfigurationData& data);

	uint8_t GetVersion();
	void SetVersion(uint8_t version);

	uint32_t GetNextFlightNumber();
	void UpdateFlightNumber();

	TelemetryProtocol GetProtocol();
	void SetProtocol(TelemetryProtocol protocol);
};

extern Configuration Config;

} /* namespace Application */

#endif /* APPLICATION_INCLUDE_CONFIGURATION_H_ */

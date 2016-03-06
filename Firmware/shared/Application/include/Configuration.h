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

struct ConfigurationData
{
	uint8_t Version;
	TelemetryProtocol Protocol;
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

class Configuration
{
private:
	const char* cfgname = "system.cfg";
	const uint8_t ConfigurationVersion = 0;
	static const uint8_t MaxHandlers = 5;

	ConfigurationData m_data;

	ConfigurationChanged* m_handlers[MaxHandlers];

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

	TelemetryProtocol GetProtocol();
	void SetProtocol(TelemetryProtocol protocol);
};

extern Configuration Config;

} /* namespace Application */

#endif /* APPLICATION_INCLUDE_CONFIGURATION_H_ */

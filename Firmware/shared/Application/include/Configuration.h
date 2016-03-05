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

const uint8_t ConfigurationVersion = 0;

enum TelemetryProtocol
{
	None = 0, FrSky = 1, MAVLink = 2, HoTT = 3, Jeti = 4, Futaba = 5, Spektrum = 6, Multiplex = 7,
};

struct ConfigurationData
{
	uint8_t Version;
	TelemetryProtocol Protocol;
}__attribute__((packed));

class Configuration
{
private:
	const char* cfgname = "system.cfg";

	ConfigurationData m_data;
public:
	void Init();

	uint8_t Load();
	uint8_t Save();

	ConfigurationData* GetConfiguration();
	void SetConfiguration(ConfigurationData* data);

	uint8_t GetVersion();
	void SetVersion(uint8_t version);

	TelemetryProtocol GetProtocol();
	void SetProtocol(TelemetryProtocol protocol);
};

extern Configuration Config;

} /* namespace Application */

#endif /* APPLICATION_INCLUDE_CONFIGURATION_H_ */

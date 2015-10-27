/*
 * Configuration.cpp
 *
 *  Created on: 15.10.2015
 *      Author: cem
 */

#include <string.h>

#include <Configuration.h>

namespace App
{

Configuration::Configuration(const char* path) :
		m_path(path), m_initialized(0), m_file()
{
	m_data.Protocol = TelemetryProtocol::Jeti;
	m_data.Version = 0x01;
}

void Configuration::checkConfiguration()
{
	if (m_initialized)
		return;

	Init();
}

void Configuration::Init()
{
	FRESULT result = FR_OK;
	uint8_t tmp[sizeof(ConfigurationData)] = { 0x00 };
	uint32_t read;

	result = f_open(&m_file, m_path, FA_READ);

	if (result == FR_NO_FILESYSTEM)
	{
		result = f_mkfs(m_path, 0, 0);

		if (result != FR_OK)
			return;

		result = f_open(&m_file, m_path, FA_READ);
	}

	if (result != FR_OK)
		return;

	result = f_read(&m_file, tmp, (UINT) sizeof(ConfigurationData), (UINT*) &read);
	if (result == FR_OK && read == sizeof(ConfigurationData))
		memcpy(&m_data, tmp, sizeof(ConfigurationData));

	f_close(&m_file);
}

void Configuration::Save()
{
	FRESULT result = FR_OK;
	uint32_t written = 0;

	result = f_open(&m_file, m_path, FA_READ | FA_WRITE | FA_CREATE_ALWAYS);
	if (result != FR_OK)
		return;

	result = f_write(&m_file, (void*) &m_data, sizeof(ConfigurationData), (UINT*) &written);

	f_close(&m_file);
}

ConfigurationData* Configuration::GetConfiguration()
{
	checkConfiguration();
	return &m_data;
}

void Configuration::SetConfiguration(ConfigurationData* data)
{
	checkConfiguration();
	memcpy(&m_data, data, sizeof(ConfigurationData));
	Save();
}

uint8_t Configuration::GetVersion()
{
	checkConfiguration();
	return m_data.Version;
}

void Configuration::SetVersion(uint8_t version)
{
	checkConfiguration();
	m_data.Version = version;
	Save();
}

TelemetryProtocol Configuration::GetProtocol()
{
	checkConfiguration();
	return m_data.Protocol;
}

void Configuration::SetProtocol(TelemetryProtocol protocol)
{
	checkConfiguration();
	m_data.Protocol = protocol;
	Save();
}

} /* namespace Application */

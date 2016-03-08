/*
 * Configuration.cpp
 *
 *  Created on: 15.10.2015
 *      Author: cem
 */

#include <Configuration.h>
#include <StorageFlashSPI.h>

namespace App
{

Configuration Config;

void Configuration::Init()
{
	if (Load() != 1)
		Save();

	if (m_data.Version < ConfigurationVersion)
	{
		for (uint8_t i = m_data.Version + 1; i <= ConfigurationVersion; i++)
		{

		}

		Save();
	}
}

uint8_t Configuration::Load()
{
	spiffs_file fd = Storage::StorageFlashSPI::Open(cfgname, SPIFFS_RDONLY, 0);

	if (fd < 0)
		return 0;

	Storage::StorageFlashSPI::FileAutoRelease far(fd);

	spiffs_stat st;

	if (Storage::StorageFlashSPI::FileStat(fd, &st) < 0)
		return 0;

	if (Storage::StorageFlashSPI::Read(fd, &m_data, sizeof(ConfigurationData)) < 0)
		return 0;

	return 1;
}

uint8_t Configuration::Save()
{
	spiffs_file fd = Storage::StorageFlashSPI::Open(cfgname, SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR, 0);
	if (fd < 0)
		return 0;

	m_data.Version = ConfigurationVersion;

	if (Storage::StorageFlashSPI::Write(fd, &m_data, sizeof(ConfigurationData)) < 0)
		return 0;

	if (Storage::StorageFlashSPI::Close(fd) < 0)
		return 0;

	return 1;
}

void Configuration::AddUpdateHandler(ConfigurationChanged& handler)
{
	for (uint8_t i = 0; i < MaxHandlers; i++)
	{
		if (m_handlers[i] == nullptr)
		{
			m_handlers[i] = &handler;
			return;
		}
	}
}

void Configuration::RemoveUpdateHandler(ConfigurationChanged& handler)
{
	for (uint8_t i = 0; i < MaxHandlers; i++)
	{
		if (m_handlers[i] == &handler)
		{
			m_handlers[i] = nullptr;
			return;
		}
	}
}

void Configuration::NotifyHandler()
{
	for (uint8_t i = 0; i < MaxHandlers; i++)
	{
		if (m_handlers[i] != nullptr)
			m_handlers[i]->UpdateConfiguration();
	}
}

ConfigurationData& Configuration::GetConfiguration()
{
	return m_data;
}

void Configuration::SetConfiguration(ConfigurationData& data)
{
	memcpy(&m_data, &data, sizeof(ConfigurationData));
	Save();
	NotifyHandler();
}

uint8_t Configuration::GetVersion()
{
	return m_data.Version;
}

void Configuration::SetVersion(uint8_t version)
{
	if (m_data.Version == version)
		return;

	m_data.Version = version;
	Save();
}

TelemetryProtocol Configuration::GetProtocol()
{
	return m_data.Protocol;
}

void Configuration::SetProtocol(TelemetryProtocol protocol)
{
	if (m_data.Protocol == protocol)
		return;

	m_data.Protocol = protocol;
	Save();
}

} /* namespace Application */

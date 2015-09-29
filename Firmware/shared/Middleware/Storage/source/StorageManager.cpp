/*
 * StorageManager.cpp
 *
 *  Created on: 28.09.2015
 *      Author: cem
 */

#include <stdint.h>

#include <StorageManager.h>
#include "diskio.h"

namespace Storage
{

void StorageManager::RegisterStorage(uint8_t storageId, StorageInterface* storage)
{
	m_storages[storageId] = storage;
}

StorageInterface* StorageManager::GetStorage(uint8_t storageId)
{
	return m_storages[storageId];
}

StorageManager Instance;

extern "C" DSTATUS disk_status(BYTE pdrv)
{
	return Instance.GetStorage(pdrv)->GetStatus();
}

extern "C" DSTATUS disk_initialize(BYTE pdrv)
{
	return Instance.GetStorage(pdrv)->Init();
}

extern "C" DRESULT disk_read(BYTE pdrv, BYTE* buff, DWORD sector, UINT count)
{
	return (DRESULT) Instance.GetStorage(pdrv)->Read(buff, sector, count);
}

extern "C" DRESULT disk_write(BYTE pdrv, const BYTE* buff, DWORD sector, UINT count)
{
	return (DRESULT) Instance.GetStorage(pdrv)->Write(buff, sector, count);
}
extern "C" DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void* buff)
{
	return (DRESULT) Instance.GetStorage(pdrv)->IOCtl((StorageCommand) cmd, buff);
}

} /* namespace Storage */

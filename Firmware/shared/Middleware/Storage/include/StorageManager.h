/*
 * StorageManager.h
 *
 *  Created on: 28.09.2015
 *      Author: cem
 */

#ifndef MIDDLEWARE_STORAGE_INCLUDE_STORAGEMANAGER_H_
#define MIDDLEWARE_STORAGE_INCLUDE_STORAGEMANAGER_H_

#include <stdint.h>

#include "StorageInterface.h"
#include "ff.h"

namespace Storage
{

class StorageManager
{
private:
	StorageInterface* m_storages[_VOLUMES];
public:
	void RegisterStorage(uint8_t storageId, StorageInterface* storage);
	StorageInterface* GetStorage(uint8_t storageId);
};

extern StorageManager Instance;

} /* namespace Storage */

#endif /* MIDDLEWARE_STORAGE_INCLUDE_STORAGEMANAGER_H_ */

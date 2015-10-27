/*
 * StorageInterface.h
 *
 *  Created on: 28.09.2015
 *      Author: cem
 */

#ifndef MIDDLEWARE_STORAGE_INCLUDE_STORAGEINTERFACE_H_
#define MIDDLEWARE_STORAGE_INCLUDE_STORAGEINTERFACE_H_

namespace StorageStatus
{
static const uint8_t NoInit = 0x01;
static const uint8_t NoDisk = 0x02;
static const uint8_t WriteProtect = 0x04;
}

namespace Storage
{

enum StorageResult
{
	OK = 0, /* 0: Successful */
	Error = 1, /* 1: R/W Error */
	WriteProtected = 2, /* 2: Write Protected */
	NotReady = 3, /* 3: Not Ready */
	InvalidParameter = 4, /* 4: Invalid Parameter */
};

enum StorageCommand
{
	/* Generic command (Used by FatFs) */
	Sync = 0, /* Complete pending write process (needed at _FS_READONLY == 0) */
	GetSectorCount = 1, /* Get media size (needed at _USE_MKFS == 1) */
	GetSectorSize = 2, /* Get sector size (needed at _MAX_SS != _MIN_SS) */
	GetBlockSize = 3, /* Get erase block size (needed at _USE_MKFS == 1) */
	GetTrim = 4, /* Inform device that the data on the block of sectors is no longer used (needed at _USE_TRIM == 1) */

	/* Generic command (Not used by FatFs) */
	ControlPower = 5, /* Get/Set power status */
	ControlLock = 6, /* Lock/Unlock media removal */
	ControlEject = 7, /* Eject media */
	ControlFormat = 8, /* Create physical format on the media */

	/* MMC/SDC specific ioctl command */
	MMCGetType = 10, /* Get card type */
	MMCGetCSD = 11, /* Get CSD */
	MMCGetCID = 12, /* Get CID */
	MMCGetOCR = 13, /* Get OCR */
	MMCGetSDStatus = 14, /* Get SD status */

	/* ATA/CF specific ioctl command */
	ATAGetRevision = 20, /* Get F/W revision */
	ATAGetModel = 21, /* Get model name */
	ATAGetSerial = 22, /* Get serial number */
};

class StorageInterface
{
private:
public:
	virtual uint8_t GetStatus()
	{
		return StorageStatus::NoInit;
	}

	virtual uint8_t Init()
	{
		return StorageStatus::NoDisk;
	}

	virtual StorageResult Read(uint8_t* buff, uint32_t sector, uint16_t count)
	{
		(void) buff;
		(void) sector;
		(void) count;
		return StorageResult::Error;
	}

	virtual StorageResult Write(const uint8_t* buff, uint32_t sector, uint16_t count)
	{
		(void) buff;
		(void) sector;
		(void) count;
		return StorageResult::Error;
	}

	virtual StorageResult IOCtl(StorageCommand cmd, void* buff)
	{
		(void) cmd;
		(void) buff;
		return StorageResult::Error;
	}

	virtual ~StorageInterface()
	{

	}
};

}

#endif /* MIDDLEWARE_STORAGE_INCLUDE_STORAGEINTERFACE_H_ */

/*
 * HCI.h
 *
 *  Created on: 10.09.2015
 *      Author: cem
 */

#ifndef MIDDLEWARE_BLUENRG_INCLUDE_HCI_H_
#define MIDDLEWARE_BLUENRG_INCLUDE_HCI_H_

#include <stdint.h>
#include <string.h>

#include "BLEInterface.h"
#include <Pool.h>

#define MAX_HCI_PACKET_SIZE 		UINT8_MAX
#define MAX_HCI_REQUESTS			5

namespace BlueNRG
{

enum OpCodeOGF
{
	LinkControl = 0x01,
	LinkPolicy = 0x02,
	ControllerBaseBand = 0x03,
	InformationParams = 0x04,
	StatusParams = 0x05,
	Testing = 0x06,
	LEController = 0x08,
	VendorSpecific = 0x3F,
};

enum HCIPacketType
{
	Command = 0x01, ACLData = 0x02, SCOData = 0x03, Event = 0x04, Vendor = 0xFF,
};

enum HCIEventCode
{
	DiconnectComplete = 0x05,
	EncryptionChange = 0x08,
	ReadRemoteVersionInformationComplete = 0x0C,
	CommandComplete = 0x0E,
	CommandStatus = 0x0F,
	HardwareError = 0x10,
	NumberOfCompletedPackets = 0x13,
	EncryptionKeyRefreshComplete = 0x30,
	LEMeta = 0x3E
};

enum HCIGenericStatusCode
{
	Success = 0x00, Error = 0x47, Timeout = 0xFF,
};

struct __attribute__((packed, aligned(1))) OpCodeType
{
	uint16_t OCF :10;
	OpCodeOGF OGF :6;

	bool operator==(const OpCodeType& rhs) const
	{
		return rhs.OCF == OCF && rhs.OGF == OGF;
	}
};

struct __attribute__((packed, aligned(1))) HCIEventStatus
{
	uint8_t NCmd;
	OpCodeType OpCode;
	uint8_t StatusCode;
};

struct __attribute__((packed, aligned(1))) HCICommandComplete
{
	uint8_t NCmd;
	OpCodeType OpCode;
	uint8_t StatusCode;
	uint8_t Parameters[254];
};

struct __attribute__((packed, aligned(1))) HCICommand
{
	OpCodeType OpCode;
	uint8_t Length;
	uint8_t Parameters[254];
};

struct __attribute__((packed, aligned(1))) HCIEvent
{
	HCIEventCode EventCode;
	uint8_t Length;
	union
	{
		uint8_t data[254];
		HCIEventStatus Status;
		HCICommandComplete Complete;
	} Data;
};

struct __attribute__((packed, aligned(1))) HCIPacket
{
	uint8_t Type;
	union
	{
		uint8_t data[255];
		HCICommand Command;
		HCIEvent Event;
	} Data;
};

struct HCIRequest
{
	HCIPacket Packet = HCIPacket();
	HCIEvent Event = HCIEvent();
	uint8_t EventSize = 0;
	HAL::OSALEventFlag EventFlag = HAL::OSALEventFlag();

	void Clear()
	{
		memset(&Event, 0, sizeof(HCIEvent));
		EventSize = 0;
		EventFlag.clear();
	}
};

typedef UTILS::AutoRelease<HCIRequest, MAX_HCI_REQUESTS> HCIAutoRelease;

class HCInterface: BLEDataHandler
{
private:
	BLEInterface& m_bleInterface;
	UTILS::Pool<HCIRequest, MAX_HCI_REQUESTS> m_pendingRequests;
	uint8_t VerifyPacket(HCIPacket* packet, uint16_t size);
public:
	~HCInterface()
	{
	}
	HCInterface(BLEInterface& bleInterface) :
			m_bleInterface(bleInterface), m_pendingRequests()
	{
		m_bleInterface.RegisterDataHandler(this);
	}

	int16_t SendCommand(OpCodeOGF ogf, uint16_t ocf, uint8_t data[], uint16_t size, HCIEvent* event);

	void OnDataReceived(uint8_t data[], uint16_t size);
};

}

#endif /* MIDDLEWARE_BLUENRG_INCLUDE_HCI_H_ */

/*
 * HCI.h
 *
 *  Created on: 10.09.2015
 *      Author: cem
 */

#ifndef MIDDLEWARE_BLUENRG_INCLUDE_HCI_H_
#define MIDDLEWARE_BLUENRG_INCLUDE_HCI_H_

#include <stdint.h>

#include "BLEInterface.h"

#define MAX_HCI_PACKET_SIZE 		UINT8_MAX

namespace BlueNRG
{

enum OpCodeLinkControl
{
	Diconnect = 0x06, ReadRemoteVersionInfo = 0x1D,
};

enum OpCodeControllerBaseBand
{
	CCSetEventMask = 0x01, HCIReset = 0x03, HCIReadTransmitPowerLevel = 0x03,
};

enum OpCodeInformationParams
{
	LocalVersionInformation = 0x01, LocalSupportedCommands = 0x02, LocalSupportedFeatures = 0x03, ReadBDADDR = 0x09,
};

enum OpCodeStatusParams
{
	ReadRSSI = 0x05,
};

enum OpCodeLEController
{
	LESetEventMask = 0x01, ReadBufferSize = 0x02, ReadLocalSupportedFeature = 0x03,

};

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

struct __attribute__((packed, aligned(1))) HCICommand
{
	OpCodeOGF OGF :6;
	uint16_t OCF :10;
	uint8_t Length;
	uint8_t Parameters[128];
};

struct __attribute__((packed, aligned(1))) HCIEvent
{
	uint8_t EventCode;
	uint8_t Length;
	uint8_t Parameters[128];
};

struct __attribute__((packed, aligned(1))) HCIPacket
{
	uint8_t Type;
	union
	{
		uint8_t data[255];
		HCICommand Command;
		HCIEvent Event;
	} data;
};

class BLEInterface;

class HCInterface
{
private:
	BLEInterface& m_bleInterface;
	HCIPacket packet;
public:
	HCInterface(BLEInterface& bleInterface) :
			m_bleInterface(bleInterface), packet()
	{
	}

	void SendCommand(OpCodeOGF ogf, uint16_t ocf, uint8_t data[], uint16_t size);

	void OnDataReceived(uint8_t data[], uint16_t size);

};

}

#endif /* MIDDLEWARE_BLUENRG_INCLUDE_HCI_H_ */

/*
 * GAP.cpp
 *
 *  Created on: 17.09.2015
 *      Author: cem
 */

#include "GAP.h"

enum GAPCommand
{
	Init = 0x8A,
};

struct __attribute__((packed, aligned(1))) GAPInitCommand
{
	BlueNRG::GAPRole role;
};

struct __attribute__((packed, aligned(1))) GAPInitCommandComplete
{
	uint16_t HGapService;
	uint16_t HDeviceNameCharacter;
	uint16_t HAppearanceCharacter;
};

namespace BlueNRG
{

HCIGenericStatusCode GAPInterface::Init(GAPRole role, uint16_t* hGapService, uint16_t* hDeviceNameChar, uint16_t* hAppearanceChar)
{
	GAPInitCommand params = { role };
	HCIEvent event;

	if (m_HCInterface.SendCommand(OpCodeOGF::VendorSpecific, GAPCommand::Init, (uint8_t*) &params, sizeof(GAPInitCommand), &event)
			< 0)
		return HCIGenericStatusCode::Timeout;

	if (event.Data.Complete.StatusCode == HCIGenericStatusCode::Success)
	{
		GAPInitCommandComplete* cmd = (GAPInitCommandComplete*) event.Data.Complete.Parameters;

		*hGapService = cmd->HGapService;
		*hDeviceNameChar = cmd->HDeviceNameCharacter;
		*hAppearanceChar = cmd->HAppearanceCharacter;
	}

	return (HCIGenericStatusCode) event.Data.Status.StatusCode;
}

} /* namespace BlueNRG */

/*
 * DJINaza.h
 *
 *  Created on: 18.09.2015
 *      Author: cem
 */

#ifndef LIBS_INCLUDE_DJINAZA_H_
#define LIBS_INCLUDE_DJINAZA_H_

#include "DJIInterface.h"

namespace LIBS
{

class DJINaza: DJIInterface
{
public:
	~DJINaza()
	{
	}

	void Process(uint16_t CANId, uint8_t length, uint8_t bytes) override;
};

} /* namespace LIBS */

#endif /* LIBS_INCLUDE_DJINAZA_H_ */

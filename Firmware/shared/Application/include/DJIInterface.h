/*
 * DJIInterface.h
 *
 *  Created on: 18.09.2015
 *      Author: cem
 */

#ifndef LIBS_INCLUDE_DJIINTERFACE_H_
#define LIBS_INCLUDE_DJIINTERFACE_H_

#include <stdint.h>

namespace LIBS
{

class DJIInterface
{
public:
	virtual ~DJIInterface();
	virtual void Process(uint16_t CANId, uint8_t length, uint8_t bytes);

};

} /* namespace LIBS */

#endif /* LIBS_INCLUDE_DJIINTERFACE_H_ */

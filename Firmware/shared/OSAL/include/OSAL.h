/*
 * OSAL.h
 *
 *  Created on: 16.10.2015
 *      Author: cem
 */

#ifndef OSAL_INCLUDE_OSAL_H_
#define OSAL_INCLUDE_OSAL_H_

#include "Timer.h"

namespace OSAL
{

using Priority = OS::TPriority;
using ISRSupport = OS::TISRW;
using EventFlag = OS::TEventFlag;
using Mutex = OS::TMutex;

template<typename T, uint16_t Size, typename S = uint8_t>
using Channel = OS::channel<T,Size,S>;

}

#endif /* OSAL_INCLUDE_OSAL_H_ */

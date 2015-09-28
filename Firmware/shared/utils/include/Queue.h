/*
 * Queue.h
 *
 *  Created on: 12.09.2015
 *      Author: cem
 */

#ifndef UTILS_INCLUDE_QUEUE_H_
#define UTILS_INCLUDE_QUEUE_H_

#include <stdint.h>
#include <stddef.h>

namespace UTILS
{

template<typename T, uint8_t queueSize>
class Queue
{
private:
	T queueItems[queueSize];
	uint16_t head;
	uint16_t tail;
	uint16_t count;

public:
	Queue() :
			queueItems(), head(0), tail(0), count(0)
	{
	}

	T* Enqueue()
	{
		if (count == queueSize)
			return NULL;

		T* item = &queueItems[head];

		head = (head + 1) % queueSize;

		count++;

		return item;
	}

	T* Dequeue()
	{
		if (count == 0)
			return NULL;

		T* ret = &queueItems[tail];

		tail = (tail + 1) % queueSize;

		count--;

		return ret;

	}

	T* Peek()
	{
		if (count == 0)
			return NULL;

		return &queueItems[tail];
	}

	uint8_t IsEmpty()
	{
		return count == 0 ? 1 : 0;
	}
};

} /* namespace UTILS */

#endif /* UTILS_INCLUDE_QUEUE_H_ */

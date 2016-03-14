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

namespace Utils
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

	uint8_t push(const T& item)
	{
		if (count == queueSize)
			return 0;

		queueItems[head] = item;
		head = (head + 1) % queueSize;
		count++;

		return 1;
	}

	uint8_t pop(T& item)
	{
		if (count == 0)
			return 0;

		item = queueItems[tail];
		tail = (tail + 1) % queueSize;
		count--;

		return 1;
	}

	uint16_t getSize()
	{
		return count;
	}

	uint8_t getItem(uint8_t index, T& item)
	{
		if (count == 0)
			return 0;

		uint16_t tmpTail = (tail + index) % queueSize;
		item = queueItems[tmpTail];

		return 1;
	}

	uint8_t IsEmpty()
	{
		return count == 0 ? 1 : 0;
	}
};

} /* namespace UTILS */

#endif /* UTILS_INCLUDE_QUEUE_H_ */

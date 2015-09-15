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
#include <stdlib.h>

namespace UTILS
{

template<typename T, uint8_t queueSize>
class Queue
{
private:
	T queueItems[queueSize];
	int16_t front;
	int16_t rear;

public:
	Queue() :
			front(0), rear(0)
	{
	}

	T* Enqueue()
	{
		// Don't allow the queue to grow more
		// than MAX_SIZE - 1
		if (Size() == queueSize - 1)
			return NULL;

		T item = queueItems[rear];

		rear = ++rear % queueSize;

		return &item;
	}

	T* Dequeue()
	{
		if (IsEmpty())
			return NULL;

		T* ret = &queueItems[front];

		// MOD is used so that front indicator
		// can wrap around
		front = ++front % queueSize;

		return ret;

	}
	T* Peek()
	{
		if (IsEmpty())
			return NULL;

		return &queueItems[front];
	}

	uint8_t Size()
	{
		return abs(rear - front);
	}

	uint8_t IsEmpty()
	{
		return (front == rear);
	}
};

} /* namespace UTILS */

#endif /* UTILS_INCLUDE_QUEUE_H_ */

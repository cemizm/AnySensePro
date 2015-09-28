/*
 * Pool.h
 *
 *  Created on: 16.09.2015
 *      Author: cem
 */

#ifndef UTILS_INCLUDE_POOL_H_
#define UTILS_INCLUDE_POOL_H_

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <algorithm>

namespace UTILS
{

template<typename T>
struct PoolItem
{
	uint8_t inuse = 0;
	T item = T();
};

template<typename T, uint8_t size>
class Pool
{
private:

	PoolItem<T> m_pool[size];
public:
	T* alloc()
	{
		for (uint8_t i = 0; i < size; i++)
		{
			if (!m_pool[i].inuse)
			{
				m_pool[i].inuse = 1;
				return &m_pool[i].item;
			}
		}

		return NULL;
	}

	void free(T* item)
	{
		for (uint8_t i = 0; i < size; i++)
		{
			if (&m_pool[i].item == item)
			{
				m_pool[i].inuse = 0;
				break;
			}
		}
	}

	uint8_t getItems(T* found[], uint8_t maxSize)
	{
		uint8_t count = 0;
		for (uint8_t i = 0; i < std::min(size, maxSize); i++)
		{
			if (m_pool[i].inuse)
			{
				found[count] = &m_pool[i].item;
				count++;
			}
		}
		return count;
	}

};

#pragma GCC diagnostic ignored "-Weffc++"
template<typename T, uint8_t size>
class AutoRelease
{
private:
	Pool<T, size>& m_pool;
	T* m_item;
public:
	AutoRelease(Pool<T, size>& pool, T* item) :
			m_pool(pool), m_item(item)
	{

	}
	~AutoRelease()
	{
		m_pool.free(m_item);
	}
};

#pragma GCC diagnostic pop

}

#endif /* UTILS_INCLUDE_POOL_H_ */

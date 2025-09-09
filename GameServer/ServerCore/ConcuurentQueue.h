#pragma once

#include <concurrent_queue.h>
#include <concurrent_priority_queue.h>

template<typename T>
class ConcuurentQueue
{
public:
	void Push(T item)
	{
		_items.push(item);
	}

	bool Pop(T& out)
	{
		return _items.try_pop(out);
	}

	void PopAll(Vector<T>& items)
	{
		T item;
		while (Pop(item))
			items.push_back(item);
	}

private:
	Concurrency::concurrent_queue<T>	_items;
};
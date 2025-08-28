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

	T Pop()
	{
		if (_items.empty())
			return T();

		T ret;
		_items.try_pop(ret);
		return ret;
	}

	void PopAll(vector<T>& items)
	{
		while (T item = Pop())
			items.push_back(item);
	}

private:
	Concurrency::concurrent_queue<T>	_items;
};
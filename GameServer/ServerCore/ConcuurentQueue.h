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
		lock_guard<mutex> _lockGuard(_lock);
		return PopNoLock();
	}

	T PopNoLock()
	{
		if (_items.empty())
			return T();

		T ret;
		_items.try_pop(ret);

		/*T ret = _items.front();
		_items.pop();*/
		return ret;
	}

	void PopAll(vector<T>& items)
	{
		while (T item = PopNoLock())
			items.push_back(item);
	}

	void Clear()
	{
		lock_guard<mutex> _lockGuard(_lock);
		_items.clear();

	}

private:
	mutex								_lock;
	Concurrency::concurrent_queue<T>	_items;
};
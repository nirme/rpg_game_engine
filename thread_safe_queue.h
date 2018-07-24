#pragma once

#ifndef _THREAD_SAFE_QUEUE
#define _THREAD_SAFE_QUEUE

#include "utility.h"



template<class _Ty> class threadSafeQueue
{
protected:
	queue<_Ty> c;
	mutable boost::mutex mutex;
	boost::condition_variable conditionVar;

public:

	void pushBack(const _Ty& _Val)
	{
		boost::mutex::scoped_lock lock(mutex);
		c.push(_Val);
		mutex.unlock();
		conditionVar.notify_one();
	};

	_Ty popFront()
	{
		boost::mutex::scoped_lock lock(mutex);

		while (empty())
		{
			conditionVar.wait(lock);
		}

		_Ty _Val = c.front();
		c.pop();
		return _Val;
	};

	bool empty()
	{
		boost::mutex::scoped_lock lock(mutex);
		return c.empty();
	};

	UINT size()
	{
		boost::mutex::scoped_lock lock(mutex);
		return c.size();
	};

	void clear()
	{
		boost::mutex::scoped_lock lock(mutex);

		while (!c.empty())
		{
			delete c.front();
			c.pop();
		}
	};

};


#endif //_THREAD_SAFE_QUEUE
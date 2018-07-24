#pragma once

#ifndef _CORE_TIMER_V2
#define _CORE_TIMER_V2

#include "utility.h"
#include "Core_Logger.h"



typedef double TIMETYPE;


class Timer
{
public:

	struct BaseClockData
	{
		DWORD threadId;
		LARGE_INTEGER lastUpdateProcessorCount;
		TIMETYPE timeDelta;
		TIMETYPE timeTotal;
		UINT timestamp;

		list<Timer*> timers;
		list<Timer*> enabledTimers;

		BaseClockData(DWORD _threadId) : threadId(_threadId), timeDelta(0.0f), timeTotal(0.0f), timestamp(0)
		{
			lastUpdateProcessorCount.QuadPart = 0;
		};
	};

protected:

	static bool initialized;

	static LARGE_INTEGER processorFrequency;
	static TIMETYPE processorSingleTickDuration;

	static map<DWORD,BaseClockData*> clocks;

	BaseClockData* baseClock;


	TIMETYPE actualTime;

	TIMETYPE startTime;
	TIMETYPE targetTime;

	bool enabled;
	list<Timer*>::iterator etIterator;

	Timer() : baseClock(NULL), actualTime(0.0f), startTime(0.0f), targetTime(0.0f), enabled(false)
	{
		etIterator = baseClock->enabledTimers.end();
	};

	int setUpTimer(DWORD _threadId);

	inline void updateTimer(TIMETYPE& _timeDelta)
	{
		actualTime += _timeDelta;
	};



public:

	static int initialize();
	static int shutdown();
	static int update();
	static int getTimer(Timer* _timer);
	int release();
	int setTimer(DWORD _durationTimeMs);
	int setTimer(TIMETYPE _durationTimeS);
	static TIMETYPE getTimeTotal();
	static TIMETYPE getTimeDelta();
	static DWORD getTimeStamp();

	inline TIMETYPE getTime()
	{
		return actualTime;
	};

	inline TIMETYPE getTimerTimePassed()
	{
		return actualTime - startTime;
	};

	inline TIMETYPE getTimerDuration()
	{
		return actualTime - startTime;
	};

	inline float getTimerProgressLinear()
	{
		return (actualTime - startTime) / targetTime;
	};

	inline float getTimerProgress(BYTE _exponent)
	{
		return pow((actualTime - startTime) / targetTime, int(_exponent));
	};

	inline bool getTimerCompleted()
	{
		return (actualTime - startTime) >= targetTime ? true : false;
	};

	inline void reset()
	{
		actualTime = 0.0f;
		startTime = 0.0f;
		targetTime = 0.0f;
	}

	inline void setEnabled(bool _enable)
	{
		if (enabled != _enable)
		{
			if (enabled)
				baseClock->enabledTimers.erase(etIterator);
			else
				etIterator = baseClock->enabledTimers.insert(this);

			enabled = _enable;
		}
	};

};



#endif //_CORE_TIMER_V2
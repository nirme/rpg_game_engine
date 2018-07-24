#include "Core_Timer_v2.h"



bool								Timer::initialized = false;
LARGE_INTEGER						Timer::processorFrequency;
TIMETYPE							Timer::processorSingleTickDuration = 0.0f;
map<DWORD,Timer::BaseClockData*>	Timer::clocks;



int Timer::setUpTimer(DWORD _threadId)
{
	actualTime = 0.0f;
	startTime = 0.0f;
	targetTime = 0.0f;

	map<DWORD,BaseClockData*>::iterator it = clocks.find(_threadId);
	if (it == clocks.end())
		return 1;

	baseClock = (*it).second;
	baseClock->timers.push_back(this);

	return 0;
};


int Timer::initialize()
{
	DWORD threadId = GetCurrentThreadId();
	map<DWORD,BaseClockData*>::iterator it = clocks.find(threadId);
	if (it != clocks.end())
		return 1;

	BaseClockData* c = new BaseClockData(threadId);
	clocks.insert(map<DWORD,BaseClockData*>::value_type(threadId, c));

	if (!initialized)
	{
		if (!QueryPerformanceFrequency(&processorFrequency))
		{
			DWORD err = GetLastError();
			wchar_t* text = NULL;
			FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, 0, text, 1, NULL);
			showError(false, L"QueryPerformanceFrequency error: %d error description: %s\n", err, text);
			LocalFree(text);
			return 1;
		}
		processorSingleTickDuration = TIMETYPE(1.0f) / TIMETYPE(processorFrequency.QuadPart);
		initialized = true;
	}


	if (!QueryPerformanceCounter(&c->lastUpdateProcessorCount))
	{
		DWORD err = GetLastError();
		wchar_t* text = NULL;
		FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, 0, text, 1, NULL);
		showError(false, L"QueryPerformanceCounter error: %d error description: %s\n", err, text);
		LocalFree(text);
		return 2;
	}

	return 0;
};


int Timer::shutdown()
{
	DWORD threadId = GetCurrentThreadId();
	map<DWORD,BaseClockData*>::iterator it = clocks.find(threadId);
	if (it == clocks.end())
		return 1;

	for (list<Timer*>::iterator itt=(*it).second->timers.begin();itt!=(*it).second->timers.end();itt++)
		(*itt)->release();

	delete (*it).second;
	clocks.erase(it);

	return 0;
};


int Timer::update()
{
	if (!initialized)
		return 1;

	DWORD threadId = GetCurrentThreadId();
	map<DWORD,BaseClockData*>::iterator it = clocks.find(threadId);
	if (it == clocks.end())
		return 2;

	BaseClockData* c = (*it).second;

	LARGE_INTEGER now;
	if (!QueryPerformanceCounter(&now))
	{
		DWORD err = GetLastError();
		wchar_t* text = NULL;
		FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, 0, text, 1, NULL);
		showError(false, L"QueryPerformanceCounter error: %d error description: %s\n", err, text);
		LocalFree(text);
		return 3;
	}

	c->timeDelta = (now.QuadPart - c->lastUpdateProcessorCount.QuadPart) * processorSingleTickDuration;
	c->lastUpdateProcessorCount = now;
	c->timeTotal += c->timeDelta;
	c->timestamp++;

	list<Timer*>::iterator itt_end = c->enabledTimers.end();
	for (list<Timer*>::iterator itt = c->enabledTimers.begin(); itt != itt_end; itt++)
		(*itt)->updateTimer(c->timeDelta);

	return 0;
};


int Timer::getTimer(Timer* _timer)
{
	if (!initialized)
		return 1;

	DWORD threadId = GetCurrentThreadId();
	Timer* timer = new Timer;
	if (timer->setUpTimer(threadId))
		return 2;

	_timer = timer;
	return 0;
};


int Timer::release()
{
	for (list<Timer*>::iterator it=baseClock->timers.begin();it!=baseClock->timers.end();it++)
	{
		if ((*it) == this)
		{
			baseClock->timers.erase(it);
			delete this;
			return 0;
		}
	}

	delete this;
	return 0;
};


int Timer::setTimer(DWORD _durationTimeMs)
{
	startTime = actualTime;
	targetTime = (TIMETYPE) (_durationTimeMs) * 0.001f;
	return 0;
};


int Timer::setTimer(TIMETYPE _durationTimeS)
{
	startTime = actualTime;
	targetTime = _durationTimeS;
	return 0;
};


TIMETYPE Timer::getTimeTotal()
{
	DWORD threadId = GetCurrentThreadId();
	map<DWORD,BaseClockData*>::iterator it = clocks.find(threadId);
	if (it == clocks.end())
		return TIMETYPE(0);

	return (*it).second->timeTotal;
};


TIMETYPE Timer::getTimeDelta()
{
	DWORD threadId = GetCurrentThreadId();
	map<DWORD,BaseClockData*>::iterator it = clocks.find(threadId);
	if (it == clocks.end())
		return TIMETYPE(0);

	return (*it).second->timeDelta;
};


DWORD Timer::getTimeStamp()
{
	DWORD threadId = GetCurrentThreadId();
	map<DWORD,BaseClockData*>::iterator it = clocks.find(threadId);
	if (it == clocks.end())
		return TIMETYPE(0);

	return (*it).second->timeDelta;
};


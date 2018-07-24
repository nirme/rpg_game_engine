#pragma once

#ifndef _PROFILER
#define _PROFILER

#include "utility.h"
#include "Logger.h"
#include <assert.h>

#define MAX_PROFILER_SAMPLES		20
#define IDS_PROFILE_HEADER1	"  Min :   Avg :   Max :   # : Profile Name"
#define IDS_PROFILE_HEADER2	"--------------------------------------------"
#define IDS_PROFILE_SAMPLE	"%3.1f : %3.1f : %3.1f : %3d : %s"


struct SingleSample;
class ProfileSample;
class ProfilerOutputHandler;
class ProfileLogHandler;

struct SingleSample
{
	bool isValid;
	bool isUsed;
	UINT callCount;
	std::string name;

	float startTime;
	float totalTime;
	float childTime;

	int parentCount;

	float averagePc;
	float minPc;
	float maxPc;
	unsigned long dataCount;

	SingleSample()
	{
		isValid=false; 
		dataCount=0;
		averagePc=minPc=maxPc=-1;
	}
};

class ProfileSample
{
public:
	ProfileSample(std::string sampleName);
	~ProfileSample();

	static void output();

	static void resetSample(std::string sampleName);
	static void resetAll();

	static ProfilerOutputHandler *outputHandler;

protected:
	int sampleIndex;
	int parentIndex;

	inline float getTime(){ return (float)timeGetTime(); }

	static SingleSample samples[MAX_PROFILER_SAMPLES];
	static int lastOpenedSample;
	static int openSampleCount;
	static float rootBegin, rootEnd;
};

class ProfilerOutputHandler
{
public:
	virtual void beginOutput()=0;
	virtual void sample(float fMin, float fAvg, float fMax, int callCount, std::string name, int parentCount)=0;
	virtual void endOutput()=0;
};


class ProfileLogHandler : public ProfilerOutputHandler  
{
public:
	void beginOutput();
	void endOutput();
	void sample(float fMin, float fAvg, float fMax, int callCount, std::string name, int parentCount);
};

#endif //_PROFILER
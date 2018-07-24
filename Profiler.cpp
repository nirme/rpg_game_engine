#include "Profiler.h"


int ProfileSample::lastOpenedSample=-1;
int ProfileSample::openSampleCount=0;
SingleSample ProfileSample::samples[MAX_PROFILER_SAMPLES];
ProfilerOutputHandler *ProfileSample::outputHandler=0;
float ProfileSample::rootBegin=0.0f;
float ProfileSample::rootEnd=0.0f;


void ProfileSample::resetSample(std::string sampleName)
{
	unsigned short i=0;
	for(i=0;i<MAX_PROFILER_SAMPLES;++i)
	{
		if(!samples[i].isValid)
			break;
		if(samples[i].name==sampleName)
		{
			samples[i].averagePc = -1;
			samples[i].maxPc = -1;
			samples[i].minPc = -1;
			samples[i].callCount = 0;
			samples[i].dataCount = 0;
			samples[i].totalTime = 0;
		}
	}
}

void ProfileSample::resetAll()
{
	unsigned short i=0;
	for(i=0;i<MAX_PROFILER_SAMPLES;++i)
	{
		if(!samples[i].isValid)
			break;
		samples[i].averagePc = -1;
		samples[i].maxPc = -1;
		samples[i].minPc = -1;
		samples[i].callCount = 0;
		samples[i].dataCount = 0;
		samples[i].totalTime = 0;
	}
}


ProfileSample::ProfileSample(std::string sampleName)
{
	unsigned short i=0;
	for(i=0;i<MAX_PROFILER_SAMPLES;++i)
	{
		if(!samples[i].isValid)
			break;
		if(samples[i].name==sampleName)
		{
			assert(!samples[i].isUsed && "Tried to profile a sample which was already being profiled");
			sampleIndex=i;
			parentIndex=lastOpenedSample;
			lastOpenedSample=i;
			samples[i].parentCount=openSampleCount;
			++openSampleCount;
			samples[i].isUsed=true;
			++samples[i].callCount;
			samples[i].startTime=getTime();
			if(parentIndex<0)rootBegin=samples[i].startTime;
			return;
		}
	}
	int storeIndex=i;
	assert(i<MAX_PROFILER_SAMPLES && "Profiler has run out of sample slots!");
	samples[i].isValid=true;
	samples[i].name=sampleName;
	sampleIndex=i;
	if(i==0)
		parentIndex=-1;
	else
		parentIndex=lastOpenedSample;
	lastOpenedSample=i;
	samples[i].parentCount=openSampleCount;
	openSampleCount++;
	samples[i].isUsed=true;
	samples[i].callCount=1;

	samples[i].totalTime=0.0f;
	samples[i].childTime=0.0f;
	samples[i].startTime=getTime();
	if(i==0)
		rootBegin=samples[i].startTime;
}

ProfileSample::~ProfileSample()
{
	float endTime=getTime();
	samples[sampleIndex].isUsed=false;
	float timeTaken = endTime - samples[sampleIndex].startTime;
	if(parentIndex>=0)
		samples[parentIndex].childTime+=timeTaken;
	else
		rootEnd=endTime;
	samples[sampleIndex].totalTime+=timeTaken;
	lastOpenedSample=parentIndex;
	--openSampleCount;
}

void ProfileSample::output()
{
	assert(outputHandler && "Profiler has no output handler set");
	outputHandler->beginOutput();
	int i=0;
	for(int i=0;i<MAX_PROFILER_SAMPLES; ++i)
	{
		if(samples[i].isValid)
		{
			float sampleTime = samples[i].totalTime-samples[i].childTime;
			float percentage = ( sampleTime / ( rootEnd - rootBegin ) ) * 100.0f;

			float totalPc = samples[i].averagePc*samples[i].dataCount;
			totalPc += percentage;
			samples[i].dataCount++;
			samples[i].averagePc=totalPc/samples[i].dataCount;
			if((samples[i].minPc==-1)||(percentage<samples[i].minPc))
				samples[i].minPc=percentage;
			if((samples[i].maxPc==-1)||(percentage>samples[i].maxPc))
				samples[i].maxPc=percentage;

			outputHandler->sample(samples[i].minPc,
			samples[i].averagePc,
			samples[i].maxPc,
			samples[i].callCount,
			samples[i].name,
			samples[i].parentCount);

			samples[i].callCount=0;
			samples[i].totalTime=0;
			samples[i].childTime=0;
		}
	}

	outputHandler->endOutput();
}


void ProfileLogHandler::beginOutput()
{
	Logger::write(LOG_APP,IDS_PROFILE_HEADER1);
	Logger::write(LOG_APP,IDS_PROFILE_HEADER2);
}

void ProfileLogHandler::endOutput()
{
	Logger::write(LOG_APP,"\n");
}

void ProfileLogHandler::sample(float fMin, float fAvg, float fMax, int callCount, std::string name, int parentCount)
{
	char namebuf[256], indentedName[256];
	char avg[16], min[16], max[16], num[16];

	sprintf(avg, "%3.1f", fAvg);
	sprintf(min, "%3.1f", fMin);
	sprintf(max, "%3.1f", fMax);
	sprintf(num, "%3d",   callCount);

	strcpy( indentedName, name.c_str());
	for( int indent=0; indent<parentCount; ++indent )
	{
		sprintf(namebuf, " %s", indentedName);
		strcpy( indentedName, namebuf);
	}

	Logger::write(LOG_APP,IDS_PROFILE_SAMPLE,min,avg,max,num,indentedName);
}

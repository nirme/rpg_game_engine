#pragma once

#ifndef _LOGGER
#define _LOGGER

#include "utility.h"
#include "Core_Defines.h"
#include "Core_Variable_Library.h"


#define LOG_NUM		3

enum LOG_TYPE
{
	LOG_APP =		0x00,
	LOG_SERVER =	0x01,
	LOG_CLIENT =	0x02,
};
//


class Logger
{
protected:

	static ofstream logsList[LOG_NUM];
	static map<int, pair<int,string>> errorList;

	Logger(){};

	static HRESULT loadStrings(string _path);

	inline static void printSystemTimestamp(LOG_TYPE _log)
	{
		char timestamp[20];
		SYSTEMTIME st;
		GetLocalTime(&st);
		sprintf_s(timestamp, 20, "%d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		logsList[_log] << timestamp;
	};

public:

	static HRESULT initialize();
	static HRESULT shutdown();

	static void write(LOG_TYPE _log, string _msg, ...);
	static void write(LOG_TYPE _log, wstring _msg, ...);

	static void write(string _msg, ...);
	static void write(wstring _msg, ...);

	static void write(int _msgnum, ...);


	static void log(bool _writeDate, LOG_TYPE _log, string _msg);
	static void log(bool _writeDate, LOG_TYPE _log, wstring _msg);


};



#endif //_LOGGER

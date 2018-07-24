#include "Core_Logger.h"



ofstream Logger::logsList[LOG_NUM];
map<int, pair<int,string>> Logger::errorList;



HRESULT Logger::loadStrings(string _path)
{
	string filename = string(PATH_DATA) + _path;
	fstream file(filename.c_str(), fstream::in);
	if (file.fail())
		return E_FAIL;

	char line[1024];
	int errnum = 0;
	int lognum = 0;
	
	while (file.getline(line, 1024))
	{
		string lineT =  trim(line);
		if (lineT.empty() || (lineT.c_str())[0] == ';')
			continue;
		sscanf_s(lineT.c_str(), "%d", &errnum);
		if (!file.getline(line, 1024))
			return S_OK;
		lineT =  trim(line);
		sscanf_s(lineT.c_str(), "%d", &lognum);
		lognum = (lognum >= LOG_NUM) ? 0 : lognum;
		if (!file.getline(line, 1024))
			return S_OK;

		lineT =  trim(line);
		errorList.insert(map<int, pair<int,string>>::value_type(errnum, pair<int,string>::pair(lognum, lineT)));
	}

	file.close();
	return S_OK;
};


HRESULT Logger::initialize()
{
	VariableLibrary::setUsedDictionary("filenames");
	
	string fname;

	{
		fname = string(PATH_LOG) + string(LOG_FILE_APPLICATION);
		logsList[LOG_APP].open(fname.c_str(), fstream::out | fstream::app | fstream::ate);
		if (logsList[LOG_APP].fail())
			return E_FAIL;
	}

	{
		fname = string(PATH_LOG) + string(LOG_FILE_SERVER);
		logsList[LOG_SERVER].open(fname.c_str(), fstream::out | fstream::app | fstream::ate);
		if (logsList[LOG_SERVER].fail())
			return E_FAIL;
	}

	{
		fname = string(PATH_LOG) + string(LOG_FILE_CLIENT);
		logsList[LOG_CLIENT].open(fname.c_str(), fstream::out | fstream::app | fstream::ate);
		if (logsList[LOG_CLIENT].fail())
			return E_FAIL;
	}


	if (VariableLibrary::valueExist("LOGGER_ERROR_LIST"))
	{
		fname = string(PATH_DATA) + VariableLibrary::getValueStr("LOGGER_ERROR_LIST");
		loadStrings(fname);
	}

	VariableLibrary::setUsedDictionary();
	return S_OK;
};


HRESULT Logger::shutdown()
{
	errorList.clear();

	for (UINT i=0;i<LOG_NUM;i++)
	{
		logsList[i].flush();
		logsList[i].close();
	};

	return S_OK;
};


void Logger::write(LOG_TYPE _log, string _msg, ...)
{
	char err[1280];
	va_list valist;
	va_start(valist, _msg);
	vsprintf_s(err, 1280, _msg.c_str(), valist);
	va_end(valist);
	printSystemTimestamp(_log);
	logsList[_log] << "\t-\t" << err << endl;
	logsList[_log].flush();
};


void Logger::write(LOG_TYPE _log, wstring _msg, ...)
{
	wchar_t err[1280];
	va_list valist;
	va_start(valist, _msg);
	vswprintf_s(err, 1280, (wchar_t*)(_msg.c_str()), valist);
	va_end(valist);

	char cerr[1280];
	UINT s=0;
	wcstombs_s(&s, cerr, 1280, err, _TRUNCATE);

	printSystemTimestamp(_log);
	logsList[_log] << "\t-\t" << cerr << endl;
	logsList[_log].flush();
};


void Logger::write(string _msg, ...)
{
	va_list valist;
	va_start(valist, _msg);
	write(LOG_APP, _msg, valist);
	va_end(valist);
};


void Logger::write(wstring _msg, ...)
{
	va_list valist;
	va_start(valist, _msg);
	write(LOG_APP, _msg, valist);
	va_end(valist);
};


void Logger::write(int _msgnum, ...)
{
	map<int, pair<int,string>>::iterator it = errorList.find(_msgnum);
	map<int, pair<int,string>>::iterator it_end = errorList.end();

	if (it != it_end)
	{
		va_list valist;
		va_start(valist, _msgnum);
		write((*it).second.first, (*it).second.second, valist);
		va_end(valist);
	}
	else
	{
		write(LOG_APP, "Unknown message number: %d", _msgnum);
	}
};


void Logger::log(bool _writeDate, LOG_TYPE _log, string _msg)
{
	if (_writeDate)
	{
		printSystemTimestamp(_log);
		logsList[_log] << "\t-\t";
	}

	logsList[_log] << _msg << endl;
	logsList[_log].flush();
};


void Logger::log(bool _writeDate, LOG_TYPE _log, wstring _msg)
{
	if (_writeDate)
	{
		printSystemTimestamp(_log);
		logsList[_log] << "\t-\t";
	}

	char cerr[1280];
	UINT s=0;
	wcstombs_s(&s, cerr, 1280, _msg.c_str(), _TRUNCATE);

	logsList[_log] << cerr << endl;
	logsList[_log].flush();
};



// This functions are declared in utility.h
int showError(bool _isCritical, wchar_t* _text, ...)
{
	wchar_t err[2048];
	va_list valist;
	va_start(valist, _text);
	vswprintf(err, _text, valist);
	va_end(valist);
	Logger::write(LOG_APP, wstring(err));
	if (_isCritical)
	{
		Logger::write(LOG_APP, "Last registered error was critical. Closing aplication.");
		// UNCOMMENT IN CASE OF WINDOWED APP
		//MessageBox(0, err, L"Error!", MB_OK | MB_ICONEXCLAMATION);
		PostQuitMessage(0);
	}
	return 0;
};


int showError(bool _isCritical, char* _text, ...)
{
	char err[2048];
	va_list valist;
	va_start(valist, _text);
	vsprintf(err, _text, valist);
	va_end(valist);
	Logger::write(LOG_APP, string(err));
	if (_isCritical)
	{
		Logger::write(LOG_APP, "Last registered error was critical. Closing aplication.");
		// UNCOMMENT IN CASE OF WINDOWED APP
		//wchar_t tmp[2048];
		//mbstowcs(tmp, err, 2048);
		//MessageBox(0, tmp, L"Error!", MB_OK | MB_ICONEXCLAMATION);
		PostQuitMessage(0);
	}
	return 0;
};

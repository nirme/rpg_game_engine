#include "Core_Sound.h"




Resource<FMOD::Sound*>::ResourceMap Resource<FMOD::Sound*>::loadedResMap;


Sound::~Sound()
{
	SAFE_RELEASE(resource);
};


#ifndef RESOURCE_MAP_WITH_STRINGS
	Sound* Sound::getResource(UINT _id)
#else //RESOURCE_MAP_WITH_STRINGS
	Sound* Sound::getResource(string _id)
#endif //RESOURCE_MAP_WITH_STRINGS
{
	return reinterpret_cast <Sound*> (getBaseResource(_id));
};


#ifdef(MULTI_THREADED_LOADING)
	#ifndef RESOURCE_MAP_WITH_STRINGS
		Sound* Sound::getResourceMultiThreaded(UINT _id)
	#else //RESOURCE_MAP_WITH_STRINGS
		Sound* Sound::getResourceMultiThreaded(string _id)
	#endif //RESOURCE_MAP_WITH_STRINGS
	{
		return reinterpret_cast <Sound*> (getBaseResourceMultiThreaded(_id));
	};
#endif //MULTI_THREADED_LOADING


int Sound::setResource(FMOD::Sound* _fmodSound, vector<BYTE> &_fileToStream, wstring &_wstrText)
{
	#ifdef(MULTI_THREADED_LOADING)
		boost::mutex::scoped_lock lock(mutex);
	#endif //MULTI_THREADED_LOADING

	if (loaded)
		return 1;

	resource = _fmodSound;
	if (_fileToStream.size())
	{
		streamed = true;
		swap(v_fileInMemory, _fileToStream);
	}
	else
	{
		streamed = false;
	}

	swap(v_text, _wstrText);

	FMOD_MODE mode;
	_fmodSound->getMode(&mode);
	is3D = mode & FMOD_3D ? true : false;

	#ifdef(MULTI_THREADED_LOADING)
		conditionVar.notify_all();
	#endif //MULTI_THREADED_LOADING

	return 0;
};


int Sound::setResource(FMOD::Sound* _fmodSound, bool _streamedFromFile, wstring &_wstrText)
{
	#ifdef(MULTI_THREADED_LOADING)
		boost::mutex::scoped_lock lock(mutex);
	#endif //MULTI_THREADED_LOADING

	if (loaded)
		return 1;

	resource = _fmodSound;
	streamed = _streamedFromFile;
	swap(v_text, _wstrText);

	FMOD_MODE mode;
	_fmodSound->getMode(&mode);
	is3D = mode & FMOD_3D ? true : false;

	#ifdef(MULTI_THREADED_LOADING)
		conditionVar.notify_all();
	#endif //MULTI_THREADED_LOADING

	return 0;
};


void Sound::setText(wstring _text)
{
	v_text = _text;
};

wstring Sound::getText()
{
	return v_text;
};


int Sound::getSoundTimeMS(float* _length)
{
	UINT length = 0;

	FMOD_RESULT fresult = FMOD_OK;
	if (FMOD_FAILED(fresult = resource->getLength(&length, FMOD_TIMEUNIT_MS)))
	{
		showError(false, "FMOD::Sound::getLength error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
		return -1;
	}

	*_length = float(length);
	return 0;
};


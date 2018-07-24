#pragma once

#ifndef _CORE_SOUND
#define _CORE_SOUND


#include "utility.h"
#include "Core_Logger.h"
#include "Core_Defines.h"
#include "Template_Basic_Resource.h"




class Sound : public Resource <FMOD::Sound*>
{
protected:
	bool streamed;
	vector<BYTE> v_fileInMemory;
	wstring v_text;
	bool is3D;


public:

	#ifndef RESOURCE_MAP_WITH_STRINGS
		Sound(UINT _id) : Resource(_id, NULL), streamed(false), is3D(false)	{};
	#else //RESOURCE_MAP_WITH_STRINGS
		Sound(string _id) : Resource(_id, NULL), streamed(false), is3D(false)	{};
	#endif //RESOURCE_MAP_WITH_STRINGS
	~Sound();

	#ifndef RESOURCE_MAP_WITH_STRINGS
		static Sound* getResource(UINT _id);
	#else //RESOURCE_MAP_WITH_STRINGS
		static Sound* getResource(string _id);
	#endif //RESOURCE_MAP_WITH_STRINGS


	#ifdef(MULTI_THREADED_LOADING)
		#ifndef RESOURCE_MAP_WITH_STRINGS
			static Sound* getResourceMultiThreaded(UINT _id);
		#else //RESOURCE_MAP_WITH_STRINGS
			static Sound* getResourceMultiThreaded(string _id);
		#endif //RESOURCE_MAP_WITH_STRINGS
	#endif //MULTI_THREADED_LOADING


	int onLostDevice(){ return 0; };
	int onResetDevice(){ return 0; };


	int setResource(FMOD::Sound* _fmodSound, vector<BYTE> &_fileToStream, wstring &_wstrText);
	int setResource(FMOD::Sound* _fmodSound, bool _streamedFromFile, wstring &_wstrText);


	void setText(wstring _text);
	wstring getText();
	int getSoundTimeMS(float* _length);

	inline const wchar_t* getTextPointer()
	{
		return v_text.c_str();
	};

	inline int getTextLength()
	{
		return v_text.size();
	};

	inline bool isStreamed()
	{
		return streamed;
	};

	inline bool getIs3D()
	{
		return is3D;
	};


};



#endif //_CORE_SOUND
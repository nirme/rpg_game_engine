#pragma once

#ifndef _CORE_GDI_FONT
#define _CORE_GDI_FONT


#include "utility.h"
#include "Core_Logger.h"
#include "Core_Variable_Library.h"
#include "Core_Defines.h"
#include "Template_Basic_Resource.h"

#include "Core_Graphic_Resource.h"



struct GDIFontDescription
{
	HANDLE fontHandle;

	wstring fontName;

	BYTE fontCharSet;
	BYTE fontOutPrecision;
	BYTE fontQuality;
	BYTE fontPitchAndFamily;

	GDIFontDescription() : HANDLE(NULL), fontCharSet(0), fontOutPrecision(0), fontQuality(0), fontPitchAndFamily(0) {};
};


int setupGDIFontDesc(void* _file, UINT _fileLength, GDIFontDescription** _gdiFontDesc);


class GDIFont : public Resource <GDIFontDescription*>
{
public:


	#ifndef RESOURCE_MAP_WITH_STRINGS
		GDIFont(UINT _id) : Resource(_id, NULL){};
	#else //RESOURCE_MAP_WITH_STRINGS
		GDIFont(string _id) : Resource(_id, NULL){};
	#endif //RESOURCE_MAP_WITH_STRINGS

	~GDIFont();

	int setResource(GDIFontDescription* _gdiFontDesc);

	#ifndef RESOURCE_MAP_WITH_STRINGS
		static GDIFont* getResource(UINT _id);
	#else //RESOURCE_MAP_WITH_STRINGS
		static GDIFont* getResource(string _id);
	#endif //RESOURCE_MAP_WITH_STRINGS


	#ifdef(MULTI_THREADED_LOADING)
		#ifndef RESOURCE_MAP_WITH_STRINGS
			static GDIFont* getResourceMultiThreaded(UINT _id);
		#else //RESOURCE_MAP_WITH_STRINGS
			static GDIFont* getResourceMultiThreaded(string _id);
		#endif //RESOURCE_MAP_WITH_STRINGS
	#endif //MULTI_THREADED_LOADING


	int onLostDevice(){ return 0; };
	int onResetDevice(){ return 0; };


	inline bool isLoaded()
	{
		if (!loaded)
			loaded = (resource != NULL) ? true : false;
		return loaded;
	};


	wstring getFontName();


	inline BYTE getDefaultCharSet()
	{
		return resource->fontCharSet;
	};


	inline BYTE getDefaultOutPrecision()
	{
		return resource->fontOutPrecision;
	};


	inline BYTE getDefaultQuality()
	{
		return resource->fontQuality;
	};


	inline BYTE getDefaultPitchAndFamily()
	{
		return resource->fontPitchAndFamily;
	};

};



#endif //_CORE_GDI_FONT

#include "Core_GDI_Font.h"




int setupGDIFontDesc(void* _file, UINT _fileLength, GDIFontDescription** _gdiFontDesc)
{
	UINT fontFaces = 0;
	HANDLE fontHandle = AddFontMemResourceEx(_file, _fileLength, NULL, &fontFaces);

	if (!fontHandle)
		return -1;

	LOGFONTW fontDesc;
	ZeroMemory(&fontDesc, sizeof(LOGFONTW));
	if (!(GetObject(fontHandle, sizeof(LOGFONTW), &fontDesc)))
		return -2;

	GDIFontDescription* gdiFontDesc = new GDIFontDescription;

	gdiFontDesc->fontHandle = fontHandle;
	gdiFontDesc->fontName = fontDesc.lfFaceName;
	gdiFontDesc->fontCharSet = fontDesc.lfCharSet;
	gdiFontDesc->fontOutPrecision = fontDesc.lfOutPrecision;
	gdiFontDesc->fontQuality = fontDesc.lfQuality;
	gdiFontDesc->fontPitchAndFamily = fontDesc.lfPitchAndFamily;

	*_gdiFontDesc = gdiFontDesc;

	return 0;
};



Resource<GDIFontDescription*>::ResourceMap Resource<GDIFontDescription*>::loadedResMap;


GDIFont::~GDIFont()
{
	if (resource)
	{
		RemoveFontMemResourceEx(resource->fontHandle);
		SAFE_DELETE(resource);
	}
};


int GDIFont::setResource(GDIFontDescription* _gdiFontDesc)
{
	#ifdef(MULTI_THREADED_LOADING)
		boost::mutex::scoped_lock lock(mutex);
	#endif //MULTI_THREADED_LOADING

	if (loaded)
		return 1;

	resource = _gdiFontDesc;
	loaded = (resource != NULL) ? true : false;

	#ifdef(MULTI_THREADED_LOADING)
		conditionVar.notify_all();
	#endif //MULTI_THREADED_LOADING

	return 0;
};




#ifndef RESOURCE_MAP_WITH_STRINGS
	GDIFont* GDIFont::getResource(UINT _id)
#else //RESOURCE_MAP_WITH_STRINGS
	GDIFont* GDIFont::getResource(string _id)
#endif //RESOURCE_MAP_WITH_STRINGS
{
	return reinterpret_cast <GDIFont*> (getBaseResource(_id));
};



#ifdef(MULTI_THREADED_LOADING)
	#ifndef RESOURCE_MAP_WITH_STRINGS
		GDIFont* GDIFont::getResourceMultiThreaded(UINT _id);
	#else //RESOURCE_MAP_WITH_STRINGS
		GDIFont* GDIFont::getResourceMultiThreaded(string _id);
	#endif //RESOURCE_MAP_WITH_STRINGS
	{
		return reinterpret_cast <GDIFont*> (getBaseResourceMultiThreaded(_id));
	};
#endif //MULTI_THREADED_LOADING


wstring GDIFont::getFontName()
{
	return resource->fontName;
};

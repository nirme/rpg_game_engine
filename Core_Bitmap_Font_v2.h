#pragma once

#ifndef _CORE_BITMAP_FONT_V2
#define _CORE_BITMAP_FONT_V2


#include "utility.h"
#include "Core_Logger.h"
#include "Core_Variable_Library.h"
#include "Core_Defines.h"
#include "Template_Basic_Resource.h"

#include "Core_Graphic_Resource.h"



struct BitmapCharacter;
struct BitmapFontDescription;
class BitmapFont;

// pointer to const data (can't change data)
// BitmapCharacter const * char = func('a');

// const pointer to data (can't change pointer)
// BitmapCharacter * const char = 2;


struct BitmapCharacter
{
	USHORT charId;
	RECT texPos;

	// next char, kerning size
	map<USHORT, short> kerningMap;


	BitmapCharacter() : charId(0), texX(0), texY(0), texWidth(0), texHeight(0) {};
	~BitmapCharacter() {};

	int setup(	USHORT _charId, RECT _texPos, USHORT* _kerningNextCharId, short* _kerningLength, USHORT _kerningArraySize);

	int setup(	USHORT _charId, RECT _texPos, KerningPair* _kerningPairs, USHORT _kerningPairsNumber);

	BitmapCharacter& operator= (BitmapCharacter const & bc);
	short getKerning(USHORT _nextChar);
};



struct BitmapFontDescription
{
	IDirect3DTexture9* bitmap;
	typedef map<USHORT, BitmapCharacter*> BitmapAlphabet;

	USHORT fontSize;
	USHORT lineHeight;
	USHORT baseHeight;
	USHORT bitmapWidth;
	USHORT bitmapHeight;
	USHORT monoTypeWidth;

	// char id, character
	BitmapAlphabet fontDescription;
	vector<BitmapCharacter> charArray;


	BitmapFontDescription() : bitmap(NULL), fontSize(0), lineHeight(0), baseHeight(0), bitmapWidth(0), bitmapHeight(0), monoTypeWidth(0) {};

	int setupFontDesc(IDirect3DTexture9* _bitmap, USHORT _fontSize, USHORT _lineHeight, USHORT _baseHeight, USHORT _bitmapWidth, USHORT _bitmapHeight, USHORT _monoTypeWidth);
	int setupCharacters(BitmapCharacter* _charactersArray, UINT _arrayLength);
	int setupAlphabetLength(UINT _length);
	int setupAlphabetAddChar(BitmapCharacter* _character, UINT _charIndex);
	int setupAutoMonotypeWidth();
};



class BitmapFont : public Resource <BitmapFontDescription*>
{
private:
	~BitmapFont();

public:


	#ifndef RESOURCE_MAP_WITH_STRINGS
		BitmapFont(UINT _id) : Resource(_id, NULL){};
	#else //RESOURCE_MAP_WITH_STRINGS
		BitmapFont(string _id) : Resource(_id, NULL){};
	#endif //RESOURCE_MAP_WITH_STRINGS


	int setResource(BitmapFontDescription* _fontDesc);

	#ifndef RESOURCE_MAP_WITH_STRINGS
		static BitmapFont* getResource(UINT _id);
	#else //RESOURCE_MAP_WITH_STRINGS
		static BitmapFont* getResource(string _id);
	#endif //RESOURCE_MAP_WITH_STRINGS


	#ifdef(MULTI_THREADED_LOADING)
		#ifndef RESOURCE_MAP_WITH_STRINGS
			static BitmapFont* getResourceMultiThreaded(UINT _id);
		#else //RESOURCE_MAP_WITH_STRINGS
			static BitmapFont* getResourceMultiThreaded(string _id);
		#endif //RESOURCE_MAP_WITH_STRINGS
	#endif //MULTI_THREADED_LOADING


	int onLostDevice(){ return 0; };
	int onResetDevice(){ return 0; };


	inline bool isLoaded()
	{
		if (!loaded)
			loaded = (resource != NULL && fontBitmap->isLoaded()) ? true : false;
		return loaded;
	};


	IDirect3DTexture9* getBitmap();
	const BitmapCharacter* getChar(USHORT _charId);

	USHORT getFontSize();
	USHORT getLineHeight();
	USHORT getBaseHeight();
	USHORT getBitmapWidth();
	USHORT getBitmapHeight();
	USHORT getMonoTypeWidth();
};



#endif //_CORE_BITMAP_FONT_V2

#include "Core_Bitmap_Font_v2.h"



Resource <BitmapFontDescription*>::ResourceMap Resource<BitmapFontDescription*>::loadedResMap;



/////////////////////////////////////////////////////
////////           BitmapCharacter           ////////
/////////////////////////////////////////////////////

int BitmapCharacter::setup(USHORT _charId, RECT _texPos, USHORT* _kerningNextCharId, short* _kerningLength, USHORT _kerningArraySize)
{
	charId = _charId;
	texPos = _texPos;

	for (USHORT i = 0; i < _kerningArraySize; ++i)
		kerningMap.insert(map<USHORT, short>::value_type(_kerningNextCharId, _kerningLength));

	return 0;
};

int BitmapCharacter::setup(USHORT _charId, RECT _texPos, KerningPair* _kerningPairs, USHORT _kerningPairsNumber)
{
	charId = _charId;
	texPos = _texPos;

	for (USHORT i = 0; i < _kerningPairsNumber; ++i)
		kerningMap.insert(map<USHORT, short>::value_type(_kerningPairs.charId, _kerningPairs.kerningLength));

	return 0;
};

BitmapCharacter& BitmapCharacter::operator= (BitmapCharacter const & bc)
{
	charId = bc.charId;
	texPos = bc.texPos;
	kerningMap = bc.kerningMap;

	return *this;
};

short BitmapCharacter::getKerning(USHORT _nextChar)
{
	map<USHORT, short>::iterator it = kerningMap.find(_nextChar);
	if (it != kerningMap.end())
		return it->second;
	return 0;
};



/////////////////////////////////////////////////////
////////        BitmapFontDescription        ////////
/////////////////////////////////////////////////////

int BitmapFontDescription::setupFontDesc(IDirect3DTexture9* _bitmap, USHORT _fontSize, USHORT _lineHeight, USHORT _baseHeight, USHORT _bitmapWidth, USHORT _bitmapHeight, USHORT _monoTypeWidth)
{
	bitmap = _bitmap;
	fontSize = _fontSize;
	lineHeight = _lineHeight;
	baseHeight = _baseHeight;
	bitmapWidth = _bitmapWidth;
	bitmapHeight = _bitmapHeight;
	monoTypeWidth = _monoTypeWidth;
	return 0;
};

int BitmapFontDescription::setupCharacters(BitmapCharacter* _charactersArray, UINT _arrayLength)
{
	charArray.resize(_arrayLength);
	memcpy(&(charArray[0]), _charactersArray, _arrayLength * sizeof(BitmapCharacter));

	for (UINT i = 0; i < _arrayLength; ++i)
	{
		fontDescription.insert(BitmapAlphabet::value_type(charArray[i].charId, &(charArray[i])));
	};

	return 0;
};

int BitmapFontDescription::setupAlphabetLength(UINT _length)
{
	charArray.resize(_length);
	return 0;
};

int BitmapFontDescription::setupAlphabetAddChar(BitmapCharacter* _character, UINT _charIndex)
{
	if (_charIndex >= charArray.size())
		return 1;

	charArray[_charIndex] = *_character;
	fontDescription.insert(BitmapAlphabet::value_type(charArray[_charIndex].charId, &(charArray[_charIndex])));

	return 0;
};

int BitmapFontDescription::setupAutoMonotypeWidth()
{
	for (UINT i = 0; charArray.size(); ++i)
		if (monoTypeWidth < charArray[i].texWidth)
			monoTypeWidth = charArray[i].texWidth;
	return 0;
};



/////////////////////////////////////////////////////
////////             BitmapFont              ////////
/////////////////////////////////////////////////////

BitmapFont::~BitmapFont()
{
	SAFE_RELEASE(resource);
};


int BitmapFont::setResource(BitmapFontDescription* _fontDesc)
{
	#ifdef(MULTI_THREADED_LOADING)
		boost::mutex::scoped_lock lock(mutex);
	#endif //MULTI_THREADED_LOADING

	if (loaded)
		return 1;

	resource = _fontDesc;
	loaded = (resource != NULL) ? true : false;

	#ifdef(MULTI_THREADED_LOADING)
		conditionVar.notify_all();
	#endif //MULTI_THREADED_LOADING

	return 0;
};


#ifndef RESOURCE_MAP_WITH_STRINGS
	BitmapFont* BitmapFont::getResource(UINT _id)
#else //RESOURCE_MAP_WITH_STRINGS
	BitmapFont* BitmapFont::getResource(string _id)
#endif //RESOURCE_MAP_WITH_STRINGS
{
	return reinterpret_cast<BitmapFont*>(this->getBaseResource(_id));
};


#ifdef(MULTI_THREADED_LOADING)
	#ifndef RESOURCE_MAP_WITH_STRINGS
		BitmapFont* BitmapFont::getResourceMultiThreaded(UINT _id)
	#else //RESOURCE_MAP_WITH_STRINGS
		BitmapFont* BitmapFont::getResourceMultiThreaded(string _id)
	#endif //RESOURCE_MAP_WITH_STRINGS
	{
		return reinterpret_cast<BitmapFont*>(getBaseResourceMultiThreaded(_id));
	};
#endif //MULTI_THREADED_LOADING



IDirect3DTexture9* BitmapFont::getBitmap()
{
	return resource->bitmap;
};


const BitmapCharacter* BitmapFont::getChar(USHORT _charId)
{
	BitmapFontDescription::BitmapAlphabet::iterator it = resource->fontDescription.find(_charId);
	if (it != resource->fontDescription.end())
		return it->second;
	return NULL;
};


USHORT BitmapFont::getFontSize()
{
	resource->fontSize;
};


USHORT BitmapFont::getLineHeight()
{
	resource->lineHeight;
};


USHORT BitmapFont::getBaseHeight()
{
	resource->baseHeight;
};


USHORT BitmapFont::getBitmapWidth()
{
	resource->bitmapWidth;
};


USHORT BitmapFont::getBitmapHeight()
{
	resource->bitmapHeight;
};


USHORT BitmapFont::getMonoTypeWidth()
{
	resource->monoTypeWidth;
};


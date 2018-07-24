#pragma once

#ifndef _CORE_GUI_ELEMENT
#define _CORE_GUI_ELEMENT

#include "utility.h"
#include "Core_Logger.h"
#include "Core_Variable_Library.h"
#include "Core_Defines.h"

#include "Core_Graphic_Resource.h"

#include "Core_Bitmap_Font_v2.h"
#include "Core_GDI_Font.h"




//////////////////////////////////////////////////
////////            GUIElement            ////////
//////////////////////////////////////////////////

class GUIElement
{
protected:
	RECT* screenPosition;

public:
	GUIElement();
	GUIElement(RECT* _screenPosition);
	virtual ~GUIElement(){};

	void setScreenPosition(RECT* _screenPosition);

	virtual int draw(ID3DXSprite* _sprite) = 0;
};




//////////////////////////////////////////////////
////////        GUIElementTexture         ////////
//////////////////////////////////////////////////

class GUIElementTexture : public GUIElement
{
private:
	Texture* texture;
	RECT texCoord;
	D3DCOLOR color;

	IDirect3DTexture9* cashedTexture;
	D3DXMATRIX transformMatrix;
	D3DXVECTOR3 position;


	GUIElementTexture();

public:

	GUIElementTexture(RECT* _screenPosition, Texture* _texture, RECT _texCoord, D3DCOLOR _color = 0xFFFFFFFF);
	~GUIElementTexture();

	int setColor(D3DCOLOR _color = 0xFFFFFFFF);

	int draw(ID3DXSprite* _sprite);
};



//////////////////////////////////////////////////
////////          GUIElementText          ////////
//////////////////////////////////////////////////

class GUIElementText : public GUIElement
{
private:
	GDIFont* gdiFont;
	ID3DXFont* pFont;

	DWORD textFormat;
	D3DCOLOR textColor;

	wstring text;
	int textLength;


	GUIElementText();

public:

	GUIElementText(RECT* _screenPosition, GDIFont* _gdiFont, ID3DXFont* _pFont, DWORD _formatPosition = 0, bool _formatSingleLine = false, D3DCOLOR _textColor = 0xFF000000);
	GUIElementText(RECT* _screenPosition, GDIFont* _gdiFont, ID3DXFont* _pFont, DWORD _format = 0, D3DCOLOR _textColor = 0xFF000000);
	~GUIElementText();

	int setColor(D3DCOLOR _color = 0xFFFFFFFF);

	int setText(wstring const & _text, int _textLength = -1);

	int setText(string const & _text, int _textLength = -1);

	int setTextLength(int _textLength);

	int draw(ID3DXSprite* _sprite);
};




//////////////////////////////////////////////////
////////       GUIElementBitmapText       ////////
//////////////////////////////////////////////////

class GUIElementBitmapText : public GUIElement
{
private:
	BitmapFont* bitmapFont;

	IDirect3DTexture9* cashedTexture;

	float textScale;
	DWORD textFormat;
	D3DCOLOR textColor;

	wstring text;
	int textLength;


	struct LetterPosition
	{
		RECT* offset;
		D3DXVECTOR3 position;
	};

	vector <LetterPosition> lettersPositions;


	GUIElementBitmapText();

public:

	GUIElementBitmapText(RECT* _screenPosition, BitmapFont* _bitmapFont, float _textScale, DWORD _formatPosition = 0, bool _formatSingleLine = false, D3DCOLOR _textColor = 0xFF000000);
	GUIElementBitmapText(RECT* _screenPosition, BitmapFont* _bitmapFont, float _textScale, DWORD _format = 0, D3DCOLOR _textColor = 0xFF000000);

	~GUIElementBitmapText();

	int setColor(D3DCOLOR _color = 0xFFFFFFFF);

	int setText(wstring const & _text, int _textLength = -1);

	int setText(string const & _text, int _textLength = -1);

	int setTextLength(int _textLength);

	int draw(ID3DXSprite* _sprite);
};



#endif //_CORE_GUI_ELEMENT

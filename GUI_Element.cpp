#include "GUI_Element.h"




GUIElement::GUIElement() : screenPosition( NULL )
{};

GUIElement::GUIElement( RECT* _screenPosition ) : screenPosition( _screenPosition )
{};


void GUIElement::setScreenPosition(RECT* _screenPosition)
{
	screenPosition = _screenPosition;
};


GUIElementTexture::GUIElementTexture() : GUIElement(), texture( NULL )
{
	texCoord.top = 0;
	texCoord.left = 0;
	texCoord.bottom = 0;
	texCoord.right = 0;
};


GUIElementTexture::GUIElementTexture( RECT* _screenPosition, Texture* _texture, RECT _texCoord, D3DCOLOR _color ) : GUIElement( _screenPosition ), texture( _texture ), texCoord( _texCoord ), color( _color ), cashedTexture( texture->getResourcePointer() )
{
	float fScaleX = (float)(screenPosition->right - screenPosition->left) / (float)(texCoord.right - texCoord.left);
	float fScaleY = (float)(screenPosition->bottom - screenPosition->top) / (float)(texCoord.bottom - texCoord.top);

	D3DXMatrixScaling(&transformMatrix, fScaleX, fScaleY, 1.0f);
	position.x = (float)(screenPosition->left) / fScaleX;
	position.y = (float)(screenPosition->top) / fScaleY;
	position.z = 0.0f;
};


GUIElementTexture::~GUIElementTexture()
{
	SAFE_RELEASE(texture);
}:


int GUIElementTexture::setColor(D3DCOLOR _color)
{
	color = _color;
	return 0;
};


int GUIElementTexture::draw(ID3DXSprite* _sprite)
{
	if (FAILED( HRESULT hr = _sprite->Draw(texture->getResourcePointer(), &texCoord, NULL, &position, color)))
	{
		showError(false, "ID3DXSprite::Draw error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -1;
	}

	return 0;
};




GUIElementText::GUIElementText() : GUIElement(), gdiFont(NULL), pFont(NULL), textFormat(0), textColor(0xFF000000), textLength(-1)
{};


GUIElementText::GUIElementText(RECT* _screenPosition, GDIFont* _gdiFont, ID3DXFont* _pFont, DWORD _formatPosition, bool _formatSingleLine, D3DCOLOR _textColor) : GUIElement(_screenPosition), gdiFont( _gdiFont ), pFont( _pFont ), textColor( _textColor ), textLength( -1 )
{
	textFormat = _formatPosition & (DT_TOP | DT_VCENTER | DT_BOTTOM | DT_LEFT | DT_CENTER | DT_RIGHT);
	if (_formatPosition & (DT_VCENTER | DT_BOTTOM) || _formatSingleLine)
		textFormat |= DT_SINGLELINE;
};


GUIElementText::GUIElementText(RECT* _screenPosition, GDIFont* _gdiFont, ID3DXFont* _pFont, DWORD _format, D3DCOLOR _textColor) : GUIElement(_screenPosition), gdiFont( _gdiFont ), pFont( _pFont ), textColor( _textColor ), textLength( -1 )
{
	textFormat = _format & (DT_TOP | DT_VCENTER | DT_BOTTOM | DT_LEFT | DT_CENTER | DT_RIGHT);
	if ((_format & (DT_VCENTER | DT_BOTTOM)) || (_format |= DT_SINGLELINE))
		textFormat |= DT_SINGLELINE;
};


GUIElementText::~GUIElementText()
{
	IDX_SAFE_RELEASE(gdiFont);
};


int GUIElementText::setColor(D3DCOLOR _color)
{
	color = _color;
	return 0;
};


int GUIElementText::setText(wstring const & _text, int _textLength)
{
	text = _text;
	textLength = _textLength;

	return 0;
};


int GUIElementText::setText(string const & _text, int _textLength)
{
	text = wstring(_text.begin(), _text.end());
	textLength = _textLength;

	return 0;
};


int GUIElementText::setTextLength(int _textLength)
{
	textLength = (_textLength < 0) ? -1 : ((_textLength >= text.size()) ? text.size() : _textLength);
	return 0;
};


int GUIElementText::draw(ID3DXSprite* _sprite)
{
	if (!(pFont->DrawTextW(_sprite, text.c_str(), textLength, &screenPosition, textFormat, textColor)))
	{
		showError(false, "ID3DXFont::DrawTextW error\n");
		return -1;
	}

	return 0;
};




GUIElementBitmapText::GUIElementBitmapText() : GUIElement(), font( NULL ), textFormat( 0 ), textColor( 0xFF000000 ), textLength( -1 )
{};


GUIElementBitmapText::GUIElementBitmapText( RECT* _screenPosition, BitmapFont* _bitmapFont, float _textScale, DWORD _formatPosition, bool _formatSingleLine, D3DCOLOR _textColor ) : GUIElement(_screenPosition), bitmapFont( _bitmapFont ), cashedTexture( bitmapFont->getResourcePointer()->bitmap ), textScale( _textScale ), textColor( _textColor )
{
	textFormat = _formatPosition & (DT_TOP | DT_VCENTER | DT_BOTTOM | DT_LEFT | DT_CENTER | DT_RIGHT);
	if (_formatSingleLine)
		textFormat |= DT_SINGLELINE;
};


GUIElementBitmapText::GUIElementBitmapText(RECT* _screenPosition, BitmapFont* _bitmapFont, float _textScale, DWORD _format, D3DCOLOR _textColor) : GUIElement(_screenPosition), bitmapFont( _bitmapFont ), cashedTexture( bitmapFont->getResourcePointer()->bitmap ), textScale( _textScale ), textColor( _textColor )
{
	textFormat = _format & (DT_TOP | DT_VCENTER | DT_BOTTOM | DT_LEFT | DT_CENTER | DT_RIGHT);
	if (_format & DT_SINGLELINE)
		textFormat |= DT_SINGLELINE;
};


GUIElementBitmapText::~GUIElementBitmapText()
{
	IDX_SAFE_RELEASE(bitmapFont);
}:


int GUIElementBitmapText::setColor(D3DCOLOR _color)
{
	color = _color;
	return 0;
};


int GUIElementBitmapText::setText(wstring const & _text, int _textLength)
{
	text = _text;
	textLength = (_textLength >= text.size()) ? text.size() : _textLength;

	UINT length = text.size();
	lettersPositions.resize(length);
	wchar_t* str = text.c_str();

	const BitmapCharacter* bmLastChar = NULL;
	const BitmapCharacter* bmChar = NULL;
	UINT lineHeight = (UINT)(textScale * (float)(bitmapFont->getLineHeight()));


	if (textFormat & DT_SINGLELINE)
	{
		float top = 0.0f;
		UINT left = 0;

		if (textFormat & DT_BOTTOM)
			top = (float)(screenPosition->bottom - lineHeight);
		else if (textFormat & DT_VCENTER)
			top = (float)(screenPosition->top + ((screenPosition->bottom - screenPosition->top - lineHeight) / 2 ));
		else
			top = (float)(screenPosition->top);

		top /= textScale;


		for (UINT i = 0; i < length; ++i)
		{
			bmChar = (str[i] == '\n' ? bitmapFont->getChar(' ') : bitmapFont->getChar(str[i]);

			if (!bmChar)
			{
				lettersPositions[i].offset = NULL;
			}
			else
			{
				lettersPositions[i].offset = &(bmChar->texPos);

				if (bmLastChar)
					left += bmLastChar->getKerning(str[i]);

				lettersPositions[i].position.x = (float)left;
				lettersPositions[i].position.y = top;
				lettersPositions[i].position.z = 0.0f;

				left += (lettersPositions[i].offset->right - lettersPositions[i].offset->left);
				bmLastChar = bmChar;
			}
		}


		float offset = 0.0f;

		if (textFormat & DT_RIGHT)
			offset = ((float)(screenPosition->right) - (left * textScale)) / textScale;
		else if (textFormat & DT_CENTER)
			offset = ((float)screenPosition->left + (((float)screenPosition->right - (float)screenPosition->left - (left * textScale)) * 0.5f)) / textScale;
		else
			offset = (float)screenPosition->left / textScale;


		for (UINT i = 0; i < length; ++i)
			if (lettersPositions[i].offset)
				lettersPositions[i].position.x += offset;
	}
	else
	{
		UINT linesWidth[200];
		UINT linesLetterNum[200];
		UINT j = 0;
		int lastLetterNum = -1;

		UINT top = 0;
		UINT left = 0;

		UINT maxWidth = (UINT)((float)(screenPosition->right - screenPosition->left) / textScale);

		for (UINT i = 0; i < length; ++i)
		{
			if (str[i] == '\n')
			{
				linesWidth[j] = left;
				linesLetterNum[j] = i - lastLetterNum;
				lastLetterNum = i;
				j++;

				top += lineHeight;
				left = 0;
				bmLastChar = NULL;
			}
			else
			{
				bmChar = bitmapFont->getChar(str[i]);
				if (!bmChar)
				{
					lettersPositions[i].offset = NULL;
				}
				else
				{
					short kerning = bmLastChar ? bmLastChar->getKerning(str[i]) : 0;
					UINT letterWidth = bmChar->texPos.right - bmChar->texPos.left;

					if (maxWidth < left + kerning + letterWidth)
					{
						i--;
						linesWidth[j] = left;
						linesLetterNum[j] = i - lastLetterNum;
						lastLetterNum = i;
						j++;

						top += lineHeight;
						left = 0;
						bmLastChar = NULL;
					}
					else
					{
						lettersPositions[i].offset = &(bmChar->texPos);

						left += kerning;

						lettersPositions[i].position.x = (float)left;
						left += letterWidth;

						lettersPositions[i].position.y = (float)top;

						lettersPositions[i].position.z = 0.0f;
					}
				}
			}
		}


		UINT lines = j;

		int jj = -1;
		int letterb = -1;
		float offsetX = 0.0f;
		float offsetY = 0.0f;

		for (UINT i = 0; i < length; ++i)
		{
			if (letterb < i)
			{
				jj++;
				letterb += linesLetterNum[jj];

				if (textFormat & DT_RIGHT)
					offsetX = ((float)(screenPosition->right) - (linesWidth[jj] * textScale)) / textScale;
				else if (textFormat & DT_CENTER)
					offsetX = ((float)screenPosition->left + (((float)screenPosition->right - (float)screenPosition->left - (linesWidth[jj] * textScale)) * 0.5f)) / textScale;
				else
					offsetX = (float)screenPosition->left / textScale;

				if (textFormat & DT_BOTTOM)
					offsetY = ((float)(screenPosition->bottom) - (jj * lineHeight * textScale)) / textScale;
				else if (textFormat & DT_VCENTER)
					offsetY = ((float)screenPosition->top + (((float)(screenPosition->bottom - screenPosition->top) - (jj * lineHeight * textScale)) * 0.5f) / textScale;
				else
					offsetY = (float)screenPosition->top / textScale;
			}


			if (lettersPositions[i].offset)
			{
				lettersPositions[i].position.x += offsetX;
				lettersPositions[i].position.y += offsetY;
			}
		}
	}

	return 0;
};


int GUIElementBitmapText::setText(string const & _text, int _textLength)
{
	return setText(wstring(_text.begin(), _text.end()), _textLength);
};


int GUIElementBitmapText::setTextLength(int _textLength)
{
	textLength = _textLength;
	return 0;
};


int GUIElementBitmapText::draw(ID3DXSprite* _sprite)
{
	HRESULT hr = S_OK;
	UINT renderLength = (textLength < 0) ? text.size() : textLength;

	for (UINT i = 0; i < renderLength; ++i)
	{
		if (FAILED(hr = _sprite->Draw(cashedTexture, lettersPositions[i].offset, NULL, &(lettersPositions[i].position), textColor)))
		{
			showError(false, "ID3DXSprite::Draw error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -1;
		}
	}

	return 0;
};



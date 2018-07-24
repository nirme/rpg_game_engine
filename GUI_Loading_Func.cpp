#include "GUI_Loading_Func.h"



int loadGUIElement(sqlite3* _db, GUIElement** _guiElement, UINT _elementId, DWORD _bitFlag)
{
	// GUI_Element
	//+---+-------------+
	//|col| name        |
	//+---+-------------+
	//| 0 | type        |
	//+---+-------------+
	//| 1 | resource_id |
	//+---+-------------+
	//| 2 | data        |
	//+---+-------------+


	int result = 0;
	sqlite3_stmt* stmt = NULL;
	ostringstream sql;
	sql << "SELECT `type`, `resource_id`, `data` FROM `GUI_Element` WHERE `id` = " << _elementId | _bitFlag;

	if ((result = sqlite3_prepare_v2(_db, sql.str().c_str(), -1, &stmt, NULL)) != SQLITE_OK)
	{
		showError(false, "sqlite3_prepare_v2 failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
		sqlite3_finalize(stmt);
		return -1;
	}

	result = sqlite3_step(stmt);

	if (result != SQLITE_ROW)
	{
		if (result != SQLITE_DONE)
		{
			showError(false, "sqlite3_step failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
			sqlite3_finalize(stmt);
			return -2;
		}

		sqlite3_finalize(stmt);

		if (!_bitFlag)
			return 1;

		sql.clear();
		sql << "SELECT `type`, `resource_id`, `data` FROM `GUI_Element` WHERE `id` = " << _elementId;

		if ((result = sqlite3_prepare_v2(_db, sql.str().c_str(), -1, &stmt, NULL)) != SQLITE_OK)
		{
			showError(false, "sqlite3_prepare_v2 failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
			sqlite3_finalize(stmt);
			return -1;
		}

		result = sqlite3_step(stmt);

		if (result != SQLITE_ROW)
		{
			if (result != SQLITE_DONE)
			{
				showError(false, "sqlite3_step failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
				sqlite3_finalize(stmt);
				return -2;
			}
			else
			{
				showError(false, "GUIElement by id = %d not found", _elementId);
				sqlite3_finalize(stmt);
				return 1;
			}
		}
	}

	int type = sqlite3_column_int(stmt, 0);
	string resourceId = sqlite3_column_text(stmt, 1);
	BYTE* data = reinterpret_cast <BYTE*> (sqlite3_column_blob(stmt, 2));

	int pp = 0;

	switch (type)
	{
		case GUI_ELEMENT_TEXTURE:
		{
			RECT* textureCoord = reinterpret_cast <RECT*> (&(data[pp]));
			pp += sizeof(RECT);
			DWORD* color = reinterpret_cast <DWORD*> (&(data[pp]));

			*_guiElement = GUIElementTexture(NULL, Texture::getResource(resourceId), *textureCoord, *color);

			break;
		}
		case GUI_ELEMENT_TEXT:
		{
			int* fontHeightInPoints = reinterpret_cast <int*> (&(data[pp]));
			pp += sizeof(int);
			DWORD* textFormat = reinterpret_cast <DWORD*> (&(data[pp]));
			pp += sizeof(DWORD);
			DWORD* textColor = reinterpret_cast <DWORD*> (&(data[pp]));
			pp += sizeof(DWORD);

			UINT* textLength = reinterpret_cast <UINT*> (&(data[pp]));
			pp += sizeof(UINT);
			wstring text = wstring(reinterpret_cast <wchar_t*> (&(data[pp])), *textLength);
			pp += sizeof(wchar_t) * textLength;
			int* textLenghtShown = reinterpret_cast <int*> (&(data[pp]));

			GDIFont* gdiFont = GDIFont::getResource(resourceId);
			ID3DXFont* pFont = NULL;

			D3DXCreateFontW(Gaphic::getDevice(), 
							*fontHeightInPoints, 
							0, 
							FW_NORMAL, 
							1, 
							FALSE, 
							DEFAULT_CHARSET, 
							OUT_DEFAULT_PRECIS, 
							ANTIALIASED_QUALITY, 
							DEFAULT_PITCH | FF_DONTCARE, 
							gdiFont->getFontName().c_str(), 
							&pFont);

			GUIElementText* element = GUIElementText(NULL, gdiFont, pFont, *textFormat, *textColor);
			element->setText(text, *textLenghtShown);

			*_guiElement = element;

			break;
		}
		case GUI_ELEMENT_BITMAPTEXT:
		{
			float* textScale = reinterpret_cast <float*> (&(data[pp]));
			pp += sizeof(float);
			DWORD* textFormat = reinterpret_cast <float*> (&(data[pp]));
			pp += sizeof(DWORD);
			DWORD* textColor = reinterpret_cast <float*> (&(data[pp]));
			pp += sizeof(DWORD);

			UINT* textLength = reinterpret_cast <float*> (&(data[pp]));
			pp += sizeof(UINT);
			wstring text = wstring(reinterpret_cast <wchar_t*> (&(data[pp])), *textLength);
			pp += sizeof(wchar_t) * textLength;
			int* textLenghtShown = reinterpret_cast <int*> (&(data[pp]));

			GUIElementBitmapText* element = GUIElementBitmapText(NULL, BitmapFont::getResource(resourceId), *textScale, *textFormat, *textColor);
			element->setText(text, *textLenghtShown);

			*_guiElement = element;

			break;
		}

		default:
		{
			sqlite3_finalize(stmt);
			return 2;
		}
	}

	sqlite3_finalize(stmt);

	return 0;
};


int loadGUIElementBackgroundTexture(sqlite3* _db, GUIElementTexture** _guiBGElement, UINT _elementId, DWORD _bitFlag)
{
	// GUI_Element
	//+---+-------------+
	//|col| name        |
	//+---+-------------+
	//| 0 | resource_id |
	//+---+-------------+
	//| 1 | data        |
	//+---+-------------+

	int result = 0;
	sqlite3_stmt* stmt = NULL;
	ostringstream sql;
	sql << "SELECT `resource_id`, `data` FROM `GUI_Element` WHERE `type` = " << GUI_ELEMENT_TEXTURE << "`id` = " << _elementId | _bitFlag;

	if ((result = sqlite3_prepare_v2(_db, sql.str().c_str(), -1, &stmt, NULL)) != SQLITE_OK)
	{
		showError(false, "sqlite3_prepare_v2 failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
		sqlite3_finalize(stmt);
		return -1;
	}

	result = sqlite3_step(stmt);

	if (result != SQLITE_ROW)
	{
		if (result != SQLITE_DONE)
		{
			showError(false, "sqlite3_step failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
			sqlite3_finalize(stmt);
			return -2;
		}

		sqlite3_finalize(stmt);

		if (!_bitFlag)
			return 1;

		sql.clear();
		sql << "SELECT `resource_id`, `data` FROM `GUI_Element` WHERE `type` = " << GUI_ELEMENT_TEXTURE << "`id` = " << _elementId;

		if ((result = sqlite3_prepare_v2(_db, sql.str().c_str(), -1, &stmt, NULL)) != SQLITE_OK)
		{
			showError(false, "sqlite3_prepare_v2 failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
			sqlite3_finalize(stmt);
			return -1;
		}

		result = sqlite3_step(stmt);

		if (result != SQLITE_ROW)
		{
			if (result != SQLITE_DONE)
			{
				showError(false, "sqlite3_step failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
				sqlite3_finalize(stmt);
				return -2;
			}
			else
			{
				showError(false, "GUIElement by id = %d not found", _elementId);
				sqlite3_finalize(stmt);
				return 1;
			}
		}
	}

	string resourceId = sqlite3_column_text(stmt, 0);
	BYTE* data = reinterpret_cast <BYTE*> (sqlite3_column_blob(stmt, 1));
	int pp = 0;
	RECT* textureCoord = reinterpret_cast <RECT*> (&(data[pp]));
	pp += sizeof(RECT);
	DWORD* color = reinterpret_cast <DWORD*> (&(data[pp]));

	*_guiBGElement = GUIElementTexture(NULL, Texture::getResource(resourceId), *textureCoord, *color);

	sqlite3_finalize(stmt);

	return 0;
};


int loadGUIControl(sqlite3* _db, GUIControl** _guiElement, UINT _controlId, DWORD _bitFlag)
{
	// GUI_Control
	//+---+-----------------+
	//|col|      name       |
	//+---+-----------------+
	//| 0 | type            |
	//+---+-----------------+
	//| 1 | position_left   |
	//+---+-----------------+
	//| 2 | position_top    |
	//+---+-----------------+
	//| 3 | position_right  |
	//+---+-----------------+
	//| 4 | position_bottom |
	//+---+-----------------+
	//| 5 | hotkey          |
	//+---+-----------------+
	//| 6 | additional_data |
	//+---+-----------------+


	int result = 0;
	sqlite3_stmt* stmt = NULL;
	ostringstream sql;
	sql << "SELECT `type`, `position_left`, `position_top`, `position_right`, `position_bottom`, `hotkey`, `additional_data` FROM `GUI_Control` WHERE `id` = " << _controlId | _bitFlag;

	if ((result = sqlite3_prepare_v2(_db, sql.str().c_str(), -1, &stmt, NULL)) != SQLITE_OK)
	{
		showError(false, "sqlite3_prepare_v2 failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
		sqlite3_finalize(stmt);
		return -10;
	}

	result = sqlite3_step(stmt);

	if (result != SQLITE_ROW)
	{
		if (result != SQLITE_DONE)
		{
			showError(false, "sqlite3_step failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
			sqlite3_finalize(stmt);
			return -20;
		}

		sqlite3_finalize(stmt);

		if (!_bitFlag)
			return 10;

		sql.clear();
		sql << "SELECT `type`, `position_left`, `position_top`, `position_right`, `position_bottom`, `hotkey`, `additional_data` FROM `GUI_Control` WHERE `id` = " << _controlId;

		if ((result = sqlite3_prepare_v2(_db, sql.str().c_str(), -1, &stmt, NULL)) != SQLITE_OK)
		{
			showError(false, "sqlite3_prepare_v2 failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
			sqlite3_finalize(stmt);
			return -10;
		}

		result = sqlite3_step(stmt);

		if (result != SQLITE_ROW)
		{
			if (result != SQLITE_DONE)
			{
				showError(false, "sqlite3_step failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
				sqlite3_finalize(stmt);
				return -20;
			}
			else
			{
				showError(false, "GUIControl by id = %d not found", _elementId);
				sqlite3_finalize(stmt);
				return 10;
			}
		}
	}


	UINT type = sqlite3_column_int(stmt, 0);
	RECT screenPosition;
	SetRect(&screenPosition, sqlite3_column_int(stmt, 1), sqlite3_column_int(stmt, 2), sqlite3_column_int(stmt, 3), sqlite3_column_int(stmt, 4));
	int hotkey = sqlite3_column_int(stmt, 5);
	BYTE* data = reinterpret_cast <BYTE*> (sqlite3_column_blob(stmt, 6));


	// `GUI_Control_Elements`
	//+---+------------+
	//|col|  name      |
	//+---+------------+
	//| 0 | element_id |
	//+---+------------+

	sqlite3_stmt* stmt2 = NULL;
	ostringstream sql2;
	sql << "SELECT `element_id` FROM `GUI_Control_Elements` WHERE ``control_id`` = " << _controlId;

	if ((result = sqlite3_prepare_v2(_db, sql2.str().c_str(), -1, &stmt2, NULL)) != SQLITE_OK)
	{
		showError(false, "sqlite3_prepare_v2 failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
		sqlite3_finalize(stmt2);
		sqlite3_finalize(stmt);
		return -10;
	}


	GUIElement* elements[20] = {0};
	int elementIndex = 0;

	while ( ((result = sqlite3_step(stmt)) == SQLITE_ROW) && elementIndex < 20 )
	{
		if ( result = loadGUIElement(_db, &(elements[elementIndex]), sqlite3_column_int(stmt2, 0), _bitFlag) )
		{
			for (UINT i = 0; i <elementIndex; ++i)
			{
				SAFE_DELETE(elements[elementIndex]);
			}
			sqlite3_finalize(stmt2);
			sqlite3_finalize(stmt);

			return result;
		}

		elementIndex++;
	}


	if (result != SQLITE_DONE)
	{
		showError(false, "sqlite3_step failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
		for (UINT i = 0; i <elementIndex; ++i)
		{
			SAFE_DELETE(elements[elementIndex]);
		}
		sqlite3_finalize(stmt);

		return -20;
	}

	sqlite3_finalize(stmt2);



	switch (type)
	{
		case GUI_CONTROL_STATIC:
		{
			*_guiElement = new GUIControlStatic(_controlId, screenPosition, elements, elementIndex);
			break;
		}
		case GUI_CONTROL_BUTTON:
		{
			*_guiElement = new GUIControlButton(_controlId, 
												screenPosition, 
												elements, 
												elementIndex, 
												reinterpret_cast <UINT*> (data));
			break;
		}
		default:
		{
			sqlite3_finalize(stmt);
			for (UINT i = 0; i <elementIndex; ++i)
			{
				SAFE_DELETE(elements[elementIndex]);
			}
			sqlite3_finalize(stmt);

			return 30;
		}
	}

	if (hotkey)
		(*_guiElement)->setHotkey(hotkey);

	sqlite3_finalize(stmt);

	return 0;
};


int loadGUIDialog(sqlite3* _db, GUIDialog** _guiDialog, UINT _dialogId, DWORD _bitFlag)
{
	// GUI_Dialog
	//+---+-------------------------------+
	//|col|             name              |
	//+---+-------------------------------+
	//| 0 | position_left                 |
	//+---+-------------------------------+
	//| 1 | position_top                  |
	//+---+-------------------------------+
	//| 2 | position_right                |
	//+---+-------------------------------+
	//| 3 | position_bottom               |
	//+---+-------------------------------+
	//| 4 | background_element_texture_id |
	//+---+-------------------------------+

	int result = 0;
	sqlite3_stmt* stmt = NULL;
	ostringstream sql;
	sql << "SELECT `position_left`, `position_top`, `position_right`, `position_bottom`, `background_element_texture_id` FROM `GUI_Dialog` WHERE `id` = " << _dialogId | _bitFlag;
	if ((result = sqlite3_prepare_v2(_db, sql.str().c_str(), -1, &stmt, NULL)) != SQLITE_OK)
	{
		showError(false, "sqlite3_prepare_v2 failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
		sqlite3_finalize(stmt);
		return -100;
	}

	result = sqlite3_step(stmt);

	if (result != SQLITE_ROW)
	{
		if (result != SQLITE_DONE)
		{
			showError(false, "sqlite3_step failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
			sqlite3_finalize(stmt);
			return -200;
		}

		sqlite3_finalize(stmt);

		if (!_bitFlag)
			return 100;

		sql.clear();
		sql << "SELECT `position_left`, `position_top`, `position_right`, `position_bottom`, `background_control_texture_id` FROM `GUI_Dialog` WHERE `id` = " << _dialogId;

		if ((result = sqlite3_prepare_v2(_db, sql.str().c_str(), -1, &stmt, NULL)) != SQLITE_OK)
		{
			showError(false, "sqlite3_prepare_v2 failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
			sqlite3_finalize(stmt);
			return -10;
		}

		result = sqlite3_step(stmt);

		if (result != SQLITE_ROW)
		{
			if (result != SQLITE_DONE)
			{
				showError(false, "sqlite3_step failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
				sqlite3_finalize(stmt);
				return -200;
			}
			else
			{
				showError(false, "GUIDialog by id = %d not found", _elementId);
				sqlite3_finalize(stmt);
				return 100;
			}
		}
	}


	RECT screenPosition;
	SetRect(&screenPosition, sqlite3_column_int(stmt, 0), sqlite3_column_int(stmt, 1), sqlite3_column_int(stmt, 2), sqlite3_column_int(stmt, 3));
	UINT bgElement = sqlite3_column_int(stmt, 4);

	sqlite3_finalize(stmt);


	GUIElementTexture* background = NULL;
	if (bgElement && (result = loadGUIElementBackgroundTexture(_db, &background, bgElement, _bitFlag)))
		return result;



	// GUI_Dialog_Controls
	//+---+------------+
	//|col|    name    |
	//+---+------------+
	//| 0 | dialog_id  |
	//+---+------------+
	//| 1 | control_id |
	//+---+------------+

	sql.clear();
	sql << "SELECT `control_id` FROM `GUI_Dialog_Controls` WHERE `dialog_id` = " << _dialogId;
	if ((result = sqlite3_prepare_v2(_db, sql.str().c_str(), -1, &stmt, NULL)) != SQLITE_OK)
	{
		showError(false, "sqlite3_prepare_v2 failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
		SAFE_DELETE(background);
		sqlite3_finalize(stmt);
		return -100;
	}


	GUIControl* controls[20] = {0};
	UINT controlsNumber = 0;

	while ( ((result = sqlite3_step(stmt)) == SQLITE_ROW) && controlsNumber < 20 )
	{
		if (result = loadGUIControl(_db, &(controls[controlsNumber]), sqlite3_column_int(stmt, 0), _bitFlag))
		{
			for (UINT i = 0; i < controlsNumber; ++i)
			{
				SAFE_DELETE(controls[i]);
			}
			SAFE_DELETE(background);
			sqlite3_finalize(stmt);

			return result;
		}
		controlsNumber++;
	}

	sqlite3_finalize(stmt);

	*_guiDialog = new GUIDialog(_dialogId, screenPosition, background, controls, controlsNumber);

	return 0;
};


int loadGUIPanel(sqlite3* _db, GUIPanel** _guiPanel, UINT _panelId, DWORD _bitFlag)
{
	// GUI_Panel
	//+---+-------------------------------+
	//|col|             name              |
	//+---+-------------------------------+
	//| 0 | panel_name                    |
	//+---+-------------------------------+
	//| 1 | background_element_texture_id |
	//+---+-------------------------------+

	int result = 0;
	sqlite3_stmt* stmt = NULL;
	ostringstream sql;
	sql << "SELECT `panel_name`, `background_element_texture_id` FROM `GUI_Panel` WHERE `id` = " << _panelId | _bitFlag;
	if ((result = sqlite3_prepare_v2(_db, sql.str().c_str(), -1, &stmt, NULL)) != SQLITE_OK)
	{
		showError(false, "sqlite3_prepare_v2 failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
		sqlite3_finalize(stmt);
		return -1000;
	}

	result = sqlite3_step(stmt);

	if (result != SQLITE_ROW)
	{
		if (result != SQLITE_DONE)
		{
			showError(false, "sqlite3_step failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
			sqlite3_finalize(stmt);
			return -2000;
		}

		sqlite3_finalize(stmt);

		if (!_bitFlag)
			return 1000;

		sql.clear();
		sql << "SELECT `panel_name`, `background_element_texture_id` FROM `GUI_Panel` WHERE `id` = " << _panelId;

		if ((result = sqlite3_prepare_v2(_db, sql.str().c_str(), -1, &stmt, NULL)) != SQLITE_OK)
		{
			showError(false, "sqlite3_prepare_v2 failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
			sqlite3_finalize(stmt);
			return -1000;
		}

		result = sqlite3_step(stmt);

		if (result != SQLITE_ROW)
		{
			if (result != SQLITE_DONE)
			{
				showError(false, "sqlite3_step failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
				sqlite3_finalize(stmt);
				return -2000;
			}
			else
			{
				showError(false, "GUIPanel by id = %d not found", _elementId);
				sqlite3_finalize(stmt);
				return 1000;
			}
		}
	}


	string panelName = sqlite3_column_text(stmt, 0);
	UINT bgElement = sqlite3_column_int(stmt, 1);

	sqlite3_finalize(stmt);


	GUIElementTexture* background = NULL;
	if (bgElement && (result = loadGUIElementBackgroundTexture(_db, &background, bgElement, _bitFlag)))
		return result;


	// GUI_Dialog_Controls
	//+---+------------+
	//|col|    name    |
	//+---+------------+
	//| 0 | control_id |
	//+---+------------+

	sql.clear();
	sql << "SELECT `control_id` FROM `GUI_Dialog_Controls` WHERE `dialog_id` = " << _dialogId;
	if ((result = sqlite3_prepare_v2(_db, sql.str().c_str(), -1, &stmt, NULL)) != SQLITE_OK)
	{
		showError(false, "sqlite3_prepare_v2 failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
		SAFE_DELETE(background);
		sqlite3_finalize(stmt);
		return -1000;
	}


	GUIDialog* dialogs[20] = {0};
	UINT dialogsNumber = 0;

	while ( ((result = sqlite3_step(stmt)) == SQLITE_ROW) && dialogsNumber < 20)
	{
		if (result = loadGUIDialog(_db, &(dialogs[dialogsNumber]), sqlite3_column_int(stmt, 0), _bitFlag))
		{
			for (UINT i = 0; i < dialogsNumber; ++i)
			{
				SAFE_DELETE(dialogs[i]);
			}
			SAFE_DELETE(background);
			sqlite3_finalize(stmt);

			return result;
		}
		controlsNumber++;
	}

	sqlite3_finalize(stmt);

	*_guiPanel = new GUIPanel(panelName, background, dialogs, dialogsNumber);

	return 0;
};


int loadGUISystem(sqlite3* _db, GUISystem* _guiSystem, UINT _systemId)
{
	int result = 0;

	sqlite3_stmt* stmt = NULL;
	ostringstream sql;

	RECT* screenSize = _guiSystem->getScreenSizeRect();
	UINT screenWidth = screenSize->right - screenSize->left;
	UINT screenHeight = screenSize->bottom - screenSize->top;

	float aspect = (float) screenWidth / (float) screenHeight;
	float aspectMax = aspect + EPSILON;
	float aspectMin = aspect - EPSILON;



	DWORD bitFlags = 0;

	// GUI_ScreenSize_Id_Bits
	//+---+---------------+
	//|col|     name      |
	//+---+---------------+
	//| 0 | screen_width  |
	//+---+---------------+
	//| 1 | bit_flag      |
	//+---+---------------+

	sql << "SELECT `screen_width`, `bit_flag` FROM `GUI_ScreenSize_Id_Bits` "
		<< "WHERE `screen_aspect` <= " << std::setprecision(8) << aspectMax << " AND `screen_aspect` >= "  << std::setprecision(8) <<  aspectMin
		<< " ORDER BY `screen_width` ASC";

	if ((result = sqlite3_prepare_v2(_db, sql.str().c_str(), -1, &stmt, NULL)) != SQLITE_OK)
	{
		showError(false, "sqlite3_prepare_v2 failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
		sqlite3_finalize(stmt);
		return -10000;
	}


	DWORD bitFlag = 0;

	while (((result = sqlite3_step(stmt)) == SQLITE_ROW) && (sqlite3_column_int(stmt, 0) <= screenWidth))
	{
		bitFlag = sqlite3_column_int(stmt, 1);
	}

	if (result != SQLITE_DONE)
	{
		showError(false, "sqlite3_step failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
		sqlite3_finalize(stmt);
		return -20000;
	}

	sqlite3_finalize(stmt);


	_guiSystem->clearPanels();

	// GUI_System_Panels
	//+---+----------+
	//|col|   name   |
	//+---+----------+
	//| 0 | panel_id |
	//+---+----------+

	sql.clear();
	sql << "SELECT `panel_id` FROM `GUI_System_Panels` WHERE `system_id` = " << _systemId;

	sqlite3_prepare_v2(_db, sql.str().c_str(), -1, &stmt, NULL);

	GUIPanel* guiPanel = NULL;

	while ((result = sqlite3_step(stmt)) == SQLITE_ROW)
	{
		if (result = loadGUIPanel(_db, &guiPanel, sqlite3_column_int(stmt, 0), bitFlag))
		{
			_guiSystem->clearPanels();
			return result;
		}

		if (_guiSystem->addPanel(guiPanel))
		{
			_guiSystem->clearPanels();
			return -40000;
		}
	}

	if (result != SQLITE_DONE)
	{
		showError(false, "sqlite3_step failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
		_guiSystem->clearPanels();
		sqlite3_finalize(stmt);
		return -20000;
	}

	sqlite3_finalize(stmt);

	return 0;
};

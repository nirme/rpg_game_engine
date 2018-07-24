#pragma once

#ifndef _CORE_GUI_DIALOG
#define _CORE_GUI_DIALOG

#include "utility.h"
#include "Core_Logger.h"
#include "Core_Variable_Library.h"
#include "Core_Defines.h"

#include "GUI_Element.h"
#include "GUI_Control.h"
#include "GUI_Control_Menager.h"



class GUIPanel;


class GUIDialog
{
	friend class GUIPanel;

protected:

	const UINT id;

	RECT screenPosition;

	GUIElementTexture* background;

	vector <GUIControl*> v_controls;

	GUIPanel* pPanel;

	bool bEnabled;
	bool bVisible;
	bool bMouseOver;
	bool bMouseDown;


	UINT getControlsNumber();
	const GUIControl* getControl(UINT _index);
	const GUIControl* getControlById(UINT _id);

	inline UINT getId()
	{
		return id;
	};

	int resetControls();

	int setPanel(GUIPanel* _pPanel);


public:

	GUIDialog(UINT _id, RECT _screenPosition, GUIElementTexture* _background, GUIControl** _ppControls, UINT _controlsNumber, bool _bEnabled = true, bool _bVisible = true);
	virtual ~GUIDialog();

	const GUIPanel* getPanel();

	RECT getPosition();
	int move(int _left, int _top);

	int setEnabled(bool _bEnabled);
	bool getEnabled();

	int setVisible(bool _bVisible);
	bool getVisible();

	virtual int draw(ID3DXSprite* _sprite);

	virtual bool setMouseOver(POINT _pointer);
	virtual int handleHotkey(BYTE _key);
	virtual int handleMouseDown();
	virtual int handleMouseUp();
};


#endif //_CORE_GUI_DIALOG

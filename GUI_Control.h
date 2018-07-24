#pragma once

#ifndef _CORE_GUI_CONTROL
#define _CORE_GUI_CONTROL

#include "utility.h"
#include "Core_Logger.h"
#include "Core_Variable_Library.h"
#include "Core_Defines.h"

#include "GUI_Element.h"
#include "GUI_System.h"


class GUIDialog;

class GUISystem;


enum GUI_CONTROL_TYPE
{
	GUI_CONTROL_UNDEFINED = 0x00, 
	GUI_CONTROL_STATIC = 0x01, 
	GUI_CONTROL_BUTTON = 0x02, 
//	GUI_CONTROL_;
};



class GUIControl
{
	friend class GUIDialog;
	friend class GUISystem;

protected:

	const UINT id;
	GUI_CONTROL_TYPE type;

	RECT screenPosition;

	bool bEnabled;
	bool bVisible;
	bool bMouseOver;
	bool bMouseDown;

	BYTE hotkey;

	GUIDialog* pDialog;
	GUISystem* pSystem;

	vector<GUIElement*> v_elements;


	int move(int _left, int _top);

	int sendControl();

	inline UINT getId()
	{
		return id;
	};

	int setDialog(GUIDialog* _pDialog);
	int setSystem(GUISystem* _pSystem);


public:

	GUIControl(	UINT _id, 
				GUI_CONTROL_TYPE _type, 
				RECT _screenPosition, 
				GUIElement** _elements, 
				UINT _elementsNumber);

	virtual ~GUIControl();

	const GUIDialog* getDialog();

	int setEnabled(bool _bEnabled);
	bool getEnabled();

	int setVisible(bool _bVisible);
	bool getVisible();

	int setHotkey(BYTE _hotkey);
	BYTE getHotkey();
	virtual int trigger();

	bool setMouseOver(POINT _pointer);

	bool getMouseOver();

	virtual int draw(ID3DXSprite* _sprite) = 0;

	virtual int handleHotkey(BYTE _hotkey);

	virtual int handleMouseDown();
	virtual int handleMouseUp();

	virtual int reset();

};
////


class GUIControlStatic : public GUIControl
{
public:

	GUIControlStatic(	UINT _id, RECT _screenPosition, GUIElement** _elements, UINT _elementsNumber) : GUIControl(_id, GUI_CONTROL_STATIC, _screenPosition, _elements, _elementsNumber)
	{};

	int draw(ID3DXSprite* _sprite);

	int trigger()	{ return 0; };
	int handleHotkey()	{ return 0; };
	int handleMouseDown()	{ return 0; };
	int handleMouseUp()	{ return 0; };

};
////




class GUIControlButton : public GUIControl
{
	struct ButtonState
	{
		UINT elements;
		UINT* elementsIndices;
	};

protected:
	ButtonState states[GUI_CONTROL_BUTTON_STATES_NUMBER];


public:

	GUIControlButton(	UINT _id, 
						RECT _screenPosition, 
						GUIElement** _elements, 
						UINT _elementsNumber, 
						UINT* _statesInfo);



	int draw(ID3DXSprite* _sprite);

};



#endif //_CORE_GUI_CONTROL

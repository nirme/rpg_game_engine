#pragma once

#ifndef _CORE_GUI_SYSTEM
#define _CORE_GUI_SYSTEM

#include "utility.h"
#include "Core_Logger.h"
#include "Core_Variable_Library.h"
#include "Core_Defines.h"

#include "GUI_Element.h"
#include "GUI_Control.h"
#include "GUI_Dialog.h"
#include "GUI_Panel.h"
#include "GUI_Control_Menager.h"

#include "Core_Input.h"




class GUISystem;


class __declspec(novtable) GUIEvent
{
public:

	virtual ~GUIEvent() = 0;
	virtual int triger(GUISystem* _system, GUIControl* _control) = 0;
};


class GUISystem
{
private:

	RECT screenSize;

	vector <GUIPanel*> v_panels;

	list <GUIPanel*> l_panelsStack;
	list <UINT> l_stackDepthsUsed

	bool pointerVisible;



	struct UsedControl
	{
		UINT controlId;
		GUIControl* control;

		UsedControl(UINT _controlId, GUIControl* _control) : controlId(_controlId), control(_control) {};
		UsedControl(GUIControl* _control) : control(_control) { controlId = control->getId(); };
	};


	typedef map <UINT /*control id*/, GUIEvent*> EventsMap;

	EventsMap m_events;
	vector <UsedControl> v_controlsUsed;




public:

	GUISystem(UINT _screenWidth = 0, UINT _screenHeight = 0);
	~GUISystem();

	int setScreenSize(UINT _screenWidth, UINT _screenHeight);

	RECT* getScreenSizeRect();

	int addPanel(GUIPanel* _panel);
	int clearPanels();

	int setPointerVisible(bool _visible);

	int update();

	int draw(ID3DXSprite* _sprite);

	int pushFrontPanel(string _panelId, bool renderPreviousPanels = false);
	int popFrontPanel();

	int setupEvent(UINT _controlId, GUIEvent* _event);
	int dropEvent(UINT _controlId);

	int addControl(GUIControl* _control, UINT _controlId);
	int freeControls();

};




class GUIPanelEvent : public GUIEvent
{
private:
	bool pushPanel;
	string panelId;
	bool renderPreviousPanels;


	int pushFrontPanel(string _panelId, bool renderPreviousPanels = false)

	int popFrontPanel()



public:

	GUIPanelEvent(bool _pushPanel, string _panelId = "", bool _renderPreviousPanels = false) : pushPanel(_pushPanel), renderPreviousPanels(_renderPreviousPanels)
	{
		panelId = _panelId;
	};

	~GUIPanelEvent() {};

	int triger(GUISystem* _system, GUIControl* _control);
	{
		if (pushPanel)
			return _system->pushFrontPanel(panelId, renderPreviousPanels);
		return _system->popFrontPanel();
	};
};






#endif //_CORE_GUI_SYSTEM

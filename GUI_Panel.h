#pragma once

#ifndef _CORE_GUI_PANEL
#define _CORE_GUI_PANEL

#include "utility.h"
#include "Core_Logger.h"
#include "Core_Variable_Library.h"
#include "Core_Defines.h"

#include "GUI_Element.h"
#include "GUI_Control.h"
#include "GUI_Dialog.h"
#include "GUI_System.h"



class GUISystem;


class GUIPanel
{
	friend class GUISystem;

private:

	const string id;

	bool bEnabled;
	bool bVisible;

	GUIElementTexture* background;
	vector <GUIDialog*> v_dialogs;

	struct CashedHotkeyData
	{
		BYTE hotkey;

		UINT dialogId;
		GUIDialog* cashedDialogPointer;
		UINT controlId;
		GUIControl* cashedControlPointer;


		CashedHotkeyData(BYTE _hotkey, UINT _dialogId, GUIDialog* _dialogPointer, UINT _controlId, GUIControl* _controlPointer) : 
						hotkey(_hotkey), dialogId(_dialogId), cashedDialogPointer(_dialogPointer), controlId(_controlId), cashedControlPointer(_controlPointer)
		{};


		CashedHotkeyData(BYTE _hotkey, GUIDialog* _dialogPointer, GUIControl* _controlPointer)
						hotkey(_hotkey), cashedDialogPointer(_dialogPointer), cashedControlPointer(_controlPointer)
		{
			dialogId = cashedDialogPointer->getId();
			controlId = cashedControlPointer->getId();
		};
	};

	vector <CashedHotkeyData> v_cashedHotkeyData;

	GUISystem* pSystem;


	int setSystem(GUISystem* _pSystem);


	inline int resetPointerAction()
	{
		for (UINT i = 0; i < v_dialogs.size(); ++i)
			v_dialogs[i]->resetControls();
	};


public:

	GUIPanel(string _id, GUIElementTexture* _background, GUIDialog** _ppDialogs, UINT _dialogsNumber, bool _bEnabled = true, bool _bVisible = true);

	~GUIPanel();

	string getId();

	int draw(ID3DXSprite* _sprite);

	int handleKeyClicked(BYTE _key);
	int handleMouseMove(POINT _pointer);
	int handleMouseDown();
	int handleMouseUp(POINT _pointer);


	int setEnabled(bool _bEnabled);
	bool getEnabled();

	int setVisible(bool _bVisible);
	bool getVisible();

};



#endif //_CORE_GUI_PANEL

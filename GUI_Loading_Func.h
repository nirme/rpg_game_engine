#pragma once

#ifndef _GUI_LOADING_FUNC
#define _GUI_LOADING_FUNC


#include "utility.h"

#include "Core_Defines.h"
#include "Core_Logger.h"
#include "Core_Variable_Library.h"

#include "Core_Graphic.h"

#include "GUI_System.h"
#include "GUI_Panel.h"
#include "GUI_Dialog.h"
#include "GUI_Control.h"
#include "GUI_Element.h"



int loadGUIElement(sqlite3* _db, GUIElement** _guiElement, UINT _elementId, DWORD _bitFlag = 0);

int loadGUIElementBackgroundTexture(sqlite3* _db, GUIElementTexture** _guiBGElement, UINT _elementId, DWORD _bitFlag = 0);

int loadGUIControl(sqlite3* _db, GUIControl** _guiElement, UINT _controlId, DWORD _bitFlag = 0);

int loadGUIDialog(sqlite3* _db, GUIDialog** _guiDialog, UINT _dialogId, DWORD _bitFlag = 0);

int loadGUIPanel(sqlite3* _db, GUIPanel** _guiPanel, UINT _panelId, DWORD _bitFlag = 0);

int loadGUISystem(sqlite3* _db, GUISystem* _guiSystem, UINT _systemId = 0);


#endif //_GUI_LOADING_FUNC

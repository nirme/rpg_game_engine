#include "GUI_Panel.h"




int GUIPanel::setSystem(GUISystem* _pSystem)
{
	pSystem = _pSystem;

	if (background)
		background->setScreenPosition(pSystem->getScreenSizeRect());

	for (UINT i = 0; i < v_dialogs.size(); ++i)
	{
		v_dialogs[i]->setPanel(this);
		for (UINT j = 0; j < v_dialogs[i]->getControlsNumber(); ++j)
			v_dialogs[i]->getControl(j)->setSystem(_pSystem);
	}
};


GUIPanel::GUIPanel(string _id, GUIElementTexture* _background, GUIDialog** _ppDialogs, UINT _dialogsNumber, bool _bEnabled, bool _bVisible) : id(_id), pSystem(NULL), background(_background), bEnabled(_bEnabled), bVisible(_bVisible)
{
	v_dialogs.resize(_dialogsNumber);
	memcpy(&(v_dialogs[0]), _ppDialogs, _dialogsNumber * sizeof(GUIDialog*));

	BYTE key = 0;

	for (UINT i = 0; i < v_dialogs.size(); ++i)
	{
		v_dialogs[i]->setPanel(this);

		for (UINT j = 0; j < v_dialogs[i]->getControlsNumber(); ++j)
		{
			GUIControl* control = v_dialogs[i]->getControl(j);
			if (key = control->getHotkey())
				v_cashedHotkeyData.push_back(CashedHotkeyData(key, v_dialogs[i], control));
		}
	}
};


GUIPanel::~GUIPanel()
{
	SAFE_DELETE(background);

	for (UINT i = 0; i < v_dialogs.size(); ++i)
	{
		SAFE_DELETE(v_dialogs[i]);
	}
};


int GUIPanel::draw(ID3DXSprite* _sprite)
{
	if (!visible)
		return 0;

	int res = 0;

	if (background)
		res += background->draw(_sprite);

	for (UINT i = 0; i < v_dialogs.size(); ++i)
		res += v_dialogs[i]->draw(_sprite);

	return res;
};


int GUIPanel::handleKeyClicked(BYTE _key)
{
	if (!bEnabled)
		return 0;

	for (UINT i = 0; i < v_cashedHotkeyData.size(); ++i)
		if (_key == v_cashedHotkeyData[i].hotkey)
			v_cashedHotkeyData[i].cashedControlPointer->trigger();
	return 0;
};


int GUIPanel::handleMouseMove(POINT _pointer)
{
	if (!bEnabled || !bVisible)
		return 0;

	for (UINT i = 0; i < v_dialogs.size(); ++i)
		v_dialogs[i]->setMouseOver(_pointer);

	return 0;
};


int GUIPanel::handleMouseDown()
{
	if (!bEnabled || !bVisible)
		return 0;

	for (UINT i = 0; i < v_dialogs.size(); ++i)
		v_dialogs[i]->handleMouseDown();

	return 0;
};


int GUIPanel::handleMouseUp(POINT _pointer)
{
	if (!bEnabled || !bVisible)
		return 0;

	for (UINT i = 0; i < v_dialogs.size(); ++i)
		v_dialogs[i]->handleMouseDown();

	return 0;
};


int GUIPanel::setEnabled(bool _bEnabled)
{
	if (bEnabled && !_bEnabled)
		resetPointerAction();

	bEnabled = _bEnabled;

	return 0;
};


bool GUIPanel::getEnabled()
{
	return bEnabled;
};


int GUIPanel::setVisible(bool _bVisible)
{
	if (!bVisible && _bVisible)
		resetPointerAction();

	bVisible = _bVisible;

	return 0;
};


bool GUIPanel::getVisible()
{
	return bVisible;
};


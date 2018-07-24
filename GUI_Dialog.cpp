#include "GUI_Dialog.h"




UINT GUIDialog::getControlsNumber()
{
	return v_controls.size();
};


const GUIControl* GUIDialog::getControl(UINT _index)
{
	if (_index >= v_controls.size())
		return NULL;

	return v_controls[_index];
};


const GUIControl* GUIDialog::getControlById(UINT _id)
{
	for (UINT i = 0; i < v_controls.size(); ++i)
		if (_id == v_controls[i]->id)
			return v_controls[i];

	return NULL;
};


int GUIDialog::resetControls()
{
	bMouseOver = false;
	bMouseDown = false;

	int res = 0;
	for (UINT i = 0; i < v_controls.size(); ++i)
		res += v_controls[i]->reset();

	return res;
};


int GUIDialog::setPanel(GUIPanel* _pPanel)
{
	pPanel = _pPanel;
	return 0;
};



GUIDialog::GUIDialog(UINT _id, RECT _screenPosition, GUIElementTexture* _background, GUIControl** _ppControls, UINT _controlsNumber, bool _bEnabled, bool _bVisible)
	: id(_id), pPanel(NULL), background(_background), bEnabled(_bEnabled), bVisible(_bVisible), pPanel(NULL), screenPosition(_screenPosition), bMouseOver(false), bMouseDown(false)
{
	if (background)
		background->setScreenPosition(&screenPosition);

	v_controls.resize(_controlsNumber);
	memcpy(&(v_controls[0]), _ppControls, _controlsNumber * sizeof(GUIControl*));

	for (UINT i = 0; i < v_controls.size(); ++i)
	{
		setDialog[i]->setDialog(this);
		v_controls[i]->setEnabled(_bEnabled);
	}
};


GUIDialog::~GUIDialog()
{
	SAFE_DELETE(background);

	for (UINT i = 0; i < v_controls.size(); ++i)
	{
		SAFE_DELETE(v_controls[i]);
	}
};


const GUIPanel* GUIDialog::getPanel()
{
	return pPanel;
};


RECT GUIDialog::getPosition()
{
	return screenPosition;
};


int GUIDialog::move(int _left, int _top)
{
	screenPosition.left += _left;
	screenPosition.right += _left;
	screenPosition.top += _top;
	screenPosition.bottom += _top;

	for (UINT i = 0; i < v_controls.size(); ++i)
	{
		v_controls[i]->move(_left, _top);
	}

	return 0;
};


int GUIDialog::setEnabled(bool _bEnabled)
{
	bEnabled = _bEnabled;
	for (UINT i = 0; i < v_controls.size(); ++i)
	{
		v_controls[i]->setEnabled(bEnabled);
	}

	return 0;
};


bool GUIDialog::getEnabled()
{
	return bEnabled;
};


int GUIDialog::setVisible(bool _bVisible)
{
	bVisible = _bVisible;
	return 0;
};


bool GUIDialog::getVisible()
{
	return bVisible;
};


int GUIDialog::draw(ID3DXSprite* _sprite)
{
	if (!bVisible)
		return 0;

	int ret = 0;

	if (background)
		ret += background->draw(_sprite);

	for (UINT i = 0; i < v_controls.size(); ++i)
	{
		ret += v_controls[i]->draw(_sprite);
	}

	return ret;
};


bool GUIDialog::setMouseOver(POINT _pointer)
{
	if (!bEnabled || !bVisible)
		return false;

	if (_pointer.x < screenPosition.left || _pointer.x > screenPosition.right || _pointer.y < screenPosition.top || _pointer.y > screenPosition.bottom)
	{
		if (bMouseOver)
		{
			for (UINT i = 0; i < v_controls.size(); ++i)
			{
				bMouseOver = bMouseOver || v_controls[i]->setMouseOver(_pointer);
			}
			bMouseOver = false;
		}

		return bMouseOver;
	}

	bMouseOver = true;
	bool ret = false;
	for (UINT i = 0; i < v_controls.size(); ++i)
	{
		ret = ret || v_controls[i]->setMouseOver(_pointer);
	}

	return bMouseOver;
};


int GUIDialog::handleHotkey(BYTE _key)
{
	int ret = 0;

	for (UINT i = 0; i < v_controls.size(); ++i)
		ret += v_controls[i]->handleHotkey(_key);

	return ret;
};


virtual int handleMouseDown()
{
	if (!bMouseOver)
		return 0;

	bMouseDown = false;
	for (UINT i = 0; i < v_controls.size(); ++i)
	{
		bMouseDown = bMouseDown || v_controls[i]->handleMouseDown();
	}

	return 0;
};


virtual int handleMouseUp()
{
	if (!bMouseDown)
		return 0;

	bMouseDown = false;

	for (UINT i = 0; i < v_controls.size(); ++i)
	{
		v_controls[i]->handleMouseUp();
	}

	return 0;
};

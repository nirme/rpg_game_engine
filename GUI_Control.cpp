#include "GUI_Control.h"



int GUIControl::setDialog(GUIDialog* _pDialog)
{
	pDialog = _pDialog;
	return 0;
};


int GUIControl::setSystem(GUISystem* _pSystem)
{
	pSystem = _pSystem;
	return 0;
};


int GUIControl::move(int _left, int _top)
{
	screenPosition.left += _left;
	screenPosition.right += _left;
	screenPosition.top += _top;
	screenPosition.bottom += _top;

	return 0;
};


int GUIControl::sendControl()
{
	return pSystem ? pSystem->addControl(this, id) : 0;
};



GUIControl::GUIControl(UINT _id, GUI_CONTROL_TYPE _type, RECT _screenPosition, GUIElement** _elements, UINT _elementsNumber) : 
	id(_id), type(_type), pSystem(NULL), bEnabled(false), bVisible(false), bMouseOver(false), bMouseDown(false), hotkey(0), pDialog(NULL), screenPosition(_screenPosition)
{
	v_elements.resize(_elementsNumber);
	memcpy(&(v_elements[0]), _elements, sizeof(GUIElement*) * _elementsNumber);

	for (UINT i = 0; i < _elementsNumber; ++i)
	{
		v_elements->setScreenPosition(&screenPosition);
	}
};


GUIControl::~GUIControl()
{
	for (UINT i = 0; i < v_elements.size(); ++i)
		delete v_elements[i];
};


const GUIDialog* getDialog()
{
	return pDialog;
};


int GUIControl::setEnabled(bool _bEnabled)
{
	bEnabled = _bEnabled;
	return 0;
};

bool GUIControl::getEnabled()
{
	return bEnabled;
};

int GUIControl::setVisible(bool _bVisible)
{
	bVisible = _bVisible;
	return 0;
};

bool GUIControl::getVisible()
{
	return bVisible;
};

int GUIControl::setHotkey(BYTE _hotkey)
{
	hotkey = _hotkey;
	return 0;
};

BYTE GUIControl::getHotkey()
{
	return hotkey;
};

int GUIControl::trigger()
{
	if (bEnabled)
		return sendControl();
	return 0;
};


bool GUIControl::setMouseOver(POINT _pointer)
{
	if (!bEnabled || !bVisible)
		return false;

	if (_pointer.x < screenPosition.left || _pointer.x > screenPosition.right || _pointer.y < screenPosition.top || _pointer.y > screenPosition.bottom)
		bMouseOver = false;
	else
		bMouseOver = true;

	return bMouseOver;
};


bool GUIControl::getMouseOver()
{
	return bMouseOver;
};


int GUIControl::handleHotkey(BYTE _hotkey)
{
	if (_hotkey == hotkey)
		return sendControl();
	return 0;
};


int GUIControl::handleMouseDown()
{
	if (bMouseOver)
		bMouseDown = true;

	return 0;
};


int GUIControl::handleMouseUp()
{
	if (!bMouseDown)
		return 0;

	bMouseDown = false;

	if (bMouseOver)
		return sendControl();

	return 0;
};


int GUIControl::reset()
{
	bMouseOver = false;
	bMouseDown = false;
	return 0;
};



int GUIControlStatic::draw(ID3DXSprite* _sprite)
{
	if (!bVisible)
		return 0;

	for (UINT i = 0; i < v_elements.size(); ++i)
	{
		res += v_elements[i]->draw(_sprite);
	}

	return res;
};



GUIControlButton::GUIControlButton(UINT _id, RECT _screenPosition, GUIElement** _elements, UINT _elementsNumber, UINT* _statesInfo) : 
	GUIControl(_id, GUI_CONTROL_BUTTON, _screenPosition, _elements, _elementsNumber)
{
	UINT p = 0;
	for (UINT i = 0; i < GUI_CONTROL_BUTTON_STATES_NUMBER; ++i)
	{
		states[i].elements = _statesInfo[p++];
		states[i].elementsIndices = new UINT [states[i].elements];

		memcpy(states[i].elementsIndices, &(_statesInfo[p]), states[i].elements * sizeof(UINT));
		p += states[i].elements;
	}
};


int GUIControlButton::draw(ID3DXSprite* _sprite)
{
	if (!bVisible)
		return 0;

	UINT stateId = GUI_CONTROL_BUTTON_STATE_FREE;
	if (!bEnabled)
		stateId = GUI_CONTROL_BUTTON_STATE_DISABLED;
	else if (bMouseDown)
		stateId = GUI_CONTROL_BUTTON_STATE_DOWN;
	else if (bMouseOver)
		stateId = GUI_CONTROL_BUTTON_STATE_ACTIVE;


	int res = 0;

	for (UINT i = 0; i < states[stateId].elements; ++i)
	{
		res += v_elements[states[stateId].elementsIndices[i]]->draw(_sprite);
	}

	return res;
};


#include "GUI_System.h"





GUISystem::GUISystem(UINT _screenWidth, UINT _screenHeight)
{
	screenSize.left = 0;
	screenSize.top = 0;
	screenSize.right = _screenWidth;
	screenSize.bottom = _screenHeight;
};


GUISystem::~GUISystem()
{
	for (UINT i = 0; i <  v_panels.size(); ++i)
		SAFE_DELETE(v_panels[i]);
};


int GUISystem::setScreenSize(UINT _screenWidth, UINT _screenHeight)
{
	screenSize.left = 0;
	screenSize.top = 0;
	screenSize.right = _screenWidth;
	screenSize.bottom = _screenHeight;

	return 0;
};


RECT* GUISystem::getScreenSizeRect()
{
	return &screenSize;
};


int GUISystem::addPanel(GUIPanel* _panel)
{
	if (!_panel)
		return 1;
	v_panels.push_back(_panel);
	_panel->setSystem(this);
	return 0;
};


int GUISystem::clearPanels()
{
	for (UINT i = 0; i < v_panels.size(); ++i)
	{
		SAFE_DELETE(v_panels[i]);
	}
	v_panels.clear();

	return 0;
};


int GUISystem::setPointerVisible(bool _visible)
{
	pointerVisible = _visible;
	return 0;
};


int GUISystem::update()
{
	BYTE keysNumber = 0;
	const BYTE* keys = Input::getKeysClicked(&keysNumber);

	GUIPanel* panel = l_panelsStack.front();

	int res = 0;
	if (keysNumber)
	{
		for (UINT i = 0; i < keysNumber; ++i)
			res += panel->handleKeyClicked(keys[i]);
	}

	if (pointerVisible)
	{
		if (Input::getMouseMoved())
			res += panel->handleMouseMove(Input::getCursorPosition());

		if (Input::getMouseButton1State())
			res += panel->handleMouseDown();

		if (Input::getMouseButton1Clicked())
			res += panel->handleMouseUp(Input::getCursorPosition());
	};


	GUISystem::EventsMap::iterator it;

	for (UINT i = 0; i < v_controlsUsed; ++i)
	{
		it = m_events.find(v_controlsUsed[i].controlId);

		if (it != m_events.end())
			res += it->second->triger(this, v_controlsUsed[i].control);
	}

	v_controlsUsed.clear();


	return res;
};


int GUISystem::draw(ID3DXSprite* _sprite)
{
	if (!l_panelsStack.size())
		return 0;

	UINT stackDepthUsed = l_stackDepthsUsed.front();
	GUIPanel* panelsToDraw[stackDepthUsed];
	UINT i = stackDepthUsed - 1;

	for (list<GUIPanel*>::iterator it = l_panelsStack.begin(); it != l_panelsStack.end(); it++)
		panelsToDraw[i--] = (*it);

	int ret = 0;
	for (i = 0; i < panelsToDraw; ++i)
		ret += panelsToDraw[i]->draw(_sprite);

	return ret;
};


int GUISystem::pushFrontPanel(string _panelId, bool renderPreviousPanels)
{
	GUIPanel* panel = NULL;
	for (UINT i = 0; i < v_panels.size(); ++i)
	{
		if (_panelId = v_panels[i]->getId())
		{
			panel = v_panels[i];
			break;
		}
	}

	if (!panel)
		return 1;


	l_panelsStack.push_front(panel);

	if (renderPreviousPanels && l_stackDepthsUsed.size())
		(*(l_stackDepthsUsed.begin()))++;
	else
		l_stackDepthsUsed.push_front(1);

	v_controlsUsed.clear();

	return 0;
};


int GUISystem::popFrontPanel()
{
	if (l_panelsStack.size())
		return 1;

	l_panelsStack.pop_front();
	if (l_stackDepthsUsed.front() == 1)
		l_stackDepthsUsed.pop_front();
	else
		(*(l_stackDepthsUsed.begin()))--;

	v_controlsUsed.clear();

	return 0;
};


int GUISystem::setupEvent(UINT _controlId, GUIEvent* _event)
{
	pair<GUISystem::EventsMap::iterator, bool> ret = m_events.insert(GUISystem::EventsMap::value_type(_controlId, _event));
	if (!ret.second)
	{
		SAFE_DELETE(ret.first);
		ret.first = _event;
	}
	return 0;
};


int GUISystem::dropEvent(UINT _controlId)
{
	m_events.erase(_controlId);
	return 0;
};


int GUISystem::addControl(GUIControl* _control, UINT _controlId)
{
	v_controlsUsed.push_back(UsedControl(_controlId, _control);
	return 0;
};


int GUISystem::freeControls()
{
	v_controlsUsed.clear();
	return 0;
};

#pragma once

#include "utility.h"

class Input
{
public:
	Input(HINSTANCE hInstance, HWND hWnd);
	~Input();
	void detect(void);
	
	BYTE getKeyBind(BYTE num);
	void setKeyBind(BYTE num, BYTE bind);
	float changeMouseSpeed(float speed);
	DIMOUSESTATE getMouseState();
	BYTE getKeyState(BYTE num);

private:
	LPDIRECTINPUT8 dInput;		// DirectInput interface
	LPDIRECTINPUTDEVICE8 dInputKeyboard;
	LPDIRECTINPUTDEVICE8 dInputMouse;
	BYTE keyState[256];		// key-info table
	DIMOUSESTATE mouseState;		//mouse-info
	BYTE keyBinds[256];
};

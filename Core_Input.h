#pragma once

#ifndef _CORE_INPUT
#define _CORE_INPUT

#include "utility.h"
#include "Core_Logger.h"



class Input
{
protected:
	static IDirectInput8* dInput;		// DirectInput interface

	static IDirectInputDevice8* dInputKeyboard;
	static IDirectInputDevice8* dInputMouse;

	static BYTE keyState[2][256];		// key-info table

	static bool keyClickedArrActual;
	static BYTE keysClicked[256];
	static UINT keysClickedNumber;

	static DIMOUSESTATE mouseState[2];		//mouse-info
	static bool mouseMoved;
	static bool mouseButtonsStateChanged;

	static int actualStateIndex;
	static int lastStateIndex;

	static POINT cursorPosition;

	static BYTE keyBinds[256];
	static float mouseSpeed;

	Input(){};


public:

	static int initialize(HINSTANCE _hInstance, HWND _hWnd);
	static int shutdown();
	static DWORD update();

	static BYTE getKeyBind(BYTE _num);
	static void setKeyBind(BYTE _num, BYTE _bind);

	static BYTE getKeyState(BYTE _num);
	static bool getKeyClicked(BYTE _num);

	static const BYTE* getKeysStates();
	static const BYTE* getKeysClicked(BYTE* _keysNumber);

	static DIMOUSESTATE getMouseState();
	static float getMouseSpeed();
	static float setMouseSpeed(float _speed);

	static long getMouseX();
	static long getMouseY();
	static long getMouseZ();

	static float getMouseScaledX();
	static float getMouseScaledY();
	static float getMouseScaledZ();

	static BYTE getMouseButton1State();
	static bool getMouseButton1Clicked();
	static BYTE getMouseButton2State();
	static bool getMouseButton2Clicked();
	static BYTE getMouseButton3State();
	static bool getMouseButton3Clicked();
	static BYTE getMouseButton4State();
	static bool getMouseButton4Clicked();

	static POINT getCursorPosition();
	static short getCursorPositionX();
	static short getCursorPositionY();

	static bool getMouseMoved();
	static bool getMBStateChanged();

};


#endif //_CORE_INPUT
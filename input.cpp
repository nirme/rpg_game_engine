#include "input.h"

Input::Input(HINSTANCE hInstance, HWND hWnd)
{
    DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8W, (void**)&dInput, NULL);

	dInput->CreateDevice(GUID_SysKeyboard, &dInputKeyboard, NULL);

    // set the data format to keyboard format
    dInputKeyboard->SetDataFormat(&c_dfDIKeyboard);

    // set the control you will have over the keyboard
    dInputKeyboard->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND | DISCL_NOWINKEY);


	dInput->CreateDevice(GUID_SysMouse,
                      &dInputMouse,
                      NULL);
    dInputMouse->SetDataFormat(&c_dfDIMouse);

	dInputMouse->SetCooperativeLevel(hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);

	int i=0;
	while (i < 256)
		keyBinds[i] = i++;
}

Input::~Input()
{
	dInputKeyboard->Unacquire();
	dInputMouse->Unacquire();
	dInput->Release();
}

void Input::detect(void)
{
	dInputKeyboard->Acquire();
	dInputKeyboard->GetDeviceState(256, (LPVOID)keyState);
	dInputMouse->Acquire();
	dInputMouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mouseState);
}

BYTE Input::getKeyBind(BYTE num)
{
	return keyBinds[num];
}

void Input::setKeyBind(BYTE num, BYTE bind)
{
	keyBinds[num] = bind;
}

BYTE Input::getKeyState(BYTE num)
{
	return keyState[keyBinds[num]];
}

DIMOUSESTATE Input::getMouseState()
{
	return mouseState;
}
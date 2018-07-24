#include "Core_Input.h"


IDirectInput8*			Input::dInput = NULL;
IDirectInputDevice8*	Input::dInputKeyboard = NULL;
IDirectInputDevice8*	Input::dInputMouse = NULL;
BYTE					Input::keyState[2][256];
DIMOUSESTATE			Input::mouseState[2];
bool					Input::mouseMoved = false;
bool					Input::mouseButtonsStateChanged = false;

int						Input::actualStateIndex = 0;
int						Input::lastStateIndex = 0;

POINT					Input::cursorPosition;
BYTE					Input::keyBinds[256];
float					Input::mouseSpeed = 1.0f;

bool					Input::keyClickedArrActual = false;
BYTE					Input::keysClicked[256];
UINT					Input::keysClickedNumber = 0;



int Input::initialize(HINSTANCE _hInstance, HWND _hWnd)
{
	HRESULT hr = S_OK;
	if (FAILED(hr = DirectInput8Create(_hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8W, (void**)&dInput, NULL)))
	{
		shutdown();
		showError(false, "DirectInput8Create error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -1;
	}

	hr = S_OK;
	if (FAILED(hr = dInput->CreateDevice(GUID_SysKeyboard, &dInputKeyboard, NULL)))
	{
		shutdown();
		showError(false, "IDirectInput8::CreateDevice error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -2;
	}

	hr = S_OK;
	if (FAILED(hr = dInputKeyboard->SetDataFormat(&c_dfDIKeyboard)))
	{
		shutdown();
		showError(false, "IDirectInputDevice8::SetDataFormat error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -3;
	}

	hr = S_OK;
	if (FAILED(hr = dInputKeyboard->SetCooperativeLevel(_hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND | DISCL_NOWINKEY)))
	{
		shutdown();
		showError(false, "IDirectInputDevice8::SetCooperativeLevel error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -4;
	}

	hr = S_OK;
	if (FAILED(hr = dInput->CreateDevice(GUID_SysMouse, &dInputMouse, NULL)))
	{
		shutdown();
		showError(false, "IDirectInput8::CreateDevice error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -5;
	}

	hr = S_OK;
	if (FAILED(hr = dInputMouse->SetDataFormat(&c_dfDIMouse)))
	{
		shutdown();
		showError(false, "IDirectInputDevice8::SetDataFormat error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -6;
	}

	hr = S_OK;
	if (FAILED(hr = dInputMouse->SetCooperativeLevel(_hWnd,  DISCL_NONEXCLUSIVE | DISCL_FOREGROUND)))
	{
		shutdown();
		showError(false, "IDirectInputDevice8::SetCooperativeLevel error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -7;
	}

	hr = S_OK;
	if (FAILED(hr = dInputKeyboard->Acquire()))
	{
		shutdown();
		showError(false, "IDirectInputDevice8::Acquire error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -8;
	}

	hr = S_OK;
	if (FAILED(hr = dInputMouse->Acquire()))
	{
		shutdown();
		showError(false, "IDirectInputDevice8::Acquire error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -9;
	}

	for(int i = 0; i < 256; ++i)
		keyBinds[i] = i;

	ZeroMemory(&(keyState[0][0]), 256);
	ZeroMemory(&(keyState[1][0]), 256);
	ZeroMemory(&(mouseState[0]), 2 * sizeof(DIMOUSESTATE));

	actualStateIndex = 1;
	lastStateIndex = 0;

	ZeroMemory(&cursorPosition, sizeof(POINT));


	keyClickedArrActual = false;
	ZeraMemory(keysClicked, sizeof(BYTE) * 256);
	keysClickedNumber = 0;


	mouseSpeed = 1.0f;
	return 0;
};


int Input::shutdown()
{
	HRESULT hr = S_OK;
	if (dInputKeyboard)
	{
		if (FAILED(hr = dInputKeyboard->Unacquire()))
			showError(false, "IDirectInputDevice8::Unacquire error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		dInputKeyboard = NULL;
	}

	hr = S_OK;
	if (dInputMouse)
	{
		if (FAILED(hr = dInputMouse->Unacquire()))
			showError(false, "IDirectInputDevice8::Unacquire error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		dInputMouse = NULL;
	}

	hr = S_OK;
	if (dInput)
	{
		if (FAILED(hr = dInput->Release()))
			showError(false, "IDirectInput8::Release error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		dInput = NULL;
	}

	return 0;
};


DWORD Input::update()
{
	int tmp = actualStateIndex;
	actualStateIndex = lastStateIndex;
	lastStateIndex = tmp;

	DWORD out = 0;
	HRESULT hr = S_OK;
	if (FAILED(hr = dInputKeyboard->GetDeviceState(256, (void*)(keyState[actualStateIndex]))))
	{
		showError(false, "IDirectInputDevice8::GetDeviceState error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		out += 0x00000001;
	}

	hr = S_OK;
	if (FAILED(hr = dInputMouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&(mouseState[actualStateIndex]))))
	{
		showError(false, "IDirectInputDevice8::GetDeviceState error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		out += 0x00000002;
	}

	if (!GetCursorPos(&cursorPosition))
	{
		DWORD err = GetLastError();
		char* text = NULL;
		FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, 0, text, 1, NULL);
		showError(false, "GetCursorPos error: %d error description: %s\n", err, text);
		LocalFree(text);
		out += 0x00000004;
	}

	mouseMoved = mouseState[actualStateIndex].lX ? true : false;
	mouseButtonsStateChanged = ((*(reinterpret_cast <DWORD*> (mouseState[actualStateIndex].rgbButtons))) != 
								(*(reinterpret_cast <DWORD*> (mouseState[lastStateIndex].rgbButtons))) ? 
								true : false;

	keyClickedArrActual = false;

	return out;
};


BYTE Input::getKeyBind(BYTE _num)
{
	return keyBinds[_num];
};


void Input::setKeyBind(BYTE _num, BYTE _bind)
{
	keyBinds[_num] = _bind;
};


BYTE Input::getKeyState(BYTE _num)
{
	return keyState[actualStateIndex][keyBinds[_num]];
};


bool Input::getKeyClicked(BYTE _num)
{
	if (keyState[actualStateIndex][keyBinds[_num]] && !keyState[lastStateIndex][keyBinds[_num]])
		return true;
	return false;
};


const BYTE* Input::getKeysStates()
{
	return keyState[actualStateIndex];
};


const BYTE* Input::getKeysClicked(BYTE* _keysNumber)
{
	if (!keyClickedArrActual)
	{
		keysClickedNumber = 0;

		for (UINT i = 0; i < 256; ++i)
			if (keyState[actualStateIndex][keyBinds[i]] && !keyState[lastStateIndex][keyBinds[i]])
				keysClicked[keysClickedNumber++] = keyBinds[i];

		keyClickedArrActual = true;
	}

	*_keysNumber = keysClickedNumber;
	return keysClicked;
};


DIMOUSESTATE Input::getMouseState()
{
	return mouseState[actualStateIndex];
};


float Input::getMouseSpeed()
{
	return mouseSpeed;
};


float Input::setMouseSpeed(float _speed)
{
	return (mouseSpeed = _speed);
};


long Input::getMouseX()
{
	return mouseState[actualStateIndex].lX;
};


long Input::getMouseY()
{
	return mouseState[actualStateIndex].lY;
};


long Input::getMouseZ()
{
	return mouseState[actualStateIndex].lZ;
};


float Input::getMouseScaledX()
{
	return (mouseSpeed * mouseState[actualStateIndex].lX);
};


float Input::getMouseScaledY()
{
	return (mouseSpeed * mouseState[actualStateIndex].lY);
};


float Input::getMouseScaledZ()
{
	return (mouseSpeed * mouseState[actualStateIndex].lZ);
};


BYTE Input::getMouseButton1State()
{
	return mouseState[actualStateIndex].rgbButtons[0];
};


bool Input::getMouseButton1Clicked()
{
	if (mouseState[actualStateIndex].rgbButtons[0] && !mouseState[lastStateIndex].rgbButtons[0])
		return true;
	return false;
};


BYTE Input::getMouseButton2State()
{
	return mouseState[actualStateIndex].rgbButtons[1];
};


bool Input::getMouseButton2Clicked()
{
	if (mouseState[actualStateIndex].rgbButtons[1] && !mouseState[lastStateIndex].rgbButtons[1])
		return true;
	return false;
};


BYTE Input::getMouseButton3State()
{
	return mouseState[actualStateIndex].rgbButtons[2];
};


bool Input::getMouseButton3Clicked()
{
	if (mouseState[actualStateIndex].rgbButtons[2] && !mouseState[lastStateIndex].rgbButtons[2])
		return true;
	return false;
};


BYTE Input::getMouseButton4State()
{
	return mouseState[actualStateIndex].rgbButtons[3];
};


bool Input::getMouseButton4Clicked()
{
	if (mouseState[actualStateIndex].rgbButtons[3] && !mouseState[lastStateIndex].rgbButtons[3])
		return true;
	return false;
};


POINT Input::getCursorPosition()
{
	return cursorPosition;
};


short Input::getCursorPositionX()
{
	return cursorPosition.x;
};	


short Input::getCursorPositionY()
{
	return cursorPosition.y;
};	


bool Input::getMouseMoved()
{
	return mouseMoved;
};


bool Input::getMBStateChanged()
{
	return mouseButtonsStateChanged;
};


#pragma once

#ifndef _CORE_APPLICATION
#define _CORE_APPLICATION

#include "utility.h"
#include "Core_Defines.h"


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

class Application
{
private:
	HINSTANCE hInstance;
	HWND hWnd;

protected:
	wchar_t className[MAX_PATH];
	wchar_t windowName[MAX_PATH]; 

	WNDCLASSEX wndClassEx;
	UINT wWidth;
	UINT wHeight;

public:
	Application();
	virtual ~Application() {};

	HWND gethWnd();
	HINSTANCE gethInst();

	int resize(UINT _wHeight, UINT _wWidth)
	{
		wWidth = _wWidth;
		wHeight = _wHeight;
	};

	int run();

	//int Resize(UINT _width, UINT _height);
	virtual int showMouse(bool _show = false);
		//{ return 0; };

	virtual int initialize()
		{ return 0; };
	virtual int shutdown()
		{ return 0; };
	virtual int frame()
		{ return 0; };
};


#endif //_CORE_APPLICATION
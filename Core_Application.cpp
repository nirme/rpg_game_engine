#include "Core_Application.h"


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg )
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	#ifdef _DX_MOUSE_USAGE
		case WM_SETCURSOR:
			return TRUE;
	#endif //_DX_MOUSE_USAGE

	#ifdef _DEBUG
		case WM_KEYDOWN:
			if( wParam == VK_ESCAPE )
				DestroyWindow(hwnd);

			break;
	#endif //_DEBUG
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
};


Application::Application()
{
	hInstance = GetModuleHandle(NULL);
	hWnd = 0;

	wcscpy(className, CLASS_NAME);
	wcscpy(windowName, WINDOW_NAME);

	wWidth = 320;
	wHeight = 240;

	ZeroMemory(&wndClassEx, sizeof(WNDCLASSEX));
	wndClassEx.cbSize =			sizeof(WNDCLASSEX);
	wndClassEx.style =			CS_HREDRAW | CS_VREDRAW;
	wndClassEx.lpfnWndProc =	WndProc;
//	wndClassEx.cbClsExtra =		NULL;
//	wndClassEx.cbWndExtra =		NULL;
	wndClassEx.hInstance =		hInstance;
//	wndClassEx.hIcon =			LoadIcon(0, IDI_APPLICATION);
	wndClassEx.hCursor =		LoadCursor(0, IDC_ARROW);
//	wndClassEx.hbrBackground =	(HBRUSH)GetStockObject(WHITE_BRUSH);
//	wndClassEx.lpszMenuName =	NULL;
	wndClassEx.lpszClassName =	CLASS_NAME;
//	wndClassEx.hIconSm =		LoadIcon(0, IDI_APPLICATION);
}

HWND Application::gethWnd()
{
	return hWnd;
}
HINSTANCE Application::gethInst()
{
	return hInstance;
}

int Application::run()
{
	RegisterClassEx(&wndClassEx);

	hWnd = CreateWindowEx(	NULL,
							CLASS_NAME,
							WINDOW_NAME,
							WS_EX_TOPMOST | WS_POPUP,
							0, 0,
							wWidth, wHeight,// set window to new resolution
							NULL,
							NULL,
							hInstance,
							NULL);
	if (!hWnd)
	{
		DWORD err = GetLastError();

		showError(true, L"RegisterClassEx() failed!");
		return 0;
	}

	ShowWindow(hWnd, SW_SHOW);

	UpdateWindow(hWnd);

	CoInitializeEx(NULL,COINIT_MULTITHREADED);

	SetCursor(NULL);

	if (initialize())
	{
		showError(true, L"Initializing application failed!");
		shutdown();
		return 0;
	}


	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while(msg.message != WM_QUIT)
	{

		if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if (frame())
				break;
		}
	}

	shutdown();

	CoUninitialize();

	UnregisterClass(className, hInstance);

	return 0;
}

int Application::showMouse(bool _show)
{
	ShowCursor(_show);
//	SetCursorPos(wWidth/2, wHeight/2);	/*only for fullscreen apps*/
	return 0;
}

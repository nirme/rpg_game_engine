//#include "utility.h"

const wchar_t className[] = L"Direct3D9App";
const wchar_t windowName[] = L"Direct3D9App";

#include "app.h"


int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
	srand(time(NULL));
	ActualApp app;
	app.run();

	D3DCOLOR_ARGB
//	system("PAUSE");
	return 0;
}

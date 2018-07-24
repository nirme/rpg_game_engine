#pragma once

#ifndef _CORE_GRAPHIC
#define _CORE_GRAPHIC

#include "utility.h"
#include "funky_utils.h"
#include "Core_Logger.h"
#include "Core_Variable_Library.h"
#include "Core_Defines.h"

#include "Base_Exceptions.h"

#include "Core_Renderer.h"



class Graphic
{
public:

	struct DeviceState
	{
		bool d3d9Initialized;
		bool deviceInitialized;
		bool vsync;
		bool fullscreen;
		UINT displayModeIndex;
		D3DDISPLAYMODE displayMode;
		D3DFORMAT backbufferFormat;

		UINT frames;
		double lastUpdateTime;
		float FPS;

		DeviceState() : d3d9Initialized(false), deviceInitialized(false), vsync(false), fullscreen(true), displayModeIndex(0), backbufferFormat(D3DFMT_UNKNOWN), frames(0), lastUpdateTime(0.0), FPS(0.0f)
		{
			displayMode.Width = 0;
			displayMode.Height = 0;
			displayMode.RefreshRate = 0;
			displayMode.Format = D3DFMT_UNKNOWN;
		};
	};

	struct Cursor
	{
		string name;
		IDirect3DSurface9* pSurface;
	};


protected:

	static bool initialized;

	static HWND hWnd;
	static IDirect3D9* pD3D9;
	static IDirect3DDevice9* pDevice;
	static D3DCAPS9 pDeviceCaps;
	static IDirect3DSwapChain9* pSwapChain;

	static bool deviceDisplayModesLoaded;
	static vector<D3DDISPLAYMODE> deviceDisplayModes;

	static DeviceState deviceState;
	static Renderer* renderer;

	typedef map< string, IDirect3DSurface9* > CursorsMap;
	static CursorsMap m_cursors;

	static int actualCursorIndex;
	static bool cursorVisible;


	static int loadDisplayModes();

	Graphic();


public:


	static void loadDisplayModes();
	static int getDisplayModesNumber();
	const static D3DDISPLAYMODE* getDisplayModesPointer();

	static void getDisplayMode( UINT index, D3DDISPLAYMODE* _dispMode );
	static void getActualDisplayMode( D3DDISPLAYMODE* _dispMode );

	static void initialize( HWND _hWnd, UINT _displayModeIndex, bool _vsync, bool _fullscreen );
	static void shutdown();

	static void updateTime( double timeDelta );

	static const D3DCAPS9* getDeviceCaps();

	static int presentFrame( bool _waitToDraw );

	static void addCursor( const string &_cursorName, IDirect3DSurface9* _cursorSurface );
	static void setCursor( const string &_cursorName );
	static void showCursor( bool _show, bool _centerCursor );

	inline static float getFPS()					{	return deviceState.FPS;		};
	inline static bool isCursorVisible()			{	return cursorVisible;		};

	inline static IDirect3DDevice9* getDevice()		{	return pDevice;				};
	inline static IDirect3D9* getIDirect3D9()		{	return pD3D9;				};


};


#endif //_CORE_GRAPHIC
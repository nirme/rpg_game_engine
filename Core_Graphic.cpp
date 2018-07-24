#include "Core_Graphic.h"



bool						Graphic::initialized = false;
HWND						Graphic::hWnd = 0;
IDirect3D9*					Graphic::pD3D9 = NULL;
IDirect3DDevice9*			Graphic::pDevice = NULL;
D3DCAPS9					Graphic::pDeviceCaps;
IDirect3DSwapChain9*		Graphic::pSwapChain = NULL;
bool						Graphic::deviceDisplayModesLoaded = false;
vector<D3DDISPLAYMODE>		Graphic::deviceDisplayModes;
DeviceState					Graphic::deviceState;
Renderer*					Graphic::renderer = NULL;
CursorsMap					Graphic::m_cursors;
int							Graphic::actualCursorIndex = 0;
bool						Graphic::cursorVisible = false;




void Graphic::loadDisplayModes()
{
	if (deviceDisplayModesLoaded)
		return;

	bool d3d9InitFlag = false;
	if (!deviceState.d3d9Initialized)
	{
		pD3D9 = Direct3DCreate9(D3D_SDK_VERSION);
		if (pD3D9 == NULL)
			throw D3DX9Exception( "Graphic::loadDisplayModes", "Direct3DCreate9", E_FAIL );

		deviceState.d3d9Initialized = true;
		d3d9InitFlag = true;
	}

	UINT x32size = pD3D9->GetAdapterModeCount(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8);
	UINT x16size = pD3D9->GetAdapterModeCount(D3DADAPTER_DEFAULT, D3DFMT_R5G6B5);
	UINT modeCount = x32size + x16size;
	if (modeCount == 0)
	{
		if (d3d9InitFlag)
		{
			IDX_SAFE_RELEASE(pD3D9);
			deviceState.d3d9Initialized = false;
		}

		throw BaseException( "No valid display mode supported by graphic device." );
	}
	deviceDisplayModes.resize(x32size + x16size);

	for(UINT i=0;i<x32size;++i)
		pD3D9->EnumAdapterModes(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8, i, &(deviceDisplayModes[i]));
	for(UINT i=0;i<x16size;++i)
		pD3D9->EnumAdapterModes(D3DADAPTER_DEFAULT, D3DFMT_R5G6B5, i, &(deviceDisplayModes[x32size + i]));

	bool sortFlag = true;
	while (sortFlag)
	{
		sortFlag = false;

		for(UINT i=0;i<modeCount-1;++i)
		{
			if (deviceDisplayModes[i] < deviceDisplayModes[i+1])
			{
				D3DDISPLAYMODE dm = deviceDisplayModes[i];
				deviceDisplayModes[i] = deviceDisplayModes[i+1];
				deviceDisplayModes[i+1] = dm;
				sortFlag = true;
			}
		}
	}

	deviceDisplayModesLoaded = true;
	if (d3d9InitFlag)
	{
		IDX_SAFE_RELEASE(pD3D9);
		deviceState.d3d9Initialized = false;
	}
};


int Graphic::getDisplayModesNumber()
{
	loadDisplayModes();
	return deviceDisplayModes.size();
};


const D3DDISPLAYMODE* Graphic::getDisplayModesPointer()
{
	loadDisplayModes();
	return &( deviceDisplayModes[0] );
};


void Graphic::getDisplayMode( UINT index, D3DDISPLAYMODE* _dispMode )
{
	loadDisplayModes();
	*_dispMode = deviceDisplayModes[index];
};


void Graphic::getActualDisplayMode( D3DDISPLAYMODE* _dispMode )
{
	if ( !deviceState.deviceInitialized )
		*_dispMode = deviceState.displayMode;
	else
		ZeroMemory( _dispMode, sizeof(D3DDISPLAYMODE) );
};


void Graphic::initialize( HWND _hWnd, UINT _displayModeIndex, bool _vsync, bool _fullscreen )
{
	if ( initialized )
		return;

	if ( _hWnd == NULL )
	{
		throw BaseException( "Specified window handle is empty." );
		return;
	}
	hWnd = _hWnd;

	if ( !deviceState.d3d9Initialized )
	{
		pD3D9 = Direct3DCreate9(D3D_SDK_VERSION);
		if (pD3D9 == NULL)
		{
			throw D3DX9Exception( "Graphic::initialize", "Direct3DCreate9", E_FAIL );
			return;
		}
		deviceState.d3d9Initialized = true;
	}

	HRESULT hr = S_OK;
	if ( FAILED( hr = pD3D9->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &pDeviceCaps ) ) )
	{
		IDX_SAFE_RELEASE( pD3D9 );
		throw D3DX9Exception( "Graphic::initialize", "IDirect3D9::GetDeviceCaps", hr );
		return;
	}


	if ( pDeviceCaps.VertexShaderVersion < D3DVS_VERSION(3,0) )
	{
		IDX_SAFE_RELEASE( pD3D9 );
		deviceState.d3d9Initialized = false;
		throw BaseException( "Vertex Shader version too low. Vertex Shader 3.0 required." );
		return;
	}

	if ( pDeviceCaps.PixelShaderVersion < D3DPS_VERSION(3,0) )
	{
		IDX_SAFE_RELEASE( pD3D9 );
		deviceState.d3d9Initialized = false;
		throw BaseException( "Pixel Shader version too low. Pixel Shader 3.0 required.");
		return;
	}


	loadDisplayModes();

	if ( _displayModeIndex >= deviceDisplayModes.size() )
		_displayModeIndex = 0;

	deviceState.displayModeIndex = _displayModeIndex;
	deviceState.displayMode = deviceDisplayModes[_displayModeIndex];

	UINT refreshRate = _fullscreen ? deviceState.displayMode.RefreshRate : 0;

	deviceState.fullscreen = _fullscreen;
	deviceState.vsync = _vsync;


	deviceState.backbufferFormat = deviceState.displayMode.Format == D3DFMT_X8R8G8B8 ? D3DFMT_A8R8G8B8 : D3DFMT_A1R5G5B5;

	D3DPRESENT_PARAMETERS d3dpp;
	{
		d3dpp.BackBufferWidth				= deviceState.displayMode.Width;
		d3dpp.BackBufferHeight				= deviceState.displayMode.Height;
		d3dpp.BackBufferFormat				= deviceState.backbufferFormat;
		d3dpp.BackBufferCount				= 1;
		d3dpp.MultiSampleType				= D3DMULTISAMPLE_NONE;
		d3dpp.MultiSampleQuality			= 0;
		d3dpp.SwapEffect					= D3DSWAPEFFECT_DISCARD; 
		d3dpp.hDeviceWindow					= hWnd;
		d3dpp.Windowed						= !deviceState.fullscreen;
		d3dpp.EnableAutoDepthStencil		= true; 
		d3dpp.AutoDepthStencilFormat		= D3DFMT_D24S8;
		d3dpp.Flags							= 0;
		d3dpp.FullScreen_RefreshRateInHz	= refreshRate;
		d3dpp.PresentationInterval			= deviceState.vsync ? D3DPRESENT_DONOTWAIT | D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;
	}


	if (FAILED(hr = pD3D9->CreateDevice(	D3DADAPTER_DEFAULT,
											D3DDEVTYPE_HAL,
											hWnd,
											D3DCREATE_MIXED_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
											&d3dpp,
											&pDevice)))
	{
		IDX_SAFE_RELEASE(pD3D9);
		deviceState.d3d9Initialized = false;

		throw ( hr == E_OUTOFMEMORY ) ? bad_alloc() : D3DX9Exception( "Graphic::initialize", "IDirect3D9::CreateDevice", hr );
		return;
	}

	deviceState.deviceInitialized = true;
	loadDeviceStates();


	if ( FAILED( hr = pDevice->GetSwapChain( 0, &pSwapChain ) ) )
	{
		IDX_SAFE_RELEASE( pDevice );
		deviceState.deviceInitialized = false;
		IDX_SAFE_RELEASE( pD3D9 );
		deviceState.d3d9Initialized = false;

		throw ( hr == E_OUTOFMEMORY ) ? bad_alloc() : D3DX9Exception( "Graphic::initialize", "IDirect3DDevice9::GetSwapChain", hr );
		return;
	}


	IDirect3DSurface9* cursor;
	if ( FAILED( hr = pDevice->CreateOffscreenPlainSurface( 32, 32, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &cursor, NULL ) ) )
	{
		IDX_SAFE_RELEASE( pSwapChain );
		IDX_SAFE_RELEASE( pDevice );
		deviceState.deviceInitialized = false;
		IDX_SAFE_RELEASE( pD3D9 );
		deviceState.d3d9Initialized = false;

		throw ( hr == E_OUTOFMEMORY ) ? bad_alloc() : D3DX9Exception( "Graphic::initialize", "IDirect3DDevice9::CreateOffscreenPlainSurface", hr );
		return;
	}

	{
		D3DLOCKED_RECT cursorLockedRect;
		RECT cursorRect;
		SetRect( &cursorRect, 0, 0, 20, 20 );
		cursor->LockRect( &cursorLockedRect, &cursorRect, 0 );
		fillCursorArrayWhite( (UINT*) cursorLockedRect.pBits );
		cursor->UnlockRect();
	}

	m_cursors.insert( CursorsMap::value_type( string(""), cursor );
	actualCursorIndex = 0;

	if ( FAILED( hr = pDevice->SetCursorProperties( 0, 0, cursor ) ) )
	{
		IDX_SAFE_RELEASE( cursor );
		IDX_SAFE_RELEASE( pSwapChain );
		IDX_SAFE_RELEASE( pDevice );
		deviceState.deviceInitialized = false;
		IDX_SAFE_RELEASE( pD3D9 );
		deviceState.d3d9Initialized = false;

		throw D3DX9Exception( "Graphic::initialize", "IDirect3DDevice9::SetCursorProperties", hr );
		return;
	}

	pDevice->ShowCursor( false );
	cursorVisible = false;

	deviceState.frames = 0;
	deviceState.lastUpdateTime = 0.0f;
	deviceState.FPS = 0.0f;
	deviceState.renderingState = 0;

	initialized = true;

	return;
};


void Graphic::shutdown()
{
	if ( !initialized )
		return;


	pDevice->ShowCursor(false);
	pDevice->SetCursorProperties(0, 0, NULL);
	cursorVisible = false;
	actualCursorIndex = -1;

	CursorsMap::iterator it = m_cursors.begin();
	CursorsMap::iterator it_end = m_cursors.end();

	for ( it; it != it_end; ++it )
		IDX_SAFE_RELEASE( (*it).second );

	m_cursors.clear();


	delete renderer;

	deviceDisplayModes.clear();
	deviceDisplayModesLoaded = false;

	IDX_SAFE_RELEASE( pSwapChain );

	ZeroMemory( pDeviceCaps, sizeof(D3DCAPS9) );


	IDX_SAFE_RELEASE( pDevice );
	deviceState.deviceInitialized = false;

	IDX_SAFE_RELEASE( pD3D9 );
	deviceState.d3d9Initialized = false;

	deviceState.vsync = false;
	deviceState.fullscreen = true;
	deviceState.displayModeIndex = 0;
	ZeroMemory( &deviceState.displayMode, sizeof(D3DDISPLAYMODE) );
	ZeroMemory( &deviceState.backbufferFormat, sizeof(D3DFORMAT) );
	deviceState.frames = 0;
	deviceState.lastUpdateTime = 0.0;
	deviceState.FPS = 0.0f;

	hWnd = NULL;


	initialized = false;
};

void Graphic::updateTime( double timeDelta )
{
	deviceState.lastUpdateTime += timeDelta;
	deviceState.frames++;
	if (deviceState.lastUpdateTime >= 1.0f)
	{
		deviceState.FPS = float(deviceState.frames) / deviceState.lastUpdateTime;
		deviceState.frames = 0;
		deviceState.lastUpdateTime = 0.0f;
	}
};

const D3DCAPS9* Graphic::getDeviceCaps()
{
	return deviceState.deviceInitialized ? &pDeviceCaps : NULL;
};


int Graphic::presentFrame( bool _waitToDraw )
{
	if (!deviceState.deviceInitialized)
	{
		throw BaseException( "Graphic not initialized." );
		return 0;
	}

	HRESULT hr = S_OK;

	if (deviceState.vsync)
	{
		if ( ( hr = pSwapChain->Present(0,0,0,0, _waitToDraw ? 0 : D3DPRESENT_DONOTWAIT) ) == D3DERR_WASSTILLDRAWING )
			return 1;

		if ( FAILED(hr) )
			throw D3DX9Exception( "Graphic::presentFrame", "IDirect3DSwapChain9::Present", hr );

		return 0;
	}

	if (FAILED(hr = pDevice->Present(0,0,0,0)))
		throw D3DX9Exception( "Graphic::presentFrame", "IDirect3DDevice9::Present", hr );

	return 0;
};


void Graphic::addCursor( const string &_cursorName, IDirect3DSurface9* _cursorSurface )
{
	if (!deviceState.deviceInitialized)
	{
		throw BaseException( "Graphic not initialized." );
		return 0;
	}

	D3DSURFACE_DESC desc;
	_cursorSurface->GetDesc(&desc);
	if ( desc.Format != D3DFMT_A8R8G8B8 )
	{
		throw BaseException( "Cursor surface not acceptable." );
		return;
	}

	CursorsMap::iterator it = m_cursors.find( _cursorName );

	if ( it == m_cursors.end() )
		m_cursors.insert( CursorsMap::value_type( _cursorName, _cursorSurface ) );
	else
	{
		IDX_SAFE_RELEASE( it->second );
		it->second = _cursorSurface
	}
};


void Graphic::setCursor( const string &_cursorName )
{
	CursorsMap::iterator it = m_cursors.find( _cursorName );

	IDirect3DSurface9* tmp = ( it != m_cursors.end() ) ? it->second : NULL;
	HRESULT hr = S_OK;

	if ( FAILED( ( hr = pDevice->SetCursorProperties( 0, 0, tmp ) ) )
		throw D3DX9Exception( "Graphic::setCursor", "IDirect3DDevice9::SetCursorProperties", hr );
};


void Graphic::showCursor( bool _show, bool _centerCursor )
{
	HRESULT hr = S_OK;

	if ( _centerCursor && FAILED( hr = pDevice->SetCursorPosition( deviceState.displayMode.Width / 2, deviceState.displayMode.Height / 2, 0 ) ) )
		throw D3DX9Exception( "Graphic::showCursor", "IDirect3DDevice9::SetCursorPosition", hr );

	if ( FAILED( hr = pDevice->ShowCursor( (BOOL)( cursorVisible = _show ) ) ) )
		throw D3DX9Exception( "Graphic::showCursor", "IDirect3DDevice9::ShowCursor", hr );
};


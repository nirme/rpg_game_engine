#pragma once

#ifndef _UTILITY
#define _UTILITY

//turn off VS warnings about strings security etc...
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS
#pragma warning(disable : 4018)
#pragma warning(disable : 4101)
#pragma warning(disable : 4244)
#pragma warning(disable : 4307)
#pragma warning(disable : 4995)



#include "Core_Defines.h"

//
#include <windows.h>

//BOOST threading libs
//include before std libs or it won't compile
#include <boost/thread.hpp>

//Direct3D 9 with helper api
#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9math.h>
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")
#pragma comment (lib, "dxguid.lib")
#include <dxerr.h>
#pragma comment (lib, "dxerr.lib")


#include <exception>

//standard libs
#include <time.h>
#include <tchar.h> 
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <sstream>
#include <stdarg.h>
#include <strsafe.h>
#include <math.h>
//#include <wchar.h>



//standard conteners
#include <bitset>
#include <vector>
#include <list>
#include <queue>
#include <map>

//required for timeGetTime()
#pragma comment (lib, "winmm.lib")

//DirectX8 Input
#define DIRECTINPUT_VERSION  0x0800
#include <dinput.h>
#pragma comment (lib, "dinput8.lib")

//SQLite3
#include "binaries/sqlite3/sqlite3.h"
#pragma comment(lib, "binaries/sqlite3/sqlite3.lib")

//LUA Script
#include "binaries/lua51/lua.h"
#include "binaries/lua51/lauxlib.h"
#include "binaries/lua51/lualib.h"
#pragma comment(lib, "binaries/lua51/lua5.1.lib")

//FMOD api
#include "binaries/FMOD/inc/fmod.hpp"
#include "binaries/FMOD/inc/fmod_errors.h"

#ifdef _DEBUG
	#pragma comment (lib, "binaries/FMOD/lib/fmodexL_vc.lib")
#else
	#pragma comment (lib, "binaries/FMOD/lib/fmodex_vc.lib")
#endif



#define SAFE_DELETE(p)			{ if (p) { delete (p);		(p)=NULL; } }
#define SAFE_DELETE_ARRAY(p)	{ if (p) { delete[] (p);	(p)=NULL; } }
#define SAFE_RELEASE(p)			{ if (p) { (p)->release();	(p)=NULL; } }
#define IDX_SAFE_RELEASE(p)		{ if (p) { (p)->Release();	(p)=NULL; } }

#define FMOD_FAILED(fr) (((FMOD_RESULT)(fr)) > FMOD_OK)


using namespace std;



#define ErrorMsg(text) MessageBox(0, (text), 0, 0)
//show error and post quit msg for main api
int showError(bool isCritical, wchar_t* _text, ...);
int showError(bool isCritical, char* _text, ...);

float getAbsoluteDistance(D3DXVECTOR3* point, D3DXVECTOR3* zero = NULL);

HRESULT setStringA(char** _Dest, const char* _Source = NULL, UINT maxSize = 0);
HRESULT setStringW(wchar_t** _Dest, const wchar_t* _Source = NULL, UINT maxSize = 0);


string trim( char* _str, int _strLength = 0 );
wstring trim( wchar_t* _str, int _strLength = 0 );

string trim( string &_str );
wstring trim( wstring &_str );


#ifdef UNICODE
#define setString  setStringW
#else
#define setString  setStringA
#endif // !UNICODE




bool compare(float _a, float _b, float _epsilon = EPSILON);
bool compare(double _a, double _b, double _epsilon = EPSILON);



struct TypeInfoCmp
{
	bool operator()(const type_info* a, const type_info* b)
	{
		return a->before(*b) ? true : false;
	};
};



//--------------------------------------------------------------------------------------
// Allows STL Map containers to use case-insensitive string comparisons
//--------------------------------------------------------------------------------------
template <typename T> class compare_case_insensitive
{
protected:
	static bool compare( typename T::value_type c1, typename T::value_type c2 )
	{
		return tolower( c1 ) < tolower( c2 );
	}
public:
	bool operator()( const T& s1, const T& s2 ) const
	{
		return lexicographical_compare( s1.begin(), s1.end(),
										s2.begin(), s2.end(),
										compare );
	}
};

typedef compare_case_insensitive <wstring>  wcompare;
typedef compare_case_insensitive <string>   scompare;


template <typename T> class p_compare_case_insensitive
{
protected:
	static bool compare( typename T::value_type c1, typename T::value_type c2 )
	{
		return tolower( c1 ) < tolower( c2 );
	}
public:
	bool operator()( const T* s1, const T* s2 ) const
	{
		return lexicographical_compare( s1->begin(), s1->end(),
										s2->begin(), s2->end(),
										compare );
	}
};

typedef		p_compare_case_insensitive <wstring>	p_wcompare;
typedef		p_compare_case_insensitive <string>		p_scompare;


bool compareCaseInsensitive( const char* _str1, const char* _str2 );


//compare display modes - return true if tested is higher/better than actual
bool operator >(const D3DDISPLAYMODE& a, const D3DDISPLAYMODE& b);
bool operator <(const D3DDISPLAYMODE& a, const D3DDISPLAYMODE& b);
bool operator ==(const D3DDISPLAYMODE& a, const D3DDISPLAYMODE& b);


string tolower(string str);

wstring stringToWstring(const string &_str);


inline float saturate( float _val )
{
	return (_val > 1.0f ? 1.0f : (_val < 0.0f ? 0.0f : _val));
};

inline float clip( float _val, float _lowerLimit, float _upperLimit )
{
	return (_val > _upperLimit ? _upperLimit : (_val < _lowerLimit ? _lowerLimit : _val));
};

float ComputeGaussianFalloffCurve( float _n, float _theta = 4.0f );


D3DXMATRIX* D3DXMatrixTransformation( D3DXMATRIX* _pOut, const D3DXVECTOR3* _pScaling, const D3DXQUATERNION* _pRotation, const D3DXVECTOR3* _pTranslation );
D3DXMATRIX* D3DXMatrixTransformation( D3DXMATRIX* _pOut, float _scaling, const D3DXQUATERNION* _pRotation, const D3DXVECTOR3* _pTranslation );
D3DXMATRIX* D3DXMatrixTransformation( D3DXMATRIX* _pOut, const D3DXQUATERNION* _pRotation, const D3DXVECTOR3* _pTranslation );



#endif //_UTILITY

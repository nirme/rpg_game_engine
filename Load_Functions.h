#pragma once

#ifndef _CORE_LOAD_FUNCTIONS
#define _CORE_LOAD_FUNCTIONS

////////    TO REMOVE

#include "utility.h"

#include "Core_Defines.h"
#include "Core_Logger.h"
#include "Core_Variable_Library.h"
#include "Core_Graphic.h"

#include "Core_Graphic_Resource.h"
#include "Core_Light.h"
#include "Core_Graphic_Object.h"
#include "Core_SoundSystem.h"

#include "Base_Node.h"

#include "Effect_Include_Menager.h"


typedef int (*ResourceLoadFunction)			(sqlite3*, void*);
typedef int (*NodeLoadFunction)				(BaseNode* /*parent*/, string /*node name*/, const void* /*data pointer*/, UINT /*data size*/, BaseNode** /*loaded node*/);



//	HELPER FUNCTIONS
int setupBonesHierarchyFromDataBlock(	const BYTE* _dataBlock,
										DWORD _dataBlockLength,
										ID3DXSkinInfo* _pSkinInfo,
										D3DXMATRIX* _pmBoneOffsets,
										D3DXFRAME** _rootFrameOut,
										DWORD* boneId = NULL);

int setupAnimationSetsFromBlock(	const BYTE* _dataBlock,
									DWORD _dataBlockLength,
									ID3DXAnimationController* _pAnimController);

int setupAnimationsFromBlock(	const BYTE* _dataBlock,
								DWORD _dataBlockLength,
								DWORD _animationsNumber,
								ID3DXKeyframedAnimationSet* _animationSet);

int setupCallbacksFromBlock(	const BYTE* _dataBlock,
								DWORD _dataBlockLength,
								DWORD _callbackKeysNumber,
								D3DXKEY_CALLBACK** _callbackKeys);



//	LOADER FUNCTIONS

int ResourceLoadTexture			(sqlite3* _db, void* _ptr, void* _aData = NULL);
int ResourceLoadMesh			(sqlite3* _db, void* _ptr, void* _aData = NULL);
int ResourceLoadEffect			(sqlite3* _db, void* _ptr, void* _aData = NULL);

int ResourceLoadLight			(sqlite3* _db, void* _ptr, void* _aData = NULL);
int ResourceLoadSound			(sqlite3* _db, void* _ptr, void* _aData = NULL);


int ResourceLoadBitmapFont		(sqlite3* _db, void* _ptr, void* _aData = NULL);


int ResourceLoadParticleSystem	(sqlite3* _db, void* _ptr, void* _aData = NULL);



int ResourceLoadGraphicObject	(sqlite3* _db, void* _ptr, void* _aData = NULL);






#endif //_CORE_LOAD_FUNCTIONS

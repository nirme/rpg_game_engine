#pragma once

#ifndef _LOAD_HELPER_FUNCTIONS
#define _LOAD_HELPER_FUNCTIONS

#include "utility.h"

#include "Core_Defines.h"
#include "Core_Logger.h"
#include "Core_Variable_Library.h"



class __declspec(novtable) UserDefinedCallbackSetup
{
public:
	int setupCallbacksFromBlock(const BYTE* _dataBlock, DWORD _dataBlockLength, DWORD _callbackKeysNumber, D3DXKEY_CALLBACK** _callbackKeys) = 0;
};



int setupBonesHierarchyFromDataBlock(	const BYTE* _dataBlock,
										DWORD _dataBlockLength,
										ID3DXSkinInfo* _pSkinInfo,
										D3DXMATRIX* _pmBoneOffsets,
										D3DXFRAME** _rootFrameOut,
										DWORD* boneId = NULL);

int setupAnimationSetsFromBlock(	const BYTE* _dataBlock, 
									DWORD _dataBlockLength, 
									ID3DXAnimationController* _pAnimController, 
									UserDefinedCallbackSetup* _callbackSetup = NULL);

int setupAnimationsFromBlock(	const BYTE* _dataBlock,
								DWORD _dataBlockLength,
								DWORD _animationsNumber,
								ID3DXKeyframedAnimationSet* _animationSet);





int setupParticleProcessor(	const BYTE* _dataBlock,
							DWORD _dataBlockLength,
							UserDefinedParticleProcessor** _particleProcessor);


#endif //_LOAD_HELPER_FUNCTIONS

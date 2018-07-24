#include "Load_Helper_Functions.h"



#define		PARTICLE_PROCESSOR_PARTICLE_FALL	0x00000001
#define		PARTICLE_PROCESSOR_PARTICLE_GUN		0x00000002


////////////////////////////////////////////////////
////////            BONES SETUP             ////////
////////////////////////////////////////////////////

/*
+-----------------------+------------------+
| ident                 | data block size  |
+-----------------------+------------------+
| BONE_FRAME            | ---              |
| BONE_NAME             | char[strlen + 1] |
| BONE_OFFSET           | D3DXMATRIX       |
| BONE_INFLUENCE_NUMBER | DWORD            |
| BONE_INF_VERT_INDEXES | DWORD[inf num]   |
| BONE_INF_VERT_WEIGHTS | float[inf num]   |
| BONE_CHILDREN_BLOCK   | ---              |
+-----------------------+------------------+
*/

int setupBonesHierarchyFromDataBlock(const BYTE* _dataBlock, DWORD _dataBlockLength, ID3DXSkinInfo* _pSkinInfo, D3DXMATRIX* _pmBoneOffsets, D3DXFRAME** _rootFrameOut, DWORD* boneId)
{
	if (_dataBlock == NULL || _dataBlockLength <= 6)
	{
		*_rootFrameOut = NULL;
		return 0;
	}

	BYTE* ptr = const_cast<BYTE*>(_dataBlock);
	UINT remainingLength = _dataBlockLength;

	WORD blockId = (WORD)*ptr;
	DWORD blockSize = (DWORD)*(ptr + 2);
	BYTE* blockData = ptr + 6;

	if (blockId != BONE_FRAME || _dataBlockLength < blockSize)
	{
		*_rootFrameOut = NULL;
		return 0;
	}

	DWORD frameRemainingSize = blockSize-6;

	WORD frameId = 0;
	DWORD frameSize = 0;
	BYTE* framePtr = blockData;

//Single frame data
	char* boneName = NULL;
	D3DXMATRIX* boneOffset = NULL;
	DWORD boneInfluenceNumber = 0;
	DWORD* boneInfVerIndexes = NULL;
	float* boneInfVerWeights = NULL;
	BYTE* boneChildrenBlock = NULL;
	DWORD boneChildrenBlockSize = 0;

	while (frameRemainingSize > 6)
	{
		frameId = (WORD)*framePtr;
		frameSize = (DWORD)*(framePtr + 2);
		framePtr = framePtr + 6;

		switch (frameId)
		{
			case BONE_NAME:
			{
				setStringA(&boneName, (char*)framePtr, frameSize-6);
				break;
			}
			case BONE_OFFSET:
			{
				boneOffset = (D3DXMATRIX*)framePtr;
				break;
			}
			case BONE_INFLUENCE_NUMBER:
			{
				boneInfluenceNumber = *(DWORD*)framePtr;
				break;
			}
			case BONE_INF_VERT_INDEXES:
			{
				boneInfVerIndexes = (DWORD*)framePtr;
				break;
			}
			case BONE_INF_VERT_WEIGHTS:
			{
				boneInfVerWeights = (float*)framePtr;
				break;
			}
			case BONE_CHILDREN_BLOCK:
			{
				boneChildrenBlock = framePtr;
				boneChildrenBlockSize = frameSize-6;
				break;
			}
		}

		frameRemainingSize -= frameSize;
		framePtr += frameSize;
	}

	DWORD boneIdn = 0;
	if (boneId == NULL)
	{
		boneId = &boneIdn;
	}


	D3DXFRAME* frame = new D3DXFRAME;
	frame->Name = boneName;
	frame->pMeshContainer = NULL;

	_pSkinInfo->SetBoneName(*boneId, frame->Name);
	_pSkinInfo->SetBoneOffsetMatrix(*boneId, boneOffset);
	if (_pmBoneOffsets)
		_pmBoneOffsets[*boneId] = *boneOffset;
	_pSkinInfo->SetBoneInfluence(*boneId, boneInfluenceNumber, boneInfVerIndexes, boneInfVerWeights);

	(*boneId)++;

//set child frame
	setupBonesHierarchyFromDataBlock(boneChildrenBlock, boneChildrenBlockSize, _pSkinInfo, _pmBoneOffsets, &(frame->pFrameFirstChild), boneId);

//set sibling frame
	setupBonesHierarchyFromDataBlock((ptr + blockSize), _dataBlockLength - blockSize, _pSkinInfo, _pmBoneOffsets, &(frame->pFrameSibling), boneId);

	return 0;
};


////////////////////////////////////////////////////
////////        ANIMATION SETS SETUP        ////////
////////////////////////////////////////////////////

/*
+----------------------+------------------+
| ident                | data block size  |
+----------------------+------------------+
| ANIMATION_SET_FRAME  | ---              |
| ANIMATION_SET_NAME   | char[strlen + 1] |
| TICKS_PER_SECOND     | double           |
| PLAYBACK_TYPE        | DWORD            |
| ANIMATIONS_NUMBER    | DWORD            |
| CALLBACK_KEYS_NUMBER | DWORD            |
| CALLBACK_KEYS_ARRAY  | ---              |
| ANIMATIONS_BLOCK     | ---              |
+----------------------+------------------+
*/

int setupAnimationSetsFromBlock(const BYTE* _dataBlock, DWORD _dataBlockLength, ID3DXAnimationController* _pAnimController, UserDefinedCallbackSetup* _callbackSetup)
{
	if (_dataBlockLength <= 6)
		return 0;

	BYTE* ptr = const_cast<BYTE*>(_dataBlock);
	DWORD remainingLength = _dataBlockLength;

	while (remainingLength > 6)
	{
		if ((WORD)(*ptr) != ANIMATION_SET_FRAME)
		{
			remainingLength -= (DWORD)(*(ptr + 2));
			ptr += (DWORD)(*(ptr + 2));
			continue;
		}

		BYTE* asPtr = ptr + 6;
		DWORD asLength = (DWORD)(*(ptr + 2)) - 6;

		WORD frameId = 0;
		DWORD frameSize = 0;
		BYTE* frameDataPtr = NULL;

		char* asName = NULL;
		double asTicksPerSecond = 0.0f;
		D3DXPLAYBACK_TYPE asPlaybackType = D3DXPLAY_ONCE;
		DWORD asAnimationNumber = 0;
		DWORD asCallbackKeysNumber = 0;
		BYTE* asCallbackKeysBlock = NULL;
		DWORD asCallbackKeysBlockSize = 0;
		BYTE* asAnimationsBlock = NULL;
		DWORD asAnimationsBlockSize = 0;
		D3DXKEY_CALLBACK* asCallbackKeys = NULL;

		while (asLength > 6)
		{
			frameId = (WORD)(*asPtr);
			frameSize = (DWORD)(*(asPtr + 2));
			frameDataPtr = asPtr + 6;

			switch(frameId)
			{
				case ANIMATION_SET_NAME:
				{
					asName = (char*)frameDataPtr;
					break;
				}
				case TICKS_PER_SECOND:
				{
					asTicksPerSecond = (double)(*frameDataPtr);
					break;
				}
				case PLAYBACK_TYPE:
				{
					asPlaybackType = (D3DXPLAYBACK_TYPE)(*frameDataPtr);
					break;
				}
				case ANIMATIONS_NUMBER:
				{
					asAnimationNumber = (DWORD)(*frameDataPtr);
					break;
				}
				case CALLBACK_KEYS_NUMBER:
				{
					asCallbackKeysBlockSize = frameSize - 6;
					asCallbackKeysNumber = (DWORD)(*frameDataPtr);
					break;
				}
				case CALLBACK_KEYS_ARRAY:
				{
					asCallbackKeysBlock = frameDataPtr;
					break;
				}
				case ANIMATIONS_BLOCK:
				{
					asAnimationsBlockSize = frameSize - 6;
					asAnimationsBlock = frameDataPtr;
					break;
				}
			}

			asLength -= frameSize;
			asPtr += frameSize;
		}

		ID3DXKeyframedAnimationSet* animationSet = NULL;

		if (_callbackSetup)
		{
			int result = _callbackSetup->setupCallbacksFromBlock(asCallbackKeysBlock, _dataBlockLength, asCallbackKeysNumber, &asCallbackKeys);
			if (result)
			{
				if (asCallbackKeys)
					SAFE_DELETE_ARRAY(asCallbackKeys);
				return 1;
			}
		}
		else
			asCallbackKeysNumber = 0;

		HRESULT hr = D3DXCreateKeyframedAnimationSet(asName, asTicksPerSecond, asPlaybackType, asAnimationNumber, asCallbackKeysNumber, asCallbackKeys, &animationSet);
		if (hr != S_OK)
		{
			if (asCallbackKeys)
				delete asCallbackKeys;
			return 2;
		}

		result = setupAnimationsFromBlock(asAnimationsBlock, asAnimationsBlockSize, asAnimationNumber, animationSet);
		if (result)
		{
			animationSet->Release();
			return 3;
		}

		hr = _pAnimController->RegisterAnimationSet(animationSet);
		if (hr != S_OK)
		{
			animationSet->Release();
			return 4;
		}

		remainingLength -= (DWORD)(*(ptr + 2));
		ptr += (DWORD)(*(ptr + 2));
	}

	return 0;
};


////////////////////////////////////////////////////
////////          ANIMATION SETUP           ////////
////////////////////////////////////////////////////

/*
+------------------------+----------------------+
|   ident                |   data block size    |
+------------------------+----------------------+
| ANIMATION_FRAME        | ---                  |
| ANIMATION_NAME         | char[strlen + 1]     |
| SCALE_KEYS_ARRAY       | D3DXKEY_VECTOR3[]    |
| ROTATION_KEYS_ARRAY    | D3DXKEY_QUATERNION[] |
| TRANSLATION_KEYS_ARRAY | D3DXKEY_VECTOR3[]    |
+------------------------+----------------------+
*/

int setupAnimationsFromBlock(const BYTE* _dataBlock, DWORD _dataBlockLength, DWORD _animationsNumber, ID3DXKeyframedAnimationSet* _animationSet)
{
	if (_dataBlockLength <= 6 || _animationsNumber == 0)
		return 0;

	BYTE* ptr = const_cast<BYTE*>(_dataBlock);
	DWORD remainingLength = _dataBlockLength;
	DWORD animNum = _animationsNumber;

	WORD animationFrameId = 0;
	DWORD animationFrameSize = 0;
	BYTE* animationFramePtr = NULL;

	while (remainingLength > 6 && animNum > 0)
	{
		animationFrameId = (WORD)*ptr;
		animationFrameSize = (DWORD)*(ptr + 2);
		BYTE* animationFramePtr = ptr + 6;

		if (animationFrameId != ANIMATION_FRAME)
		{
			ptr += animationFrameSize;
			remainingLength -= animationFrameSize;
			continue;
		}

		char* animationName = NULL;
		DWORD scaleKeysNumber = 0;
		D3DXKEY_VECTOR3* scaleKeysArray = NULL;
		DWORD rotationKeysNumber = 0;
		D3DXKEY_QUATERNION* rotationKeysArray = NULL;
		DWORD translationKeysNumber = 0;
		D3DXKEY_VECTOR3* translationKeysArray = NULL;

		DWORD remSize = animationFrameSize;
		while (remSize > 6)
		{
			WORD animId = (WORD)*animationFramePtr;
			DWORD animSize = (DWORD)*(animationFramePtr + 2);
			BYTE* animanimationFramePtr = animationFramePtr + 6;
			switch (animId)
			{
				case ANIMATION_NAME:
				{
					animationName = (char*)animanimationFramePtr;
					break;
				}
				case SCALE_KEYS_ARRAY:
				{
					scaleKeysNumber = (animSize - 6) / sizeof(D3DXKEY_VECTOR3);
					scaleKeysArray = reinterpret_cast<D3DXKEY_VECTOR3*>(animanimationFramePtr);
					break;
				}
				case ROTATION_KEYS_ARRAY:
				{
					rotationKeysNumber = (animSize - 6) / sizeof(D3DXKEY_QUATERNION);
					rotationKeysArray = reinterpret_cast<D3DXKEY_QUATERNION*>(animanimationFramePtr);
					break;
				}
				case TRANSLATION_KEYS_ARRAY:
				{
					translationKeysNumber = (animSize - 6) / sizeof(D3DXKEY_VECTOR3);
					translationKeysArray = reinterpret_cast<D3DXKEY_VECTOR3*>(animanimationFramePtr);
					break;
				}
			}

			remSize -= animSize;
			animationFramePtr += animSize;
		}

		DWORD animIndex = 0;
		HRESULT hr = _animationSet->RegisterAnimationSRTKeys(	animationName,
																scaleKeysNumber,
																rotationKeysNumber,
																translationKeysNumber,
																scaleKeysArray,
																rotationKeysArray,
																translationKeysArray,
																&animIndex);
		if (hr != S_OK)
		{
			return 1;
		}

		remainingLength -= animationFrameSize;
		ptr += animationFrameSize;
		animNum--;
	}

	return 0;
};



int setupParticleProcessor(BYTE const* _dataBlock, DWORD _dataBlockLength, UserDefinedParticleProcessor** _particleProcessor)
{
	BYTE* pointer = _dataBlock;

	DWORD ppType = *(reinterpret_cast <DWORD*> (pointer));
	pointer += sizeof(DWORD);

	switch(ppType)
	{
		case PARTICLE_PROCESSOR_PARTICLE_FALL:
		{
			ParticleProcessor_ParticleFall* pproc = new ParticleProcessor_ParticleFall;
			pproc->unserialize(reinterpret_cast <void*> (pointer));
			*_particleProcessor = pproc;
			break;
		}

		case PARTICLE_PROCESSOR_PARTICLE_GUN:
		{
			ParticleProcessor_ParticleGun* pproc = new ParticleProcessor_ParticleGun;
			pproc->unserialize(reinterpret_cast <void*> (pointer));
			*_particleProcessor = pproc;
			break;
		}

		default:
		{
			return -1;
		}
	}

	return 0;
};


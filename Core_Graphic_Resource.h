#pragma once

#ifndef _CORE_GRAPHIC_RESOURCE
#define _CORE_GRAPHIC_RESOURCE

#include "utility.h"
#include "Template_Basic_Resource.h"
#include "Hierarchical_Animation_Set.h"

#include "Core_World_Position_v3.h"



void deleteFrameHierarchy(D3DXFRAME* pRootFrame);
HRESULT setupBonePointers(D3DXFRAME* pRootFrame, D3DXMATRIX*** m_apmxBonePointers, ID3DXSkinInfo* pSkinInfo);
D3DXFRAME* cloneFrameHierarchy(D3DXFRAME* pRootFrame);



struct AnimationInfo;
class Mesh;




class AnimationInfo
{
	friend class AnimationInfo;

protected:

	struct SubsetData
	{
		DWORD subsetId;

		UINT paletteSize;
		UINT* paletteOffsetsIndexes;
		D3DXMATRIX** subsetPalette;

		SubsetData() : subsetId(0), palettesSize(0), paletteOffsetsIndexes(NULL), subsetPalette(NULL) {};
		~SubsetData()
		{
			SAFE_DELETE_ARRAY(paletteOffsetsIndexes);
			SAFE_DELETE_ARRAY(subsetPalette);
		};

		int setup(D3DXBONECOMBINATION* _boneCombination, UINT _paletteSize)
		{
			subsetId = _boneCombination->AttribId;

			list<DWORD> is;
			for (UINT i = 0; i < _paletteSize; ++i)
				if (_boneCombination->BoneId[i] != UINT_MAX)
					is.push_back(_boneCombination->BoneId[i]);

			paletteSize = is.size();
			paletteOffsetsIndexes = new UINT [paletteSize];

			UINT i = 0;
			for (list<DWORD>::iterator it = is.begin(); it != is.end(); it++)
			{
				paletteOffsetsIndexes[i++] = (*it);
			}

			return 0;
		};

		int setupLocalPalette(D3DXMATRIX* _palette)
		{
			SAFE_DELETE_ARRAY(subsetPalette);

			if (_palette)
			{
				subsetPalette = new D3DXMATRIX* [paletteSize];

				for (UINT i = 0; i < paletteSize; ++i)
				{
					subsetPalette[i] = &(_palette[paletteOffsetsIndexes[i]]);
				}
			}

			return 0;
		};

		int setupLocalPalette(D3DXMATRIX** _palettePointers)
		{
			SAFE_DELETE_ARRAY(subsetPalette);

			if (_palettePointers)
			{
				subsetPalette = new D3DXMATRIX* [paletteSize];

				for (UINT i = 0; i < paletteSize; ++i)
				{
					subsetPalette[i] = _palettePointers[paletteOffsetsIndexes[i]];
				}
			}

			return 0;
		};


		int copy(SubsetData* _subsetData)
		{
			_subsetData->subsetId = subsetId;
			_subsetData->paletteSize = paletteSize;
			_subsetData->paletteOffsetsIndexes = new UINT [paletteSize];
			memcpy(_subsetData->paletteOffsetsIndexes, paletteOffsetsIndexes, paletteSize);
			_subsetData->subsetPalette = new D3DXMATRIX* [paletteSize];

			return 0;
		};
	};
//
	struct SubsetsBuffer
	{
	public:
		UINT subsetsNumber;
		SubsetData* subsetsArray;

		SubsetsBuffer() : subsetsNumber(0), subsetsArray(NULL) {};

		SubsetsBuffer(UINT _bonesCombNumber, D3DXBONECOMBINATION* _boneCombinations, UINT _maxPaletteSize)
		{
			refCount = 1;
			subsetsNumber = _bonesCombNumber;
			subsetsArray = new SubsetData [subsetsNumber];

			for (UINT i = 0; i < subsetsNumber; ++i)
				subsetsArray[_boneCombinations[i].AttribId].setup(&(_boneCombinations[i]), _maxPaletteSize);
		};

		int clone(SubsetsBuffer** _subsetsBuffer)
		{
			SubsetsBuffer* subsetsBuffer = new SubsetsBuffer;
			subsetsBuffer->subsetsNumber = subsetsNumber;
			subsetsBuffer->subsetsArray = new SubsetData [subsetsNumber];
			for (UINT i = 0; i < subsetsNumber; ++i)
				subsetsArray[i].copy(&(subsetsBuffer->subsetsArray[i]));

			_subsetsBuffer = subsetsBuffer;
			return 0;
			
		};

		int release()
		{
			SAFE_DELETE_ARRAY(subsetsArray);
			delete this;
			return 0;
		};


		int setupLocalPalettes(D3DXMATRIX* _palette)
		{
			for (UINT i = 0; i < subsetsNumber; ++i)
			{
				subsetsArray[i].setupLocalPalette(_palette);
			}

			return 0;
		};

		int setupLocalPalettes(D3DXMATRIX** _palettePointers)
		{
			for (UINT i = 0; i < subsetsNumber; ++i)
			{
				subsetsArray[i].setupLocalPalette(_palettePointers);
			}

			return 0;
		};

	};
//

	ID3DXMesh* pBlendedMesh;
	ID3DXSkinInfo* pSkinInfo;

	HAnimationSet* pHierarchicalAnimationSet;


	SubsetsBuffer* subsetsData;

	DWORD dwMaxNumFaceInfls;

	DWORD dwPaletteSize;


	// if equipable
	D3DXMATRIX** bonesPalettePointers;

	//if controlable
	ID3DXAnimationController* pAnimController;
	D3DXFRAME* pRootFrame;

	DWORD workingPaletteTimestamp;

	D3DXMATRIX** bonesOffsetsArray;
	D3DXMATRIX** bonesTransformsArray;
	D3DXMATRIX* bonesPalette;


	virtual ~AnimationInfo();

	bool isSkinControllable()
	{
		return pAnimController ? true : false;
	};

	ID3DXSkinInfo* getSkinInfo()
	{
		return pSkinInfo;
	};

	D3DXMATRIX* getBonesPalette()
	{
		return bonesPalette;
	};
////

public:

	AnimationInfo() : pBlendedMesh(NULL), pSkinInfo(NULL), subsetsData(NULL), dwMaxNumFaceInfls(0), dwNumPaletteEntries(0) {};
	int release();

	int setup(ID3DXSkinInfo* _pSkinInfo, ID3DXMesh* _pMesh, DWORD _paletteSize, ID3DXAnimationController* _pAnimController, D3DXFRAME* _pRootFrame);
	int clone(AnimationInfo** _animationInfo);

	int updateWorkingPalette(DWORD _timestamp);
	int forceUpdateWorkingPalette(DWORD _timestamp);

	int equipMesh(AnimationInfo* _animationInfo);

	int setSubsetPalette(ID3DXEffect* _effect, D3DXHANDLE _h_paletteArray, D3DXHANDLE _h_boneInfluenceNumber, DWORD _subset);

	UINT getAnimatedSubsetsNumber();
	int getBaseSubsetId(UINT _animSubset);
	ID3DXMesh* getBlendedMesh();
	const D3DXMATRIX** getSubsetPalettePointers(DWORD _subset);
	UINT getSubsetPaletteSize(DWORD _subset);
	UINT getMaxFaceBonesInfluence();
};




class Mesh : public Resource <ID3DXMesh*>
{
private:
	~Mesh();

protected:
	DWORD subsets;
	AnimationInfoBase* pAnimInfo;


public:
	#ifndef RESOURCE_MAP_WITH_STRINGS
		Mesh ( UINT _id ) : subsets( 0 ), pAnimInfo( NULL ), Resource( _id, NULL ) {};
	#else //RESOURCE_MAP_WITH_STRINGS
		Mesh ( string _id ) : subsets( 0 ), pAnimInfo( NULL ), Resource( _id, NULL ) {};
	#endif //RESOURCE_MAP_WITH_STRINGS

	void setResource( ID3DXMesh* _mesh, DWORD _subsets, AnimationInfo* _pAnimInfo );

	void setAnimationInfo( AnimationInfo* _pAnimInfo );
	void getAnimationInfoInstance( AnimationInfo** _ppAnimInfo );

	#ifndef RESOURCE_MAP_WITH_STRINGS
		static Mesh* getResource( UINT _id );
	#else //RESOURCE_MAP_WITH_STRINGS
		static Mesh* getResource( string _id );
	#endif //RESOURCE_MAP_WITH_STRINGS


	#ifdef(MULTI_THREADED_LOADING)
		#ifndef RESOURCE_MAP_WITH_STRINGS
			static Mesh* getResourceMultiThreaded( UINT _id );
		#else //RESOURCE_MAP_WITH_STRINGS
			static Mesh* getResourceMultiThreaded( string _id );
		#endif //RESOURCE_MAP_WITH_STRINGS
	#endif //MULTI_THREADED_LOADING


	int onLostDevice(){ return 0; };
	int onResetDevice(){ return 0; };


	inline DWORD getSubsetsNum()
	{
		return subsets;
	};

	inline bool isAnimated()
	{
		if (pAnimInfo)
			return true;
		return false;
	};

	inline const D3DXVECTOR3* getCenter()
	{
		return &boundingSphere.center;
	};

	inline float getSphereRadius()
	{
		return boundingSphere.radius;
	};

};



#endif //_CORE_GRAPHIC_RESOURCE

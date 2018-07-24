#pragma once

#ifndef _SKIN_INFO
#define _SKIN_INFO

#include "utility.h"
#include "Hierarchical_Animation_Set.h"



struct SubsetInfo;
class SkinInfo;
class ControllableSkinInfo;
class EquipableSkinInfo;



struct SubsetInfo
{
	DWORD subsetId;
	vector< USHORT > vPaletteOffsets;

	SubsetInfo() : subsetId(0) {};

	void setup( D3DXBONECOMBINATION* _boneCombination, UINT _paletteSize )
	{
		subsetId = _boneCombination->AttribId;

		list< USHORT > ldTmp;
		for ( UINT i = 0; i < _paletteSize; ++i )
			if ( _boneCombination->BoneId[i] != UINT_MAX )
				ldTmp.push_back( _boneCombination->BoneId[i] );

		vPaletteOffsets.resize( ldTmp.size() );

		UINT i = 0;
		for ( list< USHORT >::iterator it = ldTmp.begin(); it != ldTmp.end(); ++it )
			vPaletteOffsets[ i++ ] = (*it);
	};
};



class SkinInfo
{
protected:

	ID3DXMesh* pBlendedMesh;
	ID3DXSkinInfo* pSkinInfo;
	DWORD dwMaxNumFaceInfls;

	vector< SubsetInfo > vSubsetsInfo;


	virtual ~SkinInfo()
	{
		IDX_SAFE_RELEASE( pBlendedMesh );
		IDX_SAFE_RELEASE( pSkinInfo );
	};


public:

	SkinInfo()
	{
		pBlendedMesh = NULL;
		pSkinInfo = NULL;
		dwMaxVertexInfl = 0;

	};

	int release()
	{
		delete this;
		return 0;
	};


	void setup( ID3DXSkinInfo* _pSkinInfo, ID3DXMesh* _pMesh, DWORD _paletteSize )
	{
		pSkinInfo = _pSkinInfo;

		vector<DWORD> meshAdjacency;
		meshAdjacency.resize( _pMesh->GetNumFaces() * 3 );
		HRESULT hr = S_OK;

		if ( FAILED( hr = _pMesh->GenerateAdjacency( EPSILON, &(meshAdjacency[0]) ) ) )
			throw D3DX9Exception( "SkinInfo::setup", "ID3DXMesh::GenerateAdjacency", hr );

		DWORD dwNumAttrGroups;
		ID3DXBuffer* pBufBoneCombos;

		if ( FAILED( hr = pSkinInfo->ConvertToIndexedBlendedMesh(	_pMesh, 
																	D3DXMESH_MANAGED | D3DXMESHOPT_VERTEXCACHE, 
																	_paletteSize, 
																	&(meshAdjacency[0]), 
																	NULL, 
																	NULL, 
																	NULL, 
																	&dwMaxVertexInfl, 
																	&dwNumAttrGroups, 
																	&pBufBoneCombos, 
																	&pBlendedMesh ) ) )
			throw D3DX9Exception( "SkinInfo::setup", "ID3DXSkinInfo::ConvertToIndexedBlendedMesh", hr );


		vSubsetsInfo.resize( dwNumAttrGroups );
		D3DXBONECOMBINATION* pBoneCombination = reinterpret_cast< D3DXBONECOMBINATION* > (pBufBoneCombos->GetBufferPointer());

		for ( DWORD i = 0; i < dwNumAttrGroups; ++i )
			vSubsetsInfo[i].setup( &(pBoneCombination[i]), _paletteSize );

		IDX_SAFE_RELEASE( pBufBoneCombos );
	};


	inline const ID3DXMesh* getBlendedMesh()	{	return pBlendedMesh;		};
	inline const ID3DXSkinInfo* getSkinInfo()	{	return pSkinInfo;			};

	inline DWORD getMaxVertexInfl()		{	return dwMaxVertexInfl;		};

	inline DWORD getSubsetsNumber()		{	return vSubsetsInfo.size();	};

	inline const SubsetInfo* getSubsetInfo( USHORT _index )	{	return &(vSubsetsInfo[_index]);	};
};



class ControllableSkinInfo : public virtual SkinInfo
{
private:

	HAnimationSet* pHAnimationSet;

public:

	ControllableSkinInfo() : SkinInfo()
	{};

	~ControllableSkinInfo()
	{
		SAFE_RELEASE( pHAnimationSet );
	};

	void setup( ID3DXSkinInfo* _pSkinInfo, ID3DXMesh* _pMesh, DWORD _paletteSize, HAnimationSet* _pHAnimationSet )
	{
		SkinInfo::setup( _pSkinInfo, _pMesh, _paletteSize );
		pHAnimationSet = _pHAnimationSet;
	};
};


class EquipableSkinInfo : public virtual SkinInfo
{
private:

	vector<string> vBoneSet;

public:

	EquipableSkinInfo() : SkinInfo()
	{};

	void setup( ID3DXSkinInfo* _pSkinInfo, ID3DXMesh* _pMesh, DWORD _paletteSize, const char* const* _ppBoneNames, USHORT _bonesNumber )
	{
		SkinInfo::setup( _pSkinInfo, _pMesh, _paletteSize );
		vBoneSet.resize( _bonesNumber );
		for ( USHORT i = 0; i < _bonesNumber; ++i )
			vBoneSet[i] = _ppBoneNames[i];
	};
};


#endif //_SKIN_INFO

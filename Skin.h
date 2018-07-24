#pragma once

#ifndef _SKIN
#define _SKIN

#include "utility.h"
#include "Hierarchical_Animation_Set.h"





/*

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

*/


class EquipableSkinController
{
private:

	Model* pModel;
	D3DXMATRIX* pPalette;


public:

	EquipableSkin()
	{};
};



#endif //_SKIN

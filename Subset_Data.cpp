#include "Subset_Data.h"



SubsetData::SubsetData() : pSubsetInfo(NULL), subsetPaletteSize(0), ppMxSubsetPalette(NULL)
{};


SubsetData::~SubsetData()
{
	SAFE_DELETE_ARRAY( ppMxSubsetPalette );
};


void SubsetData::setup( SubsetInfo* _pSubsetInfo, D3DXMATRIX* _pPalette )
{
	pSubsetInfo = _pSubsetInfo;
	subsetPaletteSize = pSubsetInfo->vPaletteOffsets.size();
	SAFE_DELETE_ARRAY( ppMxSubsetPalette );
	ppMxSubsetPalette = new D3DXMATRIX* [ subsetPaletteSize ];

	for ( USHORT i = 0; i < subsetPaletteSize; ++i )
	{
		ppMxSubsetPalette[ i ] = &( _pPalette[ pSubsetInfo->vPaletteOffsets[i] ] );
	}
};


void SubsetData::setup( SubsetInfo* _pSubsetInfo, D3DXMATRIX** _ppPalette )
{
	pSubsetInfo = _pSubsetInfo;
	subsetPaletteSize = pSubsetInfo->vPaletteOffsets.size();
	SAFE_DELETE_ARRAY( ppMxSubsetPalette );
	ppMxSubsetPalette = new D3DXMATRIX* [ subsetPaletteSize ];

	for ( USHORT i = 0; i < subsetPaletteSize; ++i )
	{
		ppMxSubsetPalette[ i ] = _ppPalette[ pSubsetInfo->vPaletteOffsets[i] ];
	}
};

#pragma once

#ifndef _SUBSET_DATA
#define _SUBSET_DATA

#include "utility.h"



struct SubsetData
{
	SubsetInfo* pSubsetInfo;
	USHORT subsetPaletteSize;
	D3DXMATRIX** ppMxSubsetPalette;

	SubsetData();
	~SubsetData();

	void setup( SubsetInfo* _pSubsetInfo, D3DXMATRIX* _pPalette );
	void setup( SubsetInfo* _pSubsetInfo, D3DXMATRIX** _ppPalette );
};

#define //_SUBSET_DATA
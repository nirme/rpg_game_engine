#include "Core_Graphic_Resource.h"



Resource <IDirect3DTexture9*>::ResourceMap Resource<IDirect3DTexture9*>::loadedResMap;
Resource <ID3DXEffect*>::ResourceMap Resource<ID3DXEffect*>::loadedResMap;
Resource <ID3DXEffect*>::ID3DXEffectStateManager* stateManager = NULL;
Resource <ID3DXMesh*>::ResourceMap Resource<ID3DXMesh*>::loadedResMap;



void deleteFrameHierarchy( D3DXFRAME* pRootFrame )
{
	if ( pRootFrame->pFrameFirstChild )
		deleteFrameHierarchy( pRootFrame->pFrameFirstChild );
	if ( pRootFrame->pFrameSibling )
		deleteFrameHierarchy( pRootFrame->pFrameSibling );

	setStringA( &( pRootFrame->Name ) );
	delete pRootFrame;
};


HRESULT setupBonePointers(D3DXFRAME* pRootFrame, D3DXMATRIX*** m_apmxBonePointers, ID3DXSkinInfo* pSkinInfo)
{
	if ( !pRootFrame || !m_apmxBonePointers || !pSkinInfo )
		return E_FAIL;

	DWORD bones = pSkinInfo->GetNumBones();
	*m_apmxBonePointers = new D3DXMATRIX* [ bones ];

	D3DXFRAME* frame = NULL;
	for ( UINT i = 0; i < bones; ++i )
	{
		frame = D3DXFrameFind( pRootFrame, pSkinInfo->GetBoneName(i) );
		if (frame == NULL)	return E_FAIL;
		(*m_apmxBonePointers)[i] = &( frame->TransformationMatrix );
	}

	return S_OK;
};


D3DXFRAME* cloneFrameHierarchy(D3DXFRAME* pRootFrame)
{
	if (pRootFrame == NULL)
		return NULL;

	D3DXFRAME* frame = new D3DXFRAME;
	frame->Name = NULL;
	setStringA(&(frame->Name), pRootFrame->Name);
	frame->pMeshContainer = pRootFrame->pMeshContainer;

	frame->pFrameFirstChild = cloneFrameHierarchy(pRootFrame->pFrameFirstChild, numBones);
	frame->pFrameSibling = cloneFrameHierarchy(pRootFrame->pFrameSibling, numBones);

	return frame;
};




///////////////////////////////////////////////////////////////
////////                 AnimationInfo                 ////////
///////////////////////////////////////////////////////////////

int AnimationInfo::release()
{
	IDX_SAFE_RELEASE(pBlendedMesh);
	IDX_SAFE_RELEASE(pSkinInfo);
	SAFE_RELEASE(subsetsData);

	SAFE_DELETE_ARRAY(bonesPalettePointers);

	IDX_SAFE_RELEASE(pAnimController);

	if (pRootFrame)
	{
		deleteFrameHierarchy(pRootFrame);
		pRootFrame = NULL;
	}

	SAFE_DELETE_ARRAY(bonesOffsetsArray);
	SAFE_DELETE_ARRAY(bonesTransformsArray);
	SAFE_DELETE_ARRAY(bonesPalette);

	return 0;
};


int AnimationInfo::setup(ID3DXSkinInfo* _pSkinInfo, ID3DXMesh* _pMesh, DWORD _paletteSize, ID3DXAnimationController* _pAnimController, D3DXFRAME* _pRootFrame)
{
	if (!_pSkinInfo || !_pMesh || _paletteSize < 30)
		return 1;

	release();

	vector<DWORD> meshAdjacency;
	meshAdjacency.resize(_pMesh->GetNumFaces() * 3);
	HRESULT hr = S_OK;

	if (FAILED(hr = _pMesh->GenerateAdjacency(EPSILON, &(meshAdjacency[0]))))
	{
		showError(false, "ID3DXMesh::GenerateAdjacency error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -1;
	}

	DWORD dwNumAttrGroups;
	ID3DXBuffer* pBufBoneCombos;

	pSkinInfo = _pSkinInfo;

	if (FAILED(hr = pSkinInfo->ConvertToIndexedBlendedMesh(	_pMesh, 
															D3DXMESH_MANAGED | D3DXMESHOPT_VERTEXCACHE, 
															_paletteSize, 
															&(meshAdjacency[0]), 
															NULL, 
															NULL, 
															NULL, 
															&dwMaxNumFaceInfls, 
															&dwNumAttrGroups, 
															&pBufBoneCombos, 
															&pBlendedMesh)))
	{
		showError(false, "ID3DXSkinInfo::ConvertToIndexedBlendedMesh error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -3;
	}

	subsetsData = new SubsetsBuffer(dwNumAttrGroups, pBufBoneCombos, _paletteSize);

	IDX_SAFE_RELEASE(pBufBoneCombos);


	dwPaletteSize = _paletteSize;

	pAnimController = _pAnimController;
	pRootFrame = _pRootFrame;

	return 0;
};


int AnimationInfo::clone(AnimationInfo** _animationInfo)
{
	AnimationInfo* animationInfo = new AnimationInfo;
	//animationInfo

	animationInfo->pBlendedMesh = pBlendedMesh;
	animationInfo->pSkinInfo = pSkinInfo;
	subsetsData->clone(&(animationInfo->subsetsData));
	animationInfo->subsetsData = subsetsData;

	animationInfo->dwMaxNumFaceInfls = dwMaxNumFaceInfls;
	animationInfo->dwPaletteSize = dwPaletteSize;


	if (pAnimController)
	{
		pAnimController->CloneAnimationController(	pAnimController->GetMaxNumAnimationOutputs(), 
													pAnimController->GetMaxNumAnimationSets(), 
													pAnimController->GetMaxNumTracks(), 
													pAnimController->GetMaxNumEvents(), 
													&(animationInfo->pAnimController));

		animationInfo->pRootFrame = cloneFrameHierarchy(pRootFrame);

		if (FAILED(hr = D3DXFrameRegisterNamedMatrices(animationInfo->pRootFrame, animationInfo->pAnimController)))
		{
			showError(false, "D3DXFrameRegisterNamedMatrices error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -1;
		}

		animationInfo->workingPaletteTimestamp = -1;

		setupBonePointers(animationInfo->pRootFrame, &(animationInfo->bonesTransformsArray), animationInfo->pSkinInfo);

		UINT dwb = animationInfo->pSkinInfo->GetNumBones();
		animationInfo->bonesOffsetsArray = new D3DXMATRIX* [dwb];
		for (UINT i = 0; i < dwb; ++i)
		{
			animationInfo->bonesOffsetsArray[i] = animationInfo->pSkinInfo->GetBoneOffsetMatrix(i);
		}


		animationInfo->bonesPalette = new D3DXMATRIX [dwb];

		// setup local palette
		animationInfo->subsetsData->setupLocalPalettes(animationInfo->bonesPalette);
	}
	else
	{
		animationInfo->bonesPalettePointers = new D3DXMATRIX* [animationInfo->pSkinInfo->GetNumBones()];
	}

	animationInfo->pBlendedMesh->AddRef();
	animationInfo->pSkinInfo->AddRef();
	animationInfo->subsetsData->addRef();

	*_animationInfo = animationInfo;

	return 0;
};


int AnimationInfo::updateWorkingPalette(DWORD _timestamp)
{
	if (!pAnimController)
		return -1;

	if (b_workingPaletteTimestamp == _timestamp)
		return 0;

	for (UINT i = 0; i < bonesNumber; ++i)
		D3DXMatrixMultiply(&(bonesPalette[i]), bonesOffsetsArray[i], bonesTransformsArray[i]);

	b_workingPaletteTimestamp = _timestamp;

	return 0;
};


int AnimationInfo::forceUpdateWorkingPalette(DWORD _timestamp)
{
	if (!pAnimController)
		return -100;

	for (UINT i = 0; i < bonesNumber; ++i)
		D3DXMatrixMultiply(&(bonesPalette[i]), bonesOffsetsArray[i], bonesTransformsArray[i]);

	b_workingPaletteTimestamp = _timestamp;

	return 0;
};


int AnimationInfo::equipMesh(AnimationInfo* _animationInfo)
{
	if (pAnimController || !_animationInfo->isSkinControllable())
		return -100;

	ID3DXSkinInfo* si = _animationInfo->getSkinInfo();
	D3DXMATRIX* basePalette = _animationInfo->getBonesPalette();

	UINT bonesSet = 0;

	for (UINT i = 0; i < pSkinInfo->GetNumBones(); ++i)
	{
		LPCSTR boneName = pSkinInfo->GetBoneName(i);

		for (UINT j = 0; j < si->GetNumBones(); ++j)
		{
			if (!strcmp(boneName, si->GetBoneName(j)))
			{
				bonesPalettePointers[i] = &(basePalette[j]);
				bonesSet++;
				break;
			}
		}
	}

	if (bonesSet != pSkinInfo->GetNumBones())
		return -1;

	subsetsData->setupLocalPalettes(animationInfo->bonesPalettePointers);

	return 0;
};


int AnimationInfo::setSubsetPalette(ID3DXEffect* _effect, D3DXHANDLE _h_paletteArray, D3DXHANDLE _h_boneInfluenceNumber, DWORD _subset)
{
	SubsetData* sd = &(subsetsData->subsetsArray[_subset]);

	_effect->SetInt(_h_boneInfluenceNumber, dwMaxNumFaceInfls);

	_effect->SetMatrixPointerArray(_h_paletteArray, sd->subsetPalette, sd->paletteSize);

	return 0;
};


UINT AnimationInfo::getAnimatedSubsetsNumber()
{
	DWORD subsets = 0;
	pBlendedMesh->GetAttributeTable(NULL, &subsets);
	return subsets;
};


int AnimationInfo::getBaseSubsetId(UINT _animSubset)
{
	if (subsetsData->subsetsNumber <= _animSubset)
		return -1;

	return subsetsData->subsetsArray[_animSubset].subsetId;
};


ID3DXMesh* AnimationInfo::getBlendedMesh()
{
	render pBlendedMesh;
};


const D3DXMATRIX** AnimationInfo::getSubsetPalettePointers(DWORD _subset)
{
	if (_subset >= subsetsData->subsetsNumber)
		return NULL;

	return subsetsData->subsetsArray[_subset].subsetPalette;
};


UINT AnimationInfo::getSubsetPaletteSize(DWORD _subset)
{
	if (_subset >= subsetsData->subsetsNumber)
		return 0;

	return subsetsData->subsetsArray[_subset].paletteSize;
};


UINT AnimationInfo::getMaxFaceBonesInfluence()
{
	return dwMaxNumFaceInfls;
};



///////////////////////////////////////////////////////////////
////////                     Mesh                      ////////
///////////////////////////////////////////////////////////////

Mesh::~Mesh()
{
	SAFE_RELEASE(pAnimInfo);
	IDX_SAFE_RELEASE(resource);
};


void Mesh::setResource(ID3DXMesh* _mesh, DWORD _subsets, AnimationInfo* _pAnimInfo)
{
	#ifdef(MULTI_THREADED_LOADING)
		boost::mutex::scoped_lock lock(mutex);
	#endif //MULTI_THREADED_LOADING

	if (loaded)
		return 1;

	resource = _mesh;
	subsets = _subsets;
	pAnimInfo = _pAnimInfo;

	loaded = (resource != NULL) ? true : false;

	#ifdef(MULTI_THREADED_LOADING)
		conditionVar.notify_all();
	#endif //MULTI_THREADED_LOADING

	return 0;
};


void Mesh::setAnimationInfo(AnimationInfo* _pAnimInfo)
{
	pAnimInfo = _pAnimInfo;
};


void Mesh::getAnimationInfoInstance(AnimationInfo** _ppAnimInfo)
{
	if (pAnimInfo)
		pAnimInfo->clone(_ppAnimInfo);
	else
		*_ppAnimInfo = NULL;
};


#ifndef RESOURCE_MAP_WITH_STRINGS
	Mesh* Mesh::getResource(UINT _id)
#else //RESOURCE_MAP_WITH_STRINGS
	Mesh* Mesh::getResource(string _id)
#endif //RESOURCE_MAP_WITH_STRINGS
{
	return reinterpret_cast <Mesh*> (getBaseResource(_id));
};


#ifdef(MULTI_THREADED_LOADING)

	#ifndef RESOURCE_MAP_WITH_STRINGS
		Mesh* Mesh::getResourceMultiThreaded(UINT _id)
	#else //RESOURCE_MAP_WITH_STRINGS
		Mesh* Mesh::getResourceMultiThreaded(string _id)
	#endif //RESOURCE_MAP_WITH_STRINGS
	{
		return reinterpret_cast <Mesh*> (getBaseResourceMultiThreaded(_id));
	};
#endif //MULTI_THREADED_LOADING


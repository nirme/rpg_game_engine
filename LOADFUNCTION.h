#pragma once

#ifndef _LOADFUNCTIONQQ
#define _LOADFUNCTIONQQ



#include "Core_Graphic.h"
#include "Core_Graphic_Resource.h"
#include "Core_Light.h"
#include "Core_SoundSystem.h"




void printMatrix(string name, D3DXMATRIX* mx)
{
	showError(false, "%s:", name.c_str());
	showError(false, "%2.4f\t%2.4f\t%2.4f\t%2.4f\t", mx->_11, mx->_12, mx->_13, mx->_14);
	showError(false, "%2.4f\t%2.4f\t%2.4f\t%2.4f\t", mx->_21, mx->_22, mx->_23, mx->_24);
	showError(false, "%2.4f\t%2.4f\t%2.4f\t%2.4f\t", mx->_31, mx->_32, mx->_33, mx->_34);
	showError(false, "%2.4f\t%2.4f\t%2.4f\t%2.4f\t", mx->_41, mx->_42, mx->_43, mx->_44);
	showError(false, "");
};



D3DXMESHCONTAINER* getMeshFromFrameRoot(D3DXFRAME* root)
{
	if (!root)
		return NULL;

	if (root->pMeshContainer)
		return root->pMeshContainer;

	D3DXMESHCONTAINER* m = NULL;
	m = getMeshFromFrameRoot(root->pFrameSibling);
	if (m)
		return m;

	m = getMeshFromFrameRoot(root->pFrameFirstChild);
	if (m)
		return m;

	return NULL;
};


char* HeapCopy( LPCSTR sName )
{
    DWORD dwLen = ( DWORD )strlen( sName ) + 1;
    char* sNewName = new char[ dwLen ];
    if( sNewName )
        strcpy_s( sNewName, dwLen, sName );
    return sNewName;
};



class AllocateHierarchy : public ID3DXAllocateHierarchy
{
	STDMETHOD( CreateFrame )( THIS_ LPCSTR Name, LPD3DXFRAME *ppNewFrame )
	{
		*ppNewFrame = NULL;

		*ppNewFrame = new D3DXFRAME;
		ZeroMemory( *ppNewFrame, sizeof(D3DXFRAME) );

		if(Name)
			(*ppNewFrame)->Name = ( char* ) HeapCopy( ( char* ) Name );
		else
			(*ppNewFrame)->Name = ( char* ) HeapCopy( "<no_name>" );

		return S_OK;
	};


    STDMETHOD( CreateMeshContainer )( THIS_ LPCSTR Name, CONST D3DXMESHDATA * pMeshData, CONST D3DXMATERIAL * pMaterials, CONST D3DXEFFECTINSTANCE * pEffectInstances, DWORD NumMaterials, CONST DWORD * pAdjacency, LPD3DXSKININFO pSkinInfo, LPD3DXMESHCONTAINER * ppNewMeshContainer )
	{
		*ppNewMeshContainer = NULL;
		*ppNewMeshContainer = new D3DXMESHCONTAINER;

		(*ppNewMeshContainer)->Name = HeapCopy(Name);
		(*ppNewMeshContainer)->MeshData.Type = D3DXMESHTYPE_MESH;
		pMeshData->pMesh->CloneMeshFVF(D3DXMESH_MANAGED, pMeshData->pMesh->GetFVF(), Graphic::getDevice(), (&(*ppNewMeshContainer)->MeshData.pMesh));
		(*ppNewMeshContainer)->pMaterials = 0;
		(*ppNewMeshContainer)->pEffects = 0;
		(*ppNewMeshContainer)->NumMaterials = 0;
		(*ppNewMeshContainer)->pAdjacency = new DWORD [pMeshData->pMesh->GetNumFaces() * 3];
		memcpy((*ppNewMeshContainer)->pAdjacency, pAdjacency, pMeshData->pMesh->GetNumFaces() * 3);
		(*ppNewMeshContainer)->pSkinInfo = 0;
		if (pSkinInfo)
			pSkinInfo->Clone(&((*ppNewMeshContainer)->pSkinInfo));

		return S_OK;
	};


    STDMETHOD( DestroyFrame )( THIS_ LPD3DXFRAME pFrameToFree )
	{
		if( pFrameToFree->Name )
			delete [] pFrameToFree->Name;

		delete pFrameToFree;

		return S_OK;
	}


    STDMETHOD( DestroyMeshContainer )( THIS_ LPD3DXMESHCONTAINER pMeshContainerToFree )
	{
		if (pMeshContainerToFree->pSkinInfo)
			pMeshContainerToFree->pSkinInfo->Release();

		if (pMeshContainerToFree->pAdjacency)
			delete [] pMeshContainerToFree->pAdjacency;

		if (pMeshContainerToFree->Name)
			delete [] pMeshContainerToFree->Name;

		pMeshContainerToFree->MeshData.pMesh->Release();

		return S_OK;
	}

public:
	AllocateHierarchy(){};
};






////////////////////////////////////////////////////
////////              TEXTURE               ////////
////////////////////////////////////////////////////

int ResourceLoadTexture(void* _ptr)
{
	Texture* pTexture = static_cast <Texture*> (_ptr);
	if (pTexture->isLoaded())
		return 0;

	if (pTexture->getRefCount() == 0)
	{
		delete pTexture;
		return 0;
	}

	HRESULT hr = S_OK;
	IDirect3DTexture9* texture = NULL;

	WCHAR file[500];
	if (pTexture->getId() == 0)
		wcscpy(file, L"data\\Tiny_skin.dds\0");
	else if (pTexture->getId() == 1)
		wcscpy(file, L"data\\frontb.bmp\0");
	else if (pTexture->getId() == 2)
		wcscpy(file, L"data\\life.dds\0");
	else if (pTexture->getId() == 3)
		wcscpy(file, L"data\\mana.dds\0");
	else if (pTexture->getId() == 10)
		wcscpy(file, L"data\\snowflakes.dds\0");
	else if (pTexture->getId() == 11)
		wcscpy(file, L"data\\creepy_eye.dds\0");
	else if (pTexture->getId() == 12)
		wcscpy(file, L"data\\waterdrop.dds\0");


	if ((hr = D3DXCreateTextureFromFile(Graphic::getDevice(), file/*Tiny_skin.dds"*/, (IDirect3DTexture9**) &texture)) != S_OK)
		showError(true, "Error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));

	pTexture->setResource(texture);
	if (pTexture->getRefCount() == 0)
		delete pTexture;

	return 0;
};



////////////////////////////////////////////////////
////////                MESH                ////////
////////////////////////////////////////////////////

int ResourceLoadMesh(void* _ptr)
{
	Mesh* pMesh = static_cast <Mesh*> (_ptr);
	if (pMesh->isLoaded())
		return 0;

	if (pMesh->getRefCount() == 0)
	{
		delete pMesh;
		return 0;
	}

	ID3DXMesh* pMesh_resource = NULL;
	DWORD pMesh_subsets = 0;
	IDirect3DDevice9* id = Graphic::getDevice();
	HRESULT hr = S_OK;
//	if ((hr = D3DXLoadMeshFromX(L"data\\tiny.x", D3DXMESH_MANAGED, id, NULL, NULL, NULL, NULL, &pMesh_resource)) != S_OK)
//		showError(true, "Error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));


	D3DXFRAME* rootFrame;
	ID3DXAnimationController* animationController;
	AllocateHierarchy ah;
	hr = D3DXLoadMeshHierarchyFromXA("data\\tiny_4anim.x", 0, Graphic::getDevice(), &ah, 0, &rootFrame, &animationController);
	D3DXMESHCONTAINER* mc = getMeshFromFrameRoot(rootFrame);
	ID3DXMesh* mesh = mc->MeshData.pMesh;
	ID3DXSkinInfo* skinInfo = mc->pSkinInfo;
	UINT animSetNum = animationController->GetNumAnimationSets();
	AnimationInfo* ai = new AnimationInfo;
	DWORD numBones = skinInfo->GetNumBones();
	ai->pAnimController = animationController;
	ai->pRootFrame = rootFrame->pFrameFirstChild;
	delete rootFrame;
	ai->pSkinInfo = skinInfo;

	ai->m_amxBoneOffsets = new D3DXMATRIX [numBones];
	ai->m_amxWorkingPalette = new D3DXMATRIX [numBones];
	ZeroMemory(ai->m_amxWorkingPalette, sizeof(D3DXMATRIX) * numBones);
	for (UINT i=0;i<numBones;++i)
		memcpy((&(ai->m_amxBoneOffsets[i])), skinInfo->GetBoneOffsetMatrix(i), sizeof(D3DXMATRIX));

	setupBonePointers(ai->pRootFrame, &(ai->m_apmxBonePointers), ai->pSkinInfo);
	hr = S_OK;

	ai->m_dwNumPaletteEntries = min((Graphic::getDeviceCaps()->MaxVertexShaderConst - 128) / 4, skinInfo->GetNumBones());
	ai->m_amxWorkingPalette = new D3DXMATRIX [ai->m_dwNumPaletteEntries];


	vector<DWORD> meshAdjacency;
	meshAdjacency.resize(mesh->GetNumFaces() * 3);
	hr = mesh->GenerateAdjacency(EPSILON, &(meshAdjacency[0]));

	ID3DXMesh* pBlendedMesh = NULL;
	ID3DXBuffer* m_pBufBoneCombos = NULL;


	hr = ai->pSkinInfo->ConvertToIndexedBlendedMesh(mesh,
													D3DXMESH_MANAGED | D3DXMESHOPT_VERTEXCACHE,
													ai->m_dwNumPaletteEntries,
													&(meshAdjacency[0]),
													NULL,
													NULL,
													NULL,
													&(ai->m_dwMaxNumFaceInfls),
													&(ai->m_dwNumAttrGroups),
													&(ai->m_pBufBoneCombos),
													&(ai->pBlendedMesh));

	DWORD subset = 0;
	mesh->GetAttributeTable(NULL, &subset);
	pMesh->setResource(mesh, subset, ai);



// FOR ID3DXAnimationController TO WORK AS IT SHOULD YOU NEED TO USE D3DXFrameRegisterNamedMatrices()
// BECOUSE OF THIS IT IS BEST TO SET FRAMES TRANSFORMATION MATRICES TO AN ARRAY OF MATRICES



/*
//SET MESH FORMAT
	DWORD fvf = pMesh_resource->GetFVF();

	pMesh_resource->GetAttributeTable(NULL, &pMesh_subsets);
	pMesh->setResource(pMesh_resource, pMesh_subsets, NULL);
*/

	if (pMesh->getRefCount() == 0)
	{
		delete pMesh;
	}

	return 0;
};



////////////////////////////////////////////////////
////////               EFFECT               ////////
////////////////////////////////////////////////////

int ResourceLoadEffect(void* _ptr)
{
	Effect* pEffect = static_cast <Effect*> (_ptr);
	if (pEffect->isLoaded())
		return 0;

	if (pEffect->getRefCount() == 0)
	{
		delete pEffect;
		return 0;
	}



	wstring name = L"data\\ground.fx";

	if (pEffect->getId() == 10)
	{
		name = L"data\\particles_full.fx";
//		name = L"data\\particles_billboard_t1.fx";
//		name = L"data\\particles_billboard_t2.fx";
	}


	DWORD pEffect_flags = D3DXSHADER_NO_PRESHADER | D3DXSHADER_OPTIMIZATION_LEVEL3 | D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY;

	ID3DXEffect* pEffect_resource = NULL;
	ID3DXBuffer* pEffect_errorBuffer = NULL;
	HRESULT hr = S_OK;
	if ((hr = D3DXCreateEffectFromFile(Graphic::getDevice(), name.c_str(), NULL, NULL, pEffect_flags, NULL, &pEffect_resource, &pEffect_errorBuffer))
		!= S_OK)
		showError(true, "Error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));

	if (pEffect_errorBuffer)
	{
		WCHAR wsz[256];
		MultiByteToWideChar( CP_ACP, 0, ( LPSTR )pEffect_errorBuffer->GetBufferPointer(), -1, wsz, 256 );
		wsz[ 255 ] = 0;
		showError(true, wsz);
		IDX_SAFE_RELEASE(pEffect_errorBuffer);
	}


	if (pEffect_errorBuffer)
	{
		IDX_SAFE_RELEASE(pEffect_errorBuffer);
	}

	pEffect->setResource(pEffect_resource);
	if (pEffect->getRefCount() == 0)
		delete pEffect;

	return 0;
};



////////////////////////////////////////////////////
////////               LIGHT                ////////
////////////////////////////////////////////////////

int ResourceLoadLight(void* _ptr)
{
	return 0;
};









////////////////////////////////////////////////////
////////            SOUND DATA              ////////
////////////////////////////////////////////////////

int ResourceLoadSound(void* _ptr)
{
	Sound* pSound = static_cast <Sound*> (_ptr);
	if (pSound->isLoaded())
		return 0;

	if (pSound->getRefCount() == 0)
	{
		delete pSound;
		return 0;
	}

	if (pSound->getId() < 7)
	{
		UINT res = 0;

		WIN32_FIND_DATAA findFileData;

		list<pair<string, DWORD>> filenames;
		HANDLE hFind = FindFirstFileA(".\\MUSIC\\*", &findFileData);
		bool r = 1;
		while (r)
		{
			if (findFileData.cFileName[0] != '.' && findFileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
			{
				DWORD ss = (findFileData.nFileSizeHigh * (MAXDWORD+1)) + findFileData.nFileSizeLow;
				filenames.push_back(pair<string, DWORD>::pair(string(findFileData.cFileName), ss));
			}
			r = FindNextFileA(hFind, &findFileData);
		}
		FindClose(hFind);

		filenames.sort();

		UINT id = pSound->getId() % filenames.size();
		list<pair<string, DWORD>>::iterator it = filenames.begin();
		for (UINT i=0;i<id;++i)
			it++;

		string pSoundData_name = (*it).first;
		bool pSD_streamed = true;
		wchar_t* pSD_text = L"no_text";

		string path = string(".\\MUSIC\\") + (*it).first;
/*
	HANDLE hfile = CreateFileA(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
	DWORD size = (*it).second;
	BYTE* mem = new BYTE [size];
	void* pSD_audioFile = mem;
	UINT pSD_audioFileSize = size;
	DWORD rsize = 0;
	ReadFile(hfile, pSD_audioFile, size, &rsize, NULL);

	if (res = pSoundData->setResource(pSD_audioFile, pSD_audioFileSize, pSD_streamed, pSD_text))
	{
		showError(true, "Sound::setResource error no: %d", res);
		return 2;
	}
*/

		if (res = SoundSystem::readInSoundFromFile(path, true, wstring(L""), false, pSound))
		{
			showError(true, "SoundSystem::readInSoundFromFile error no: %d", res);
			return 2;
		}


		int s = 0;
		pSound->getResourcePointer()->getNumTags(&s, NULL);
		FMOD_TAG tag;
		wstring title, artist;

		for (int y=0;y<s;++y)
		{
			pSound->getResourcePointer()->getTag(NULL, y, &tag);

			switch(tag.datatype)
			{
				case FMOD_TAGDATATYPE_STRING:
				{
					if (string("TITLE") == string(tag.name))
						title = stringToWstring(string((char*)(tag.data)));
					else if (string("ARTIST") == string(tag.name))
						artist = stringToWstring(string((char*)(tag.data)));

					break;
				}
				case FMOD_TAGDATATYPE_STRING_UTF8:
				{
					if (string("TITLE") == string(tag.name))
					{
						wchar_t wstr[1024];
						UINT size = 0;
						mbstowcs_s(&size, wstr, 1024, (char*)(tag.data), 1023);

						title = wstring(wstr);
					}
					else if (string("ARTIST") == string(tag.name))
					{
						wchar_t wstr[1024];
						UINT size = 0;
						mbstowcs_s(&size, wstr, 1024, (char*)(tag.data), 1023);

						artist = wstring(wstr);
					}

					break;
				}
				default:
					break;
			}
		}

		pSound->setText(wstring(L"TITLE: ") + title + wstring(L"\n") + wstring(L"ARTIST: ") + artist + wstring(L"\n"));
	}
	else
	{
		string path = string(".\\MUSIC\\New Folder\\");
		UINT i = pSound->getId() - 7;
		switch (i)
		{
		case 0:
			path += string("StarCraft 2 - Battlecruiser Quotes1.wav");
			break;
		case 1:
			path += string("StarCraft 2 - Battlecruiser Quotes2.wav");
			break;
		case 2:
			path += string("StarCraft 2 - Battlecruiser Quotes3.wav");
			break;
		case 3:
			path += string("StarCraft 2 - Battlecruiser Quotes4.wav");
			break;
		case 4:
			path += string("StarCraft 2 - Marauder Quotes1.wav");
			break;
		case 5:
			path += string("StarCraft 2 - Marauder Quotes2.wav");
			break;
		case 6:
			path += string("StarCraft 2 - Thor Quotes1.wav");
			break;
		}

		if (int res = SoundSystem::readInSoundFromFile(path, false, wstring(L""), true, pSound))
		{
			showError(true, "SoundSystem::readInSoundFromFile error no: %d", res);
			return 2;
		}
	}

/*
	delete mem;
	CloseHandle(hfile);
*/

	return 0;
};









int (*Resource <IDirect3DTexture9*>::requestFunction)(void*) =	&ResourceLoadTexture;
int (*Resource <ID3DXMesh*>::requestFunction)(void*) =			&ResourceLoadMesh;
int (*Resource <ID3DXEffect*>::requestFunction)(void*) =		&ResourceLoadEffect;
int (*Resource <LIGHTSTRUC*>::requestFunction)(void*) =			&ResourceLoadLight;
int (*Resource <FMOD::Sound*>::requestFunction)(void*) =		&ResourceLoadSound;




#endif //_LOADFUNCTIONQQ
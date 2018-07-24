#include "Loader_v2.h"



#define		TEXTURE_SCALING_TYPE		/* D3DX_FILTER_POINT */	/* D3DX_FILTER_LINEAR */	D3DX_FILTER_TRIANGLE	/* D3DX_FILTER_BOX */



////////////////////////////////////////////////////
////////              TEXTURE               ////////
////////////////////////////////////////////////////

int Loader::resourceLoadTexture(sqlite3* _db, void* _ptr)
{
	Texture* pTexture = static_cast <Texture*> (_ptr);
	if (pTexture->isLoaded())
		return 0;

	if (pTexture->getRefCount() == 0)
	{
		delete pTexture;
		return 0;
	}


	// texture
	// +---+------------+
	// |col|    name    |
	// +---+------------+
	// | 0 | filedata   |
	// +---+------------+
	// | 1 | mip_levels |
	// +---+------------+

	sqlite3_stmt* stmt = NULL;
	ostringstream sql;
	sql << "SELECT `filedata`, `mip_levels` FROM `texture` WHERE `name` = '" << pTexture->getId() << "'";

	int result = 0;


	if ((result = sqlite3_prepare_v2(_db, sql.str().c_str(), -1, &stmt, NULL)) != SQLITE_OK)
	{
		showError(false, "sqlite3_prepare_v2 failed with error code %d:\n\"%s\"\n", result, sqlite3_errmsg(_db));
		sqlite3_finalize(stmt);
		return -100;
	}

	if ((result = sqlite3_step(stmt)) != SQLITE_ROW)
	{
		if (result != SQLITE_DONE)
		{
			showError(false, "sqlite3_step failed with error code %d:\n\"%s\"\n", result, sqlite3_errmsg(_db));
			sqlite3_finalize(stmt);
			return -200;
		}

		showError(false, "texture by name %s not found\n", pTexture->getId().c_str());
		sqlite3_finalize(stmt);
		return 1;
	}


	HRESULT hr = S_OK;
	//set texture
	IDirect3DTexture9* texture = NULL;

	if (FAILED(hr = D3DXCreateTextureFromFileInMemoryEx(Graphic::getDevice(), 
														sqlite3_column_blob(stmt,0), 
														sqlite3_column_bytes(stmt,0), 
														D3DX_DEFAULT, 
														D3DX_DEFAULT, 
														sqlite3_column_int(stmt,1), 
														D3DX_DEFAULT,
														D3DFMT_FROM_FILE,
														D3DPOOL_DEFAULT,
														TEXTURE_SCALING_TYPE | D3DX_FILTER_DITHER,
														TEXTURE_SCALING_TYPE | D3DX_FILTER_DITHER,
														0,
														NULL,
														NULL,
														&texture)))
	{
		showError(false, "D3DXCreateTextureFromFileInMemoryEx error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		sqlite3_finalize(stmt);
		return -1;
	}

	sqlite3_finalize(stmt);


	if (pTexture->setResource(texture))
	{
		IDX_SAFE_RELEASE(texture);
	}

	if (pTexture->getRefCount() == 0)
		delete pTexture;

	return 0;
};



////////////////////////////////////////////////////
////////                MESH                ////////
////////////////////////////////////////////////////

int Loader::resourceLoadMesh(sqlite3* _db, void* _ptr)
{
	Mesh* pMesh = static_cast <Mesh*> (_ptr);
	if (pMesh->isLoaded())
		return 0;

	if (pMesh->getRefCount() == 0)
	{
		delete pMesh;
		return 0;
	}


	// mesh
	// +---+--------------------+
	// |col|        name        |
	// +---+--------------------+
	// | 0 | id                 |
	// | 1 | vertex_declaration |
	// | 2 | faces_count        |
	// | 3 | vertices_count     |
	// | 4 | vertices_data      |
	// | 5 | index_data         |
	// | 6 | bs_center_x        |
	// | 7 | bs_center_y        |
	// | 8 | bs_center_z        |
	// | 9 | bs_radius          |
	// +---+--------------------+

	int result = 0;
	HRESULT hr = S_OK;
	sqlite3_stmt* stmt = NULL;
	ostringstream sql;

	sql << "SELECT `id`, `fvf_declaration`, `faces_count`, `vertices_count`, `vertices_data`, `index_data` FROM `mesh` WHERE `name` = '" << pMesh->getId() << "'";
	if ((result = sqlite3_prepare_v2(_db, sql.str().c_str(), -1, &stmt, NULL)) != SQLITE_OK)
	{
		showError(false, "sqlite3_prepare_v2 failed with error code %d:\n\"%s\"\n", result, sqlite3_errmsg(_db));
		sqlite3_finalize(stmt);
		return -100;
	}

	if ((result = sqlite3_step(stmt)) != SQLITE_ROW)
	{
		if (result != SQLITE_DONE)
		{
			showError(false, "sqlite3_step failed with error code %d:\n\"%s\"\n", result, sqlite3_errmsg(_db));
			sqlite3_finalize(stmt);
			return -200;
		}

		showError(false, "mesh by name %s not found\n", pMesh->getId().c_str());
		sqlite3_finalize(stmt);
		return 1;
	}


	UINT meshId = sqlite3_column_int(stmt, 0);

	ID3DXMesh* pMesh_resource = NULL;

	if (sqlite3_column_bytes(stmt, 1) == sizeof(DWORD))
	{
		DWORD fvf = 0;
		memcpy(&fvf, sqlite3_column_blob(stmt, 1), sizeof(DWORD));

		if (FAILED(hr = D3DXCreateMeshFVF(	(DWORD) sqlite3_column_int(stmt,2), 
											(DWORD) sqlite3_column_int(stmt,3), 
											D3DXMESH_MANAGED, 
											fvf, 
											Graphic::getDevice(), 
											&pMesh_resource)))
		{
			showError(false, "D3DXCreateMeshFVF error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			sqlite3_finalize(stmt);
			return -2;
		}
	}
	else
	{
		IDirect3DVertexDeclaration9* vertexDeclaration = NULL;
		D3DVERTEXELEMENT9* vertexElements = reinterpret_cast <D3DVERTEXELEMENT9*> (sqlite3_column_blob(stmt, 1));

		if (FAILED(hr = D3DXCreateMesh(	(DWORD) sqlite3_column_int(stmt,2), 
										(DWORD) sqlite3_column_int(stmt,3), 
										D3DXMESH_MANAGED, 
										vertexElements, 
										Graphic::getDevice(), 
										&pMesh_resource)))
		{
			showError(false, "D3DXCreateMeshFVF error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			sqlite3_finalize(stmt);
			return -3;
		}
	}

	void* memory = NULL;


	if (FAILED(hr = pMesh_resource->LockVertexBuffer(0, &memory)))
	{
		showError(false, "ID3DXMesh::LockVertexBuffer error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		sqlite3_finalize(stmt);
		IDX_SAFE_RELEASE(pMesh_resource);
		return -4;
	}

	memcpy(memory, sqlite3_column_blob(stmt, 4), sqlite3_column_bytes(stmt, 4));

	if (FAILED(hr = pMesh_resource->UnlockVertexBuffer()))
	{
		showError(false, "ID3DXMesh::UnlockVertexBuffer error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		sqlite3_finalize(stmt);
		IDX_SAFE_RELEASE(pMesh_resource);
		return -6;
	}


	if (FAILED(hr = pMesh_resource->LockIndexBuffer(0, &memory)))
	{
		showError(false, "ID3DXMesh::LockIndexBuffer error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		sqlite3_finalize(stmt);
		IDX_SAFE_RELEASE(pMesh_resource);
		return -5;
	}

	memcpy(memory, sqlite3_column_blob(stmt, 5), sqlite3_column_bytes(stmt, 5));

	if (FAILED(hr = pMesh_resource->UnlockIndexBuffer()))
	{
		showError(false, "ID3DXMesh::UnlockIndexBuffer error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		sqlite3_finalize(stmt);
		IDX_SAFE_RELEASE(pMesh_resource);
		return -7;
	}

	D3DXVECTOR3 center = D3DXVECTOR3(	(float) sqlite3_column_double(stmt, 6)
										(float) sqlite3_column_double(stmt, 7)
										(float) sqlite3_column_double(stmt, 8));
	float radius = (float) sqlite3_column_double(stmt, 9);

	sqlite3_finalize(stmt);
	sql.clear();



	// attribute_info
	// +---+--------------+
	// |col|     name     |
	// +---+--------------+
	// | 0 | attrib_id    |
	// | 1 | face_start   |
	// | 2 | face_count   |
	// | 3 | vertex_start |
	// | 4 | vertex_count |
	// +---+--------------+


	sql << "SELECT COUNT(*) AS count FROM `attribute_info` WHERE `mesh_id` = " << meshId ;
	if ((result = sqlite3_prepare_v2(_db, sql.str().c_str(), -1, &stmt, NULL)) != SQLITE_OK)
	{
		showError(false, "sqlite3_prepare_v2 failed with error code %d:\n\"%s\"\n", result, sqlite3_errmsg(_db));
		IDX_SAFE_RELEASE(pMesh_resource);
		sqlite3_finalize(stmt);
		return -100;
	}

	if ((result = sqlite3_step(stmt)) != SQLITE_ROW)
	{
		if (result != SQLITE_DONE)
		{
			showError(false, "sqlite3_step failed with error code %d:\n\"%s\"\n", result, sqlite3_errmsg(_db));
			IDX_SAFE_RELEASE(pMesh_resource);
			sqlite3_finalize(stmt);
			return -200;
		}

		showError(false, "attribute_info by id %d not found\n", meshId);
		IDX_SAFE_RELEASE(pMesh_resource);
		sqlite3_finalize(stmt);
		return 2;
	}

	UINT attributesNumber = sqlite3_column_int(stmt, 0);
	sqlite3_finalize(stmt);
	sql.clear();


	sql << "SELECT `attrib_id`, `face_start`, `face_count`, `vertex_start`, `vertex_count` FROM `attribute_info` WHERE `mesh_id` = " << meshId ;

	if ((result = sqlite3_prepare_v2(_db, sql.str().c_str(), -1, &stmt, NULL)) != SQLITE_OK)
	{
		showError(false, "sqlite3_prepare_v2 failed with error code %d:\n\"%s\"\n", result, sqlite3_errmsg(_db));
		IDX_SAFE_RELEASE(pMesh_resource);
		sqlite3_finalize(stmt);
		return -100;
	}

	if ((result = sqlite3_step(stmt)) != SQLITE_ROW)
	{
		if (result != SQLITE_DONE)
		{
			showError(false, "sqlite3_step failed with error code %d:\n\"%s\"\n", result, sqlite3_errmsg(_db));
			IDX_SAFE_RELEASE(pMesh_resource);
			sqlite3_finalize(stmt);
			return -200;
		}

		showError(false, "attribute_info by id %d not found\n", meshId);
		IDX_SAFE_RELEASE(pMesh_resource);
		sqlite3_finalize(stmt);
		return 2;
	}


	D3DXATTRIBUTERANGE* attributes = new D3DXATTRIBUTERANGE [attributesNumber];

	UINT i = 0;
	while ((result = sqlite3_step(stmt)) != SQLITE_ROW)
	{
		attributes[i].AttribId = sqlite3_column_int(stmt, 0);
		attributes[i].FaceStart = sqlite3_column_int(stmt, 1);
		attributes[i].FaceCount = sqlite3_column_int(stmt, 2);
		attributes[i].VertexStart = sqlite3_column_int(stmt, 3);
		attributes[i].VertexCount = sqlite3_column_int(stmt, 4);
		i++;
	}
	if (result != SQLITE_DONE)
	{
		showError(false, "sqlite3_step failed with error code %d:\n\"%s\"\n", result, sqlite3_errmsg(_db));
		SAFE_DELETE(attributes);
		IDX_SAFE_RELEASE(pMesh_resource);
		sqlite3_finalize(stmt);
		return -200;
	}

	sqlite3_finalize(stmt);
	sql.clear();

	if (FAILED(hr = pMesh_resource->SetAttributeTable(attributes, attributesNumber)))
	{
		showError(false, "ID3DXMesh::UnlockIndexBuffer error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		SAFE_DELETE(attributes);
		IDX_SAFE_RELEASE(pMesh_resource);
		return -8;
	}

	SAFE_DELETE(attributes);

	DWORD pMesh_subsets = attributesNumber;


	AnimationInfo* pMesh_animInfo = NULL;


	// mesh_animation JOIN animation_info
	// +---+-----------------+
	// |col|      name       |
	// +---+-----------------+
	// | 0 | bones_number    |
	// | 1 | frame_bone_data |
	// | 2 | animation_data  |
	// +---+-----------------+

	sql << "SELECT `bones_number`, `frame_bone_data`, `animation_data` FROM `mesh_animation` JOIN `animation_info` ON `animation_info_id` = `id` WHERE `mesh_id` = " << meshId;

	if ((result = sqlite3_prepare_v2(_db, sql.str().c_str(), -1, &stmt, NULL)) != SQLITE_OK)
	{
		showError(false, "sqlite3_prepare_v2 failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
		IDX_SAFE_RELEASE(pMesh_resource);
		sqlite3_finalize(stmt);
		return -100;
	}

	if ((result = sqlite3_step(stmt)) == SQLITE_ROW)
	{
		ID3DXSkinInfo* pSkinInfo = NULL;
		D3DVERTEXELEMENT9 vertexElements[65] = {0};

		if (FAILED(hr = pMesh_resource->GetDeclaration(vertexElements)))
		{
			showError(false, "ID3DXMesh::GetDeclaration error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			IDX_SAFE_RELEASE(pMesh_resource);
			sqlite3_finalize(stmt);
			return -9;
		}

		if (FAILED(hr = D3DXCreateSkinInfo(pMesh_resource->GetNumVertices(), vertexElements, sqlite3_column_int(stmt, 0), &pSkinInfo)))
		{
			showError(false, "D3DXCreateSkinInfo error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			IDX_SAFE_RELEASE(pMesh_resource);
			sqlite3_finalize(stmt);
			return -10;
		}

		D3DXMATRIX* boneOffsets = new D3DXMATRIX [sqlite3_column_int(stmt, 0)];
		D3DXFRAME* pRootFrame = NULL;

		if (int res = setupBonesHierarchyFromDataBlock( sqlite3_column_blob(stmt, 1), sqlite3_column_bytes(stmt, 1), pSkinInfo, boneOffsets, &pRootFrame ))
		{
			showError(false, "setupBonesHierarchyFromDataBlock error: %d\n", res);
			IDX_SAFE_RELEASE(pSkinInfo);
			IDX_SAFE_RELEASE(pMesh_resource);
			sqlite3_finalize(stmt);
			return 4;
		}

		//SET ANIMATION CONTROLLER
		VariableLibrary::setUsedDictionary("engine_variables");
		UINT MaxNumAnimationOutputs = VariableLibrary::getValueInt("MAX_ANIMATION_BONES");
		UINT MaxNumAnimationSets = VariableLibrary::getValueInt("MAX_ANIMATION_SETS");
		UINT MaxNumTracks = VariableLibrary::getValueInt("MAX_ANIMATION_TRACKS");
		UINT MaxNumEvents = VariableLibrary::getValueInt("MAX_ANIMATION_EVENTS");
		VariableLibrary::setUsedDictionary();

		ID3DXAnimationController* pAnimController = NULL;
		if (FAILED(hr = D3DXCreateAnimationController( MaxNumAnimationOutputs, MaxNumAnimationSets, MaxNumTracks, MaxNumEvents, &pAnimController )))
		{
			showError(false, "D3DXCreateAnimationController error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			SAFE_DELETE_ARRAY(boneOffsets);
			deleteFrameHierarchy(pRootFrame);
			IDX_SAFE_RELEASE(pSkinInfo);
			IDX_SAFE_RELEASE(pMesh_resource);
			sqlite3_finalize(stmt);
			return -11;
		}

		if (int res = setupAnimationSetsFromBlock( sqlite3_column_blob(stmt, 2), sqlite3_column_bytes(stmt, 2), pAnimController, callbackSetup ))
		{
			showError(false, "setupAnimationSetsFromBlock error: %d\n", res);
			IDX_SAFE_RELEASE(pAnimController);
			SAFE_DELETE_ARRAY(boneOffsets);
			deleteFrameHierarchy(pRootFrame);
			IDX_SAFE_RELEASE(pSkinInfo);
			IDX_SAFE_RELEASE(pMesh_resource);
			sqlite3_finalize(stmt);
			return 5;
		}

		if (int res = pMesh_animInfo->setup( pSkinInfo, pMesh_resource, min(sqlite3_column_int(stmt, 0), MATRIX_PALETTE_SIZE_DEFAULT), pAnimController, pRootFrame))
		{
			showError(false, "AnimationInfo::setup error: %d\n", res);
			IDX_SAFE_RELEASE(pAnimController);
			SAFE_DELETE_ARRAY(boneOffsets);
			deleteFrameHierarchy(pRootFrame);
			IDX_SAFE_RELEASE(pSkinInfo);
			IDX_SAFE_RELEASE(pMesh_resource);
			sqlite3_finalize(stmt);
			return 6;
		}

	}
	else if (result != SQLITE_DONE)
	{
		showError(false, "sqlite3_step failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
		IDX_SAFE_RELEASE(pMesh_resource);
		sqlite3_finalize(stmt);
		return -200;
	}

	sqlite3_finalize(stmt);


	if (pMesh->setResource(pMesh_resource, center, radius, pMesh_subsets, pMesh_animInfo))
	{
		IDX_SAFE_RELEASE(pMesh_resource);
		SAFE_RELEASE(pMesh_animInfo);
	}

	if (pMesh->getRefCount() == 0)
		delete pMesh;

	return 0;
};



////////////////////////////////////////////////////
////////               EFFECT               ////////
////////////////////////////////////////////////////

int Loader::resourceLoadEffect (sqlite3* _db, void* _ptr)
{
	Effect* pEffect = static_cast <Effect*> (_ptr);
	if (pEffect->isLoaded())
		return 0;

	if (pEffect->getRefCount() == 0)
	{
		delete pEffect;
		return 0;
	}


	// effect
	//+---+----------+
	//|col|   name   |
	//+---+----------+
	//| 0 | filedata |
	//+---+----------+

	int result = 0;
	sqlite3_stmt* stmt = NULL;
	ostringstream sql;
	sql << "SELECT `filedata`, `compiled` FROM `effect` WHERE `name` = '" << pEffect->getId() << "'";
	if ((result = sqlite3_prepare_v2(_db, sql.str().c_str(), -1, &stmt, NULL)) != SQLITE_OK)
	{
		showError(false, "sqlite3_prepare_v2 failed with error code %d:\n\"%s\"\n", result, sqlite3_errmsg(_db));
		sqlite3_finalize(stmt);
		return -100;
	}

	if ((result = sqlite3_step(stmt)) != SQLITE_ROW)
	{
		if (result != SQLITE_DONE)
		{
			showError(false, "sqlite3_step failed with error code %d:\n\"%s\"\n", result, sqlite3_errmsg(_db));
			sqlite3_finalize(stmt);
			return -200;
		}

		sqlite3_finalize(stmt);
		showError(false, "texture by name %s not found\n", pTexture->getId().c_str());
		return 1;
	}


	HRESULT hr = S_OK;
	void* sourceData = sqlite3_column_blob(stmt,0);
	UINT sourceDataLength = sqlite3_column_bytes(stmt,0);
	DWORD flags = D3DXFX_DONOTSAVESTATE | D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY | D3DXSHADER_OPTIMIZATION_LEVEL3 | D3DXFX_NOT_CLONEABLE;
	ID3DXEffect* effect = NULL;
	ID3DXBuffer* errorBuffer = NULL;

	if (FAILED(hr = D3DXCreateEffect( Graphic::getDevice(), sourceData, sourceDataLength, &(effectMacros[0]), effectIncludeMenager, flags, effectPool, &effect, &errorBuffer )))
	{
		sqlite3_finalize(stmt);
		showError(false, "D3DXCreateEffect error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));

		if (errorBuffer)
		{
			showError(false, "Error: %s \n", (char*)pEffect_errorBuffer->GetBufferPointer());
			IDX_SAFE_RELEASE(errorBuffer);
			return -2;
		}

		return -1
	}

	sqlite3_finalize(stmt);

	if (errorBuffer)
	{
		showError(false, "Error: %s \n", (char*) errorBuffer->GetBufferPointer());
		IDX_SAFE_RELEASE(errorBuffer);
		return -2;
	}


	if (pEffect->setResource(effect))
	{
		IDX_SAFE_RELEASE(effect);
	}

	if (pEffect->getRefCount() == 0)
		delete pEffect;

	return 0;
};



////////////////////////////////////////////////////
////////             SOUND_DATA             ////////
////////////////////////////////////////////////////

int Loader::resourceLoadSound(sqlite3* _db, void* _ptr)
{
	Sound* pSound = static_cast <Sound*> (_ptr);
	if (pSound->isLoaded())
		return 0;

	if (pSound->getRefCount() == 0)
	{
		delete pSound;
		return 0;
	}


	// sound
	// +---+-----------+
	// |col|   name    |
	// +---+-----------+
	// | 0 | audiofile |
	// | 1 | streamed  |
	// | 2 | is3D      |
	// | 3 | text      |
	// +---+-----------+

	int result = 0;
	sqlite3_stmt* stmt = NULL;
	ostringstream sql;

	sql << "SELECT `audiofile`, `streamed`, `is3D`, `text` FROM `sound` WHERE `name` = '" << pSound->getId() << "'";

	if ((result = sqlite3_prepare_v2(_db, sql.str().c_str(), -1, &stmt, NULL)) != SQLITE_OK)
	{
		showError(false, "sqlite3_prepare_v2 failed with error code %d:\n\"%s\"\n", result, sqlite3_errmsg(_db));
		sqlite3_finalize(stmt);
		return -100;
	}

	if ((result = sqlite3_step(stmt)) != SQLITE_ROW)
	{
		if (result != SQLITE_DONE)
		{
			showError(false, "sqlite3_step failed with error code %d:\n\"%s\"\n", result, sqlite3_errmsg(_db));
			sqlite3_finalize(stmt);
			return -200;
		}

		showError(false, "sound by name %s not found\n", pSound->getId().c_str());
		sqlite3_finalize(stmt);
		return 1;
	}


	bool streamed = sqlite3_column_int(stmt, 1) ? true : false;

	void* fileInMemory = NULL;
	vector <BYTE> fileToStream;

	const void* file = sqlite3_column_blob(stmt, 0);
	UINT fileSize = sqlite3_column_bytes(stmt, 0);

	if (streamed)
	{
		memcpy(&(fileToStream[0]), file, fileSize);
		fileInMemory = &(fileToStream[0]);
	}
	else
	{
		fileInMemory = file;
	}

	bool is3D = sqlite3_column_int(stmt, 2) ? true : false;

	wstring soundText(reinterpret_cast <wchar_t*> (sqlite3_column_blob(stmt, 3)), (UINT) (sqlite3_column_bytes(stmt, 3) / sizeof(wchar_t)));

	FMOD::Sound* fmodSound = NULL;

	if (SoundSystem::readInSoundFromMemory(fileInMemory, fileSize, streamed, is3D, &fmodSound))
	{
		showError(false, "SoundSystem::readInSoundFromMemory failed\n");
		sqlite3_finalize(stmt);
		return -1;
	}

	sqlite3_finalize(stmt);


	if (pSound->setResource(fmodSound, fileToStream, wstring soundText))
	{
		SAFE_RELEASE(fmodSound);
	}

	if (pSound->getRefCount() == 0)
		delete pSound;


	return 0;
};



////////////////////////////////////////////////////
////////             BITMAP FONT            ////////
////////////////////////////////////////////////////

int Loader::resourceLoadBitmapFont(sqlite3* _db, void* _ptr)
{
	BitmapFont* pBitmapFont = reinterpret_cast <BitmapFont*> (_ptr);

	if (pBitmapFont->isLoaded())
		return 0;

	if (pBitmapFont->getRefCount() == 0)
	{
		delete pBitmapFont;
		return 0;
	}


	// bitmap_font
	//+---+-----------------+
	//|col|      name       |
	//+---+-----------------+
	//| 0 | id              |
	//| 1 | texture_id      |
	//| 2 | font_size       |
	//| 3 | line_height     |
	//| 4 | base            |
	//| 5 | bitmap_width    |
	//| 6 | bitmap_height   |
	//| 7 | mono_type_width |
	//+---+-----------------+

	int result = 0;

	sqlite3_stmt* stmt = NULL;
	ostringstream sql;
	sql << "SELECT `id`, `texture_id`, `font_size`, `line_height`, `base`, `bitmap_width`, `bitmap_height`, `mono_type_width` FROM `bitmap_font` WHERE `name` = '" << pBitmapFont->getId() << "'";

	if ((result = sqlite3_prepare_v2(_db, sql.str().c_str(), -1, &stmt, NULL)) != SQLITE_OK)
	{
		showError(false, "sqlite3_prepare_v2 failed with error code %d:\n\"%s\"\n", result, sqlite3_errmsg(_db));
		sqlite3_finalize(stmt);
		return -100;
	}

	if ((result = sqlite3_step(stmt)) != SQLITE_ROW && result != SQLITE_DONE)
	{
		if (result != SQLITE_DONE)
		{
			showError(false, "sqlite3_step failed with error code %d:\n\"%s\"\n", result, sqlite3_errmsg(_db));
			sqlite3_finalize(stmt);
			return -200;
		}

		showError(false, "bitmap font by name %s not found\n", pBitmapFont->getId());
		sqlite3_finalize(stmt);
		return 1;
	}

	UINT fontID = sqlite3_column_int(stmt, 0);
	string textureId = sqlite3_column_text(stmt, 1);
	UINT fontSize = sqlite3_column_int(stmt, 2);
	UINT lineHeight = sqlite3_column_int(stmt, 3);
	UINT baseHeight = sqlite3_column_int(stmt, 4);
	UINT bitmapWidth = sqlite3_column_int(stmt, 5);
	UINT bitmapHeight = sqlite3_column_int(stmt, 6);
	UINT monoTypeWidth = sqlite3_column_int(stmt, 7);

	sqlite3_finalize(stmt);

	BitmapFontDescription* fontDesc = new BitmapFontDescription;
	fontDesc->setupFontDesc(Texture::getResource(textureId), fontSize, lineHeight, baseHeight, bitmapWidth, bitmapHeight, monoTypeWidth);

	sql.clear();
	sql << "SELECT COUNT(*) FROM `bitmap_character` WHERE `font_id` = " << fontID;

	if ((result = sqlite3_prepare_v2(_db, sql.str().c_str(), -1, &stmt, NULL)) != SQLITE_OK)
	{
		showError(false, "sqlite3_prepare_v2 failed with error code %d:\n\"%s\"\n", result, sqlite3_errmsg(_db));
		sqlite3_finalize(stmt);
		return -100;
	}

	if ((result = sqlite3_step(stmt)) != SQLITE_ROW && result != SQLITE_DONE)
	{
		if (result != SQLITE_DONE)
		{
			showError(false, "sqlite3_step failed with error code %d:\n\"%s\"\n", result, sqlite3_errmsg(_db));
			sqlite3_finalize(stmt);
			return -200;
		}

		showError(false, "bitmap font characters by name %s not found\n", pBitmapFont->getId());
		sqlite3_finalize(stmt);
		return 2;
	}

	UINT charsCount = sqlite3_column_int(stmt, 0);

	sqlite3_finalize(stmt);


	sql.clear();
	sql << "SELECT `char_id`, `tex_x`, `tex_y`, `tex_width`, `tex_height`, `kerning` FROM `bitmap_character` WHERE `font_id` = " << fontID;

	if ((result = sqlite3_prepare_v2(_db, sql.str().c_str(), -1, &stmt, NULL)) != SQLITE_OK)
	{
		showError(false, "sqlite3_prepare_v2 failed with error code %d:\n\"%s\"\n", result, sqlite3_errmsg(_db));
		sqlite3_finalize(stmt);
		SAFE_DELETE(fontDesc);
		return -100;
	}

	fontDesc->setupAlphabetLength(charsCount);
	int charIndex = 0;

	while ((result = sqlite3_step(stmt)) == SQLITE_ROW)
	{
		USHORT char_id = sqlite3_column_int(stmt, 0);
		USHORT tex_x = sqlite3_column_int(stmt, 1);
		USHORT tex_y = sqlite3_column_int(stmt, 2);
		USHORT tex_width = sqlite3_column_int(stmt, 3);
		USHORT tex_height = sqlite3_column_int(stmt, 4);
		void* kerning = sqlite3_column_blob(stmt, 5);
		USHORT kerningPairs = sqlite3_column_bytes(stmt, 5) / (2 * sizeof(short));

		BitmapCharacter bmChar;
		bmChar.setup(char_id, tex_x, tex_y, tex_width, tex_height, reinterpret_cast<KerningPair*> (kerning) , kerningPairs);

		fontDesc->setupAlphabetAddChar(&bmChar, charIndex);

		charIndex++;
	}

	if (result != SQLITE_DONE)
	{
		showError(false, "sqlite3_step failed with error code %d:\n\"%s\"\n", result, sqlite3_errmsg(_db));
		sqlite3_finalize(stmt);
		SAFE_DELETE(fontDesc);
		return -200;
	}

	sqlite3_finalize(stmt);


	if (pBitmapFont->setResource(fontDesc))
	{
		SAFE_DELETE(fontDesc);
	}

	if (pBitmapFont->getRefCount() == 0)
		delete pBitmapFont;

	return 0;
};



////////////////////////////////////////////////////
////////              GDI FONT              ////////
////////////////////////////////////////////////////

int Loader::resourceLoadGDIFont(sqlite3* _db, void* _ptr)
{
	GDIFont* pGDIFont = reinterpret_cast <GDIFont*> (_ptr);

	if (pGDIFont->isLoaded())
		return 0;

	if (pGDIFont->getRefCount() == 0)
	{
		delete pGDIFont;
		return 0;
	}


	// gdi_font
	//+---+-----------+
	//|col|   name    |
	//+---+-----------+
	//| 0 | font_file |
	//+---+-----------+

	int result = 0;
	sqlite3_stmt* stmt = NULL;
	ostringstream sql;
	sql << "SELECT `font_file` FROM `gdi_font` WHERE `name` = '" << pGDIFont->getId() << "'";

	if ((result = sqlite3_prepare_v2(_db, sql.str().c_str(), -1, &stmt, NULL)) != SQLITE_OK)
	{
		showError(false, "sqlite3_prepare_v2 failed with error code %d:\n\"%s\"\n", result, sqlite3_errmsg(_db));
		sqlite3_finalize(stmt);
		return -100;
	}

	if ((result = sqlite3_step(stmt)) != SQLITE_ROW && result != SQLITE_DONE)
	{
		if (result != SQLITE_DONE)
		{
			showError(false, "sqlite3_step failed with error code %d:\n\"%s\"\n", result, sqlite3_errmsg(_db));
			sqlite3_finalize(stmt);
			return -200;
		}

		showError(false, "GDI font by name %s not found\n", pGDIFont->getId());
		sqlite3_finalize(stmt);
		return 1;
	}

	UINT fontID = sqlite3_column_int(stmt, 0);

	GDIFontDescription* gdiFontDesc = NULL;

	if (int ret = setupGDIFontDesc(sqlite3_column_blob(stmt, 0), sqlite3_column_bytes(stmt, 0), &gdiFontDesc))
	{
		showError(false, "setupGDIFontDesc failed with error code %d\n", ret);
		sqlite3_finalize(stmt);
		return -1;
	}

	sqlite3_finalize(stmt);


	if (pGDIFont->setResource(gdiFontDesc))
	{
		SAFE_DELETE(gdiFontDesc);
	}

	if (pGDIFont->getRefCount() == 0)
		delete pGDIFont;

	return 0;
};




////////////////////////////////////////////////////
////////               LIGHT                ////////
////////////////////////////////////////////////////

int Loader::resourceLoadLight(sqlite3* _db, void* _ptr)
{
	Light* pLight = reinterpret_cast <Light*> (_ptr);


	// light
	//+---+------------------+
	//|col|       name       |
	//+---+------------------+
	//| 0 | light_type       |
	//| 1 | priority         |
	//| 2 | generate_shadows |
	//| 3 | color_difuse     |
	//| 4 | color_specular   |
	//| 5 | range            |
	//| 6 | attenuation0     |
	//| 7 | attenuation1     |
	//| 8 | attenuation2     |
	//| 9 | hotspot_angle    |
	//|10 | cutoff_angle     |
	//|11 | falloff          |
	//+---+------------------+

	int result = 0;
	sqlite3_stmt* stmt = NULL;
	ostringstream sql;
	sql << "SELECT `light_type`, `priority`, `generate_shadows`, `color_difuse`, `color_specular`, `range`, `attenuation0`, `attenuation1`, \
			`attenuation2`, `hotspot_angle`, `cutoff_angle`, `falloff` FROM `light` WHERE `name` = '" << pLight->getId() << "'";


	if ((result = sqlite3_prepare_v2(_db, sql.str().c_str(), -1, &stmt, NULL)) != SQLITE_OK)
	{
		showError(false, "sqlite3_prepare_v2 failed with error code %d:\n\"%s\"\n", result, sqlite3_errmsg(_db));
		sqlite3_finalize(stmt);
		return -100;
	}

	if ((result = sqlite3_step(stmt)) != SQLITE_ROW && result != SQLITE_DONE)
	{
		if (result != SQLITE_DONE)
		{
			showError(false, "sqlite3_step failed with error code %d:\n\"%s\"\n", result, sqlite3_errmsg(_db));
			sqlite3_finalize(stmt);
			return -200;
		}

		showError(false, "light by name %s not found\n", pLight->getId());
		sqlite3_finalize(stmt);
		return 1;
	}



	BYTE lightType = sqlite3_column_int(stmt, 0);
	if (lightType >= LIGHT_DIRECTIONAL)
		lightType = LIGHT_DIRECTIONAL - 1;
	BYTE lightPriority = sqlite3_column_int(stmt, 1);
	bool lightGenerateShadows = sqlite3_column_int(stmt, 2) ? true : false;

	LIGHTSTRUC lightStruc = new LIGHTSTRUC;
	D3DXCOLOR color;

	color = D3DXCOLOR((DWORD) sqlite3_column_int64(stmt, 3));
	lightStruc._color_diffuse = D3DXVECTOR4(color.r, color.g, color.b, (float) lightType);

	color = D3DXCOLOR((DWORD) sqlite3_column_int64(stmt, 4));
	lightStruc._color_specular = D3DXVECTOR4(color.r, color.g, color.b, 1.0f);


	if (lightType == LIGHT_DIRECTIONAL)
	{
		lightStruc._position = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 0.0f);
	}
	else
	{
		lightStruc._position = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f);

		lightStruc._range = sqlite3_column_double(stmt, 5);

		lightStruc._attenuation0 = sqlite3_column_double(stmt, 6);
		lightStruc._attenuation1 = sqlite3_column_double(stmt, 7);
		lightStruc._attenuation2 = sqlite3_column_double(stmt, 8);


		if (lightType == LIGHT_SPOT)
		{
			lightStruc._cos_hotspot = cos((float) sqlite3_column_double(stmt, 9));
			lightStruc._cos_cutoff = cos((float) sqlite3_column_double(stmt, 10));
			lightStruc._falloff = sqlite3_column_double(stmt, 11);

			lightStruc._spot_direction = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 0.0f);
		}
	}

	sqlite3_finalize(stmt);


	return pLight->setup(lightType, &lightStruc, lightPriority, lightGenerateShadows);
};



////////////////////////////////////////////////////
////////          PARTICLE SYSTEM           ////////
////////////////////////////////////////////////////

int Loader::resourceLoadParticleSystem(sqlite3* _db, void* _ptr)
{

	ParticleSystem_v4* pParticleSystem = reinterpret_cast <ParticleSystem_v4*> (_ptr);


	// particle_system
	//+---+-----------------------------------+
	//|col|               name                |
	//+---+-----------------------------------+
	//| 0 | texture_id                        |
	//| 1 | sprites_number                    |
	//| 2 | sprites_columns                   |
	//| 3 | sprites_rows                      |
	//| 4 | emissiveFactor                    |
	//| 5 | max_particles_number              |
	//| 6 | particleSorting                   |
	//| 7 | billboarding_type                 |
	//| 8 | particle_batches_number           |
	//| 9 | draw_only_particles_before_camera |
	//|10 | particle_processor_type           |
	//|11 | particle_processor_data           |
	//+---+-----------------------------------+



	int result = 0;
	sqlite3_stmt* stmt = NULL;
	ostringstream sql;
	sql << "SELECT `texture_id`, `sprites_number`, `sprites_columns`, `sprites_rows`, `emissiveFactor`, `max_particles_number`, `particleSorting`, \
		`billboarding_type`, `particle_batches_number`, `draw_only_particles_before_camera`, `particle_processor_type`, `particle_processor_data` \
		FROM `particle_system` WHERE `name` = '" << pParticleSystem->getId() << "'";


	if ((result = sqlite3_prepare_v2(_db, sql.str().c_str(), -1, &stmt, NULL)) != SQLITE_OK)
	{
		showError(false, "sqlite3_prepare_v2 failed with error code %d:\n\"%s\"\n", result, sqlite3_errmsg(_db));
		sqlite3_finalize(stmt);
		return -100;
	}

	if ((result = sqlite3_step(stmt)) != SQLITE_ROW && result != SQLITE_DONE)
	{
		if (result != SQLITE_DONE)
		{
			showError(false, "sqlite3_step failed with error code %d:\n\"%s\"\n", result, sqlite3_errmsg(_db));
			sqlite3_finalize(stmt);
			return -200;
		}

		showError(false, "particle system by name %s not found\n", pParticleSystem->getId());
		sqlite3_finalize(stmt);
		return 1;
	}


	Texture* tex = Texture::getResource(sqlite3_column_text(stmt, 0));

	UINT spritesNumber = sqlite3_column_int(stmt, 1);
	UINT spritesColumns = sqlite3_column_int(stmt, 2);
	UINT spritesRows = sqlite3_column_int(stmt, 3);
	float emissiveFactor = sqlite3_column_double(stmt, 4);
	UINT maxParticlesNumber = sqlite3_column_int(stmt, 5);
	DWORD sorting = sqlite3_column_int(stmt, 6);
	DWORD billboarding = sqlite3_column_int(stmt, 7);
	USHORT batchesNumber = sqlite3_column_int(stmt, 8);
	bool drawOnlyParticlesBeforeCamera = sqlite3_column_int(stmt, 9) ? true : false;
	UserDefinedParticleProcessor* particleProcessor = NULL;

	if (result = loadParticleProcessor(sqlite3_column_int(stmt, 10), sqlite3_column_blob(stmt, 11), &particleProcessor))
	{
		showError(false, "loadParticleProcessor failed with error code %d\n", result);
		sqlite3_finalize(stmt);
		return 2;
	}

	sqlite3_finalize(stmt);

	if (result = pParticleSystem->setup(	Graphic::getDevice(), 
											particleProcessor, 
											tex, 
											spritesNumber, 
											spritesColumns, 
											spritesRows, 
											emissiveFactor, 
											maxParticlesNumber, 
											sorting, 
											billboarding, 
											batchesNumber, 
											drawOnlyParticlesBeforeCamera))
	{
		showError(false, "ParticleSystem_v4::setup failed with error code %d\n", result);
		delete particleProcessor;
		return 3;
	}

	return 0;
};


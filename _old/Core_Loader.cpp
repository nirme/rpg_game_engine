#include "Core_Loader.h"


//
boost::thread Loader::thread;
LoaderQueue Loader::requestQueue;
sqlite3** Loader::ppSqlDatabase = NULL;

UINT Loader::additinalLoadTypes = 0;
loadFunc* Loader::loaderFunction = NULL;

// setting load functions in loader table for quick res type resolving
// unfortunately setting arrays members through indexes is only allowed in C
/*HRESULT (*Loader::loaderFunction[RESOURCE_NUMBER])(void*) = 
{
	/*[RESOURCE_TEXTURE]*///		&(Loader::loadTexture),
	/*[RESOURCE_EFFECT]*///		&(Loader::loadEffect),
	/*[RESOURCE_MESH]*///			&(Loader::loadMesh),
	/*[RESOURCE_SOUNDDATA]*///	&(Loader::loadSoundData),
	/*RESOURCE_LIGHT*///			&(Loader::loadLight)
/*
};
*/


///////////////////////////////////////////////////////////////////////////////////////////
////////    VERY IMPORTANT!!! IT SETS FUNCTION CONNECTING RESOURCES WITH LOADER    ////////
///////////////////////////////////////////////////////////////////////////////////////////
int (*Texture::loaderFunction)(void*) =		&resourceLoaderTexture;
int (*Effect::loaderFunction)(void*) =		&resourceLoaderEffect;
int (*Mesh::loaderFunction)(void*) =		&resourceLoaderMesh;
int (*SoundData::loaderFunction)(void*) =	&resourceLoaderSoundData;
int (*Light::loaderFunction)(void*) =		&resourceLoaderLight;




//////////////////////////////////////////////////////////////////////////////
////////                       LOADER FUNCTIONS                       ////////
//////////////////////////////////////////////////////////////////////////////
D3DXFRAME* createFrameHierarchy(const void* dataPtr, UINT memSize, ID3DXSkinInfo* pSkinInfo, D3DXMATRIX* boneOffsets)
{
	if (memSize == 0 || dataPtr == NULL || pSkinInfo == NULL)
		return NULL;

	BYTE* ptr = (BYTE*) dataPtr;
	UINT* addr;
	UINT size;
	UINT memCount = 0;
	addr = (UINT*) ptr;
	while (addr[0] != 0xFFFFFFFF && memCount < memSize)
	{
		size = addr[1];
		ptr += size;
		memCount += size;
		addr = (UINT*) ptr;
	}
	if (memCount >= memSize)
		return NULL;
	size = addr[1];

	D3DXFRAME* frame = new D3DXFRAME;
	frame->pMeshContainer = NULL;

	ptr += 2 * sizeof(UINT);
	memCount += 2 * sizeof(UINT);

	UINT id = 0;
	memcpy(&id, &(ptr[0]), sizeof(UINT));
	UINT len = 0;
	memcpy(&id, &(ptr[1]), sizeof(UINT));
	ptr += 2 * sizeof(UINT);
	memCount += 2 * sizeof(UINT);

	if (len > 0)
	{
		frame->Name = new char [len+1];
		memcpy(frame->Name, ptr, len);
		frame->Name[len] = '\0';
		ptr += len;
		memCount += len;
	}
	else

		frame->Name = NULL;

	pSkinInfo->SetBoneName((DWORD) id, frame->Name);
	pSkinInfo->SetBoneOffsetMatrix((DWORD) id, (D3DXMATRIX*) ptr);
	if (boneOffsets != NULL)
		memcpy(&(boneOffsets[id]), ptr, sizeof(D3DXMATRIX));
	ptr += sizeof(D3DXMATRIX);
	memCount += sizeof(D3DXMATRIX);

	UINT boneInfluences = 0;
	memcpy(&boneInfluences, ptr, sizeof(UINT));
	ptr += sizeof(UINT);
	memCount += sizeof(UINT);

	pSkinInfo->SetBoneInfluence(id, boneInfluences, (DWORD*) &(ptr[0]), (float*) &(ptr[boneInfluences * sizeof(UINT)]));
	ptr += (sizeof(UINT) + sizeof(float)) * boneInfluences;
	memCount += (sizeof(UINT) + sizeof(float)) * boneInfluences;

	UINT childrenSize = 0;
	memcpy(&childrenSize, ptr, sizeof(UINT));
	ptr += sizeof(UINT);
	memCount += sizeof(UINT);

	frame->pFrameFirstChild = createFrameHierarchy(ptr, childrenSize, pSkinInfo, boneOffsets);

	ptr += childrenSize;
	memCount += childrenSize;

	frame->pFrameSibling = createFrameHierarchy(ptr, memSize - memCount, pSkinInfo, boneOffsets);

	return frame;
};


HRESULT parseAnimationData(const void* data, UINT size, ID3DXAnimationController* pAnimController)
{
	ID3DXKeyframedAnimationSet* animSet = NULL;
	BYTE* ptr = (BYTE*) data;
	UINT sizePtr = 0;
	UINT* addr = NULL;

	char* name = NULL;
	double ticksPerSecond = 0.0f;
	UINT playbackType = 0;
	UINT numCallbackKeys = 0;
	D3DXKEY_CALLBACK* callbackKeys = NULL;
	UINT numAnimations = 0;

	BYTE* animationBlock = NULL;
	UINT animationBlockSize = 0;

	HRESULT hr = S_OK;


	while(sizePtr < size)
	{
		addr = (UINT*) ptr;

		if (addr[0] != ANIMATION_SET_BEGIN)
		{
			ptr += addr[1];
			sizePtr += addr[1];
			continue;
		}

		ptr += 2 * sizeof(UINT);
		sizePtr += 2 * sizeof(UINT);

		{//AnimationSet name
			addr = (UINT*) ptr;
			name = (char*) (ptr + sizeof(UINT));

			ptr += sizeof(UINT) + addr[0];
			sizePtr += sizeof(UINT) + addr[0];
		}

		{//TicksPerSecond
			memcpy(&ticksPerSecond, ptr, sizeof(double));
			ptr += sizeof(double);
			sizePtr += sizeof(double);
		}

		{//PlaybackType
			memcpy(&playbackType, ptr, sizeof(UINT));
			ptr += sizeof(UINT);
			sizePtr += sizeof(UINT);
		}

		{//callbacks skipping
			addr = (UINT*) ptr;
			ptr += (2 * sizeof(UINT)) + addr[1];
			sizePtr += (2 * sizeof(UINT)) + addr[1];
		}

		{//Animation block
			addr = (UINT*) ptr;
			numAnimations = addr[0];
			animationBlockSize = addr[1];
			animationBlock = ptr + (2 * sizeof(int));

			ptr += (2 * sizeof(int)) + animationBlockSize;
			sizePtr += (2 * sizeof(int)) + animationBlockSize;
		}

		hr = D3DXCreateKeyframedAnimationSet(	name,
												ticksPerSecond,
												(D3DXPLAYBACK_TYPE) playbackType,
												numAnimations,
												numCallbackKeys,
												callbackKeys,
												&animSet);
		if (FAILED(hr))
		return hr;

		hr = parseAnimationsBlock(animationBlock, animationBlockSize, numAnimations, animSet);
		if (FAILED(hr))
			return hr;

		hr = pAnimController->RegisterAnimationSet(animSet);
		if (FAILED(hr))
			return hr;
	}

	return S_OK;
};


HRESULT parseAnimationsBlock(const void* data, UINT size, UINT numAnimation, ID3DXKeyframedAnimationSet* pAnimSet)
{
	BYTE* ptr = (BYTE*) data;
	UINT sizePtr = 0;
	UINT numAnimCur = 0;
	UINT* addr = NULL;

	char* name;
	UINT NumScaleKeys = 0;
	D3DXKEY_VECTOR3* pScaleKeys;
	UINT NumRotationKeys = 0;
	D3DXKEY_QUATERNION* pRotationKeys;
	UINT NumTranslationKeys = 0;
	D3DXKEY_VECTOR3* pTranslationKeys;

	while (sizePtr < size && numAnimCur < numAnimation)
	{
		{//name
			addr = (UINT*) ptr;
			name = (char*) (ptr + sizeof(UINT));

			ptr += sizeof(UINT) + addr[0];
			sizePtr += sizeof(UINT) + addr[0];
		}

		{//Scale keys
			addr = (UINT*) ptr;
			NumScaleKeys = addr[0];

			ptr += sizeof(UINT);
			sizePtr += sizeof(UINT);

			pScaleKeys = (D3DXKEY_VECTOR3*) ptr;

			ptr += sizeof(D3DXKEY_VECTOR3) * NumScaleKeys;
			sizePtr += sizeof(D3DXKEY_VECTOR3) * NumScaleKeys;
		}

		{//Rotation keys
			addr = (UINT*) ptr;
			NumRotationKeys = addr[0];

			ptr += sizeof(UINT);
			sizePtr += sizeof(UINT);

			pRotationKeys = (D3DXKEY_QUATERNION*) ptr;

			ptr += sizeof(D3DXKEY_QUATERNION) * NumRotationKeys;
			sizePtr += sizeof(D3DXKEY_QUATERNION) * NumRotationKeys;
		}

		{//Translation keys
			addr = (UINT*) ptr;
			NumTranslationKeys = addr[0];

			ptr += sizeof(UINT);
			sizePtr += sizeof(UINT);

			pTranslationKeys = (D3DXKEY_VECTOR3*) ptr;

			ptr += sizeof(D3DXKEY_VECTOR3) * NumTranslationKeys;
			sizePtr += sizeof(D3DXKEY_VECTOR3) * NumTranslationKeys;
		}

		DWORD index = 0;

		HRESULT hr = pAnimSet->RegisterAnimationSRTKeys(	name,
															NumScaleKeys,
															NumRotationKeys,
															NumTranslationKeys,
															pScaleKeys,
															pRotationKeys,
															pTranslationKeys,
															&index);
		if (FAILED(hr))
			return hr;

		++numAnimCur;
	}

	return S_OK;
};
//



//////////////////////////////////////////////////////////////////////////////
////////                        LOADER METHODS                        ////////
//////////////////////////////////////////////////////////////////////////////
HRESULT Loader::Initialise(UINT _additinalLoadTypes)
{
	additinalLoadTypes = _additinalLoadTypes;

	loaderFunction = new loadFunc [RESOURCE_NUMBER + _additinalLoadTypes];

	loaderFunction[0] = &(Loader::loadTexture);
	loaderFunction[1] = &(Loader::loadEffect);
	loaderFunction[2] = &(Loader::loadMesh);
	loaderFunction[3] = &(Loader::loadSoundData);
	loaderFunction[4] = &(Loader::loadLight);


	int flags = SQLITE_OPEN_READONLY;
	ppSqlDatabase = new sqlite3* [RESOURCE_NUMBER];

	string file;
	string filepath;
// Set texture database
	{
		file = VariableLibrary::getValueStr("filenames", "RESOURCE_TEXTURE");
		filepath = string(PATH_DATA) + file;
		ppSqlDatabase[RESOURCE_TEXTURE] = NULL;
		int result = sqlite3_open_v2(filepath.c_str(), &(ppSqlDatabase[RESOURCE_TEXTURE]), flags, NULL);
		if (result != SQLITE_OK)
		{
			showError(true, "SQLite failed loading file \"%s\" with error code %d:\n\"%s\"",
						filepath.c_str(), result, sqlite3_errmsg(ppSqlDatabase[RESOURCE_TEXTURE]));
			return E_FAIL;
		}
	}

// Set effect database
	{
		file = VariableLibrary::getValueStr("filenames", "RESOURCE_EFFECT");
		filepath = string(PATH_DATA) + file;
		ppSqlDatabase[RESOURCE_EFFECT] = NULL;
		int result = sqlite3_open_v2(filepath.c_str(), &(ppSqlDatabase[RESOURCE_EFFECT]), flags, NULL);
		if (result != SQLITE_OK)
		{
			showError(true, "SQLite failed loading file \"%s\" with error code %d:\n\"%s\"",
						filepath.c_str(), result, sqlite3_errmsg(ppSqlDatabase[RESOURCE_EFFECT]));
			return E_FAIL;
		}
	}

// Set mesh database
	{
		file = VariableLibrary::getValueStr("filenames", "RESOURCE_MESH");
		filepath = string(PATH_DATA) + file;
		ppSqlDatabase[RESOURCE_MESH] = NULL;
		int result = sqlite3_open_v2(filepath.c_str(), &(ppSqlDatabase[RESOURCE_MESH]), flags, NULL);
		if (result != SQLITE_OK)
		{
			showError(true, "SQLite failed loading file \"%s\" with error code %d:\n\"%s\"",
						filepath.c_str(), result, sqlite3_errmsg(ppSqlDatabase[RESOURCE_MESH]));
			return E_FAIL;
		}
	}

// Set sounddata database
	{
		file = VariableLibrary::getValueStr("filenames", "RESOURCE_SOUNDDATA");
		filepath = string(PATH_DATA) + file;
		ppSqlDatabase[RESOURCE_SOUNDDATA] = NULL;
		int result = sqlite3_open_v2(filepath.c_str(), &(ppSqlDatabase[RESOURCE_SOUNDDATA]), flags, NULL);
		if (result != SQLITE_OK)
		{
			showError(true, "SQLite failed loading file \"%s\" with error code %d:\n\"%s\"",
						filepath.c_str(), result, sqlite3_errmsg(ppSqlDatabase[RESOURCE_SOUNDDATA]));
			return E_FAIL;
		}
	}

// Set other resources database
	{
		file = VariableLibrary::getValueStr("filenames", "RESOURCE_OTHER");
		filepath = string(PATH_DATA) + file;
		ppSqlDatabase[RESOURCE_SOUNDDATA] = NULL;
		int result = sqlite3_open_v2(filepath.c_str(), &(ppSqlDatabase[RESOURCE_LIGHT]), flags, NULL);
		if (result != SQLITE_OK)
		{
			showError(true, "SQLite failed loading file \"%s\" with error code %d:\n\"%s\"",
						filepath.c_str(), result, sqlite3_errmsg(ppSqlDatabase[RESOURCE_LIGHT]));
			return E_FAIL;
		}
	}

	thread = boost::thread(&(Loader::workerFunction));

	return S_OK;
};


HRESULT Loader::Shutdown()
{
	// Send interrupt signal which will throw thread_interrupted exception
	//and wait for the thread to finish work
	thread.interrupt();
	thread.join();

	requestQueue.Clear();

	// Close all opened DB links
	int result = SQLITE_OK;
	for (int i=0;i<RESOURCE_NUMBER;++i)
	{
		result += sqlite3_close(ppSqlDatabase[i]);
	}

	if (result != SQLITE_OK)
		return E_FAIL;
	return S_OK;
};


HRESULT Loader::workerFunction()
{
	QueueItem* item;
	while (true)
	{
		try
		{
			item = requestQueue.WaitAndGetRequest();
		}
		catch (boost::thread_interrupted e)
		{
			return S_OK;
		}

		HRESULT hr = (*loaderFunction[item->eRequestType])(item->pResourcePointer);
		delete item;
		item = NULL;

		if (FAILED(hr))
			return hr;
	}
};


HRESULT Loader::createStmt(sqlite3* _db, UINT _id, sqlite3_stmt** _stmt, const char* _sql)
{
	char sql[512];

	if (!_sql)
		sprintf_s(sql, 512, SELECT_BY_ID_QUERY, _id);
	else
		sprintf_s(sql, 512, _sql, _id);

	int result = sqlite3_prepare_v2(_db, sql, -1, _stmt, NULL);
	if (result != SQLITE_OK)
	{
		showError(true, "sqlite3_prepare_v2 failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
		return E_FAIL;
	}

	result = sqlite3_step(*_stmt);
	if (result != SQLITE_ROW)
	{
		showError(true, "sqlite3_step failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
		return E_FAIL;
	}
	return S_OK;
};



HRESULT Loader::loadTexture(void* pObject)
{
	Texture* pTexture = static_cast <Texture*> (pObject);
	if (pTexture->isLoaded())
		return S_OK;

	sqlite3_stmt* stmt = NULL;

	HRESULT result = Loader::createStmt(ppSqlDatabase[RESOURCE_TEXTURE], pTexture->getId(), &stmt);
	if (result != S_OK)
	{
		sqlite3_finalize(stmt);
		return result;
	}

	{
		// column 1 is always "name" and may be empty
		if (sqlite3_column_bytes(stmt,1) != 1)
		{
			result = pTexture->setName((char*) sqlite3_column_text(stmt, 1));
			if (result != S_OK)
			{
				sqlite3_finalize(stmt);
				return result;
			}
		}

		IDirect3DBaseTexture9* texture = NULL;
		result = D3DXCreateTextureFromFileInMemory(	Graphic::GetDevice(),
													sqlite3_column_blob(stmt,2),
													sqlite3_column_bytes(stmt,2),
													(IDirect3DTexture9**) &texture);
		if (result != S_OK)
		{
			sqlite3_finalize(stmt);
			return result;
		}

		pTexture->setResource(texture);
	}

	sqlite3_finalize(stmt);
	return S_OK;
};


HRESULT Loader::loadEffect(void* pObject)
{
	Effect* pEffect = static_cast <Effect*> (pObject);
	if (pEffect->isLoaded())
		return S_OK;

	sqlite3_stmt* stmt = NULL;

	HRESULT result = createStmt(ppSqlDatabase[RESOURCE_EFFECT], pEffect->getId(), &stmt);
	RETURN_IF_ERROR(result);

	{
		// column 1 is always "name" and may be empty
		if (sqlite3_column_bytes(stmt,1) != 1)
		{
			result = pEffect->setName((char*) sqlite3_column_text(stmt, 1));
			RETURN_IF_ERROR(result);
		}


		D3DXMACRO* pMacros = NULL;

		{
			sqlite3* db = ppSqlDatabase[RESOURCE_EFFECT];
			sqlite3_stmt* stmt2 = NULL;
			char sql2[512];

			sprintf_s(sql2, 512, "SELECT COUNT(*) FROM d3dxmacro WHERE effect_id = %d ", pEffect->getId());
			int result = sqlite3_prepare_v2(db, sql2, -1, &stmt2, NULL);
			if (result != SQLITE_OK)
			{
				showError(true, "sqlite3_prepare_v2 failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(db));
				return E_FAIL;
			}
			sqlite3_step(stmt2);
			int defSize = sqlite3_column_int(stmt2,0);
			sqlite3_finalize(stmt2);

			if (defSize > 0)
			{
				pMacros = new D3DXMACRO [defSize+1];

				sprintf_s(sql2, 512, "SELECT name, definition FROM d3dxmacro WHERE effect_id = %d ", pEffect->getId());
				int result = sqlite3_prepare_v2(db, sql2, -1, &stmt2, NULL);
				if (result != SQLITE_OK)
				{
					showError(true, "sqlite3_prepare_v2 failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(db));
					return E_FAIL;
				}

				int i = 0;
				char* tmp = NULL;
				while (sqlite3_step(stmt2) == SQLITE_ROW)
				{
					setStringA(&tmp, (char*) sqlite3_column_text(stmt2,0));
					pMacros[i].Name = tmp;
					tmp = NULL;
					setStringA(&tmp, (char*) sqlite3_column_text(stmt2,1));
					pMacros[i].Definition = tmp;
					tmp = NULL;
					++i;
				}
				pMacros[i].Name = NULL;
				pMacros[i].Definition = NULL;

				sqlite3_finalize(stmt2);
			}
		}


		bool compiled = (sqlite3_column_int(stmt, 3) == 0) ? false : true;

// usable only if effect file in memory is not compiled text file
		DWORD flags_for_text = D3DXSHADER_OPTIMIZATION_LEVEL3 | D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY;
// not sure about D3DXFX_LARGEADDRESSAWARE so better leave it out for now
		DWORD flags = D3DXFX_DONOTSAVESTATE | D3DXFX_NOT_CLONEABLE; // | D3DXFX_LARGEADDRESSAWARE

		if (!compiled)
			flags = flags | flags_for_text;

		ID3DXEffect* effect = NULL;
		ID3DXBuffer* errBuffer = NULL;
		HRESULT result = D3DXCreateEffect(	Graphic::GetDevice(),
											sqlite3_column_blob(stmt,1),
											sqlite3_column_bytes(stmt,1),
											pMacros,
											NULL,
											flags,
											NULL,
											&effect,
											&errBuffer);

		if (result != D3D_OK)
		{
			effect->release();
			if (errBuffer)
			{
				char* err = NULL;
				setStringA(&err, static_cast <char*> (errBuffer->GetBufferPointer()));
				errBuffer->release();
				showError(true, "D3DXCreateEffect failed with compile errors:\n%s", err);
				setStringA(&err, NULL);
			}
			return result;
		}

		if (pMacros)
		{
			int i = 0;
			char* tmp = NULL;
			while (pMacros[i].Name != NULL)
			{
				tmp = const_cast <char*> (pMacros[i].Definition);
				setStringA(&tmp);
				tmp = const_cast <char*> (pMacros[i].Name);
				setStringA(&tmp);
				i++;
			}
		}

		SAFE_RELEASE(errBuffer);

		pEffect->setResource(effect);
	}

	sqlite3_finalize(stmt);
	return S_OK;
};


HRESULT Loader::loadMesh(void* pObject)
{
	Mesh* pMesh = static_cast <Mesh*> (pObject);
	if (pMesh->isLoaded())
		return S_OK;

	sqlite3_stmt* stmt = NULL;

	HRESULT result = createStmt(ppSqlDatabase[RESOURCE_MESH], pMesh->getId(), &stmt);
	RETURN_IF_ERROR(result);

	ID3DXMesh* mesh = NULL;
	DWORD subsets = 1;
	BoundingSphere* boundingVol = NULL;
	AnimationInfo* animInfo = NULL;


	// column 1 is always "name" and may be empty
	if (sqlite3_column_bytes(stmt,1) != 1)
	{
		result = pMesh->setName((char*) sqlite3_column_text(stmt, 1));
		RETURN_IF_ERROR(result);
	}

//if vertex declaration is in std FVF
	if (sqlite3_column_bytes(stmt,2) == 4)
	{
		DWORD fvf = 0;
		memcpy(&fvf, sqlite3_column_blob(stmt,2), 4);

		result = D3DXCreateMeshFVF(	(DWORD) sqlite3_column_int(stmt,3),
									(DWORD)sqlite3_column_int(stmt,4),
									D3DXMESH_MANAGED,
									fvf,
									Graphic::GetDevice(),
									&mesh);
		if (result != D3D_OK)
		{
			showError(true, "D3DXCreateMeshFVF failed with error code %d", result);
			return E_FAIL;
		}


		// set mesh data
		{
			void* memory = NULL;

			// load vertex data
			result = mesh->LockVertexBuffer(0, &memory);
			RETURN_IF_ERROR(result);
			memcpy(memory, sqlite3_column_blob(stmt,5), sqlite3_column_bytes(stmt,5));
			result = mesh->UnlockVertexBuffer();
			RETURN_IF_ERROR(result);
			memory = NULL;

			// load index data
			result = mesh->LockIndexBuffer(0,&memory);
			RETURN_IF_ERROR(result);
			memcpy(memory, sqlite3_column_blob(stmt,6), sqlite3_column_bytes(stmt,6));
			result = mesh->UnlockIndexBuffer();
			RETURN_IF_ERROR(result);
			memory = NULL;
		}


		// set attribute table
		{
			D3DXATTRIBUTERANGE* attributeTable = NULL;

			sqlite3* db = ppSqlDatabase[RESOURCE_MESH];
			sqlite3_stmt* stmt2 = NULL;
			char sql[512];

			sprintf_s(sql, 512, "SELECT COUNT(*) FROM attribute_info WHERE res_id = %d ", pMesh->getId());
			int res = sqlite3_prepare_v2(db, sql, -1, &stmt2, NULL);
			if (res != SQLITE_OK)
			{
				showError(true, "sqlite3_prepare_v2 failed with error code %d:\n\"%s\"", res, sqlite3_errmsg(db));
				return E_FAIL;
			}
			sqlite3_step(stmt2);
			int size = sqlite3_column_int(stmt2, 0);
			attributeTable = new D3DXATTRIBUTERANGE [size];
			sqlite3_finalize(stmt2);


			sprintf_s(sql, 512, "SELECT * FROM attribute_info WHERE res_id = %d ORDER BY attrib_id ", pMesh->getId());
			res = sqlite3_prepare_v2(db, sql, -1, &stmt2, NULL);
			if (res != SQLITE_OK)
			{
				showError(true, "sqlite3_prepare_v2 failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(db));
				return E_FAIL;
			}

			int i = 0;
			while (sqlite3_step(stmt2) == SQLITE_ROW)
			{
				attributeTable[i].AttribId = (DWORD) sqlite3_column_int(stmt2, 1);
				attributeTable[i].FaceStart = (DWORD) sqlite3_column_int(stmt2, 2);
				attributeTable[i].FaceCount = (DWORD) sqlite3_column_int(stmt2, 3);
				attributeTable[i].VertexStart = (DWORD) sqlite3_column_int(stmt2, 4);
				attributeTable[i].VertexCount = (DWORD) sqlite3_column_int(stmt2, 5);
				++i;
			}

			sqlite3_finalize(stmt2);

			mesh->SetAttributeTable(attributeTable, (DWORD) size);
			subsets = (DWORD) size;
		}

		//create bouding volume of exact type
		int type = sqlite3_column_int(stmt, 7);
		if (type != BOUNDING_VOLUME_NONE)
		{
			if (sqlite3_column_bytes(stmt, 8) > 0)
			{
				// pointers to connect to memory from table
				D3DXVECTOR3* volumeCenter = NULL;
				float* maxRadius = NULL;
				D3DXVECTOR3* boxMax = NULL;
				D3DXVECTOR3* boxMin = NULL;
				D3DXVECTOR3* cylinderBottomCenter = NULL;
				float* cylinderHeight = NULL;
				float* cylinderRadius = NULL;
				//

				short addr = 0;
				short addrSize = 0;
				int ptrSize = 0;
				int size = sqlite3_column_bytes(stmt, 8);
				const void* mem = sqlite3_column_blob(stmt, 8);
				BYTE* ptr = (BYTE*) mem;

				do
				{
					memcpy(&addr, ptr, sizeof(short));
					ptr += sizeof(short);
					ptrSize += sizeof(short);
					memcpy(&addrSize, ptr, sizeof(short));
					ptr += sizeof(short);
					ptrSize += sizeof(short);

					switch (addr)
					{
					case FLOAT_MAX_RADIUS:
						maxRadius = (float*) ptr;
						break;
					case FLOAT_CYLINDER_HEIGHT:
						cylinderHeight = (float*) ptr;
						break;
					case FLOAT_CYLINDER_RADIUS:
						cylinderRadius = (float*) ptr;
						break;

					case VECTOR3_VOLUME_CENTER:
						volumeCenter = (D3DXVECTOR3*) ptr;
						break;
					case VECTOR3_BOX_MAX:
						boxMax = (D3DXVECTOR3*) ptr;
						break;
					case VECTOR3_BOX_MIN:
						boxMin = (D3DXVECTOR3*) ptr;
						break;
					case VECTOR3_CYLINDER_BOTTOM_CENTER:
						cylinderBottomCenter = (D3DXVECTOR3*) ptr;
						break;
					}

					ptr += addrSize;
					ptrSize += addrSize;
				}
				while(ptrSize < size);


				switch (type)
				{
				case BOUNDING_VOLUME_SPHERE:
					boundingVol = new BoundingSphere(*volumeCenter, *maxRadius);
					break;
				case BOUNDING_VOLUME_BOX:
					boundingVol = new BoundingBox(*boxMax, *boxMin);
					break;
				case BOUNDING_VOLUME_CYLINDER:
					boundingVol = new BoundingCylinder(*cylinderBottomCenter, *cylinderHeight, *cylinderRadius);
					break;
				default:
					boundingVol = new BoundingSphere(*volumeCenter, *maxRadius);
					break;
				}

			}
			else
			{
				void* mem;
				mesh->LockVertexBuffer(0, &mem);

				switch (type)
				{
				case BOUNDING_VOLUME_SPHERE:
					boundingVol = new BoundingSphere((D3DXVECTOR3*) mem, mesh->GetNumVertices(), mesh->GetNumBytesPerVertex());
					break;

				case BOUNDING_VOLUME_BOX:
					boundingVol = new BoundingBox((D3DXVECTOR3*) mem, mesh->GetNumVertices(), mesh->GetNumBytesPerVertex());
					break;

				case BOUNDING_VOLUME_CYLINDER:
					boundingVol = new BoundingCylinder((D3DXVECTOR3*) mem, mesh->GetNumVertices(), mesh->GetNumBytesPerVertex());
					break;

				default:
					boundingVol = new BoundingSphere((D3DXVECTOR3*) mem, mesh->GetNumVertices(), mesh->GetNumBytesPerVertex());
					break;
				}

				mesh->UnlockVertexBuffer();
			}
		}


		//create animation info
		{
			char sql[512];
			sqlite3* db = ppSqlDatabase[RESOURCE_MESH];
			sqlite3_stmt* stmt2 = NULL;
			sprintf_s(sql, 512, "SELECT * FROM animation_info WHERE res_id = %d ", pMesh->getId());

			int result = sqlite3_prepare_v2(db, sql, -1, &stmt2, NULL);
			if (result != SQLITE_OK)
			{
				showError(true, "sqlite3_prepare_v2 failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(db));
				return E_FAIL;
			}

			result = sqlite3_step(stmt2);
			if (result != SQLITE_ROW && result != SQLITE_DONE)
			{
				showError(true, "sqlite3_step failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(db));
				return E_FAIL;
			}

			if (result == SQLITE_ROW)
			{

				animInfo = new AnimationInfo;
				D3DXCreateSkinInfoFVF(	mesh->GetNumVertices(),
										mesh->GetFVF(),
										(DWORD) sqlite3_column_int(stmt2,1),
										&(animInfo->pSkinInfo));
				animInfo->m_amxBoneOffsets = new D3DXMATRIX [sqlite3_column_int(stmt2,1)];
				if (animInfo->m_amxBoneOffsets == NULL)
					return E_OUTOFMEMORY;

				const void* mem = sqlite3_column_blob(stmt2,2);

				animInfo->pRootFrame = createFrameHierarchy(sqlite3_column_blob(stmt2,2),
															(UINT) sqlite3_column_bytes(stmt2,2),
															animInfo->pSkinInfo,
															animInfo->m_amxBoneOffsets);

				animInfo->m_dwNumPaletteEntries = min(	Graphic::GetDeviceCaps()->MaxVertexShaderConst - 10,
														animInfo->pSkinInfo->GetNumBones());

				animInfo->m_amxWorkingPalette = new D3DXMATRIX [animInfo->m_dwNumPaletteEntries];
				if (animInfo->m_amxWorkingPalette == NULL)
					return E_OUTOFMEMORY;

				setupBonePointers(animInfo->pRootFrame, &(animInfo->m_apmxBonePointers), animInfo->pSkinInfo);

				DWORD* adjacency = new DWORD [mesh->GetNumFaces() * 3];
				mesh->GenerateAdjacency(EPSILON, adjacency);
				ID3DXMesh* animMesh = NULL;
				HRESULT hr = animInfo->pSkinInfo->ConvertToIndexedBlendedMesh(	mesh,
																				D3DXMESH_MANAGED | D3DXMESHOPT_VERTEXCACHE,
																				animInfo->m_dwNumPaletteEntries,
																				adjacency,
																				NULL,
																				NULL,
																				NULL,
																				&(animInfo->m_dwMaxNumFaceInfls),
																				&(animInfo->m_dwNumAttrGroups),
																				&(animInfo->m_pBufBoneCombos),
																				&animMesh);
				if( FAILED( hr ) )
					return hr;

				delete [] adjacency;

				animInfo->pOrigMesh = mesh;
				mesh = animMesh;


				ID3DXAnimationController* pAnimController = NULL;

				VariableLibrary::setUsedDictionary("engine_variables");
				hr = D3DXCreateAnimationController(	VariableLibrary::getValueInt("MAX_ANIMATION_BONES"),
													VariableLibrary::getValueInt("MAX_ANIMATION_SETS"),
													VariableLibrary::getValueInt("MAX_ANIMATION_TRACKS"),
													VariableLibrary::getValueInt("MAX_ANIMATION_EVENTS"),
													&pAnimController);
				VariableLibrary::setUsedDictionary(NULL);

				if( FAILED( hr ) )
					return hr;


				hr = parseAnimationData(	sqlite3_column_blob(stmt2,3),
											sqlite3_column_bytes(stmt2,3),
											pAnimController);
				if( FAILED( hr ) )
					return hr;

				hr = D3DXFrameRegisterNamedMatrices(animInfo->pRootFrame, pAnimController);
				if( FAILED( hr ) )
					return hr;

				animInfo->pAnimController = pAnimController;
			}

			sqlite3_finalize(stmt2);

		}

		pMesh->setResource(mesh, subsets, boundingVol, animInfo);
	}

	sqlite3_finalize(stmt);
	return S_OK;
};


HRESULT Loader::loadSoundData(void* pObject)
{
	SoundData* pSoundData = static_cast <SoundData*> (pObject);
	if (pSoundData->isLoaded())
		return S_OK;

	sqlite3_stmt* stmt = NULL;

	HRESULT result = createStmt(ppSqlDatabase[RESOURCE_SOUNDDATA], pSoundData->getId(), &stmt);
	RETURN_IF_ERROR(result);

	{
		const void* mem = sqlite3_column_blob(stmt, 4);
		wchar_t* text = (wchar_t*) mem;
		result = pSoundData->setResource(	sqlite3_column_blob(stmt,3),
											sqlite3_column_bytes(stmt,3),
											((sqlite3_column_int(stmt, 2) == 0) ? false : true),
											(char*) sqlite3_column_text(stmt, 1),
											text);

		RETURN_IF_ERROR(result);
	}

	sqlite3_finalize(stmt);
	return S_OK;
};


HRESULT Loader::loadLight(void* pObject)
{
	Light* pLight = static_cast <Light*> (pObject);

	if (pLight->LoadLightFromMap() == S_OK)
		return S_OK;

	sqlite3_stmt* stmt = NULL;

	HRESULT result = Loader::createStmt(ppSqlDatabase[RESOURCE_LIGHT], pLight->getId(), &stmt, "SELECT * FROM light_info WHERE id = %d");
	if (result != S_OK)
	{
		sqlite3_finalize(stmt);
		return result;
	}

	{
		D3DLIGHT9 ls;
		memcpy(&ls, 0, sizeof(D3DLIGHT9));

		ls.Type = (D3DLIGHTTYPE) sqlite3_column_int(stmt,1);
		colorValueFromDword((DWORD) sqlite3_column_int(stmt,2), &(ls.Diffuse));
		colorValueFromDword((DWORD) sqlite3_column_int(stmt,3), &(ls.Specular));
		colorValueFromDword((DWORD) sqlite3_column_int(stmt,4), &(ls.Ambient));
		ls.Position = LIGHT_BASE_POSITION;
		ls.Direction = LIGHT_BASE_DIRECTION;
		ls.Range = (float) sqlite3_column_double(stmt,5);
		ls.Falloff = (float) sqlite3_column_double(stmt,6);
		ls.Attenuation0 = (float) sqlite3_column_double(stmt,7);
		ls.Attenuation1 = (float) sqlite3_column_double(stmt,8);
		ls.Attenuation2 = (float) sqlite3_column_double(stmt,9);
		ls.Theta = (float) sqlite3_column_double(stmt,10);
		ls.Phi = (float) sqlite3_column_double(stmt,11);

		pLight->LoadToResourceMap(ls);
	}

	sqlite3_finalize(stmt);
	return S_OK;

};


//////////////////////////////////////////////////////////////////////////////
////////                  RESOURCE LOADERS FUNCTIONS                  ////////
//////////////////////////////////////////////////////////////////////////////

int resourceLoaderTexture(void* pItem)
{
	QueueItem* item = new QueueItem(pItem, RESOURCE_TEXTURE);
	Loader::AddRequest(item);
	return 0;
};

int resourceLoaderEffect(void* pItem)
{
	QueueItem* item = new QueueItem(pItem, RESOURCE_EFFECT);
	Loader::AddRequest(item);
	return 0;
};

int resourceLoaderMesh(void* pItem)
{
	QueueItem* item = new QueueItem(pItem, RESOURCE_MESH);
	Loader::AddRequest(item);
	return 0;
};

int resourceLoaderSoundData(void* pItem)
{
	QueueItem* item = new QueueItem(pItem, RESOURCE_SOUNDDATA);
	Loader::AddRequest(item);
	return 0;
};

int resourceLoaderLight(void* pItem)
{
	QueueItem* item = new QueueItem(pItem, RESOURCE_LIGHT);
	Loader::AddRequest(item);
	return 0;
};

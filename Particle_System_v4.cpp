#include "Particle_System_v4.h"




UINT							ParticleSystem_v4::particleQuadRefCount		= 0;
IDirect3DVertexBuffer9*			ParticleSystem_v4::pVertexBuffer			= NULL;
IDirect3DVertexDeclaration9*	ParticleSystem_v4::pVertexDeclaration		= NULL;



ParticleSystem_v4::ParticleSystem_v4(string _id, WorldPosition* _pPosition)
{
	initialized = false;

	id = _id;
	pPosition = _pPosition;

	maxParticlesNumber = 0;
	pTexture = NULL;
	pInstanceBuffer = NULL;
	pParticles = NULL;
	pParticleListFirstElement = NULL;
	pParticleListLastElement = NULL;
	particleListSize = 0;
	drawOnlyParticlesBeforeCamera = false;
	numberOfParticlesToRender = 0;
	particleInsertStamp = 0;
	particlesToEmit = 0.0f;
	instancesActual = false;
	spritesNumber = 0;
	sprites = D3DXVECTOR2(0.0f, 0.0);
	pMaterial = NULL;
	billboardingType = 0;
	particleProcessor = NULL;
	updatesSorted = 0;
	particleSortingType = 0;
	accumulatedTime = 0.0f;
	updateTimeStamp = 0;
	lastInsertSorted = false;
};


ParticleSystem_v4::~ParticleSystem_v4()
{
	if (initialized)
		shutdown();
};


string ParticleSystem_v4::getId()
{
	return id;
};

int ParticleSystem_v4::setId(string _id)
{
	id = _id;
	return 0;
};



int ParticleSystem_v4::setup(	IDirect3DDevice9* _pDevice, 
								UserDefinedParticleProcessor* _particleProcessor, 
								Texture* _texture, 
								UINT _spritesNumber, 
								UINT _spritesColumns, 
								UINT _spritesRows, 
								float _emissiveFactor, 
								UINT _maxParticlesNumber, 
								DWORD _particleSorting,
								DWORD _billboardingType, 
								UINT _particleBatches, 
								bool _drawOnlyParticlesBeforeCamera)
{
	if (initialized)
		return 1;

	{ // safe check
		if (!_pDevice)
			return 101;

		if (!_particleProcessor)
			return 102;

		if (!_texture)
			return 103;

		if (!_spritesNumber)
			return 104;

		if (!_spritesColumns)
			return 105;

		if (!_spritesRows)
			return 106;

		if (!_maxParticlesNumber)
			return 107;

		if (_particleBatches > 8)
			return 108;
	}


	if (int res = setBuffers(_pDevice, _maxParticlesNumber))
	{
		return res + (res > 0 ? 100 : -100);
	}

	pTexture = _texture;

	spritesNumber = _spritesNumber;
	sprites = D3DXVECTOR2(float(_spritesColumns), float(_spritesRows));

	emissiveFactor = _emissiveFactor;

	billboardingType = _billboardingType <= 2 ? _billboardingType : 2;

	particleProcessor = _particleProcessor;

	particleProcessor->setSpritesNumber(spritesNumber);
	particleProcessor->setMaxParticlesNumber(_maxParticlesNumber);

	pParticleListFirstElement = NULL;
	pParticleListLastElement = NULL;
	particleListSize = 0;

	if (_particleBatches > 1)
		vParticleBatch.resize(_particleBatches);

	particleInsertStamp = 0;
	particlesToEmit = 0.0f;
	instancesActual = false;
	lastInsertSorted = false;

	particleSortingType = _particleSorting <= 2 ? _particleSorting : 2;

	drawOnlyParticlesBeforeCamera = _drawOnlyParticlesBeforeCamera;

	numberOfParticlesToRender = 0;

	updatesSorted = 0;

	accumulatedTime = 0.0f;
	updateTimeStamp = 0;


	initialized = true;

	return 0;
};


int ParticleSystem_v4::shutdown()
{
	if (!initialized)
		return 1;

	setBuffers(NULL, 0);

	SAFE_RELEASE(pTexture);

	pParticleListFirstElement = NULL;
	pParticleListLastElement = NULL;
	particleListSize = 0;

	drawOnlyParticlesBeforeCamera = false;
	numberOfParticlesToRender = 0;

	vParticleBatch.resize(0);
	particleInsertStamp = 0;

	particlesToEmit = 0;

	instancesActual = false;

	sprites = D3DXVECTOR2(0.0f, 0.0f);
	emissiveFactor
	SAFE_DELETE(pMaterial);

	billboardingType = 0;

	SAFE_DELETE(particleProcessor);

	updatesSorted = 0;
	particleSortingType = 0;

	accumulatedTime = 0.0f;
	updateTimeStamp = 0;

	emissiveFactor = 0.0f;
	lastInsertSorted = false;


	return 0;
};




int ParticleSystem_v4::update(float _timeDelta, Camera* _camera)
{
	if (!initialized)
		return 1;

	accumulatedTime += _timeDelta;
	updateTimeStamp ++;

	particleProcessor->updateSystem(_timeDelta);

	int res = 0;

	if (!_camera || particleSortingType == PARTICLE_SORTING_NONE)
	{
		lastInsertSorted = false;

		if (res = updateParticlesNoSorting(_timeDelta))
			return res + (res > 0 ? 100 : -100);

		if (res = emitParticlesNoSorting(_timeDelta))
			return res + (res > 0 ? 200 : -200);

		return 0;
	}


	D3DXPLANE viewPlane;
	createCameraPlaneInLocalCoord(&viewPlane, _camera);

	if (particleSortingType == PARTICLE_SORTING_PARTIAL && !(drawOnlyParticlesBeforeCamera && vParticleBatch.size() > 1))
	{
		lastInsertSorted = true;

		if (res = updateParticlesWithDistanceFromCamera( _timeDelta, viewPlane))
			return res + (res > 0 ? 100 : -100);

		if (res = emitParticlesInsertSorting(_timeDelta, viewPlane))
			return res + (res > 0 ? 200 : -200);

		if (res = sortSinglePassBubbleSort())
			return res + (res > 0 ? 300 : -300);

		return 0;
	}

	//particleSortingType == PARTICLE_SORTING_FULL || (drawOnlyParticlesBeforeCamera && vParticleBatch.size() > 1)
	if (vParticleBatch.size() > 1)
	{
		if (res = updateParticlesNoSorting(_timeDelta))
			return res + (res > 0 ? 100 : -100);

		if (res = emitParticlesNoSorting(_timeDelta))
			return res + (res > 0 ? 200 : -200);

		if (res = updateParticlesDistanceFromCamera(viewPlane))
			return res + (res > 0 ? 400 : -400);
	}
	else
	{
		if (res = updateParticlesWithDistanceFromCamera( _timeDelta, viewPlane))
			return res + (res > 0 ? 100 : -100);

		if (res = emitParticlesInsertSorting(_timeDelta, viewPlane))
			return res + (res > 0 ? 200 : -200);
	}

	if (res = sortAllParticles())
		return res + (res > 0 ? 500 : -500);

	return 0;
};


int ParticleSystem_v4::updateBeforeDrawing(IDirect3DDevice9* _pDevice, Camera* _camera)
{
	int res = 0;

	if (particleSortingType == PARTICLE_SORTING_PARTIAL && !lastInsertSorted)
	{
		D3DXPLANE viewPlane;
		createCameraPlaneInLocalCoord(&viewPlane, _camera);

		if (res = updateParticlesDistanceFromCamera(viewPlane))
			return res + (res > 0 ? 1000 : -1000);
	}


	if (particleSortingType == PARTICLE_SORTING_FULL || (particleSortingType == PARTICLE_SORTING_PARTIAL && !lastInsertSorted))
		if ((res = sortAllParticles()) < 0)
			return res - 100;

	lastInsertSorted = true;

	if (res = updateInstances())
		return res + (res > 0 ? 100 : -100);

	return 0;
};


int ParticleSystem_v4::sortParticles(Camera* _camera)
{
	if (!_camera)
		return 1;

	D3DXPLANE viewPlane;
	createCameraPlaneInLocalCoord(&viewPlane, _camera);

	int res = 0;
	if (res = updateParticlesDistanceFromCamera(viewPlane))
		return res + (res > 0 ? 1000 : -1000);

	if (res = sortAllParticles())
		return res + (res > 0 ? 2000 : -2000);

	return 0;
};


int ParticleSystem_v4::particleQuadAddRef()
{
	if (!particleQuadRefCount)
	{
		void* pVoid;

		//create particle quad
		ParticleVertex vertices[4];
		vertices[0] = ParticleVertex(-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0, 1);
		vertices[1] = ParticleVertex(-0.5f, 0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0, 0);
		vertices[2] = ParticleVertex(0.5f, 0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1, 0);
		vertices[3] = ParticleVertex(0.5f, -0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1, 1);


		if (FAILED(hr = _pDevice->CreateVertexBuffer(sizeof(vertices), D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &pVertexBuffer, NULL)))
		{
			showError(false, "IDirect3DDevice9::CreateVertexBuffer error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -1;
		}


		if (FAILED(hr = pVertexBuffer->Lock(0, 0, &pVoid, D3DLOCK_NOSYSLOCK)))
		{
			IDX_SAFE_RELEASE(pVertexBuffer);
			showError(false, "IDirect3DVertexBuffer9::Lock error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -2;
		}

		memcpy(pVoid, vertices, sizeof(vertices));

		if (FAILED(hr = pVertexBuffer->Unlock()))
		{
			IDX_SAFE_RELEASE(pVertexBuffer);
			showError(false, "IDirect3DVertexBuffer9::Unlock error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -3;
		}


		UINT particleVertexPositionOffset		= 0;
		UINT particleVertexNormalOffset			= sizeof(D3DXVECTOR3);
		UINT particleVertexTangentOffset		= 2 * sizeof(D3DXVECTOR3);
		UINT particleVertexBitangentOffset		= 3 * sizeof(D3DXVECTOR3);
		UINT particleVertexUVindexOffset		= 4 * sizeof(D3DXVECTOR3);

		UINT particleInstanceTranslationOffset	= 0;
		UINT particleInstanceRotationOffset		= sizeof(D3DXVECTOR3);
		UINT particleInstanceSizeOffset			= 2 * sizeof(D3DXVECTOR3);
		UINT particleInstanceSpriteIndexOffset	= 2 * sizeof(D3DXVECTOR3) + sizeof(float);
		UINT particleInstanceColorOffset		= 2 * sizeof(D3DXVECTOR3) + 2 * sizeof(float);

		D3DVERTEXELEMENT9 vertexElements[] = 
		{
			{0, particleVertexPositionOffset,		D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_POSITION,	0},
			{0, particleVertexNormalOffset,			D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_NORMAL,	0},
			{0, particleVertexTangentOffset,		D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TANGENT,	0},
			{0, particleVertexBitangentOffset,		D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_BINORMAL,	0},
			{0, particleVertexUVindexOffset,		D3DDECLTYPE_SHORT2,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TEXCOORD,	0},

			{1, particleInstanceTranslationOffset,	D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TEXCOORD,	1},
			{1, particleInstanceRotationOffset,		D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TEXCOORD,	2},
			{1, particleInstanceSizeOffset,			D3DDECLTYPE_FLOAT1,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TEXCOORD,	3},
			{1, particleInstanceSpriteIndexOffset,	D3DDECLTYPE_FLOAT1,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TEXCOORD,	4},
			{1, particleInstanceColorOffset,		D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_COLOR,		0},

			D3DDECL_END()
		};


		if (FAILED(hr = _pDevice->CreateVertexDeclaration(vertexElements, &pVertexDeclaration)))
		{
			IDX_SAFE_RELEASE(pVertexBuffer);
			showError(false, "IDirect3DDevice9::CreateVertexDeclaration error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -7;
		}
	}

	particleQuadRefCount ++;

	return 0;
};


int ParticleSystem_v4::particleQuadRelease()
{
	particleQuadRefCount --;

	if (!particleQuadRefCount)
	{
		IDX_SAFE_RELEASE(pVertexBuffer);
		IDX_SAFE_RELEASE(pVertexDeclaration);
	}

	return 0;
};


int ParticleSystem_v4::sortAllParticles()
{
	if (particleListSize < 2)
		return 0;

	instancesActual = false;

	if (updatesSorted > particleListSize * 0.1f)
		return particleListInsertsort();
	else
	{
		return particleListQuicksort();
	}
};


int ParticleSystem_v4::setBuffers(IDirect3DDevice9* _pDevice, UINT _maxParticlesNumber)
{
	if (_maxParticlesNumber == maxParticlesNumber)
		return 0;

	if (_maxParticlesNumber && !_pDevice)
		return -1;

	if (maxParticlesNumber)
	{
		if (_maxParticlesNumber)
		{
			IDX_SAFE_RELEASE(pInstanceBuffer);

			HRESULT hr = S_OK;
			if (FAILED(hr = _device->CreateVertexBuffer(	_maxParticlesNumber * sizeof(ParticleInstance),
															D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 
															0, 
															D3DPOOL_DEFAULT, 
															&pInstanceBuffer, 
															NULL)))
			{
				IDX_SAFE_RELEASE(pInstanceBuffer);
				SAFE_DELETE_ARRAY(pParticles);
				particleProcessorData.resize(0);
				maxParticlesNumber = 0;
				particleQuadRelease();

				showError(false, "IDirect3DDevice9::CreateVertexBuffer error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
				return -2;
			}


			SAFE_DELETE_ARRAY(pParticles);

			pParticles = new Particle [_maxParticlesNumber];
			if (!pParticles)
			{
				IDX_SAFE_RELEASE(pInstanceBuffer);
				SAFE_DELETE_ARRAY(pParticles);
				particleProcessorData.resize(0);
				maxParticlesNumber = 0;
				particleQuadRelease();

				hr = E_OUTOFMEMORY;
				showError(false, "Memory allocation error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
				return -3;
			}


			if (particleProcessor)
			{
				UINT userDefParticleSize = particleProcessor->getUserDefinedParticleDataSize();
				particleProcessorData.resize(userDefParticleSize * _maxParticlesNumber);
				
				for (UINT i=0;i<_maxParticlesNumber;++i)
				{
					pParticles[i].userDefinedData = reinterpret_cast <void*> (&(particleProcessorData[i * userDefParticleSize]));
				}

				particleProcessor->setupAllocatedParticles(&(particleProcessorData[0]), _maxParticlesNumber);
			}
		}
		else //!_maxParticlesNumber
		{
			IDX_SAFE_RELEASE(pInstanceBuffer);
			SAFE_DELETE_ARRAY(pParticles);
			particleProcessorData.resize(0);
			particleQuadRelease();
		}
	}
	else //!maxParticlesNumber
	{
		HRESULT hr = S_OK;
		if (FAILED(hr = _device->CreateVertexBuffer(	_maxParticlesNumber * sizeof(ParticleInstance),
														D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 
														0, 
														D3DPOOL_DEFAULT, 
														&pInstanceBuffer, 
														NULL)))
		{
			showError(false, "IDirect3DDevice9::CreateVertexBuffer error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -4;
		}


		pParticles = new Particle [_maxParticlesNumber];
		if (!pParticles)
		{
			IDX_SAFE_RELEASE(pInstanceBuffer);

			hr = E_OUTOFMEMORY;
			showError(false, "Memory allocation error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -5;
		}


		if (particleProcessor)
		{
			UINT userDefParticleSize = particleProcessor->getUserDefinedParticleDataSize();
			particleProcessorData.resize(userDefParticleSize * _maxParticlesNumber);
			
			for (UINT i=0;i<_maxParticlesNumber;++i)
			{
				pParticles[i].userDefinedData = reinterpret_cast <void*> (&(particleProcessorData[i * userDefParticleSize]));
			}

			particleProcessor->setupAllocatedParticles(&(particleProcessorData[0]), _maxParticlesNumber);
		}


		if (int res = particleQuadAddRef())
		{
			IDX_SAFE_RELEASE(pInstanceBuffer);
			SAFE_DELETE_ARRAY(pParticles);
			particleProcessorData.resize(0);
			return res + (res > 0 ? 10 : -10);
		}
	}

	maxParticlesNumber = _maxParticlesNumber;
	instancesActual = false;

	return 0;
};


D3DXPLANE* ParticleSystem_v4::createCameraPlaneInLocalCoord(D3DXPLANE* _plane, Camera* _camera)
{
	D3DXMATRIX* particlesMx = pPosition->getWorldPosition();
	D3DXMATRIX inverseMx;

	D3DXVECTOR3* particlesPosition = pPosition->getWorldPositionVector();

	D3DXVECTOR3	normalizedFacingVector;
	D3DXVec3TransformNormal(&normalizedFacingVector, &(D3DXVECTOR3(0.0f, 0.0f, 1.0f)), particlesMx);

	D3DXVECTOR3 xaxis;
	D3DXVec3Normalize(&xaxis, D3DXVec3Cross(&xaxis, D3DXVec3TransformNormal(&xaxis, &(D3DXVECTOR3(0.0f, 1.0f, 0.0f)), particlesMx), &normalizedFacingVector));

	D3DXVECTOR3 yaxis;
	D3DXVec3Cross(&yaxis, &normalizedFacingVector, &xaxis);

	inverseMx._11 = particlesMx->_11;
	inverseMx._12 = particlesMx->_21;
	inverseMx._13 = particlesMx->_31;
	inverseMx._14 = 0.0f;
	inverseMx._21 = particlesMx->_12;
	inverseMx._22 = particlesMx->_22;
	inverseMx._23 = particlesMx->_32;
	inverseMx._24 = 0.0f;
	inverseMx._31 = particlesMx->_13;
	inverseMx._32 = particlesMx->_23;
	inverseMx._33 = particlesMx->_33;
	inverseMx._34 = 0.0f;
	inverseMx._41 = -1.0f * D3DXVec3Dot(&xaxis, particlesPosition);
	inverseMx._42 = -1.0f * D3DXVec3Dot(&yaxis, particlesPosition);
	inverseMx._43 = -1.0f * D3DXVec3Dot(&normalizedFacingVector, particlesPosition);
	inverseMx._44 = 1.0f;


	D3DXVECTOR3 camPos;
	D3DXVECTOR3 camNrm;
	D3DXVec3TransformCoord(&camPos, _camera->getCameraWorldPosition()->getWorldPositionVector(), &inverseMx);
	D3DXVec3TransformNormal(&camNrm, &(_camera->getNormalizedFacingVector()), &inverseMx);
	D3DXPlaneFromPointNormal(_plane, &camPos, &camNrm);
	D3DXPlaneNormalize(_plane, _plane);

	return _plane;
};


int ParticleSystem_v4::updateParticlesNoSorting(float _timeDelta)
{
	if (UINT batches = vParticleBatch.size())
	{
		for (UINT i = 0; i < batches; vParticleBatch[i++].timeFromLastUpdate += _timeDelta);

		UINT batchIndex = updateTimeStamp % batches;
		float tdelta = vParticleBatch[batchIndex].timeFromLastUpdate;
		vParticleBatch[batchIndex].timeFromLastUpdate = 0.0f;

		Particle* pPart = NULL;
		quickListNode<Particle*>* batchElement = vParticleBatch[batchIndex].localList.begin();
		while (batchElement)
		{
			pPart = batchElement->val;
			particleProcessor->updateParticle(pPart, tdelta);
			if (!pPart->alive)
			{
				particleListRemove(pPart);
				batchElement = vParticleBatch[batchIndex].localList.remove(batchElement);
			}
			else
				batchElement = batchElement->getNext();
		}
	}
	else //don't use particles batching
	{
		Particle* pPartTemp = NULL;
		Particle* pPart = pParticleListFirstElement;
		while (pPart)
		{
			particleProcessor->updateParticle(pPart, _timeDelta);
			if (!pPart->alive)
			{
				pPartTemp = pPart->next;
				particleListRemove(pPart);
				pPart = pPartTemp;
			}
			else
				pPart = pPart->next;
		}
	}

	instancesActual = false;

	return 0;
};


int ParticleSystem_v4::emitParticlesNoSorting(float _timeDelta)
{
	particlesToEmit += particleProcessor->getNumberParticlesToEmit(_timeDelta, particleListSize);
	UINT em = UINT(particlesToEmit);

	if (em > (maxParticlesNumber - particleListSize))
		em = maxParticlesNumber - particleListSize;


	if (UINT batches = vParticleBatch.size())
	{
		UINT batchIndex = updateTimeStamp % batches;

		UINT j = 0;
		for (UINT i=0; i<em; ++i)
		{
			while (j < maxParticlesNumber && pParticles[j].alive)
				j++;

			particleProcessor->emitParticle(&(pParticles[j]));
			particleListPushBack(&(pParticles[j]));
			vParticleBatch[particleInsertStamp++ % batches].localList.pushBack(&(pParticles[j]));

			particlesToEmit -= 1.0f;
		}
	}
	else //don't use particles batching
	{
		UINT j = 0;
		for (UINT i = 0; i < em; ++i)
		{
			while (j<maxParticlesNumber && pParticles[j].alive)
				j++;

			particleProcessor->emitParticle(&(pParticles[j]));
			particleListPushBack(&(pParticles[j]));
			particlesToEmit -= 1.0f;
		}
	}

	if (!(updateTimeStamp % 60) && particlesToEmit >= 1.0f)
		particlesToEmit = 0.0f;

	instancesActual = false;

	return 0;
};


int ParticleSystem_v4::updateParticlesWithDistanceFromCamera(float _timeDelta, D3DXPLANE const & _viewPlane)
{
	if (UINT batches = vParticleBatch.size())
	{
		for (UINT i = 0; i < batches; vParticleBatch[i++].timeFromLastUpdate += _timeDelta);

		UINT batchIndex = updateTimeStamp % batches;
		float tdelta = vParticleBatch[batchIndex].timeFromLastUpdate;
		vParticleBatch[batchIndex].timeFromLastUpdate = 0.0f;

		Particle* pPart = NULL;
		quickListNode<Particle*>* batchElement = vParticleBatch[batchIndex].localList.begin();
		while (batchElement)
		{
			pPart = batchElement->val;
			particleProcessor->updateParticle(pPart, tdelta);
			if (!pPart->alive)
			{
				particleListRemove(pPart);
				batchElement = vParticleBatch[batchIndex].localList.remove(batchElement);
			}
			else
			{
				pPart->distanceFromCamera =	_viewPlane.a * pPart->position.x + 
											_viewPlane.b * pPart->position.y + 
											_viewPlane.c * pPart->position.z + 
											_viewPlane.d;

				batchElement = batchElement->getNext();
			}
		}
	}
	else //don't use particles batching
	{
		Particle* pPartTemp = NULL;
		Particle* pPart = pParticleListFirstElement;
		while (pPart)
		{
			particleProcessor->updateParticle(pPart, _timeDelta);
			if (!pPart->alive)
			{
				pPartTemp = pPart->next;
				particleListRemove(pPart);
				pPart = pPartTemp;
			}
			else
			{
				pPart->distanceFromCamera =	_viewPlane.a * pPart->position.x + 
											_viewPlane.b * pPart->position.y + 
											_viewPlane.c * pPart->position.z + 
											_viewPlane.d;

				pPart = pPart->next;
			}
		}
	}

	instancesActual = false;

	return 0;
};


int ParticleSystem_v4::emitParticlesInsertSorting(float _timeDelta, D3DXPLANE const & _viewPlane)
{
	particlesToEmit += particleProcessor->getNumberParticlesToEmit(_timeDelta, particleListSize);
	UINT em = UINT(particlesToEmit);

	if (!em)
		return 0;

	if (em > (maxParticlesNumber - particleListSize))
		em = maxParticlesNumber - particleListSize;


	quickList<Particle*> particlesToInsert;
	quickListNode<Particle*>* ptiElement;

	if (UINT batches = vParticleBatch.size())
	{
		UINT batchIndex = updateTimeStamp % batches;

		UINT j = 0;
		for (UINT i=0; i<em; ++i)
		{
			while (j < maxParticlesNumber && pParticles[j].alive)
				j++;

			particleProcessor->emitParticle(&(pParticles[j]));
			vParticleBatch[particleInsertStamp++ % batches].localList.pushBack(&(pParticles[j]));

			pParticles[j].distanceFromCamera =	_viewPlane.a * pParticles[j].position.x + 
												_viewPlane.b * pParticles[j].position.y + 
												_viewPlane.c * pParticles[j].position.z + 
												_viewPlane.d;

			//presorting of emited particles
			ptiElement = particlesToInsert.begin();
			while (ptiElement && ptiElement->val->distanceFromCamera > pParticles[j].distanceFromCamera)
				ptiElement = ptiElement->getNext();
			particlesToInsert.insertBefore(ptiElement, (&pParticles[j]));
		}

		particlesToEmit -= float(em);
	}
	else //don't use particles batching
	{
		UINT j = 0;
		for (UINT i = 0; i < em; ++i)
		{
			while (j<maxParticlesNumber && pParticles[j].alive)
				j++;

			particleProcessor->emitParticle(&(pParticles[j]));
			pParticles[j].distanceFromCamera =	_viewPlane.a * pParticles[j].position.x + 
												_viewPlane.b * pParticles[j].position.y + 
												_viewPlane.c * pParticles[j].position.z + 
												_viewPlane.d;

			//presorting of emited particles
			ptiElement = particlesToInsert.begin();
			while (ptiElement && ptiElement->val->distanceFromCamera > pParticles[j].distanceFromCamera)
				ptiElement = ptiElement->getNext();
			particlesToInsert.insertBefore(ptiElement, (&pParticles[j]));
		}

		particlesToEmit -= float(em);
	}


	//now insert presorted particles into main list;
	if (particlesToInsert.size())
	{
		Particle* p = pParticleListFirstElement;
		float dist = particlesToInsert.front()->distanceFromCamera;
		while (p)
		{
			if (p->distanceFromCamera < dist)
			{
				particleListInsert(p->prev, particlesToInsert.front());
				particlesToInsert.popFront();
				if (particlesToInsert.size())
					dist = particlesToInsert.front()->distanceFromCamera;
				else
					p = NULL;
			}
			else
				p = p->next;
		}
		//push back any left particles
		while (particlesToInsert.size())
		{
			particleListPushBack(particlesToInsert.front());
			particlesToInsert.popFront();
		}
	}


	if (!(updateTimeStamp % 60) && particlesToEmit >= 1.0f)
		particlesToEmit = 0.0f;

	instancesActual = false;

	return 0;
};


int ParticleSystem_v4::updateParticlesDistanceFromCamera(D3DXPLANE const & _viewPlane)
{
	Particle* pPart = pParticleListFirstElement;
	while (pPart)
	{
		pPart->distanceFromCamera =	_viewPlane.a * pPart->position.x + 
									_viewPlane.b * pPart->position.y + 
									_viewPlane.c * pPart->position.z + 
									_viewPlane.d;

		pPart = pPart->next;
	}

	instancesActual = false;

	return 0;
};


int ParticleSystem_v4::updateInstances()
{
	if (instancesActual)
	{
		return 0;
	}


	numberOfParticlesToRender = particleListSize;

	Particle* p = NULL;

	if (drawOnlyParticlesBeforeCamera)
	{
		p = pParticleListLastElement;

		while (p && p->distanceFromCamera < 0.0f)
		{
			p = p->prev;
			--numberOfParticlesToRender;
		}
	}

	if (!numberOfParticlesToRender)
	{
		instancesActual = true;
		return 0;
	}

	p = pParticleListFirstElement;
	UINT tt = 0;
	while (p)
	{
		tt++;
		p = p->next;
	}

	p = pParticleListFirstElement;
	ParticleInstance* pInst;
	HRESULT hr = S_OK;

	if (FAILED(hr = pInstanceBuffer->Lock(0, numberOfParticlesToRender, reinterpret_cast <void**> (&pInst), D3DLOCK_DISCARD)))
	{
		showError(false, "IDirect3DVertexBuffer9::Lock error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -1;
	}

	{
		for (UINT i = 0; i < numberOfParticlesToRender; i++, p = p->next)
		{
			//copy all elements one by one
			pInst[i].translation = p->position;
			pInst[i].rotation = p->rotation;
			pInst[i].size = p->size;
			pInst[i].spriteIndex = p->spriteIndex;
			pInst[i].color = p->color;
		}

	}

	if (FAILED(hr = pInstanceBuffer->Unlock()))
	{
		showError(false, "IDirect3DVertexBuffer9::Unlock error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -2;
	}

	instancesActual = true;

	return 0;
};


int ParticleSystem_v4::drawParticles(IDirect3DDevice9* _pDevice, Camera* _camera, const D3DXCOLOR* _ambientLight)
{
	if (!numberOfParticlesToRender)
		return 0;

	Effect* pEffect = pMaterial->getEffect();

	ID3DXEffect* id3dxEffect = pEffect->getResourcePointer();

	HRESULT hr = S_OK;

	if (FAILED(hr = _pDevice->SetStreamSource(0, pVertexBuffer, 0, sizeof(ParticleSystem_v4::ParticleVertex))))
	{
		showError(false, "IDirect3DDevice9::SetStreamSource error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -1;
	}

	if (FAILED(hr = _pDevice->SetStreamSourceFreq(0, D3DSTREAMSOURCE_INDEXEDDATA | numberOfParticlesToRender)))
	{
		showError(false, "IDirect3DDevice9::SetStreamSourceFreq error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -2;
	}

	if (FAILED(hr = _pDevice->SetStreamSource(1, pInstanceBuffer, 0, sizeof(ParticleSystem_v4::ParticleInstance))))
	{
		showError(false, "IDirect3DDevice9::SetStreamSource error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -3;
	}

	if (FAILED(hr = _pDevice->SetStreamSourceFreq(1, D3DSTREAMSOURCE_INSTANCEDATA | 1)))
	{
		showError(false, "IDirect3DDevice9::SetStreamSourceFreq error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -4;
	}

	if (FAILED(hr = _pDevice->SetVertexDeclaration(pVertexDeclaration)))
	{
		showError(false, "IDirect3DDevice9::SetVertexDeclaration error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -5;
	}

	if (_ambientLight && pEffect->setAmbientLight(_ambientLight))
		return -5;

	int r = 0;

	if (r = pMaterial->apply())
		return r - 10;

	if (r = pMaterial->applyMatrices(	_camera->getTimestamp(),
										pPosition->getWorldPosition(),
										_camera->getViewMatrix(),
										_camera->getProjectionMatrix(),
										(billboardingType != BILLBOARDING_VIEWPLANE ? _camera->getViewInverseMatrix() : _camera->getBillboardingSphericalMatrix()),
										_camera->getViewProjectionMatrix()))
		return r - 20;


	UINT uiPasses = 0;
	
	{//effect rendering
		if (FAILED(hr = effect->Begin(&uiPasses, 0)))
		{
			showError(false, "ID3DXEffect::Begin error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -31;
		}

		for (UINT i = 0; i < uiPasses; ++i)
		{

			if (FAILED(hr = effect->BeginPass(i)))
			{
				showError(false, "ID3DXEffect::BeginPass error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
				return -32;
			}

			if (FAILED(hr = _pDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2)))
			{
				showError(false, "IDirect3DDevice9::DrawPrimitive error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
				return -33;
			}


			if (FAILED(hr = effect->EndPass()))
			{
				showError(false, "ID3DXEffect::EndPass error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
				return -34;
			}
		}

		if (FAILED(hr = effect->End()))
		{
			showError(false, "ID3DXEffect::End error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -35;
		}
	}


	if (FAILED(hr = _pDevice->SetStreamSourceFreq(0,1)))
	{
		showError(false, "IDirect3DDevice9::SetStreamSourceFreq error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -36;
	}

	if (FAILED(hr = _pDevice->SetStreamSourceFreq(1,1)))
	{
		showError(false, "IDirect3DDevice9::SetStreamSourceFreq error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -37;
	}

	return 0;
};


int ParticleSystem_v4::sortSinglePassBubbleSort()
{
	if (particleListSize < 2)
		return 0;

	Particle* p1 = NULL;
	Particle* p2 = pParticleListFirstElement->next;

	while (p2)
	{
		p1 = p2->prev;

		if (p1->distanceFromCamera < p2->distanceFromCamera)
		{
			particleListSwitchWithNext(p1);
			p2 = p1->next;
			//p2 = p2->next;
		}
		else
			p2 = p2->next;
	}

	instancesActual = false;

	updatesSorted++;

	return 0;
};


int ParticleSystem_v4::particleListInsertsort()
{
	if (particleListSize < 2)
		return 0;

	Particle* pf = pParticleListFirstElement->next;
	Particle* k = NULL;

	while (pf)
	{
		k = pf->prev;

		if (k->distanceFromCamera < pf->distanceFromCamera)
		{
			k = k->prev;
			while (k && k->distanceFromCamera < pf->distanceFromCamera)
				k = k->prev;

			Particle* tmp = pf->next;
			particleListMove(k, pf);
			pf = tmp;
		}
		else
			pf = pf->next;
	}

	return 0;
};


int ParticleSystem_v4::particleListQuicksort()
{
	if (particleListSize < 2)
		return 0;

	Particle* pf = pParticleListFirstElement;
	Particle* pl = pParticleListLastElement;

	UINT size = particleListSize;
	UINT f = 0;
	UINT l = size-1;
	Particle** arr = new Particle* [size];
	if (!arr)
		return 100;

	while (size)
	{
		arr[f++] = pf;
		pf = pf->next;
		size--;

		if (size)
		{
			arr[l--] = pf;
			pf = pf->next;
			size--;

			if (size)
			{
				arr[l--] = pl;
				pl = pl->prev;
				size--;

				if (size)
				{
					arr[f++] = pl;
					pl = pl->prev;
					size--;
				}
			}
		}
	}


	UINT elements = particleListSize;
	Particle* pivot = NULL;
	int begin[1000] = {0};
	int end[1000] = {0};
	int i = 0;
	int left = 0;
	int right = 0;

	begin[0] = 0;
	end[0] = elements;

	while (i >= 0)
	{
		left = begin[i];
		right = end[i] - 1;

		if (left < right)
		{
			pivot = arr[left];

			if (i >= 999)
			{
				SAFE_DELETE_ARRAY(arr);
				return 1;
			}

			while (left < right)
			{
				while (arr[right]->distanceFromCamera <= pivot->distanceFromCamera && left < right)
					right--;

				if (left < right)
					arr[left++] = arr[right];

				while (arr[left]->distanceFromCamera >= pivot->distanceFromCamera && left < right)
					left++;

				if (left < right)
					arr[right--] = arr[left];
			}

			arr[left] = pivot;

			begin[i+1] = left + 1;
			end[i+1] = end[i];
			end[i++] = left;
		}
		else
			i--;
	}


	for (UINT i = 1; i < elements-1; ++i)
	{
		arr[i]->prev = arr[i-1];
		arr[i]->next = arr[i+1];
	}

	pParticleListFirstElement = arr[0];
	arr[0]->prev = NULL;
	arr[0]->next = arr[1];

	pParticleListLastElement = arr[elements-1];
	arr[elements-1]->prev = arr[elements-2];
	arr[elements-1]->next = NULL;

	SAFE_DELETE_ARRAY(arr);

	return 0;
};


WorldPosition* ParticleSystem_v4::getWorldPosition()
{
	return pPosition;
};

int ParticleSystem_v4::setWorldPosition(WorldPosition* _pPosition)
{
	if (!_pPosition)
		return 1;

	pPosition = _pPosition;
	return 0;
};


const Texture* ParticleSystem_v4::getTexture()
{
	return pTexture;
};

const D3DXVECTOR2* ParticleSystem_v4::getSpritesVector()
{
	return &sprites;
};


float ParticleSystem_v4::getEmissiveFactor()
{
	return emissiveFactor;
};

int ParticleSystem_v4::setEmissiveFactor(float _emissiveFactor)
{
	emissiveFactor = _emissiveFactor;
	return 0;
};


UINT ParticleSystem_v4::getMaxParticlesNumber()
{
	return maxParticlesNumber;
};


int ParticleSystem_v4::setMaxParticlesNumber(IDirect3DDevice9* _pDevice, UINT _maxParticlesNumber)
{
	if (!_maxParticlesNumber)
		return 1;

	return setBuffers(_pDevice, _maxParticlesNumber);
};


UINT ParticleSystem_v4::getAliveParticlesNumber()
{
	return particleListSize;
};


bool ParticleSystem_v4::getDrawOnlyBeforeCamera()
{
	return drawOnlyParticlesBeforeCamera;
};


int ParticleSystem_v4::setDrawOnlyBeforeCamera(bool _drawOnlyParticlesBeforeCamera)
{
	drawOnlyParticlesBeforeCamera = _drawOnlyParticlesBeforeCamera;
	return 0;
};


UINT ParticleSystem_v4::getParticlesBeforeCameraNumber()
{
	return numberOfParticlesToRender;
};


UINT ParticleSystem_v4::getParticleBatchesNumber()
{
	return vParticleBatch.size();
};


int ParticleSystem_v4::setParticleBatchesNumber(UINT _batches)
{
	if (_batches > 8)
		return 1;

	if (_batches <= 1)
		_batches = 0;

	if (vParticleBatch.size() == _batches)
		return 0;

	if (!_batches)
		vParticleBatch.resize(0);
	else if (!vParticleBatch.size())
			particleListClear();

	vParticleBatch.resize(_batches);

	return 0;
};


DWORD ParticleSystem_v4::getBillboardingType()
{
	return billboardingType;
};


int ParticleSystem_v4::setBillboardingType(DWORD _billboardingType)
{
	if (_billboardingType > 2)
		_billboardingType = 2;

	billboardingType = _billboardingType;
	return pMaterial->setEffectParameter(PARTICLE_SYSTEM_BASE_BILLBOARDING_TYPE_SEMANTIC, &billboardingType, sizeof(billboardingType));
};


DWORD ParticleSystem_v4::getParticleSortingType()
{
	return particleSortingType;
};


int ParticleSystem_v4::setParticleSortingType(DWORD _particleSortingType)
{
	lastInsertSorted = false;

	if (_particleSortingType > PARTICLE_SORTING_FULL)
		_particleSortingType = PARTICLE_SORTING_FULL;

	particleSortingType = _particleSortingType;
	return 0;
};


float ParticleSystem_v4::getTotalTime()
{
	return accumulatedTime;
};


int ParticleSystem_v4::setBaseTexture(Texture* _texture, UINT _spritesColumns, UINT _spritesRows)
{
	if (!_texture || !_spritesColumns || !_spritesRows)
		return 1;

	sprites = D3DXVECTOR2(float(_spritesColumns), float(_spritesRows));

	pMaterial->setTexture(PARTICLE_SYSTEM_BASE_TEXTURE_SEMANTIC, _texture);
	pMaterial->setEffectParameter(PARTICLE_SYSTEM_BASE_SPRITES_SEMANTIC, &sprites, sizeof(sprites));

	return 0;
};


int ParticleSystem_v4::setAdditionalTexture(string _shaderVarName, Texture* _texture)
{
	if (!_shaderVarName.size() || !_texture)
		return 1;

	return pMaterial->setTexture(_shaderVarName.c_str(), _texture);
};


UserDefinedParticleProcessor* ParticleSystem_v4::getParticleProcessor()
{
	return particleProcessor;
};


const IDirect3DVertexBuffer9* ParticleSystem_v4::getInstanceBuffer()
{
	return pInstanceBuffer;
};


UINT ParticleSystem_v4::getNumberParticlesToRender()
{
	return numberOfParticlesToRender;
};


const IDirect3DVertexBuffer9* ParticleSystem_v4::getParticleVertexBuffer()
{
	return pVertexBuffer;
};


const IDirect3DVertexDeclaration9* ParticleSystem_v4::getParticleVertexDeclaration()
{
	return pVertexDeclaration;
};


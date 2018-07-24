#pragma once

#ifndef _PARTICLE_SYSTEM_V4
#define _PARTICLE_SYSTEM_V4


#include "utility.h"
#include "Core_Logger.h"
#include "Core_Variable_Library.h"
#include "Core_Defines.h"

#include "Particle_System_Base.h"

#include "Quick_List.h"

#include "Core_Graphic_Resource.h"
#include "Core_Graphic_Object.h"
#include "Core_World_Position_v3.h"

#include "Core_Camera.h"



class ParticleSystem_v4
{
private:

	struct ParticleVertex
	{
		union
		{
			float m[13];
			struct
			{
				D3DXVECTOR3 position;
				D3DXVECTOR3 normal;
				D3DXVECTOR3 tangent;
				D3DXVECTOR3 bitangent;
				short uIndex;
				short vIndex;
			};
		};

		ParticleVertex()
		{
			ZeroMemory(m, 13 * sizeof(float));
		};

		ParticleVertex(	float const & _x, float const & _y, float const & _z, 
						float const & _nx, float const & _ny, float const & _nz,
						float const & _tx, float const & _ty, float const & _tz,
						float const & _bx, float const & _by, float const & _bz,
						short const & _uIndex, short const & _vIndex) 
						: uIndex(_uIndex), vIndex(_vIndex)
		{
			position = D3DXVECTOR3(_x, _y, _z);
			normal = D3DXVECTOR3(_nx, _ny, _nz);
			tangent = D3DXVECTOR3(_tx, _ty, _tz);
			bitangent = D3DXVECTOR3(_bx, _by, _bz);
		};
	};


	struct ParticleInstance
	{
		union
		{
			float m[9];
			struct
			{
				D3DXVECTOR3 translation;
				D3DXVECTOR3 rotation;
				float size;
				float spriteIndex;
				D3DCOLOR color;
			};
		};
	};


	struct ParticleBatch
	{
		quickList<Particle*> localList;
		float timeFromLastUpdate;

		ParticleBatch() : timeFromLastUpdate(0.0f) {};
	};



private:

	bool initialized;

	string id;
	WorldPosition* pPosition;

	static UINT particleQuadRefCount;
	static IDirect3DVertexBuffer9* pVertexBuffer;
	static IDirect3DVertexDeclaration9* pVertexDeclaration;

	Texture* pTexture;

	IDirect3DVertexBuffer9* pInstanceBuffer;

	UINT maxParticlesNumber;

	Particle* pParticles;
	vector<BYTE> particleProcessorData;

	Particle* pParticleListFirstElement;
	Particle* pParticleListLastElement;
	UINT particleListSize;

	bool drawOnlyParticlesBeforeCamera;
	UINT numberOfParticlesToRender;

	vector<ParticleBatch> vParticleBatch;
	DWORD particleInsertStamp;

	float particlesToEmit;

	bool instancesActual;

	UINT spritesNumber;
	D3DXVECTOR2 sprites;

	float emissiveFactor;

	DWORD billboardingType;

	UserDefinedParticleProcessor* particleProcessor;

	UINT updatesSorted;
	DWORD particleSortingType;
	bool lastInsertSorted;

	float accumulatedTime;
	DWORD updateTimeStamp;



public:

	ParticleSystem_v4(string _id, WorldPosition* _pPosition);
	~ParticleSystem_v4();

	string getId();
	int setId(string _id);

	int setup(	IDirect3DDevice9* _pDevice, 
				UserDefinedParticleProcessor* _particleProcessor, 
				Texture* _texture, 
				UINT _spritesNumber, 
				UINT _spritesColumns, 
				UINT _spritesRows, 
				float _emissiveFactor, 
				UINT _maxParticlesNumber, 
				DWORD _particleSorting = PARTICLE_SORTING_PARTIAL,
				DWORD _billboardingType = BILLBOARDING_NONE, 
				UINT _particleBatches = 0, 
				bool _drawOnlyParticlesBeforeCamera = true);

	int shutdown();

	int update(float _timeDelta, Camera* _camera = NULL);

	int updateBeforeDrawing(IDirect3DDevice9* _pDevice, Camera* _camera);

	int sortParticles(Camera* _camera);


protected:

	int particleQuadAddRef();
	int particleQuadRelease();

	int sortAllParticles();

	int setBuffers(IDirect3DDevice9* _pDevice, UINT _maxParticlesNumber);
	D3DXPLANE* createCameraPlaneInLocalCoord(D3DXPLANE* _plane, Camera* _camera);

	int updateParticlesNoSorting(float _timeDelta);
	int emitParticlesNoSorting(float _timeDelta);

	int updateParticlesWithDistanceFromCamera(float _timeDelta, D3DXPLANE const & _viewPlane);
	int emitParticlesInsertSorting(float _timeDelta, D3DXPLANE const & _viewPlane);

	int updateParticlesDistanceFromCamera(D3DXPLANE const & _viewPlane);

	int updateInstances();


	//remove
	int drawParticles(IDirect3DDevice9* _pDevice, Camera* _camera, const D3DXCOLOR* _ambientLight);

	int sortSinglePassBubbleSort();
	int particleListInsertsort();
	int particleListQuicksort();


public: //setters getters

	WorldPosition* getWorldPosition();
	int setWorldPosition(WorldPosition* _pPosition);

	const Texture* getTexture();
	const D3DXVECTOR2* getSpritesVector();

	float getEmissiveFactor();
	int setEmissiveFactor(float _emissiveFactor);

	UINT getMaxParticlesNumber();
	int setMaxParticlesNumber(IDirect3DDevice9* _pDevice, UINT _maxParticlesNumber);
	UINT getAliveParticlesNumber();

	bool getDrawOnlyBeforeCamera();
	int setDrawOnlyBeforeCamera(bool _drawOnlyParticlesBeforeCamera);
	UINT getParticlesBeforeCameraNumber();

	UINT getParticleBatchesNumber();
	int setParticleBatchesNumber(UINT _batches);

	DWORD getBillboardingType();
	int setBillboardingType(DWORD _billboardingType);

	DWORD getParticleSortingType();
	int  setParticleSortingType(DWORD _particleSortingType);

	float getTotalTime();

	UserDefinedParticleProcessor* getParticleProcessor();

	const IDirect3DVertexBuffer9* getInstanceBuffer();
	UINT getNumberParticlesToRender();

	static const IDirect3DVertexBuffer9* getParticleVertexBuffer();
	static const IDirect3DVertexDeclaration9* getParticleVertexDeclaration();



protected: //functions to manipulate particle list

	inline void particleListInsert(Particle* _prev, Particle* _p)
	{
		if (_prev)
		{
			_p->prev = _prev;
			_p->next = _prev->next;

			if (_prev->next)
				_prev->next->prev = _p;
			else
				pParticleListLastElement = _p;

			_prev->next = _p;
		}
		else //!_prev
		{
			_p->prev = NULL;
			_p->next = pParticleListFirstElement;
			if (pParticleListFirstElement)
				pParticleListFirstElement->prev = _p;
			else
				pParticleListLastElement = _p;

			pParticleListFirstElement = _p;
		}

		particleListSize++;
	};

	inline void particleListRemove(Particle* _p)
	{
		if (_p->next)
			_p->next->prev = _p->prev;
		else
			pParticleListLastElement = _p->prev;

		if (_p->prev)
			_p->prev->next = _p->next;
		else
			pParticleListFirstElement = _p->next;

		_p->prev = NULL;
		_p->next = NULL;

		particleListSize--;
	};

	inline void particleListPushFront(Particle* _p)
	{
		_p->prev = NULL;
		_p->next = pParticleListFirstElement;

		if (pParticleListFirstElement)
			pParticleListFirstElement->prev = _p;
		else
			pParticleListLastElement = _p;

		pParticleListFirstElement = _p;

		particleListSize++;
	};

	inline void particleListPushBack(Particle* _p)
	{
		_p->next = NULL;
		_p->prev = pParticleListLastElement;

		if (pParticleListLastElement)
			pParticleListLastElement->next = _p;
		else
			pParticleListFirstElement = _p;

		pParticleListLastElement = _p;

		particleListSize++;
	};

	inline void particleListPopFront()
	{
		Particle* p = pParticleListFirstElement;

		pParticleListFirstElement = pParticleListFirstElement->next;
		if (pParticleListFirstElement)
			pParticleListFirstElement->prev = NULL;
		else
			pParticleListLastElement = NULL;

		p->prev = NULL;
		p->next = NULL;

		particleListSize--;
	};

	inline void particleListPopBack()
	{
		Particle* p = pParticleListLastElement;

		pParticleListLastElement = pParticleListLastElement->prev;
		if (pParticleListLastElement)
			pParticleListLastElement->next = NULL;
		else
			pParticleListFirstElement = NULL;

		p->prev = NULL;
		p->next = NULL;

		particleListSize--;
	};

	inline void particleListSwitch(Particle* _p1, Particle* _p2)
	{
		Particle* prev1 = _p1->prev;
		Particle* next1 = _p1->next;
		Particle* prev2 = _p2->prev;
		Particle* next2 = _p2->next;

		_p1->prev = prev2;
		if (prev2)
			prev2->next = _p1;
		else
			pParticleListFirstElement = _p1;

		_p1->next = next2;
		if (next2)
			next2->prev = _p1;
		else
			pParticleListLastElement = _p1;


		_p2->prev = prev1;
		if (prev1)
			prev1->next = _p2;
		else
			pParticleListFirstElement = _p2;

		_p2->next = next1;
		if (next1)
			next1->prev = _p2;
		else
			pParticleListLastElement = _p2;
	};

	inline void particleListSwitchWithNext(Particle* _p)
	{
		Particle* p2 = _p->next;

		p2->prev = _p->prev;
		_p->next = p2->next;

		p2->next = _p;
		_p->prev = p2;

		if (p2->prev)
			p2->prev->next = p2;
		else
			pParticleListFirstElement = p2;

		if (_p->next)
			_p->next->prev = _p;
		else
			pParticleListLastElement = _p;
	};

	inline void particleListMove(Particle* _prev, Particle* _p)
	{
		Particle* prev = _p->prev;
		Particle* next = _p->next;

		if (prev)
			prev->next = next;
		else
			pParticleListFirstElement = next;

		if (next)
			next->prev = prev;
		else
			pParticleListLastElement = prev;


		if (_prev)
		{
			_p->prev = _prev;
			_p->next = _prev->next;

			if (_prev->next)
				_prev->next->prev = _p;
			else
				pParticleListLastElement = _p;

			_prev->next = _p;
		}
		else //!_prev
		{
			_p->prev = NULL;
			_p->next = pParticleListFirstElement;
			if (pParticleListFirstElement)
				pParticleListFirstElement->prev = _p;
			else
				pParticleListLastElement = _p;

			pParticleListFirstElement = _p;
		}
	};


	inline void particleListClear()
	{
		pParticleListFirstElement = NULL;
		pParticleListLastElement = NULL;

		for (UINT i = 0; i < maxParticlesNumber; ++i)
		{
			pParticles[i].alive = false;
			pParticles[i].distanceFromCamera = 0.0f;
			pParticles[i].prev = NULL;
			pParticles[i].next = NULL;
		}
	};

};


#endif //_PARTICLE_SYSTEM_V4

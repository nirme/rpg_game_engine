#pragma once

#ifndef _PARTICLE_SYSTEM_BASE
#define _PARTICLE_SYSTEM_BASE

#include "utility.h"
#include "Core_Logger.h"
#include "Core_Variable_Library.h"
#include "Core_Defines.h"


class ParticleSystem_v4;


struct __declspec(align(32)) Particle
{
	friend class ParticleSystem_v4;

	D3DXVECTOR3 position;
	D3DXVECTOR3 rotation;
	float size;
	float spriteIndex;
	D3DCOLOR color;
	bool alive;

	void* userDefinedData;


private:
	float distanceFromCamera;

	Particle* prev;
	Particle* next;

	Particle() : color(-1), size(1.0f), spriteIndex(0), alive(false), userDefinedData(NULL), distanceFromCamera(0.0f), prev(NULL), next(NULL)
	{
		position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		rotation = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	};
};



//class defined by user should never delete particles or change prev/next pointers
class __declspec(novtable) UserDefinedParticleProcessor
{
friend class ParticleSystem_v4;

private:

	virtual UINT getUserDefinedParticleDataSize() = 0;

	virtual int setupAllocatedParticles(void* _userDefinedParticleData, UINT _particleNumber) = 0;

	virtual int updateSystem(float const & _timeDelta) = 0;

	virtual int emitParticle(Particle* _particle) = 0;

	virtual int updateParticle(Particle* _particle, float const & _timeDelta) = 0;

	virtual float getNumberParticlesToEmit(float _timeDelta, float _activeParticlesNumber) = 0;

	virtual int setSpritesNumber(UINT _spritesInTexture) = 0;

	virtual int setMaxParticlesNumber(UINT _maxParticles) = 0;

	virtual int unserialize(void* _serialisedData) = 0;



public:

//	virtual int unserialize (void* _serialisedData) = 0;

};




#endif //_PARTICLE_SYSTEM_BASE

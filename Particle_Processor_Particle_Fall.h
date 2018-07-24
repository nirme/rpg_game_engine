#pragma once

#ifndef _PARTICLE_PROCESSOR_PARTICLE_FALL
#define _PARTICLE_PROCESSOR_PARTICLE_FALL


#include "utility.h"
#include "Core_Logger.h"
#include "Core_Variable_Library.h"
#include "Core_Defines.h"

#include "Particle_System_Base.h"



class ParticleProcessor_ParticleFall : public UserDefinedParticleProcessor
{
public:

	struct Droplet
	{
		D3DXVECTOR3 velocity;
		D3DXVECTOR3 rotationSpeed;
	};


private:

	D3DXVECTOR3 particleStartPoint;
	D3DXVECTOR3* particleStartPointLinked;

	D3DXVECTOR3* particleStartPointUsed;
	float startCircleRadius;

	float cylinderHeight;
	float cylinderRadius;
	float cylinderRadiusSquared;

	int spriteNumber;

	D3DXVECTOR3 baseRotationSpeed;
	D3DXVECTOR3 baseRotationSpeedDeviation;

	D3DXVECTOR3 baseVelocity;
	D3DXVECTOR3 baseVelocityDeviation;

	float baseSize;
	float baseSizeDeviation;


	float particlesEmitedPerSecond;


	bool useWind;
	D3DXVECTOR3 windSpeed;


public:

	ParticleProcessor_ParticleFall();
	~ParticleProcessor_ParticleFall(){};
	UINT getUserDefinedParticleDataSize();
	int setupAllocatedParticles(void* _userDefinedParticleData, UINT _particleNumber);
	int updateSystem(float const & _timeDelta);
	int updateParticle(Particle* _particle, float const & _timeDelta);
	float getNumberParticlesToEmit(float _timeDelta, float _activeParticlesNumber);
	int emitParticle(Particle* _particle);
	int setSpritesNumber(UINT _spritesInTexture);
	int setMaxParticlesNumber(UINT _maxParticles);

	int unserialize(void* _serialisedData);

//LOCAL FUNCTION
	int setParticleStartPoint(D3DXVECTOR3 _particleStartPoint);
	int setParticleStartPointLinked(D3DXVECTOR3* _particleStartPointLinked);
	D3DXVECTOR3* getParticleStartPoint();
	bool isParticleStartPointLinked();
	float getStartCircleRadius();
	int setStartCircleRadius(float _startCircleRadius);
	float getCylinderHeight();
	int setCylinderHeight(float _cylinderHeight);
	float getCylinderRadius();
	int setCylinderRadius(float _cylinderRadius);
	D3DXVECTOR3 getBaseRotationSpeed();
	int setBaseRotationSpeed(D3DXVECTOR3 _baseRotationSpeed);
	D3DXVECTOR3 getBaseRotationSpeedDeviation();
	int setBaseRotationSpeedDeviation(D3DXVECTOR3 _baseRotationSpeedDeviation);
	D3DXVECTOR3 getBaseVelocity();
	int setBaseVelocity(D3DXVECTOR3 _baseVelocity);
	D3DXVECTOR3 getBaseVelocityDeviation();
	int setBaseVelocityDeviation(D3DXVECTOR3 _baseVelocityDeviation);
	float getBaseSize();
	int setBaseSize(float _baseSize);
	float getBaseSizeDeviation();
	int setBaseSizeDeviation(float _baseSizeDeviation);

	float getParticlesEmitedPerSecond();
	int setParticlesEmitedPerSecond(float _particlesEmitedPerSecond);

	D3DXVECTOR3* getWindSpeed();
	int setWind(D3DXVECTOR3 _windSpeed);
	bool isWindUsed();
};





#endif //_PARTICLE_PROCESSOR_PARTICLE_FALL
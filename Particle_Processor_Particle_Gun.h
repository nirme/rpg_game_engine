#pragma once

#ifndef _PARTICLE_PROCESSOR_PARTICLE_GUN
#define _PARTICLE_PROCESSOR_PARTICLE_GUN


#include "utility.h"
#include "Core_Logger.h"
#include "Core_Variable_Library.h"
#include "Core_Defines.h"

#include "Particle_System_Base.h"

#include "Core_World_Position_v3.h"



class ParticleProcessor_ParticleGun : public UserDefinedParticleProcessor
{
public:

	struct Bullet
	{
		D3DXVECTOR3 velocity;
		float lifeMaxLength;
		float lifetime;
		float sizeChangePerSecond;
	};


private:

	WorldPosition* position;
	DWORD worldPositionTimestamp;

	D3DXVECTOR3 startPoint;
	D3DXVECTOR3* startPointLinked;
	D3DXVECTOR3* startPointUsed;

	D3DXVECTOR3 startPointDeviation;

	//direction
	float directionZRotation;
	float directionYRotation;

	float sinY, cosY, sinZ, cosZ;

	D3DXVECTOR3 directionNormalised;

	float baseVelocity;
	float baseVelocityDeviation;

	float innerEjectionAngle;
	float outerEjectionAngle;

	float baseSize;
	float baseSizeDeviation;
	float baseSizeChangePerSecond;
	float baseSizeChangePerSecondDeviation;

	float maxLifetime;
	float maxLifetimeDeviation;

	// D3DCOLOR is defined as DWORD and it's better to keep components in predefined order 
	// rather than assuming it will stay in the same order on different systems
	// 0=a, 1=r, 2=g, 3=b
	BYTE baseColor[4];
	BYTE baseColorDeviation[4];


	bool useGravity;
	float gravityAcceleration;
	D3DXVECTOR3 gravityUsed;

	float mediumResistance;


	float sphereRadius;
	float sphereRadiusSquared;


	int spriteNumber;


	float particlesEmitedPerSecond;


public:

	ParticleProcessor_ParticleGun();
	~ParticleProcessor_ParticleGun(){};

	UINT getUserDefinedParticleDataSize();
	int setupAllocatedParticles(void* _userDefinedParticleData, UINT _particleNumber);
	int updateSystem(float const & _timeDelta);
	int emitParticle(Particle* _particle);
	int updateParticle(Particle* _particle, float const & _timeDelta);
	float getNumberParticlesToEmit(float _timeDelta, float _activeParticlesNumber);
	int setSpritesNumber(UINT _spritesInTexture);
	int setMaxParticlesNumber(UINT _maxParticles);

	int unserialize(void* _serialisedData);

	//setters/getters

	int setStartPoint(D3DXVECTOR3* _startPoint);
	int setStartPointLinked(D3DXVECTOR3* _startPoint);
	D3DXVECTOR3 getStartPoint();
	D3DXVECTOR3* getStartPointPointer();
	bool getStartPointLinked();


	int setStartPointDeviation(D3DXVECTOR3* _startPointDeviation);
	D3DXVECTOR3 getStartPointDeviation(D3DXVECTOR3 _startPointDeviation);

	int setDirection(float _directionZRotation, float _directionYRotation);
	int setDirectionFromNormal(D3DXVECTOR3* _direction);
	float getDirectionRotationX();
	float getDirectionRotationY();
	D3DXVECTOR3 getDirectionNormalised();

	int setEjection(float _innerEjectionAngle, float _outerEjectionAngle);
	float getEjectionInnerAngle();
	float getEjectionOuterAngle();

	int setVelocity(float _baseVelocity, float _baseVelocityDeviation);
	float getVelocity();
	float getVelocityDeviation();

	int setLifetime(float _maxLifetime, float _maxLifetimeDeviation);
	float getLifetime();
	float getLifetimeDeviation();

	int setSize(float _baseSize, float _baseSizeDeviation);
	float getSize();
	float getSizeDeviation();

	int setSizeChange(float _baseSizeChangePerSecond, float _baseSizeChangePerSecondDeviation);
	float getSizeChangePerSecond();
	float getSizeChangePerSecondDeviation();

	int setColor(D3DXCOLOR* _baseColor, D3DXCOLOR* _baseColorDeviation);
	D3DXCOLOR getColor();
	D3DXCOLOR getColorDeviation();
	D3DCOLOR getColorDword();
	D3DCOLOR getColorDeviationDword();

	int setBoundingSphereRadius(float _sphereRadius);
	float getBoundingSphereRadius();

	int setParticlesPerSecond(float _particlesEmitedPerSecond);
	float getParticlesPerSecond();

	int setMediumResistance(float _mediumResistance);
	float getMediumResistance();

	int setGravity(float _acceleration);
	int setPosition(WorldPosition* _position);
	bool usingGravity();
	WorldPosition* getPosition();
	float getGravityAcceleration();

};

#endif //_PARTICLE_PROCESSOR_PARTICLE_GUN

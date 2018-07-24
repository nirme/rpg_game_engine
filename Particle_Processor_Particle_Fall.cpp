#include "Particle_Processor_Particle_Fall.h"




ParticleProcessor_ParticleFall::ParticleProcessor_ParticleFall()
{
	particleStartPoint = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	particleStartPointLinked = NULL;
	particleStartPointUsed = &particleStartPoint;
	startCircleRadius = 0.0f;

	cylinderHeight = 0.0f;
	cylinderRadius = 0.0f;
	cylinderRadiusSquared = 0.0f;

	spriteNumber = 0;

	baseRotationSpeed = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	baseRotationSpeedDeviation = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	baseVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	baseVelocityDeviation = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	baseSize = 0.0f;
	baseSizeDeviation = 0.0f;


	particlesEmitedPerSecond = 0.0f;


	useWind = false;
	windSpeed = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
};


UINT ParticleProcessor_ParticleFall::getUserDefinedParticleDataSize()
{
	return sizeof(ParticleProcessor_ParticleFall::Droplet);
};


int ParticleProcessor_ParticleFall::setupAllocatedParticles(void* _userDefinedParticleData, UINT _particleNumber)
{
	ParticleProcessor_ParticleFall::Droplet* data = reinterpret_cast <ParticleProcessor_ParticleFall::Droplet*> (_userDefinedParticleData);

	for (UINT i=0;i<_particleNumber;++i)
	{
		data[i].velocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		data[i].rotationSpeed = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}

	return 0;
};


int ParticleProcessor_ParticleFall::updateSystem(float const & _timeDelta)
{
	return 0;
};


int ParticleProcessor_ParticleFall::updateParticle(Particle* _particle, float const & _timeDelta)
{
	ParticleProcessor_ParticleFall::Droplet* s = reinterpret_cast <ParticleProcessor_ParticleFall::Droplet*> (_particle->userDefinedData);

	_particle->position += (s->velocity * _timeDelta);

	D3DXVECTOR3 dist = *particleStartPointUsed - _particle->position;


	if (dist.y > cylinderHeight || 
		((dist.x * dist.x) + (dist.z * dist.z)) > cylinderRadiusSquared)
	{
		_particle->alive = false;
		return 0;
	}

	if (useWind)
		_particle->position += (windSpeed * _timeDelta);

	_particle->rotation += (s->rotationSpeed * _timeDelta);

	return 0;
};


float ParticleProcessor_ParticleFall::getNumberParticlesToEmit(float _timeDelta, float _activeParticlesNumber)
{
	return particlesEmitedPerSecond * _timeDelta;
};


int ParticleProcessor_ParticleFall::emitParticle(Particle* _particle)
{
	float angle = RANDOM_POS_UNIT_FLOAT * CONST_TAU;
	float r = RANDOM_POS_UNIT_FLOAT * startCircleRadius;

	_particle->position = D3DXVECTOR3(particleStartPointUsed->x + (r * cos(angle)), particleStartPointUsed->y, particleStartPointUsed->z + (r * sin(angle)));
	_particle->rotation = D3DXVECTOR3(0.0f, (RANDOM_UNIT_FLOAT * CONST_TAU), 0.0f);

	_particle->color = -1;
	_particle->size = baseSize + (RANDOM_UNIT_FLOAT * baseSizeDeviation);
	_particle->spriteIndex = rand() % spriteNumber;

	ParticleProcessor_ParticleFall::Droplet* s = reinterpret_cast <ParticleProcessor_ParticleFall::Droplet*> (_particle->userDefinedData);

	s->rotationSpeed = D3DXVECTOR3(	baseRotationSpeed.x + (RANDOM_UNIT_FLOAT * baseRotationSpeedDeviation.x), 
									baseRotationSpeed.y + (RANDOM_UNIT_FLOAT * baseRotationSpeedDeviation.y), 
									baseRotationSpeed.z + (RANDOM_UNIT_FLOAT * baseRotationSpeedDeviation.z));

	s->velocity = D3DXVECTOR3(	baseVelocity.x + (RANDOM_UNIT_FLOAT * baseVelocityDeviation.x), 
								baseVelocity.y + (RANDOM_UNIT_FLOAT * baseVelocityDeviation.y), 
								baseVelocity.z + (RANDOM_UNIT_FLOAT * baseVelocityDeviation.z));

	_particle->alive = true;

	return 0;
};


int ParticleProcessor_ParticleFall::setSpritesNumber(UINT _spritesInTexture)
{
	spriteNumber = _spritesInTexture;
	return 0;
};


int ParticleProcessor_ParticleFall::setMaxParticlesNumber(UINT _MaxParticles)
{
	return 0;
};


int ParticleProcessor_ParticleFall::unserialize(void* _serialisedData)
{
	BYTE* dataPointer = _serialisedData;


	setParticleStartPoint(*(reinterpret_cast <D3DXVECTOR3*> (dataPointer)));
	dataPointer += sizeof(D3DXVECTOR3);

	setStartCircleRadius(*(reinterpret_cast <float*> (dataPointer)));
	dataPointer += sizeof(float);

	setCylinderHeight(*(reinterpret_cast <float*> (dataPointer)));
	dataPointer += sizeof(float);

	setCylinderRadius(*(reinterpret_cast <float*> (dataPointer)));
	dataPointer += sizeof(float);

	setBaseRotationSpeed(*(reinterpret_cast <D3DXVECTOR3*> (dataPointer)));
	dataPointer += sizeof(D3DXVECTOR3);

	setBaseRotationSpeedDeviation(*(reinterpret_cast <D3DXVECTOR3*> (dataPointer)));
	dataPointer += sizeof(D3DXVECTOR3);

	setBaseVelocity(*(reinterpret_cast <D3DXVECTOR3*> (dataPointer)));
	dataPointer += sizeof(D3DXVECTOR3);

	setBaseVelocityDeviation(*(reinterpret_cast <D3DXVECTOR3*> (dataPointer)));
	dataPointer += sizeof(D3DXVECTOR3);

	setBaseSize(*(reinterpret_cast <float*> (dataPointer)));
	dataPointer += sizeof(float);

	setBaseSizeDeviation(*(reinterpret_cast <float*> (dataPointer)));
	dataPointer += sizeof(float);

	setParticlesEmitedPerSecond(*(reinterpret_cast <float*> (dataPointer)));
	dataPointer += sizeof(float);


	return 0;
};


int ParticleProcessor_ParticleFall::setParticleStartPoint(D3DXVECTOR3 _particleStartPoint)
{
	particleStartPointLinked = NULL;
	particleStartPoint = _particleStartPoint;
	particleStartPointUsed = &particleStartPoint;
	return 0;
};

int ParticleProcessor_ParticleFall::setParticleStartPointLinked(D3DXVECTOR3* _particleStartPointLinked)
{
	particleStartPointLinked = _particleStartPointLinked;
	particleStartPointUsed = particleStartPointLinked;
	return 0;
};

D3DXVECTOR3* ParticleProcessor_ParticleFall::getParticleStartPoint()
{
	if (particleStartPointLinked)
		return particleStartPointLinked;
	return &particleStartPoint;
};

bool ParticleProcessor_ParticleFall::isParticleStartPointLinked()
{
	if (particleStartPointLinked)
		return true;
	return false;
};

float ParticleProcessor_ParticleFall::getStartCircleRadius()
{
	return startCircleRadius;
};

int ParticleProcessor_ParticleFall::setStartCircleRadius(float _startCircleRadius)
{
	startCircleRadius = _startCircleRadius;
	return 0;
};

float ParticleProcessor_ParticleFall::getCylinderHeight()
{
	return cylinderHeight;
};

int ParticleProcessor_ParticleFall::setCylinderHeight(float _cylinderHeight)
{
	cylinderHeight = _cylinderHeight;
	return 0;
};

float ParticleProcessor_ParticleFall::getCylinderRadius()
{
	return cylinderRadius;
};

int ParticleProcessor_ParticleFall::setCylinderRadius(float _cylinderRadius)
{
	cylinderRadius = _cylinderRadius;
	cylinderRadiusSquared = cylinderRadius * cylinderRadius;
	return 0;
};

D3DXVECTOR3 ParticleProcessor_ParticleFall::getBaseRotationSpeed()
{
	return baseRotationSpeed;
};

int ParticleProcessor_ParticleFall::setBaseRotationSpeed(D3DXVECTOR3 _baseRotationSpeed)
{
	baseRotationSpeed = _baseRotationSpeed;
	return 0;
};

D3DXVECTOR3 ParticleProcessor_ParticleFall::getBaseRotationSpeedDeviation()
{
	return baseRotationSpeedDeviation;
};

int ParticleProcessor_ParticleFall::setBaseRotationSpeedDeviation(D3DXVECTOR3 _baseRotationSpeedDeviation)
{
	baseRotationSpeedDeviation = _baseRotationSpeedDeviation;
	return 0;
};

D3DXVECTOR3 ParticleProcessor_ParticleFall::getBaseVelocity()
{
	return baseVelocity;
};

int ParticleProcessor_ParticleFall::setBaseVelocity(D3DXVECTOR3 _baseVelocity)
{
	baseVelocity = _baseVelocity;
	return 0;
};

D3DXVECTOR3 ParticleProcessor_ParticleFall::getBaseVelocityDeviation()
{
	return baseVelocityDeviation;
};

int ParticleProcessor_ParticleFall::setBaseVelocityDeviation(D3DXVECTOR3 _baseVelocityDeviation)
{
	baseVelocityDeviation = _baseVelocityDeviation;
	return 0;
};

float ParticleProcessor_ParticleFall::getBaseSize()
{
	return baseSize;
};

int ParticleProcessor_ParticleFall::setBaseSize(float _baseSize)
{
	baseSize = _baseSize;
	return 0;
};

float ParticleProcessor_ParticleFall::getBaseSizeDeviation()
{
	return baseSizeDeviation;
};

int ParticleProcessor_ParticleFall::setBaseSizeDeviation(float _baseSizeDeviation)
{
	baseSizeDeviation = _baseSizeDeviation;
	return 0;
};


float ParticleProcessor_ParticleFall::getParticlesEmitedPerSecond()
{
	return particlesEmitedPerSecond;
};

int ParticleProcessor_ParticleFall::setParticlesEmitedPerSecond(float _particlesEmitedPerSecond)
{
	particlesEmitedPerSecond = _particlesEmitedPerSecond;
	return 0;
};

D3DXVECTOR3* ParticleProcessor_ParticleFall::getWindSpeed()
{
	return &windSpeed;
};

int ParticleProcessor_ParticleFall::setWind(D3DXVECTOR3 _windSpeed)
{
	windSpeed = _windSpeed;
	useWind = true;
	return 0;
};

bool ParticleProcessor_ParticleFall::isWindUsed()
{
	return useWind;
};

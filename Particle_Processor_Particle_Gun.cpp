#include "Particle_Processor_Particle_Gun.h"




ParticleProcessor_ParticleGun::ParticleProcessor_ParticleGun()
{
	position = NULL;
	worldPositionTimestamp = 0;
	startPoint = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	startPointLinked = NULL;
	startPointUsed = &startPoint;
	startPointDeviation = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	directionZRotation = 0.0f;
	directionYRotation = 0.0f;
	sinY = 0.0f;
	cosY = 0.0f;
	sinZ = 0.0f;
	cosZ = 0.0f;
	directionNormalised = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	baseVelocity = 0.0f;
	baseVelocityDeviation = 0.0f;
	innerEjectionAngle = 0.0f;
	outerEjectionAngle = 0.0f;
	baseSize = 0.0f;
	baseSizeDeviation = 0.0f;
	baseSizeChangePerSecond = 0.0f;
	baseSizeChangePerSecondDeviation = 0.0f;
	maxLifetime = 0.0f;
	maxLifetimeDeviation = 0.0f;
	baseColor[0] = 0xFF;
	baseColor[1] = 0xFF;
	baseColor[2] = 0xFF;
	baseColor[3] = 0xFF;
	baseColorDeviation[0] = 0xFF;
	baseColorDeviation[1] = 0xFF;
	baseColorDeviation[2] = 0xFF;
	baseColorDeviation[3] = 0xFF;
	useGravity = false;
	gravityAcceleration = 0.0f;
	gravityUsed = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	mediumResistance = 0.0f;
	sphereRadius = 0.0f;
	sphereRadiusSquared = 0.0f;
	spriteNumber = 0;
	particlesEmitedPerSecond = 0.0f;
};



UINT ParticleProcessor_ParticleGun::getUserDefinedParticleDataSize()
{
	return sizeof(Bullet);
};

int ParticleProcessor_ParticleGun::setupAllocatedParticles(void* _userDefinedParticleData, UINT _particleNumber)
{
	ParticleProcessor_ParticleGun::Bullet* data = reinterpret_cast <ParticleProcessor_ParticleGun::Bullet*> (_userDefinedParticleData);

	for (UINT i=0;i<_particleNumber;++i)
	{
		data[i].velocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		data[i].lifeMaxLength = 0.0f;
		data[i].lifetime = 0.0f;
		data[i].sizeChangePerSecond = 0.0f;
	}

	return 0;
};


int ParticleProcessor_ParticleGun::updateSystem(float const & _timeDelta)
{
	if (position && useGravity)
	{
		if (worldPositionTimestamp == position->getWorldPositionTimestamp())
			return 0;

		D3DXMATRIX* wp = position->getWorldPosition();
		D3DXVec3Normalize(&gravityUsed, &D3DXVECTOR3(-(wp->_12), -(wp->_22), -(wp->_32)));
		gravityUsed *= gravityAcceleration;
		worldPositionTimestamp = position->getWorldPositionTimestamp();

		return 0;
	}

	useGravity = false;
	return 0;
};


int ParticleProcessor_ParticleGun::emitParticle(Particle* _particle)
{
	ParticleProcessor_ParticleGun::Bullet* data = reinterpret_cast <ParticleProcessor_ParticleGun::Bullet*> (_particle->userDefinedData);

	float alpha = RANDOM_POS_UNIT_FLOAT * (outerEjectionAngle - innerEjectionAngle) + innerEjectionAngle;

	float beta = RANDOM_POS_UNIT_FLOAT * CONST_TAU;
	float A = sin(alpha);

	data->velocity = D3DXVECTOR3(A * cos(beta), cos(alpha), A * -sin(beta)) * (baseVelocity + RANDOM_UNIT_FLOAT * baseVelocityDeviation);

	data->velocity = D3DXVECTOR3(	(data->velocity.x * cosZ * cosY) - (data->velocity.y * sinZ * cosY) + (data->velocity.z * sinY), 
									(data->velocity.x * sinZ) + (data->velocity.y * cosZ), 
									(data->velocity.y * sinZ * sinY) + (data->velocity.z * cosY) - (data->velocity.x * cosZ * sinY));

	data->lifeMaxLength = maxLifetime + RANDOM_UNIT_FLOAT * maxLifetimeDeviation;
	data->lifetime = 0.0f;

	_particle->size = baseSize + RANDOM_UNIT_FLOAT * baseSizeDeviation;
	data->sizeChangePerSecond = baseSizeChangePerSecond + RANDOM_UNIT_FLOAT * baseSizeChangePerSecondDeviation;


	_particle->position = D3DXVECTOR3(	startPointUsed->x + RANDOM_UNIT_FLOAT * startPointDeviation.x, 
										startPointUsed->y + RANDOM_UNIT_FLOAT * startPointDeviation.y, 
										startPointUsed->z + RANDOM_UNIT_FLOAT * startPointDeviation.z);

	_particle->position = D3DXVECTOR3(	(_particle->position.x * cosZ * cosY) - (_particle->position.y * sinZ * cosY) + (_particle->position.z * sinY), 
									(_particle->position.x * sinZ) + (_particle->position.y * cosZ), 
									(_particle->position.y * sinZ * sinY) + (_particle->position.z * cosY) - (_particle->position.x * cosZ * sinY));


	_particle->rotation = D3DXVECTOR3(0.0f, 0.0f, 0.0f);


	_particle->color = D3DCOLOR_ARGB(	(BYTE)(baseColor[0] + RANDOM_UNIT_FLOAT * baseColorDeviation[0]),
										(BYTE)(baseColor[1] + RANDOM_UNIT_FLOAT * baseColorDeviation[1]),
										(BYTE)(baseColor[2] + RANDOM_UNIT_FLOAT * baseColorDeviation[2]),
										(BYTE)(baseColor[3] + RANDOM_UNIT_FLOAT * baseColorDeviation[3]));

	_particle->spriteIndex = rand() % spriteNumber;

	_particle->alive = true;


	return 0;
};


int ParticleProcessor_ParticleGun::updateParticle(Particle* _particle, float const & _timeDelta)
{

	ParticleProcessor_ParticleGun::Bullet* data = reinterpret_cast <ParticleProcessor_ParticleGun::Bullet*> (_particle->userDefinedData);

	_particle->position += data->velocity * _timeDelta;

	data->lifetime += _timeDelta;

	if (data->lifetime >= data->lifeMaxLength)
	{
		_particle->alive = false;
		return 0;
	}

	float dist = _particle->position.x * _particle->position.x + _particle->position.y * _particle->position.y + _particle->position.z * _particle->position.z;
	if (dist > sphereRadiusSquared)
	{
		_particle->alive = false;
		return 0;
	}

	if (mediumResistance)
		data->velocity -= data->velocity * mediumResistance * _timeDelta;

	if (useGravity)
		data->velocity += gravityUsed * _timeDelta;

	_particle->size += data->sizeChangePerSecond * _timeDelta;

	float lf = data->lifetime / data->lifeMaxLength;

	(reinterpret_cast <BYTE*> (&(_particle->color)))[3] = int((1.0f - lf * lf) * 255);

	return 0;
};


float ParticleProcessor_ParticleGun::getNumberParticlesToEmit(float _timeDelta, float _activeParticlesNumber)
{
	return particlesEmitedPerSecond * _timeDelta;
};


int ParticleProcessor_ParticleGun::setSpritesNumber(UINT _spritesInTexture)
{
	spriteNumber = _spritesInTexture;
	return 0;
};


int ParticleProcessor_ParticleGun::setMaxParticlesNumber(UINT _maxParticles)
{
	return 0;
};


int ParticleProcessor_ParticleGun::unserialize(void* _serialisedData)
{
	BYTE* dataPointer = _serialisedData;
	float tmp1(0.0f), tmp2(0.0f);
	D3DXCOLOR* col1 = NULL;
	D3DXCOLOR* col2 = NULL;

	setStartPoint(reinterpret_cast <D3DXVECTOR3*> (dataPointer));
	dataPointer += sizeof(D3DXVECTOR3);

	setStartPointDeviation(reinterpret_cast <D3DXVECTOR3*> (dataPointer));
	dataPointer += sizeof(D3DXVECTOR3);

	setDirectionFromNormal(reinterpret_cast <D3DXVECTOR3*> (dataPointer));
	dataPointer += sizeof(D3DXVECTOR3);


	tmp1 = *(reinterpret_cast <float*> (dataPointer));
	dataPointer += sizeof(float);
	tmp2 = *(reinterpret_cast <float*> (dataPointer));
	dataPointer += sizeof(float);
	setEjection(tmp1, tmp2);

	tmp1 = *(reinterpret_cast <float*> (dataPointer));
	dataPointer += sizeof(float);
	tmp2 = *(reinterpret_cast <float*> (dataPointer));
	dataPointer += sizeof(float);
	setVelocity(tmp1, tmp2);

	tmp1 = *(reinterpret_cast <float*> (dataPointer));
	dataPointer += sizeof(float);
	tmp2 = *(reinterpret_cast <float*> (dataPointer));
	dataPointer += sizeof(float);
	setLifetime(tmp1, tmp2);

	tmp1 = *(reinterpret_cast <float*> (dataPointer));
	dataPointer += sizeof(float);
	tmp2 = *(reinterpret_cast <float*> (dataPointer));
	dataPointer += sizeof(float);
	setSize(tmp1, tmp2);

	tmp1 = *(reinterpret_cast <float*> (dataPointer));
	dataPointer += sizeof(float);
	tmp2 = *(reinterpret_cast <float*> (dataPointer));
	dataPointer += sizeof(float);
	setSizeChange(tmp1, tmp2);


	col1 = *(reinterpret_cast <D3DXCOLOR*> (dataPointer));
	dataPointer += sizeof(D3DXCOLOR);
	col2 = *(reinterpret_cast <D3DXCOLOR*> (dataPointer));
	dataPointer += sizeof(D3DXCOLOR);
	int setColor(col1, col2);

	int setBoundingSphereRadius(*(reinterpret_cast <float*> (dataPointer)));
	dataPointer += sizeof(float);

	int setParticlesPerSecond(*(reinterpret_cast <float*> (dataPointer)));
	dataPointer += sizeof(float);

	int setMediumResistance(*(reinterpret_cast <float*> (dataPointer)));
	dataPointer += sizeof(float);

	int setGravity(*(reinterpret_cast <float*> (dataPointer)));
	dataPointer += sizeof(float);


	return 0;
};



int ParticleProcessor_ParticleGun::setStartPoint(D3DXVECTOR3* _startPoint)
{
	startPoint = *_startPoint;
	startPointUsed = &startPoint;
	return 0;
};

int ParticleProcessor_ParticleGun::setStartPointLinked(D3DXVECTOR3* _startPoint)
{
	startPointLinked = _startPoint;
	startPointUsed = startPointLinked;
	return 0;
};

D3DXVECTOR3 ParticleProcessor_ParticleGun::getStartPoint()
{
	return *startPointUsed;
};

D3DXVECTOR3* ParticleProcessor_ParticleGun::getStartPointPointer()
{
	return startPointUsed;
};

bool ParticleProcessor_ParticleGun::getStartPointLinked()
{
	if (startPointLinked)
		return true;
	return false;
};

int ParticleProcessor_ParticleGun::setStartPointDeviation(D3DXVECTOR3* _startPointDeviation)
{
	startPointDeviation = *_startPointDeviation;
	return 0;
};

D3DXVECTOR3 ParticleProcessor_ParticleGun::getStartPointDeviation(D3DXVECTOR3 _startPointDeviation)
{
	return startPointDeviation;
};


int ParticleProcessor_ParticleGun::setDirection(float _directionZRotation, float _directionYRotation)
{
	directionZRotation = fmod(_directionZRotation, CONST_TAU);
	directionYRotation = fmod(_directionYRotation, CONST_TAU);

	sinY = sin(directionYRotation);
	cosY = cos(directionYRotation);
	sinZ = sin(directionZRotation);
	cosZ = cos(directionZRotation);

	return 0;
};

int ParticleProcessor_ParticleGun::setDirectionFromNormal(D3DXVECTOR3* _direction)
{
	D3DXVec3Normalize(&directionNormalised, _direction);

	directionZRotation = acos(directionNormalised.y);
	sinZ = sin(directionZRotation);
	cosZ = cos(directionZRotation);

	directionYRotation = asin(directionNormalised.z / sinZ);
	sinY = sin(directionYRotation);
	cosY = cos(directionYRotation);

	return 0;
};

float ParticleProcessor_ParticleGun::getDirectionRotationX()
{
	return directionZRotation;
};

float ParticleProcessor_ParticleGun::getDirectionRotationY()
{
	return directionYRotation;
};

D3DXVECTOR3 ParticleProcessor_ParticleGun::getDirectionNormalised()
{
	return directionNormalised;
};


int ParticleProcessor_ParticleGun::setEjection(float _innerEjectionAngle, float _outerEjectionAngle)
{
	innerEjectionAngle = _innerEjectionAngle > D3DX_PI ? D3DX_PI : (_innerEjectionAngle < 0.0f ? 0.0f : _innerEjectionAngle);
	outerEjectionAngle = _outerEjectionAngle > D3DX_PI ? D3DX_PI : (_outerEjectionAngle < 0.0f ? 0.0f : (_outerEjectionAngle < _innerEjectionAngle ? _innerEjectionAngle : _outerEjectionAngle));
	return 0;
};

float ParticleProcessor_ParticleGun::getEjectionInnerAngle()
{
	return innerEjectionAngle;
};

float ParticleProcessor_ParticleGun::getEjectionOuterAngle()
{
	return outerEjectionAngle;
};


int ParticleProcessor_ParticleGun::setVelocity(float _baseVelocity, float _baseVelocityDeviation)
{
	baseVelocity = _baseVelocity;
	baseVelocityDeviation = _baseVelocityDeviation;
	return 0;
};

float ParticleProcessor_ParticleGun::getVelocity()
{
	return baseVelocity;
};

float ParticleProcessor_ParticleGun::getVelocityDeviation()
{
	return baseVelocityDeviation;
};


int ParticleProcessor_ParticleGun::setLifetime(float _maxLifetime, float _maxLifetimeDeviation)
{
	maxLifetime = _maxLifetime;
	maxLifetimeDeviation = _maxLifetimeDeviation;
	return 0;
};

float ParticleProcessor_ParticleGun::getLifetime()
{
	return maxLifetime;
};

float ParticleProcessor_ParticleGun::getLifetimeDeviation()
{
	return maxLifetimeDeviation;
};


int ParticleProcessor_ParticleGun::setSize(float _baseSize, float _baseSizeDeviation)
{
	baseSize = _baseSize;
	baseSizeDeviation = _baseSizeDeviation;
	return 0;
};

float ParticleProcessor_ParticleGun::getSize()
{
	return baseSize;
};

float ParticleProcessor_ParticleGun::getSizeDeviation()
{
	return baseSizeDeviation;
};


int ParticleProcessor_ParticleGun::setSizeChange(float _baseSizeChangePerSecond, float _baseSizeChangePerSecondDeviation)
{
	baseSizeChangePerSecond = _baseSizeChangePerSecond;
	baseSizeChangePerSecondDeviation = _baseSizeChangePerSecondDeviation;
	return 0;
};

float ParticleProcessor_ParticleGun::getSizeChangePerSecond()
{
	return baseSizeChangePerSecond;
};

float ParticleProcessor_ParticleGun::getSizeChangePerSecondDeviation()
{
	return baseSizeChangePerSecondDeviation;
};


int ParticleProcessor_ParticleGun::setColor(D3DXCOLOR* _baseColor, D3DXCOLOR* _baseColorDeviation)
{
	baseColor[0] = (BYTE)(_baseColor->a * 255);
	baseColor[1] = (BYTE)(_baseColor->r * 255);
	baseColor[2] = (BYTE)(_baseColor->g * 255);
	baseColor[3] = (BYTE)(_baseColor->b * 255);

	baseColorDeviation[0] = (BYTE)(_baseColorDeviation->a * 255);
	baseColorDeviation[1] = (BYTE)(_baseColorDeviation->r * 255);
	baseColorDeviation[2] = (BYTE)(_baseColorDeviation->g * 255);
	baseColorDeviation[3] = (BYTE)(_baseColorDeviation->b * 255);

	return 0;
};

D3DXCOLOR ParticleProcessor_ParticleGun::getColor()
{
	return  D3DXCOLOR(float(baseColor[0]) / 255.0f, float(baseColor[1]) / 255.0f, float(baseColor[2]) / 255.0f, float(baseColor[3]) / 255.0f);
};

D3DXCOLOR ParticleProcessor_ParticleGun::getColorDeviation()
{
	return  D3DXCOLOR(float(baseColorDeviation[0]) / 255.0f, float(baseColorDeviation[1]) / 255.0f, float(baseColorDeviation[2]) / 255.0f, float(baseColorDeviation[3]) / 255.0f);
};

D3DCOLOR ParticleProcessor_ParticleGun::getColorDword()
{
	return D3DCOLOR_ARGB(DWORD(baseColor[0]), DWORD(baseColor[1]), DWORD(baseColor[2]), DWORD(baseColor[3]));
};

D3DCOLOR ParticleProcessor_ParticleGun::getColorDeviationDword()
{
	return D3DCOLOR_ARGB(DWORD(baseColorDeviation[0]), DWORD(baseColorDeviation[1]), DWORD(baseColorDeviation[2]), DWORD(baseColorDeviation[3]));
};


int ParticleProcessor_ParticleGun::setBoundingSphereRadius(float _sphereRadius)
{
	sphereRadius = _sphereRadius;
	sphereRadiusSquared = sphereRadius * sphereRadius;
	return 0;
};

float ParticleProcessor_ParticleGun::getBoundingSphereRadius()
{
	return sphereRadius;
};


int ParticleProcessor_ParticleGun::setParticlesPerSecond(float _particlesEmitedPerSecond)
{
	particlesEmitedPerSecond = _particlesEmitedPerSecond < 0.0f ? 0.0f : _particlesEmitedPerSecond;
	return 0;
};

float ParticleProcessor_ParticleGun::getParticlesPerSecond()
{
	return particlesEmitedPerSecond;
};


int ParticleProcessor_ParticleGun::setMediumResistance(float _mediumResistance)
{
	mediumResistance = _mediumResistance;
	return 0;
};

float ParticleProcessor_ParticleGun::getMediumResistance()
{
	return mediumResistance;
};


int ParticleProcessor_ParticleGun::setGravity(float _acceleration)
{
	gravityAcceleration = _acceleration;

	if (_acceleration > EPSILON && position)
		useGravity = false;
	else
		useGravity = true;

	return 0;
};

int ParticleProcessor_ParticleGun::setPosition(WorldPosition* _position)
{
	position = _position;
	worldPositionTimestamp = -1;

	if (_acceleration > EPSILON && position)
		useGravity = false;
	else
		useGravity = true;

	return 0;
};

bool ParticleProcessor_ParticleGun::usingGravity()
{
	return useGravity;
};

WorldPosition* ParticleProcessor_ParticleGun::getPosition()
{
	return position;
};

float ParticleProcessor_ParticleGun::getGravityAcceleration()
{
	return gravityAcceleration;
};


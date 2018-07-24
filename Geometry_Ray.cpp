#include "Geometry_Ray.h"



Ray::Ray()
{
	origin = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	direction = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
	directionInverse = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
};


Ray::Ray(D3DXVECTOR3* _origin, D3DXVECTOR3* _direction)
{
	origin = *_origin;
	D3DXVec3Normalize( &direction, _direction );
	directionInverse.x = 1.0f / direction.x;
	directionInverse.y = 1.0f / direction.y;
	directionInverse.z = 1.0f / direction.z;
};


Ray::Ray(D3DXMATRIX* _matrix)
{
	origin.x = _matrix->_41;
	origin.y = _matrix->_42;
	origin.z = _matrix->_43;
	float w = 1.0f / _matrix->_34;
	D3DXVec3Normalize( &direction, &D3DXVECTOR3( _matrix->_31 * w, _matrix->_32 * w, _matrix->_33 * w ) );
	directionInverse.x = 1.0f / direction.x;
	directionInverse.y = 1.0f / direction.y;
	directionInverse.z = 1.0f / direction.z;
};


void Ray::updateOrigin(D3DXVECTOR3* _origin)
{
	origin = *_origin;
};


void Ray::updateDirection(D3DXVECTOR3* _direction)
{
	D3DXVec3Normalize( &direction, _direction );
	directionInverse.x = 1.0f / direction.x;
	directionInverse.y = 1.0f / direction.y;
	directionInverse.z = 1.0f / direction.z;
};


void Ray::update(D3DXVECTOR3* _origin, D3DXVECTOR3* _direction)
{
	origin = *_origin;
	D3DXVec3Normalize( &direction, _direction );
	directionInverse.x = 1.0f / direction.x;
	directionInverse.y = 1.0f / direction.y;
	directionInverse.z = 1.0f / direction.z;
};


void Ray::update(D3DXMATRIX* _matrix)
{
	origin.x = _matrix->_41;
	origin.y = _matrix->_42;
	origin.z = _matrix->_43;
	float w = 1.0f / _matrix->_34;
	D3DXVec3Normalize( &direction, &D3DXVECTOR3( _matrix->_31 * w, _matrix->_32 * w, _matrix->_33 * w ) );
	directionInverse.x = 1.0f / direction.x;
	directionInverse.y = 1.0f / direction.y;
	directionInverse.z = 1.0f / direction.z;
};


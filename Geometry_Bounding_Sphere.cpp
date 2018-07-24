#include "Geometry_Bounding_Sphere.h"


BoundingSphere::BoundingSphere( WorldPosition* _p_worldPosition, float _radius, D3DXVECTOR3* _center ) : BoundingVolume( _p_worldPosition )
{
	if ( _center )
		center = *_center;
	else
		ZeroMemory( &center, sizeof(D3DXVECTOR3) );

	radius = _radius;

	dw_worldPositionTimestamp = p_worldPosition->getWorldPositionTimestamp();
	dw_worldPositionTimestamp--;

	update();
};


BoundingSphere::~BoundingSphere()
{};


bool BoundingSphere::intersectRay( Ray* _ray, float* _distance )
{
	update();

	const D3DXVECTOR3* rayDirection = _ray->getDirection();

	D3DXVECTOR3 diff = cashedCenter - *_ray->getOrigin();
	float a0 = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z - cashedRadiusSq;

	if (a0 <= 0.0f)
		return false;

	float a1 = rayDirection->x * diff.x + rayDirection->y * diff.y + rayDirection->z * diff.z;

	if (a1 <= 0.0f)
		return false;

	float discr = a1 * a1 - a0;

	if (discr < 0.0f)
		return false;

	float root = sqrt(discr);

	*_distance = abs(a1) - root;

	return true;
};


bool BoundingSphere::intersectPoint( const D3DXVECTOR3* _point )
{
	update();

	D3DXVECTOR3 delta = *_point - cashedCenter;
	return ( delta.x * delta.x + delta.y * delta.y + delta.z * delta.z ) < cashedRadiusSq;
};


float BoundingSphere::distanceFromPointSquared( const D3DXVECTOR3* _point )
{
	float d = distanceFromPoint( _point );
	return d * d;
};


float BoundingSphere::distanceFromPoint( const D3DXVECTOR3* _point )
{
	update();

	D3DXVECTOR3 delta = *_point - cashedCenter;
	return sqrt( delta.x * delta.x + delta.y * delta.y + delta.z * delta.z ) - cashedRadius;
};


bool BoundingSphere::distanceFromPointLT( const D3DXVECTOR3* _point, float _testValue )
{
	update();

	D3DXVECTOR3 delta = *_point - cashedCenter;
	float test = _testValue + cashedRadius;
	test *= test;
	return ( delta.x * delta.x + delta.y * delta.y + delta.z * delta.z ) < test;
};

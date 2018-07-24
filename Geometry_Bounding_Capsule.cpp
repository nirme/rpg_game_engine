#include "Geometry_Bounding_Capsule.h"
#include "Geometry_Bounding_Capsule_X.h"
#include "Geometry_Bounding_Capsule_Z.h"



BoundingCapsule::BoundingCapsule( WorldPosition* _p_worldPosition, float _extent, float _radius, D3DXVECTOR3* _center ) : BoundingVolume( _p_worldPosition )
{
	if ( _center )
		center = *_center;
	else
		ZeroMemory( &center, sizeof(D3DXVECTOR3) );

	extent = _extent;
	radius = _radius;

	dw_worldPositionTimestamp = p_worldPosition->getWorldPositionTimestamp();
	dw_worldPositionTimestamp--;

	update();
};


BoundingCapsule::~BoundingCapsule()
{};


bool BoundingCapsule::intersectRay( Ray* _ray, float* _distance )
{
	update();

	const D3DXVECTOR3* rayOrigin = _ray->getOrigin();
	const D3DXVECTOR3* rayDirection = _ray->getDirection();

	// check if the capsule isn't behind ray origin
	float qDist1 = ( cashedCenter.x - rayOrigin->x ) * rayDirection->x + ( cashedCenter.y - rayOrigin->y ) * rayDirection->y + ( cashedCenter.z - rayOrigin->z ) * rayDirection->z + cashedRadius;
	float qDist2 = qDist1 + cashedDirection.x * rayDirection->x + cashedDirection.y * rayDirection->y + cashedDirection.z * rayDirection->z;


	if ( qDist1 * qDist2 <= 0.0f )
		return false;

	float t[2];
	float distance;

	D3DXVECTOR3 U;
	D3DXVECTOR3 V;

	if (abs( cashedDirection.x ) >= abs( cashedDirection.y ) )
	{
		float invLength = 1.0f / sqrt( cashedDirection.x * cashedDirection.x + cashedDirection.z * cashedDirection.z );
		U.x = -cashedDirection.z * invLength;
		U.y =  0.0f;
		U.z = +cashedDirection.x * invLength;
		V.x =  cashedDirection.y * U.z;
		V.y =  cashedDirection.z * U.x - cashedDirection.x * U.z;
		V.z = -cashedDirection.y * U.x;
	}
	else
	{
		float invLength = 1.0f / sqrt( cashedDirection.y * cashedDirection.y + cashedDirection.z * cashedDirection.z );
		U.x =  0.0f;
		U.y = +cashedDirection.z * invLength;
		U.z = -cashedDirection.y * invLength;
		V.x =  cashedDirection.y * U.z - cashedDirection.z * U.y;
		V.y = -cashedDirection.x * U.z;
		V.z =  cashedDirection.x * U.y;
	}



	float rSqr = cashedRadius * cashedRadius;

	// Convert incoming line origin to capsule coordinates.
	D3DXVECTOR3 diff( (*rayOrigin) - cashedCenter );
	D3DXVECTOR3 P (	U.x * diff.x + U.y * diff.y + U.z * diff.z, 
					V.x * diff.x + V.y * diff.y + V.z * diff.z, 
					cashedDirection.x * diff.x + cashedDirection.y * diff.y + cashedDirection.z * diff.z );

	float dz = cashedDirection.x * rayDirection->x + cashedDirection.y * rayDirection->y + cashedDirection.z * rayDirection->z;
	if ( abs( dz ) >= ( 1.0f - EPSILON ) )
	{
		float radialSqrDist = rSqr - P.x * P.x - P.y * P.y;
		if ( radialSqrDist < 0.0f )
			return false;

		float zOffset = sqrt( radialSqrDist ) + cashedExtent;
		if ( dz > 0.0f )
		{
			t[0] = -P.z - zOffset;
			t[1] = -P.z + zOffset;
		}
		else
		{
			t[0] = P.z - zOffset;
			t[1] = P.z + zOffset;
		}

		distance = min( t[0], t[1] );

		if ( distance < 0.0f )
			return false;

		if (_distance)
			*_distance = distance;
		return true;
	}

	D3DXVECTOR3 D (	U.x * rayDirection->x + U.y * rayDirection->y + U.z * rayDirection->z, 
					V.x * rayDirection->x + V.y * rayDirection->y + V.z * rayDirection->z, 
					dz );

	float a0 = P.x * P.x + P.y * P.y - rSqr;
	float a1 = P.x * D.x + P.y * D.y;
	float a2 = D.x * D.x + D.y * D.y;
	float discr = a1 * a1 - a0 * a2;
	if ( discr < 0.0f )
		return false;

	float root, inv, tValue, zValue;
	int quantity = 0;
	if ( discr > EPSILON )
	{
		root = sqrt( discr );
		inv = 1.0f / a2;
		tValue = ( -a1 - root ) * inv;
		zValue = P.z + tValue * D.z;
		if ( abs( zValue ) <= cashedExtent )
		{
			t[quantity++] = tValue;
		}

		tValue = ( -a1 + root ) * inv;
		zValue = P.z + tValue * D.z;
		if ( abs( zValue ) <= cashedExtent )
		{
			t[quantity++] = tValue;
		}

		if ( quantity == 2 )
		{
			distance = min( t[0], t[1] );

			if ( distance < 0.0f )
				return false;

			if (_distance)
				*_distance = distance;
			return true;
		}
	}
	else
	{
		tValue = -a1 / a2;
		zValue = P.z + tValue * D.z;
		if ( abs( zValue ) <= cashedExtent )
			return false;
	}

	float PZpE = P.z + cashedExtent;
	a1 += PZpE * D.z;
	a0 += PZpE * PZpE;
	discr = a1 * a1 - a0;
	if ( discr > EPSILON )
	{
		root = sqrt( discr );
		tValue = -a1 - root;
		zValue = P.z + tValue * D.z;
		if ( zValue <= -cashedExtent )
		{
			t[quantity++] = tValue;
			if ( quantity == 2 )
			{
				distance = min( t[0], t[1] );

				if ( distance < 0.0f )
					return false;

				if (_distance)
					*_distance = distance;
				return true;
			}
		}

		tValue = -a1 + root;
		zValue = P.z + tValue * D.z;
		if ( zValue <= -cashedExtent )
		{
			t[quantity++] = tValue;
			if ( quantity == 2 )
			{
				distance = min( t[0], t[1] );

				if ( distance < 0.0f )
					return false;

				if (_distance)
					*_distance = distance;
				return true;
			}
		}
	}
	else if ( abs( discr ) <= EPSILON )
	{
		tValue = -a1;
		zValue = P.z + tValue * D.z;
		if (zValue <= -cashedExtent)
		{
			t[quantity++] = tValue;
			if (quantity == 2)
			{
				distance = min( t[0], t[1] );

				if ( distance < 0.0f )
					return false;

				if (_distance)
					*_distance = distance;
				return true;
			}
		}
	}

	a1 -= 2.0f * cashedExtent * D.z;
	a0 -= 4.0f * cashedExtent * P.z;
	discr = a1 * a1 - a0;
	if ( discr > EPSILON )
	{
		root = sqrt( discr );
		tValue = -a1 - root;
		zValue = P.z + tValue * D.z;
		if ( zValue >= cashedExtent )
		{
			t[quantity++] = tValue;
			if (quantity == 2)
			{
				distance = min( t[0], t[1] );

				if ( distance < 0.0f )
					return false;

				if (_distance)
					*_distance = distance;
				return true;
			}
		}

		tValue = -a1 + root;
		zValue = P.z + tValue * D.z;
		if ( zValue >= cashedExtent )
		{
			t[quantity++] = tValue;
			if ( quantity == 2 )
			{
				distance = min( t[0], t[1] );

				if ( distance < 0.0f )
					return false;

				if (_distance)
					*_distance = distance;
				return true;
			}
		}
	}
	else if ( abs( discr ) <= EPSILON )
	{
		tValue = -a1;
		zValue = P.z + tValue * D.z;
		if ( zValue >= cashedExtent )
		{
			t[quantity++] = tValue;
			if ( quantity == 2 )
			{
				distance = min( t[0], t[1] );

				if ( distance < 0.0f )
					return false;

				if (_distance)
					*_distance = distance;
				return true;
			}
		}
	}


	if (quantity == 2)
	{
		distance = min( t[0], t[1] );

		if ( distance < 0.0f )
			return false;

		if (_distance)
			*_distance = distance;
		return true;
	}

	return false;
};


bool BoundingCapsule::intersectPoint( const D3DXVECTOR3* _point )
{
	update();


	D3DXVECTOR3 delta = *_point - cashedCenter;

	float cosDist = delta.x * cashedDirection.x + delta.y * cashedDirection.y + delta.z * cashedDirection.z;

	if ( abs( cosDist ) > cashedExtent )
	{
		if ( cosDist > 0.0f )
			delta += cashedExtent * cashedDirection;
		else
			delta -= cashedExtent * cashedDirection;

		return delta.x * delta.x + delta.y * delta.y + delta.z * delta.z < cashedRadius * cashedRadius;
	}

	return delta.x * delta.x + delta.y * delta.y + delta.z * delta.z - cosDist * cosDist < cashedRadius * cashedRadius;
};


float BoundingCapsule::distanceFromPointSquared( const D3DXVECTOR3* _point )
{
	float d = distanceFromPoint( _point );
	return d * d;
};


float BoundingCapsule::distanceFromPoint( const D3DXVECTOR3* _point )
{
	update();


	D3DXVECTOR3 delta = *_point - cashedCenter;

	float cosDist = delta.x * cashedDirection.x + delta.y * cashedDirection.y + delta.z * cashedDirection.z;

	if ( abs( cosDist ) > cashedExtent )
	{
		if ( cosDist > 0.0f )
			delta += cashedExtent * cashedDirection;
		else
			delta -= cashedExtent * cashedDirection;

		return sqrt( delta.x * delta.x + delta.y * delta.y + delta.z * delta.z ) - cashedRadius;
	}

	return sqrt( delta.x * delta.x + delta.y * delta.y + delta.z * delta.z - cosDist * cosDist ) - cashedRadius;
};


bool BoundingCapsule::distanceFromPointLT( const D3DXVECTOR3* _point, float _testValue )
{
	update();


	D3DXVECTOR3 delta = *_point - cashedCenter;

	float cosDist = delta.x * cashedDirection.x + delta.y * cashedDirection.y + delta.z * cashedDirection.z;

	float deltaLengthSq;

	float test = _testValue + cashedRadius;
	test *= test;

	if ( abs( cosDist ) > cashedExtent )
	{
		if ( cosDist > 0.0f )
			delta += cashedExtent * cashedDirection;
		else
			delta -= cashedExtent * cashedDirection;

		return ( delta.x * delta.x + delta.y * delta.y + delta.z * delta.z ) < test;
	}

	return ( delta.x * delta.x + delta.y * delta.y + delta.z * delta.z - cosDist * cosDist ) < test;
};



BoundingCapsuleX::BoundingCapsuleX( WorldPosition* _p_worldPosition, float _extent, float _radius, D3DXVECTOR3* _center ) : 
	BoundingCapsule( _p_worldPosition, _extent, _radius, _center )
{};


BoundingCapsuleZ::BoundingCapsuleZ( WorldPosition* _p_worldPosition, float _extent, float _radius, D3DXVECTOR3* _center ) : 
	BoundingCapsule( _p_worldPosition, _extent, _radius, _center )
{};

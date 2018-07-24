#include "Geometry_Bounding_Box.h"


BoundingBox::BoundingBox( WorldPosition* _p_worldPosition, float _axisXExtent, float _axisYExtent, float _axisZExtent, D3DXVECTOR3* _center ) : BoundingVolume( _p_worldPosition )
{
	if ( _center )
		center = *_center;
	else
		ZeroMemory( &center, sizeof(D3DXVECTOR3) );

	v_axisExtent[0] = _axisXExtent;
	v_axisExtent[1] = _axisYExtent;
	v_axisExtent[2] = _axisZExtent;

	dw_worldPositionTimestamp = p_worldPosition->getWorldPositionTimestamp();
	dw_worldPositionTimestamp--;

	update();
};


BoundingBox::~BoundingBox()
{};


bool BoundingBox::intersectRay( Ray* _ray, float* _distance )
{
	update();

	const D3DXVECTOR3* rayDirection = _ray->getDirection();

	D3DXVECTOR3 diff = cashedCenter - *_ray->getOrigin();
	float BOrigin[3] = 
	{
		diff.x * v_cashedAxis[0].x + diff.y * v_cashedAxis[0].y + diff.z * v_cashedAxis[0].z, 
		diff.x * v_cashedAxis[1].x + diff.y * v_cashedAxis[1].y + diff.z * v_cashedAxis[1].z, 
		diff.x * v_cashedAxis[2].x + diff.y * v_cashedAxis[2].y + diff.z * v_cashedAxis[2].z
	};

	float BDirection[3] = 
	{
		rayDirection->x * v_cashedAxis[0].x + rayDirection->y * v_cashedAxis[0].y + rayDirection->z * v_cashedAxis[0].z, 
		rayDirection->x * v_cashedAxis[1].x + rayDirection->y * v_cashedAxis[1].y + rayDirection->z * v_cashedAxis[1].z, 
		rayDirection->x * v_cashedAxis[2].x + rayDirection->y * v_cashedAxis[2].y + rayDirection->z * v_cashedAxis[2].z
	};

	float t0 = 0.0f;
	float t1 = FLT_MAX;

	float denom;
	float numer;

	BYTE i;
	for ( BYTE j = 0; j < 6; ++j )
	{
		i = j / 2;

		if ( j % 2 )
		{
			denom = BDirection[i];
			numer = BOrigin[i] - v_cashedAxisExtent[i];
		}
		else
		{
			denom = -(BDirection[i]);
			numer = -BOrigin[i] - v_cashedAxisExtent[i];
		}


		if ( denom > 0.0f )
		{
			if ( numer > ( denom * t1 ) )
				return false;

			if ( numer > ( denom * t0) )
				t0 = numer / denom;

			continue;
		}

		if ( denom < 0.0f )
		{
			if ( numer > ( denom * t0 ) )
				return false;

			if ( numer > denom * t1 )
				t1 = numer / denom;

			continue;
		}

		if ( numer > 0.0f )
			return false;
	}

	if ( t1 <= t0 )
		return false;

	if (_distance)
		*_distance = t0;

	return true;	
};


bool BoundingBox::intersectPoint( const D3DXVECTOR3* _point )
{
	update();

	D3DXVECTOR3 delta = cashedCenter - *_point;

	if ( abs( delta.x * v_cashedAxis[0].x + delta.y * v_cashedAxis[0].y + delta.z * v_cashedAxis[0].z ) >= v_cashedAxisExtent[0] || 
		 abs( delta.x * v_cashedAxis[1].x + delta.y * v_cashedAxis[1].y + delta.z * v_cashedAxis[1].z ) >= v_cashedAxisExtent[1] || 
		 abs( delta.x * v_cashedAxis[2].x + delta.y * v_cashedAxis[2].y + delta.z * v_cashedAxis[2].z ) >= v_cashedAxisExtent[2] )
		return false;

	return false;
};


float BoundingBox::distanceFromPointSquared( const D3DXVECTOR3* _point )
{
	update();

	D3DXVECTOR3 delta = *_point - cashedCenter;
	float d, closest, distSq = 0.0f;

	if ( ( closest = abs( delta.x * v_cashedAxis[0].x + delta.y * v_cashedAxis[0].y + delta.z * v_cashedAxis[0].z ) ) > v_cashedAxisExtent[0] )
	{
		d = closest - v_cashedAxisExtent[0];
		distSq += d * d;
	}

	if ( ( closest = abs( delta.x * v_cashedAxis[1].x + delta.y * v_cashedAxis[1].y + delta.z * v_cashedAxis[1].z ) ) > v_cashedAxisExtent[1] )
	{
		d = closest - v_cashedAxisExtent[1];
		distSq += d * d;
	}

	if ( ( closest = abs( delta.x * v_cashedAxis[2].x + delta.y * v_cashedAxis[2].y + delta.z * v_cashedAxis[2].z ) ) > v_cashedAxisExtent[2] )
	{
		d = closest - v_cashedAxisExtent[2];
		distSq += d * d;
	}

	return distSq;
};


float BoundingBox::distanceFromPoint( const D3DXVECTOR3* _point )
{
	return sqrt( distanceFromPointSquared( _point ) );
};


bool BoundingBox::distanceFromPointLT( const D3DXVECTOR3* _point, float _testValue )
{
	update();

	D3DXVECTOR3 delta = *_point - cashedCenter;
	float d, closest, distSq = 0.0f;

	if ( ( closest = abs( delta.x * v_cashedAxis[0].x + delta.y * v_cashedAxis[0].y + delta.z * v_cashedAxis[0].z ) ) > v_cashedAxisExtent[0] )
	{
		d = closest - v_cashedAxisExtent[0];
		distSq += d * d;
	}

	if ( ( closest = abs( delta.x * v_cashedAxis[1].x + delta.y * v_cashedAxis[1].y + delta.z * v_cashedAxis[1].z ) ) > v_cashedAxisExtent[1] )
	{
		d = closest - v_cashedAxisExtent[1];
		distSq += d * d;
	}

	if ( ( closest = abs( delta.x * v_cashedAxis[2].x + delta.y * v_cashedAxis[2].y + delta.z * v_cashedAxis[2].z ) ) > v_cashedAxisExtent[2] )
	{
		d = closest - v_cashedAxisExtent[2];
		distSq += d * d;
	}

	return distSq < _testValue;
};

#include "Geometry_Bounding_Volume.h"

#include "Geometry_Bounding_Sphere.h"
#include "Geometry_Bounding_Box.h"
#include "Geometry_Bounding_Box_AABB.h"
#include "Geometry_Bounding_Capsule.h"
#include "Geometry_Bounding_Cone.h"
#include "Geometry_Bounding_Frustum.h"



BoundingVolume::BoundingVolume( WorldPosition* _p_worldPosition ) : p_worldPosition( _p_worldPosition )
{
	dw_worldPositionTimestamp = p_worldPosition->getWorldPositionTimestamp();
	dw_worldPositionTimestamp--;
};


BoundingVolume::~BoundingVolume()
{};


void BoundingVolume::setWorldPosition( WorldPosition* _p_worldPosition )
{
	p_worldPosition = _p_worldPosition;
	dw_worldPositionTimestamp = p_worldPosition->getWorldPositionTimestamp();
	dw_worldPositionTimestamp--;
};

WorldPosition* BoundingVolume::getWorldPosition()
{
	return p_worldPosition;
};


bool BoundingVolume::intersectAABB( BoundingVolume* _volume )
{
	update();
	_volume->update();

	const D3DXVECTOR3* AABB2Center = _volume->getAABBCenter();
	const float* v_AABB2Extent = _volume->getAABBExtent();

	if ( abs(AABBCenter.x - AABB2Center->x ) > ( v_AABBExtent[0] + v_AABB2Extent[0] ) )
		return false;

	if ( abs(AABBCenter.y - AABB2Center->y ) > ( v_AABBExtent[1] + v_AABB2Extent[1] ) )
		return false;

	if ( abs(AABBCenter.z - AABB2Center->z ) > ( v_AABBExtent[2] + v_AABB2Extent[2] ) )
		return false;

	return true;
};


bool BoundingVolume::intersectAABBRay( Ray* _ray, float* _distance )
{
	update();

	const D3DXVECTOR3* origin = _ray->getOrigin();
	const D3DXVECTOR3* direction = _ray->getDirection();
	const D3DXVECTOR3* directionInverse = _ray->getDirectionInverse();

	float t1 = ( AABBCenter.x - v_AABBExtent[0] - origin->x ) * directionInverse->x;
	float t2 = ( AABBCenter.x + v_AABBExtent[0] - origin->x ) * directionInverse->x;
	float t3 = ( AABBCenter.y - v_AABBExtent[1] - origin->y ) * directionInverse->y;
	float t4 = ( AABBCenter.y + v_AABBExtent[1] - origin->y ) * directionInverse->y;
	float t5 = ( AABBCenter.z - v_AABBExtent[2] - origin->z ) * directionInverse->z;
	float t6 = ( AABBCenter.z + v_AABBExtent[2] - origin->z ) * directionInverse->z;

	float tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
	float tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

	if (tmax < 0 || tmin > tmax)
	    return false;

	if (_distance)
		*_distance = tmin;

	return true;
};


bool BoundingVolume::intersectAABBPoint( const D3DXVECTOR3* _point )
{
	update();

	if ( abs( AABBCenter.x - _point->x ) >= v_AABBExtent[0] || 
		 abs( AABBCenter.y - _point->y ) >= v_AABBExtent[1] || 
		 abs( AABBCenter.z - _point->z ) >= v_AABBExtent[2] )
		return false;

	return false;
};


float BoundingVolume::distanceAABBFromPointSquared( const D3DXVECTOR3* _point )
{
	update();

	float diff[3] = { _point->x - AABBCenter.x, _point->y - AABBCenter.y, _point->z - AABBCenter.z };

	float sqrDistance = 0.0f;
	float delta;

	for ( BYTE i = 0; i < 3; ++i )
	{
		if ( diff[i] < -v_AABBExtent[i] )
		{
			delta = diff[i] + v_AABBExtent[i];
			sqrDistance += delta * delta;
		}
		else if ( diff[i] > v_AABBExtent[i] )
		{
			delta = diff[i] - v_AABBExtent[i];
			sqrDistance += delta * delta;
		}
	}

	return sqrDistance;
};


bool BoundingVolume::distanceAABBFromPointLT( const D3DXVECTOR3* _point, float _testValue )
{
	update();

	float diff[3] = { _point->x - AABBCenter.x, _point->y - AABBCenter.y, _point->z - AABBCenter.z };

	float sqrDistance = 0.0f;
	float delta;

	for ( BYTE i = 0; i < 3; ++i )
	{
		if ( diff[i] < -v_AABBExtent[i] )
		{
			delta = diff[i] + v_AABBExtent[i];
			sqrDistance += delta * delta;
		}
		else if ( diff[i] > v_AABBExtent[i] )
		{
			delta = diff[i] - v_AABBExtent[i];
			sqrDistance += delta * delta;
		}
	}

	return sqrDistance < _testValue * _testValue;
};




bool intersectFunc( BoundingSphere* _sphere1, BoundingSphere* _sphere2 )
{
	_sphere1->update();
	_sphere2->update();


	const D3DXVECTOR3* sphere1Center = _sphere1->getCenter();
	const D3DXVECTOR3* sphere2Center = _sphere2->getCenter();

	float doubleRadiusSq = _sphere1->getRadius() + _sphere2->getRadius();
	doubleRadiusSq *= doubleRadiusSq;


	float tmp = sphere2Center->x - sphere1Center->x;
	float distSq = tmp * tmp;

	tmp = sphere2Center->y - sphere1Center->y;
	distSq += tmp * tmp;

	tmp = sphere2Center->z - sphere1Center->z;
	distSq += tmp * tmp;


	return distSq < doubleRadiusSq;
};
////

bool intersectFunc( BoundingBox* _box1, BoundingBox* _box2 )
{
	_box1->update();
	_box2->update();

	const D3DXVECTOR3* box1Axis = _box1->getAxis();
	const float* box1AxisExtent = _box1->getAxisExtent();

	const D3DXVECTOR3* box2Axis = _box2->getAxis();
	const float* box2AxisExtent = _box2->getAxisExtent();

	bool existsParallelPair = false;
	int i;

	D3DXVECTOR3 delta;
	D3DXVec3Subtract( &delta, _box2, _box1 );

	float C[3][3];
	float AbsC[3][3];
	float AD[3];
	float r01;

	for (i = 0; i < 3; ++i)
	{
		C[0][i] = D3DXVec3Dot( &(box1Axis[0]), &(box2Axis[i]) );
		AbsC[0][i] = abs( C[0][i] );
		if ( AbsC[0][i] > EPSILON_ONE )
		{
			existsParallelPair = true;
		}
	}
	AD[0] = D3DXVec3Dot( &(box1Axis[0]), &delta );
	r01 = box1AxisExtent[0] + box2AxisExtent[0] * AbsC[0][0] + box2AxisExtent[1] * AbsC[0][1] + box2AxisExtent[2] * AbsC[0][2];
	if ( abs( AD[0] ) > r01 )
	{
		return false;
	}

	for (i = 0; i < 3; ++i)
	{
		C[1][i] = D3DXVec3Dot( &(box1Axis[1]), &(box2Axis[i]) );
		AbsC[1][i] = abs( C[1][i] );
		if ( AbsC[1][i] > EPSILON_ONE )
		{
			existsParallelPair = true;
		}
	}
	AD[1] = D3DXVec3Dot( &(box1Axis[1]), &delta );
	r01 = box1AxisExtent[1] + box2AxisExtent[0] * AbsC[1][0] + box2AxisExtent[1] * AbsC[1][1] + box2AxisExtent[2] * AbsC[1][2];
	if ( abs( AD[1] ) > r01 )
	{
		return false;
	}

	for (i = 0; i < 3; ++i)
	{
		C[2][i] = D3DXVec3Dot( &(box1Axis[2]), &(box2Axis[i]) );
		AbsC[2][i] = abs( C[2][i] );
		if ( AbsC[2][i] > EPSILON_ONE )
		{
			existsParallelPair = true;
		}
	}
	AD[2] = D3DXVec3Dot( &(box1Axis[2]), &delta );
	r01 = box1AxisExtent[2] + box2AxisExtent[0] * AbsC[2][0] + box2AxisExtent[1] * AbsC[2][1] + box2AxisExtent[2] * AbsC[2][2];
	if ( abs( AD[2] ) > r01 )
	{
		return false;
	}

	r01 = box1AxisExtent[0] * AbsC[0][0] + box1AxisExtent[1] * AbsC[1][0] + box1AxisExtent[2] * AbsC[2][0] + box2AxisExtent[0];
	if ( abs( D3DXVec3Dot( &(box2Axis[0]), &delta ) ) > r01 )
	{
		return false;
	}

	r01 = box1AxisExtent[0] * AbsC[0][1] + box1AxisExtent[1] * AbsC[1][1] + box1AxisExtent[2] * AbsC[2][1] + box2AxisExtent[1];
	if ( abs( D3DXVec3Dot( &(box2Axis[1]), &delta ) ) > r01 )
	{
		return false;
	}

	r01 = box1AxisExtent[0] * AbsC[0][2] + box1AxisExtent[1] * AbsC[1][2] + box1AxisExtent[2] * AbsC[2][2] + box2AxisExtent[2];
	if ( abs( D3DXVec3Dot( &(box2Axis[2]), &delta ) ) > r01 )
	{
		return false;
	}

	if (existsParallelPair)
	{
		return true;
	}

	r01 = box1AxisExtent[1] * AbsC[2][0] + box1AxisExtent[2] * AbsC[1][0] + box2AxisExtent[1] * AbsC[0][2] + box2AxisExtent[2] * AbsC[0][1];
	if ( abs( AD[2] * C[1][0] - AD[1] * C[2][0] ) > r01 )
	{
		return false;
	}

	r01 = box1AxisExtent[1] * AbsC[2][1] + box1AxisExtent[2] * AbsC[1][1] + box2AxisExtent[0] * AbsC[0][2] + box2AxisExtent[2] * AbsC[0][0];
	if ( abs( AD[2] * C[1][1] - AD[1] * C[2][1] ) > r01 )
	{
		return false;
	}

	r01 = box1AxisExtent[1] * AbsC[2][2] + box1AxisExtent[2] * AbsC[1][2] + box2AxisExtent[0] * AbsC[0][1] + box2AxisExtent[1] * AbsC[0][0];
	if ( abs( AD[2] * C[1][2] - AD[1] * C[2][2] ) > r01 )
	{
		return false;
	}

	r01 = box1AxisExtent[0] * AbsC[2][0] + box1AxisExtent[2] * AbsC[0][0] + box2AxisExtent[1] * AbsC[1][2] + box2AxisExtent[2] * AbsC[1][1];
	if ( abs( AD[0] * C[2][0] - AD[2] * C[0][0] ) > r01 )
	{
		return false;
	}

	r01 = box1AxisExtent[0] * AbsC[2][1] + box1AxisExtent[2] * AbsC[0][1] + box2AxisExtent[0] * AbsC[1][2] + box2AxisExtent[2] * AbsC[1][0];
	if ( abs( AD[0] * C[2][1] - AD[2] * C[0][1] ) > r01 )
	{
		return false;
	}

	r01 = box1AxisExtent[0] * AbsC[2][2] + box1AxisExtent[2] * AbsC[0][2] + box2AxisExtent[0] * AbsC[1][1] + box2AxisExtent[1] * AbsC[1][0];
	if ( abs( AD[0] * C[2][2] - AD[2] * C[0][2] ) > r01 )
	{
		return false;
	}

	r01 = box1AxisExtent[0] * AbsC[1][0] + box1AxisExtent[1] * AbsC[0][0] + box2AxisExtent[1] * AbsC[2][2] + box2AxisExtent[2] * AbsC[2][1];
	if ( abs( AD[1] * C[0][0] - AD[0] * C[1][0] ) > r01 )
	{
		return false;
	}

	r01 = box1AxisExtent[0] * AbsC[1][1] + box1AxisExtent[1] * AbsC[0][1] + box2AxisExtent[0] * AbsC[2][2] + box2AxisExtent[2] * AbsC[2][0];
	if ( abs( AD[1] * C[0][1] - AD[0] * C[1][1] ) > r01 )
	{
		return false;
	}

	r01 = box1AxisExtent[0] * AbsC[1][2] + box1AxisExtent[1] * AbsC[0][2] + box2AxisExtent[0] * AbsC[2][1] + box2AxisExtent[1] * AbsC[2][0];
	if ( abs( AD[1] * C[0][2] - AD[0] * C[1][2] ) > r01 )
	{
		return false;
	}

	return true;
};
////

bool intersectFunc( BoundingBox* _box, BoundingSphere* _sphere )
{
	_box->update();
	_sphere->update();


	const D3DXVECTOR3* boxAxis = _box->getAxis();
	const float* boxAxisExtent = _box->getAxisExtent();


	D3DXVECTOR3 diff;
	D3DXVec3Subtract( &diff, _sphere->getCenter(), _box->getCenter() );

	float ax = abs( D3DXVec3Dot( &diff, &(boxAxis[0]) );
	float ay = abs( D3DXVec3Dot( &diff, &(boxAxis[1]) );
	float az = abs( D3DXVec3Dot( &diff, &(boxAxis[2]) );
	float dx = ax - boxAxisExtent[0];
	float dy = ay - boxAxisExtent[1];
	float dz = az - boxAxisExtent[2];

	if ( ax < boxAxisExtent[0] )
	{
		if ( ay < boxAxisExtent[1] )
		{
			if ( az < boxAxisExtent[2] )
				return true;

			return dz < _sphere->getRadius();

		}

		if ( az < boxAxisExtent[2] )
			return dy < _sphere->getRadius();

		return dy * dy + dz * dz < _sphere->getRadiusSquared();

	}

	if (ay < boxAxisExtent[1])
	{
		if (az < boxAxisExtent[2])
			return dx < _sphere->getRadius();

		return dx * dx + dz * dz < _sphere->getRadiusSquared();
	}

	if (az < boxAxisExtent[2])
		return dx * dx + dy * dy < _sphere->getRadiusSquared();

	return dx * dx + dy * dy + dz * dz < _sphere->getRadiusSquared();
};
////



//bool intersectFunc( BoundingBoxAABB* _boxAABB1, BoundingBoxAABB* _boxAABB2)
////

bool intersectFunc( BoundingBoxAABB* _boxAABB, BoundingBox* _box)
{
	_box->update();
	_boxAABB->update();


	const D3DXVECTOR3* boxAxis = _box->getAxis();
	const float* boxAxisExtent = _box->getAxisExtent();

	const float* boxAABBExtent = _boxAABB->getAxisExtent();

	const float cutoff = 1.0f - EPSILON;
	bool existsParallelPair = false;
	BYTE i;

	D3DXVECTOR3 delta ( *_box->getCenter() - *_boxAABB->getCenter() );

	float AbsC[3][3];
	float r01;


	for ( i = 0; i < 3; ++i )
	{
		AbsC[0][i] = abs( boxAxis[i].x );
		if ( AbsC[0][i] > cutoff )
		{
			existsParallelPair = true;
		}
	}
	r01 = boxAABBExtent[0] + boxAxisExtent[0] * AbsC[0][0] + boxAxisExtent[1] * AbsC[0][1] + boxAxisExtent[2] * AbsC[0][2];
	if ( abs( delta.x ) > r01)
	{
		return false;
	}

	for ( i = 0; i < 3; ++i )
	{
		AbsC[1][i] = abs( boxAxis[i].y );
		if ( AbsC[1][i] > cutoff )
		{
			existsParallelPair = true;
		}
	}
	r01 = boxAABBExtent[1] + boxAxisExtent[0] * AbsC[1][0] + boxAxisExtent[1] * AbsC[1][1] + boxAxisExtent[2] * AbsC[1][2];
	if ( abs( delta.y ) > r01 )
	{
		return false;
	}

	for ( i = 0; i < 3; ++i )
	{
		AbsC[2][i] = abs( boxAxis[i].z );
		if ( AbsC[2][i] > cutoff )
		{
			existsParallelPair = true;
		}
	}

	r01 = boxAABBExtent[2] + boxAxisExtent[0] * AbsC[2][0] + boxAxisExtent[1] * AbsC[2][1] + boxAxisExtent[2] * AbsC[2][2];
	if ( abs( delta.z ) > r01 )
	{
		return false;
	}

	r01 = boxAABBExtent[0] * AbsC[0][0] + boxAABBExtent[1] * AbsC[1][0] + boxAABBExtent[2] * AbsC[2][0] + boxAxisExtent[0];
	if ( abs( D3DXVec3Dot( &(boxAxis[0]), &delta ) ) > r01 )
	{
		return false;
	}

	r01 = boxAABBExtent[0] * AbsC[0][1] + boxAABBExtent[1] * AbsC[1][1] + boxAABBExtent[2] * AbsC[2][1] + boxAxisExtent[1];
	if ( abs( D3DXVec3Dot( &(boxAxis[1]), &delta ) ) > r01 )
	{
		return false;
	}

	r01 = boxAABBExtent[0] * AbsC[0][2] + boxAABBExtent[1] * AbsC[1][2] + boxAABBExtent[2] * AbsC[2][2] + boxAxisExtent[2];
	if ( abs( D3DXVec3Dot( &(boxAxis[2]), &delta ) ) > r01 )
	{
		return false;
	}


	if (existsParallelPair)
	{
		return true;
	}


	r01 = boxAABBExtent[1] * AbsC[2][0] + boxAABBExtent[2] * AbsC[1][0] + boxAxisExtent[1] * AbsC[0][2] + boxAxisExtent[2] * AbsC[0][1];
	if ( abs( delta.z * boxAxis[0].y - delta.y * boxAxis[0].z ) > r01 )
	{
		return false;
	}

	r01 = boxAABBExtent[1] * AbsC[2][1] + boxAABBExtent[2] * AbsC[1][1] + boxAxisExtent[0] * AbsC[0][2] + boxAxisExtent[2] * AbsC[0][0];
	if ( abs( delta.z * boxAxis[1].y - delta.y * boxAxis[1].z ) > r01 )
	{
		return false;
	}

	r01 = boxAABBExtent[1] * AbsC[2][2] + boxAABBExtent[2] * AbsC[1][2] + boxAxisExtent[0] * AbsC[0][1] + boxAxisExtent[1] * AbsC[0][0];
	if ( abs( delta.z * boxAxis[2].y - delta.y * boxAxis[2].z ) > r01 )
	{
		return false;
	}

	r01 = boxAABBExtent[0] * AbsC[2][0] + boxAABBExtent[2] * AbsC[0][0] + boxAxisExtent[1] * AbsC[1][2] + boxAxisExtent[2] * AbsC[1][1];
	if ( abs( delta.x * boxAxis[0].z - delta.z * boxAxis[0].x ) > r01 )
	{
		return false;
	}

	r01 = boxAABBExtent[0] * AbsC[2][1] + boxAABBExtent[2] * AbsC[0][1] + boxAxisExtent[0] * AbsC[1][2] + boxAxisExtent[2] * AbsC[1][0];
	if ( abs( delta.x * boxAxis[1].z - delta.z * boxAxis[1].x ) > r01 )
	{
		return false;
	}

	r01 = boxAABBExtent[0] * AbsC[2][2] + boxAABBExtent[2] * AbsC[0][2] + boxAxisExtent[0] * AbsC[1][1] + boxAxisExtent[1] * AbsC[1][0];
	if ( abs( delta.x * boxAxis[2].z - delta.z * boxAxis[2].x ) > r01 )
	{
		return false;
	}

	r01 = boxAABBExtent[0] * AbsC[1][0] + boxAABBExtent[1] * AbsC[0][0] + boxAxisExtent[1] * AbsC[2][2] + boxAxisExtent[2] * AbsC[2][1];
	if ( abs( delta.y * boxAxis[0].x - delta.x * boxAxis[0].y ) > r01 )
	{
		return false;
	}

	r01 = boxAABBExtent[0] * AbsC[1][1] + boxAABBExtent[1] * AbsC[0][1] + boxAxisExtent[0] * AbsC[2][2] + boxAxisExtent[2] * AbsC[2][0];
	if ( abs( delta.y * boxAxis[1].x - delta.x * boxAxis[1].y ) > r01 )
	{
		return false;
	}

	r01 = boxAABBExtent[0] * AbsC[1][2] + boxAABBExtent[1] * AbsC[0][2] + boxAxisExtent[0] * AbsC[2][1] + boxAxisExtent[1] * AbsC[2][0];
	if ( abs( delta.y * boxAxis[2].x - delta.x * boxAxis[2].y ) > r01 )
	{
		return false;
	}

	return true;
};
////

bool intersectFunc( BoundingBoxAABB* _boxAABB, BoundingSphere* _sphere )
{
	_boxAABB->update();
	_sphere->update();


	const float* boxAABBAxisExtent = _boxAABB->getAxisExtent();


	D3DXVECTOR3 diff;
	D3DXVec3Subtract( &diff, _sphere->getCenter(), _boxAABB->getCenter() );

	float ax = abs( diff.x );
	float ay = abs( diff.y );
	float az = abs( diff.z );
	float dx = ax - boxAABBAxisExtent[0];
	float dy = ay - boxAABBAxisExtent[1];
	float dz = az - boxAABBAxisExtent[2];

	if ( ax <= boxAABBAxisExtent[0] )
	{
		if ( ay <= boxAABBAxisExtent[1] )
		{
			if ( az <= boxAABBAxisExtent[2] )
				return true;

			return dz < _sphere->getRadius();

		}

		if ( az <= boxAABBAxisExtent[2] )
			return dy < _sphere->getRadius();

		return dy * dy + dz * dz <= _sphere->getRadiusSquared();

	}

	if (ay <= boxAABBAxisExtent[1])
	{
		if (az <= boxAABBAxisExtent[2])
			return dx <= _sphere->getRadius();

		return dx * dx + dz * dz <= _sphere->getRadiusSquared();
	}

	if (az <= boxAABBAxisExtent[2])
		return dx * dx + dy * dy <= _sphere->getRadiusSquared();

	return dx * dx + dy * dy + dz * dz <= _sphere->getRadiusSquared();
};
////



bool intersectFunc( BoundingCapsule* _capsule1, BoundingCapsule* _capsule2 )
{
	_capsule1->update();
	_capsule2->update();

	const D3DXVECTOR3* capsule1Line = _capsule1->getDirection();
	float capsule1Extend = _capsule1->getExtent();

	const D3DXVECTOR3* capsule2Line = _capsule2->getDirection();
	float capsule2Extend = _capsule2->getExtent();

	D3DXVECTOR3 diff;
	D3DXVec3Subtract( &diff, _capsule1->getCenter(), _capsule2->getCenter() );


	float a01 = -( D3DXVec3Dot( capsule1Line, capsule2Line ) );
	float b0 = D3DXVec3Dot( &diff, capsule1Line );
	float b1 = -( D3DXVec3Dot( &diff, capsule2Line ) );

	float c = D3DXVec3LengthSq( &diff );
	float det = abs( 1.0f - a01 * a01 );
	float s0, s1, sqrDist, extDet0, extDet1, tmpS0, tmpS1;

	float maxDistSq = _capsule1->getRadius() + _capsule2->getRadius();
	maxDistSq *= maxDistSq;


	if (det >= EPSILON)
	{
		s0 = a01 * b1 - b0;
		s1 = a01 * b0 - b1;
		extDet0 = capsule1Extend * det;
		extDet1 = capsule2Extend * det;

		if ( s0 >= -extDet0 )
		{
			if ( s0 <= extDet0 )
			{
				if ( s1 >= -extDet1 )
				{
					if ( s1 <= extDet1 )
					{
						float invDet = 1.0f / det;
						s0 *= invDet;
						s1 *= invDet;
						sqrDist = s0 * ( s0 + a01 * s1 + 2.0f * b0 ) + s1 * ( a01 * s0 + s1 + 2.0f * b1 ) + c;
						return sqrDist < maxDistSq;
					}

					s1 = capsule2Extend;
					tmpS0 = -( a01 * s1 + b0 );
					if ( tmpS0 < -capsule1Extend )
					{
						s0 = -capsule1Extend;
						sqrDist = s0 * ( s0 - 2.0f * tmpS0 ) + s1 * ( s1 + 2.0f * b1 ) + c;
						return sqrDist < maxDistSq;
					}

					if ( tmpS0 <= capsule1Extend )
					{
						s0 = tmpS0;
						sqrDist = -s0 * s0 + s1 * ( s1 + 2.0f * b1 ) + c;
						return sqrDist < maxDistSq;
					}

					s0 = capsule1Extend;
					sqrDist = s0 * ( s0 - 2.0f * tmpS0 ) + s1 * ( s1 + 2.0f * b1 ) + c;
					return sqrDist < maxDistSq;

				}

				s1 = -capsule2Extend;
				tmpS0 = -( a01 * s1 + b0 );
				if ( tmpS0 < -capsule1Extend )
				{
					s0 = -capsule1Extend;
					sqrDist = s0 * ( s0 - 2.0f * tmpS0 ) + s1 * ( s1 + 2.0f * b1 ) + c;
					return sqrDist < maxDistSq;
				}

				if ( tmpS0 <= capsule1Extend )
				{
					s0 = tmpS0;
					sqrDist = -s0 * s0 + s1 * ( s1 + 2.0f * b1 ) + c;
					return sqrDist < maxDistSq;
				}

				s0 = capsule1Extend;
				sqrDist = s0 * ( s0 - 2.0f * tmpS0 ) + s1 * ( s1 + 2.0f * b1 ) + c;
				return sqrDist < maxDistSq;

			}

			if ( s1 >= -extDet1)
			{
				if ( s1 <= extDet1)
				{
					s0 = capsule1Extend;
					tmpS1 = -(a01* s0 + b1 );
					if ( tmpS1 < -capsule2Extend )
					{
						s1 = -capsule2Extend;
						sqrDist = s1 * ( s1 - 2.0f * tmpS1 ) + s0 * ( s0 + 2.0f * b0 ) + c;
						return sqrDist < maxDistSq;
					}

					if ( tmpS1 <= capsule2Extend )
					{
						s1 = tmpS1;
						sqrDist = -s1 * s1 + s0 * ( s0 + 2.0f * b0 ) + c;
						return sqrDist < maxDistSq;
					}

					s1 = capsule2Extend;
					sqrDist = s1 * ( s1 - 2.0f * tmpS1 ) + s0 * ( s0 + 2.0f * b0 ) + c;
					return sqrDist < maxDistSq;

				}

				s1 = capsule2Extend;
				tmpS0 = -( a01 * s1 + b0 );
				if ( tmpS0 < -capsule1Extend )
				{
					s0 = -capsule1Extend;
					sqrDist = s0 * ( s0 - 2.0f * tmpS0 ) + s1 * ( s1 + 2.0f * b1 ) + c;
					return sqrDist < maxDistSq;
				}

				if ( tmpS0 <= capsule1Extend )
				{
					s0 = tmpS0;
					sqrDist = -s0 * s0 + s1 * ( s1 + 2.0f * b1 ) + c;
					return sqrDist < maxDistSq;
				}

				s0 = capsule1Extend;
				tmpS1 = -(a01* s0 + b1 );
				if ( tmpS1 < -capsule2Extend )
				{
					s1 = -capsule2Extend;
					sqrDist = s1 * ( s1 - 2.0f * tmpS1 ) + s0 * ( s0 + 2.0f * b0 ) + c;
					return sqrDist < maxDistSq;
				}

				if ( tmpS1 <= capsule2Extend )
				{
					s1 = tmpS1;
					sqrDist = -s1 * s1 + s0 * ( s0 + 2.0f * b0 ) + c;
					return sqrDist < maxDistSq;
				}

				s1 = capsule2Extend;
				sqrDist = s1 * ( s1 - 2.0f * tmpS1 ) + s0 * ( s0 + 2.0f * b0 ) + c;
				return sqrDist < maxDistSq;

			}

			s1 = -capsule2Extend;
			tmpS0 = -( a01 * s1 + b0 );
			if ( tmpS0 < -capsule1Extend )
			{
				s0 = -capsule1Extend;
				sqrDist = s0 * ( s0 - 2.0f * tmpS0 ) + s1 * ( s1 + 2.0f * b1 ) + c;
				return sqrDist < maxDistSq;
			}

			if ( tmpS0 <= capsule1Extend )
			{
				s0 = tmpS0;
				sqrDist = -s0 * s0 + s1 * ( s1 + 2.0f * b1 ) + c;
				return sqrDist < maxDistSq;
			}

			s0 = capsule1Extend;
			tmpS1 = -( a01 * s0 + b1 );
			if ( tmpS1 > capsule2Extend )
			{
				s1 = capsule2Extend;
				sqrDist = s1 * ( s1 - 2.0f * tmpS1 ) + s0 * ( s0 + 2.0f * b0 ) + c;
				return sqrDist < maxDistSq;
			}

			if ( tmpS1 >= -capsule2Extend )
			{
				s1 = tmpS1;
				sqrDist = -s1 * s1 + s0 * ( s0 + 2.0f * b0 ) + c;
				return sqrDist < maxDistSq;
			}

			s1 = -capsule2Extend;
			sqrDist = s1 * ( s1 - 2.0f * tmpS1 ) + s0 * ( s0 + 2.0f * b0 ) + c;
			return sqrDist < maxDistSq;

		}

		if ( s1 >= -extDet1 )
		{
			if ( s1 <= extDet1 )
			{
				s0 = -capsule1Extend;
				tmpS1 = -(a01* s0 + b1 );
				if ( tmpS1 < -capsule2Extend )
				{
					s1 = -capsule2Extend;
					sqrDist = s1 * ( s1 - 2.0f * tmpS1 ) + s0 * ( s0 + 2.0f * b0 ) + c;
					return sqrDist < maxDistSq;
				}

				if ( tmpS1 <= capsule2Extend )
				{
					s1 = tmpS1;
					sqrDist = -s1 * s1 + s0 * ( s0 + 2.0f * b0 ) + c;
					return sqrDist < maxDistSq;
				}

				s1 = capsule2Extend;
				sqrDist = s1 * ( s1 - 2.0f * tmpS1 ) + s0 * ( s0 + 2.0f * b0 ) + c;
				return sqrDist < maxDistSq;

			}

			s1 = capsule2Extend;
			tmpS0 = -( a01 * s1 + b0 );
			if ( tmpS0 > capsule1Extend )
			{
				s0 = capsule1Extend;
				sqrDist = s0 * ( s0 - 2.0f * tmpS0 ) + s1 * ( s1 + 2.0f * b1 ) + c;
				return sqrDist < maxDistSq;
			}

			if ( tmpS0 >= -capsule1Extend )
			{
				s0 = tmpS0;
				sqrDist = -s0 * s0 + s1 * ( s1 + 2.0f * b1 ) + c;
				return sqrDist < maxDistSq;
			}

			s0 = -capsule1Extend;
			tmpS1 = -( a01 * s0 + b1 );
			if ( tmpS1 < -capsule2Extend )
			{
				s1 = -capsule2Extend;
				sqrDist = s1 * ( s1 - 2.0f * tmpS1 ) + s0 * ( s0 + 2.0f * b0 ) + c;
				return sqrDist < maxDistSq;
			}

			if ( tmpS1 <= capsule2Extend )
			{
				s1 = tmpS1;
				sqrDist = -s1 * s1 + s0 * ( s0 + 2.0f * b0 ) + c;
				return sqrDist < maxDistSq;
			}

			s1 = capsule2Extend;
			sqrDist = s1 * ( s1 - 2.0f * tmpS1 ) + s0 * ( s0 + 2.0f * b0 ) + c;
			return sqrDist < maxDistSq;

		}

		s1 = -capsule2Extend;
		tmpS0 = -( a01 * s1 + b0 );
		if ( tmpS0 > capsule1Extend )
		{
			s0 = capsule1Extend;
			sqrDist = s0 * ( s0 - 2.0f * tmpS0) + s1 * ( s1 + 2.0f * b1 ) + c;
			return sqrDist < maxDistSq;
		}

		if ( tmpS0 >= -capsule1Extend )
		{
			s0 = tmpS0;
			sqrDist = -s0 * s0 + s1 * ( s1 + 2.0f * b1 ) + c;
			return sqrDist < maxDistSq;
		}

		s0 = -capsule1Extend;
		tmpS1 = -( a01* s0 + b1 );
		if ( tmpS1 < -capsule2Extend )
		{
			s1 = -capsule2Extend;
			sqrDist = s1 * ( s1 - 2.0f * tmpS1 ) + s0 * ( s0 + 2.0f * b0 ) + c;
			return sqrDist < maxDistSq;
		}

		if ( tmpS1 <= capsule2Extend )
		{
			s1 = tmpS1;
			sqrDist = -s1 * s1 + s0 * ( s0 + 2.0f * b0 ) + c;
			return sqrDist < maxDistSq;
		}

		s1 = capsule2Extend;
		sqrDist = s1 * ( s1 - 2.0f * tmpS1 ) + s0 * ( s0 + 2.0f * b0 ) + c;
		return sqrDist < maxDistSq;

	}

	float e0pe1 = capsule1Extend + capsule2Extend;
	float sign = a01 > 0.0f ? -1.0f : 1.0f;
	float b0Avr = 0.5f * ( b0 - sign * b1 );
	float lambda = -b0Avr;
	if ( lambda < -e0pe1 )
	{
		lambda = -e0pe1;
	}
	else if ( lambda > e0pe1 )
	{
		lambda = e0pe1 ;
	}

	s1 = -sign * lambda * capsule2Extend / e0pe1;
	s0 = lambda + sign*s1;
	sqrDist = lambda * ( lambda + 2.0f * b0Avr ) + c;

	return sqrDist < maxDistSq;

};
////

bool intersectFunc( BoundingCapsule* _capsule, BoundingBoxAABB* _boxAABB)
{
	_capsule->update();
	_boxAABB->update();


	const D3DXVECTOR3* capsuleLine = _capsule->getDirection();
	float capsuleExtent = _capsule->getExtent();
	float capsuleRadius = _capsule->getRadius();

	const float* boxAABBExtent = _boxAABB->getAxisExtent();


	float AWdU[3];

	D3DXVECTOR3 diff;
	D3DXVec3Subtract( &diff, _capsule->getCenter(), _boxAABB->getCenter() );

	AWdU[0] = abs( capsuleLine->x );
	if ( abs( diff.x ) >= ( boxAABBExtent[0] + capsuleExtent * AWdU[0] + capsuleRadius ) )
		return false;

	AWdU[1] = abs( capsuleLine->y );
	if ( abs( diff.y ) >= ( boxAABBExtent[1] + capsuleExtent * AWdU[1] + capsuleRadius ) )
		return false;

	AWdU[2] = abs( capsuleLine->z );
	if ( abs( diff.z ) >= ( boxAABBExtent[2] + capsuleExtent * AWdU[2] + capsuleRadius ) )
		return false;

	//cross( capsuleLine , diff )
	float crossX = capsuleLine->y * diff.z - capsuleLine->z * diff.y;
	float crossY = capsuleLine->z * diff.x - capsuleLine->x * diff.z;
	diff.z = capsuleLine->x * diff.y - capsuleLine->y * diff.x;
	diff.x = crossX;
	diff.y = crossY;


	if ( abs( diff.x ) >= ( boxAABBExtent[1] * AWdU[2] + boxAABBExtent[2] * AWdU[1] + capsuleRadius ) )
		return false;

	if ( abs( diff.y ) >= ( boxAABBExtent[0] * AWdU[2] + boxAABBExtent[2] * AWdU[0] + capsuleRadius ) )
		return false;

	if ( abs( diff.z ) >= ( boxAABBExtent[0] * AWdU[1] + boxAABBExtent[1] * AWdU[0] + capsuleRadius ) )
		return false;

	return true;
};
////

bool intersectFunc( BoundingCapsule* _capsule, BoundingBox* _box)
{
	_capsule->update();
	_box->update();


	const D3DXVECTOR3* capsuleLine = _capsule->getDirection();
	float capsuleExtent = _capsule->getExtent();
	float capsuleRadius = _capsule->getRadius();

	const D3DXVECTOR3* boxAxis = _box->getAxis();
	const float* boxAxisExtent = _box->getAxisExtent();


	float AWdU[3], RHS;

	D3DXVECTOR3 diff = *_capsule->getCenter() - *_box->getCenter();

	AWdU[0] = abs( D3DXVec3Dot( capsuleLine, &(boxAxis) );
	RHS = boxAxisExtent[0] + capsuleExtent * AWdU[0] + capsuleRadius;
	if ( abs( D3DXVec3Dot( &diff, &(boxAxis[0]) ) ) > RHS )
	{
		return false;
	}

	AWdU[1] = abs( D3DXVec3Dot( capsuleLine, &(boxAxis[1]) ) );
	RHS = boxAxisExtent[1] + capsuleExtent * AWdU[1] + capsuleRadius;
	if ( abs( D3DXVec3Dot( &diff, &(boxAxis[1]) ) ) > RHS )
	{
		return false;
	}

	AWdU[2] = abs( D3DXVec3Dot( capsuleLine, &(boxAxis[2]) ) );
	RHS = boxAxisExtent[2] + capsuleExtent * AWdU[2] + capsuleRadius;
	if ( abs( D3DXVec3Dot( &diff, &(boxAxis[2]) ) ) > RHS )
	{
		return false;
	}

	//cross( capsuleLine , diff )
	float crossX = capsuleLine->y * diff.z - capsuleLine->z * diff.y;
	float crossY = capsuleLine->z * diff.x - capsuleLine->x * diff.z;
	diff.z = capsuleLine->x * diff.y - capsuleLine->y * diff.x;
	diff.x = crossX;
	diff.y = crossY;


	RHS = boxAxisExtent[1] * AWdU[2] + boxAxisExtent[2] * AWdU[1] + capsuleRadius;
	if ( abs( D3DXVec3Dot( &diff, &(boxAxis[0]) ) ) > RHS )
	{
		return false;
	}

	RHS = boxAxisExtent[0] * AWdU[2] + boxAxisExtent[2] * AWdU[0] + capsuleRadius;
	if ( abs( D3DXVec3Dot( &diff, &(boxAxis[1]) ) ) > RHS )
	{
		return false;
	}

	RHS = boxAxisExtent[0] * AWdU[1] + boxAxisExtent[1] * AWdU[0] + capsuleRadius;
	if ( abs( D3DXVec3Dot( &diff, &(boxAxis[2]) ) ) > RHS )
	{
		return false;
	}

	return true;
};
////

bool intersectFunc( BoundingCapsule* _capsule, BoundingSphere* _sphere )
{
	_capsule->update();
	_sphere->update();


	const D3DXVECTOR3* sphereCenter = _sphere->getCenter();

	const D3DXVECTOR3* capsuleCenter = _capsule->getCenter();
	const D3DXVECTOR3* capsuleLine = _capsule->getDirection();
	float capsuleExtent = _capsule->getExtent();

	float maxDistanceSquared = _capsule->getRadius();
	maxDistanceSquared *= maxDistanceSquared;
	maxDistanceSquared += _sphere->getRadiusSquared();


	D3DXVECTOR3 delta = *sphereCenter - ( *capsuleCenter + capsuleExtent * *capsuleLine );
	if ( D3DXVec3Dot( &delta, capsuleLine ) >= 0.0f )
	{
		return D3DXVec3LengthSq( &delta ) < maxDistanceSquared;
	}

	delta = *sphereCenter - ( *capsuleCenter - capsuleExtent * *capsuleLine );
	if ( D3DXVec3Dot( &delta, capsuleLine ) <= 0.0f )
		return D3DXVec3LengthSq( &delta ) < maxDistanceSquared;

	delta = *sphereCenter - *capsuleCenter;
	float projDelta = D3DXVec3Dot( &delta, capsuleLine );
	return D3DXVec3LengthSq( &delta ) < maxDistanceSquared;
};
////



bool intersectFunc( BoundingCone* _cone1, BoundingCone* _cone2 )
{
	_cone1->update();
	_cone2->update();


	float capsule1Radius = _cone1->getRadius();
	float capsule2Radius = _cone2->getRadius();

	float capsule1Extend = ( _cone1->getHeight() + capsule1Radius ) * 0.5f;
	float capsule2Extend = ( _cone2->getHeight() + capsule2Radius ) * 0.5f;

	const D3DXVECTOR3* capsule1Line = _cone1->getDirection();
	const D3DXVECTOR3* capsule2Line = _cone2->getDirection();

	D3DXVECTOR3 capsule1Center = *_cone1->getApex() + capsule1Extend * *capsule1Line;
	D3DXVECTOR3 capsule2Center = *_cone2->getApex() + capsule2Extend * *capsule2Line;


	D3DXVECTOR3 diff;
	D3DXVec3Subtract( &diff, &capsule1Center, &capsule2Center );

	float a01 = -( D3DXVec3Dot( capsule1Line, capsule2Line ) );
	float b0 = D3DXVec3Dot( &diff, capsule1Line );
	float b1 = -( D3DXVec3Dot( &diff, capsule2Line ) );

	float c = D3DXVec3LengthSq( &diff );
	float det = abs( 1.0f - a01 * a01 );
	float s0, s1, sqrDist, extDet0, extDet1, tmpS0, tmpS1;

	float maxDistSq = capsule1Radius + capsule2Radius;
	maxDistSq *= maxDistSq;


	if ( det >= EPSILON )
	{
		s0 = a01 * b1 - b0;
		s1 = a01 * b0 - b1;
		extDet0 = capsule1Extend * det;
		extDet1 = capsule2Extend * det;

		if ( s0 >= -extDet0 )
		{
			if ( s0 <= extDet0 )
			{
				if ( s1 >= -extDet1 )
				{
					if ( s1 <= extDet1 )
					{
						float invDet = 1.0f / det;
						s0 *= invDet;
						s1 *= invDet;
						sqrDist = s0 * ( s0 + a01 * s1 + 2.0f * b0 ) + s1 * ( a01 * s0 + s1 + 2.0f * b1 ) + c;
						return sqrDist < maxDistSq;
					}

					s1 = capsule2Extend;
					tmpS0 = -( a01 * s1 + b0 );
					if ( tmpS0 < -capsule1Extend )
					{
						s0 = -capsule1Extend;
						sqrDist = s0 * ( s0 - 2.0f * tmpS0 ) + s1 * ( s1 + 2.0f * b1 ) + c;
						return sqrDist < maxDistSq;
					}

					if ( tmpS0 <= capsule1Extend )
					{
						s0 = tmpS0;
						sqrDist = -s0 * s0 + s1 * ( s1 + 2.0f * b1 ) + c;
						return sqrDist < maxDistSq;
					}

					s0 = capsule1Extend;
					sqrDist = s0 * ( s0 - 2.0f * tmpS0 ) + s1 * ( s1 + 2.0f * b1 ) + c;
					return sqrDist < maxDistSq;

				}

				s1 = -capsule2Extend;
				tmpS0 = -( a01 * s1 + b0 );
				if ( tmpS0 < -capsule1Extend )
				{
					s0 = -capsule1Extend;
					sqrDist = s0 * ( s0 - 2.0f * tmpS0 ) + s1 * ( s1 + 2.0f * b1 ) + c;
					return sqrDist < maxDistSq;
				}

				if ( tmpS0 <= capsule1Extend )
				{
					s0 = tmpS0;
					sqrDist = -s0 * s0 + s1 * ( s1 + 2.0f * b1 ) + c;
					return sqrDist < maxDistSq;
				}

				s0 = capsule1Extend;
				sqrDist = s0 * ( s0 - 2.0f * tmpS0 ) + s1 * ( s1 + 2.0f * b1 ) + c;
				return sqrDist < maxDistSq;

			}

			if ( s1 >= -extDet1 )
			{
				if ( s1 <= extDet1 )
				{
					s0 = capsule1Extend;
					tmpS1 = -( a01 * s0 + b1 );
					if ( tmpS1 < -capsule2Extend )
					{
						s1 = -capsule2Extend;
						sqrDist = s1 * ( s1 - 2.0f * tmpS1 ) + s0 * ( s0 + 2.0f * b0 ) + c;
						return sqrDist < maxDistSq;
					}

					if ( tmpS1 <= capsule2Extend )
					{
						s1 = tmpS1;
						sqrDist = -s1 * s1 + s0 * ( s0 + 2.0f * b0 ) + c;
						return sqrDist < maxDistSq;
					}

					s1 = capsule2Extend;
					sqrDist = s1 * ( s1 - 2.0f * tmpS1 ) + s0 * ( s0 + 2.0f * b0 ) + c;
					return sqrDist < maxDistSq;

				}

				s1 = capsule2Extend;
				tmpS0 = -( a01 * s1 + b0 );
				if ( tmpS0 < -capsule1Extend )
				{
					s0 = -capsule1Extend;
					sqrDist = s0 * ( s0 - 2.0f * tmpS0 ) + s1 * ( s1 + 2.0f * b1 ) + c;
					return sqrDist < maxDistSq;
				}

				if ( tmpS0 <= capsule1Extend )
				{
					s0 = tmpS0;
					sqrDist = -s0 * s0 + s1 * ( s1 + 2.0f * b1 ) + c;
					return sqrDist < maxDistSq;
				}

				s0 = capsule1Extend;
				tmpS1 = -( a01 * s0 + b1 );
				if ( tmpS1 < -capsule2Extend )
				{
					s1 = -capsule2Extend;
					sqrDist = s1 * ( s1 - 2.0f * tmpS1 ) + s0 * ( s0 + 2.0f * b0 ) + c;
					return sqrDist < maxDistSq;
				}

				if ( tmpS1 <= capsule2Extend )
				{
					s1 = tmpS1;
					sqrDist = -s1 * s1 + s0 * ( s0 + 2.0f * b0 ) + c;
					return sqrDist < maxDistSq;
				}

				s1 = capsule2Extend;
				sqrDist = s1 * ( s1 - 2.0f * tmpS1 ) + s0 * ( s0 + 2.0f * b0 ) + c;
				return sqrDist < maxDistSq;

			}

			s1 = -capsule2Extend;
			tmpS0 = -( a01 * s1 + b0 );
			if ( tmpS0 < -capsule1Extend )
			{
				s0 = -capsule1Extend;
				sqrDist = s0 * ( s0 - 2.0f * tmpS0 ) + s1 * ( s1 + 2.0f * b1 ) + c;
				return sqrDist < maxDistSq;
			}

			if ( tmpS0 <= capsule1Extend )
			{
				s0 = tmpS0;
				sqrDist = -s0 * s0 + s1 * ( s1 + 2.0f * b1 ) + c;
				return sqrDist < maxDistSq;
			}

			s0 = capsule1Extend;
			tmpS1 = -( a01 * s0 + b1 );
			if ( tmpS1 > capsule2Extend )
			{
				s1 = capsule2Extend;
				sqrDist = s1 * ( s1 - 2.0f * tmpS1 ) + s0 * ( s0 + 2.0f * b0 ) + c;
				return sqrDist < maxDistSq;
			}

			if ( tmpS1 >= -capsule2Extend )
			{
				s1 = tmpS1;
				sqrDist = -s1 * s1 + s0 * ( s0 + 2.0f * b0 ) + c;
				return sqrDist < maxDistSq;
			}

			s1 = -capsule2Extend;
			sqrDist = s1 * ( s1 - 2.0f * tmpS1 ) + s0 * ( s0 + 2.0f * b0 ) + c;
			return sqrDist < maxDistSq;

		}

		if ( s1 >= -extDet1 )
		{
			if ( s1 <= extDet1 )
			{
				s0 = -capsule1Extend;
				tmpS1 = -( a01 * s0 + b1 );
				if ( tmpS1 < -capsule2Extend )
				{
					s1 = -capsule2Extend;
					sqrDist = s1 * ( s1 - 2.0f * tmpS1 ) + s0 * ( s0 + 2.0f * b0 ) + c;
					return sqrDist < maxDistSq;
				}

				if ( tmpS1 <= capsule2Extend )
				{
					s1 = tmpS1;
					sqrDist = -s1 * s1 + s0 * ( s0 + 2.0f * b0 ) + c;
					return sqrDist < maxDistSq;
				}

				s1 = capsule2Extend;
				sqrDist = s1 * ( s1 - 2.0f * tmpS1 ) + s0 * ( s0 + 2.0f * b0 ) + c;
				return sqrDist < maxDistSq;

			}

			s1 = capsule2Extend;
			tmpS0 = -( a01 * s1 + b0 );
			if ( tmpS0 > capsule1Extend )
			{
				s0 = capsule1Extend;
				sqrDist = s0 * ( s0 - 2.0f * tmpS0 ) + s1 * ( s1 + 2.0f * b1 ) + c;
				return sqrDist < maxDistSq;
			}

			if ( tmpS0 >= -capsule1Extend )
			{
				s0 = tmpS0;
				sqrDist = -s0 * s0 + s1 * ( s1 + 2.0f * b1 ) + c;
				return sqrDist < maxDistSq;
			}

			s0 = -capsule1Extend;
			tmpS1 = -( a01 * s0 + b1 );
			if ( tmpS1 < -capsule2Extend )
			{
				s1 = -capsule2Extend;
				sqrDist = s1 * ( s1 - 2.0f * tmpS1 ) + s0 * ( s0 + 2.0f * b0 ) + c;
				return sqrDist < maxDistSq;
			}

			if ( tmpS1 <= capsule2Extend )
			{
				s1 = tmpS1;
				sqrDist = -s1 * s1 + s0 * ( s0 + 2.0f * b0 ) + c;
				return sqrDist < maxDistSq;
			}

			s1 = capsule2Extend;
			sqrDist = s1 * ( s1 - 2.0f * tmpS1 ) + s0 * ( s0 + 2.0f * b0 ) + c;
			return sqrDist < maxDistSq;

		}

		s1 = -capsule2Extend;
		tmpS0 = -( a01 * s1 + b0 );
		if ( tmpS0 > capsule1Extend )
		{
			s0 = capsule1Extend;
			sqrDist = s0 * ( s0 - 2.0f * tmpS0 ) + s1 * ( s1 + 2.0f * b1 ) + c;
			return sqrDist < maxDistSq;
		}

		if ( tmpS0 >= -capsule1Extend )
		{
			s0 = tmpS0;
			sqrDist = -s0 * s0 + s1 * ( s1 + 2.0f * b1 ) + c;
			return sqrDist < maxDistSq;
		}

		s0 = -capsule1Extend;
		tmpS1 = -( a01 * s0 + b1 );
		if ( tmpS1 < -capsule2Extend )
		{
			s1 = -capsule2Extend;
			sqrDist = s1 * ( s1 - 2.0f * tmpS1 ) + s0 * ( s0 + 2.0f * b0 ) + c;
			return sqrDist < maxDistSq;
		}

		if ( tmpS1 <= capsule2Extend )
		{
			s1 = tmpS1;
			sqrDist = -s1 * s1 + s0 * ( s0 + 2.0f * b0  ) + c;
			return sqrDist < maxDistSq;
		}

		s1 = capsule2Extend;
		sqrDist = s1 * ( s1 - 2.0f * tmpS1 ) + s0 * ( s0 + 2.0f * b0 ) + c;
		return sqrDist < maxDistSq;

	}

	float e0pe1 = capsule1Extend + capsule2Extend;
	float sign = a01 > 0.0f ? -1.0f : 1.0f;
	float b0Avr = 0.5f * ( b0 - sign * b1 );
	float lambda = -b0Avr;
	if ( lambda < -e0pe1 )
	{
		lambda = -e0pe1;
	}
	else if ( lambda > e0pe1 )
	{
		lambda = e0pe1;
	}

	s1 = -sign * lambda * capsule2Extend / e0pe1;
	s0 = lambda + sign * s1;
	sqrDist = lambda * ( lambda + 2.0f * b0Avr ) + c;

	return sqrDist < maxDistSq;
};
////

bool intersectFunc( BoundingCone* _cone, BoundingCapsule* _capsule)
{
	_cone->update();
	_capsule->update();


	float coneHeight = _cone->getHeight();
	const D3DXPLANE* coneFrustum = _cone->getFrustum();


	const D3DXVECTOR3* capsuleCenter = _capsule->getCenter();
	const D3DXVECTOR3* capsuleLine = _capsule->getDirection();
	float capsuleExtent = _capsule->getExtent();
	float capsuleRadius = _capsule->getRadius();


	float dist1 = D3DXPlaneDotCoord( &(coneFrustum[CONE_FRUST_FRONT]), capsuleCenter );
	float dist2 = D3DXPlaneDotNormal( &(coneFrustum[CONE_FRUST_FRONT]), capsuleLine );

	if ( ( dist1 - capsuleExtent * abs(dist2) ) >= capsuleRadius || ( dist1 - capsuleExtent * abs(dist2) ) <= - ( capsuleRadius - coneHeight ) )
			return false;


	for ( BYTE i = CONE_FRUST_FRONT + 1; i < 5; ++i )
	{
		dist1 = D3DXPlaneDotCoord( &(coneFrustum[i]), capsuleCenter );
		dist2 = D3DXPlaneDotNormal( &(coneFrustum[i]), capsuleLine );
		
		if ( ( dist1 - capsuleExtent * abs(dist2) ) >= capsuleRadius )
			return false;
	}

	return true;
};
////

bool intersectFunc( BoundingCone* _cone, BoundingBoxAABB* _boxAABB)
{
	_cone->update();
	_boxAABB->update();


	const D3DXVECTOR3* boxAABBCenter = _boxAABB->getCenter();
	const float* boxAABBExtent = _boxAABB->getAxisExtent();
	const D3DXPLANE* coneFrustum = _cone->getFrustum();
	float coneHeight = _cone->getHeight();


	float boxPlaneDist = D3DXPlaneDotCoord( &(coneFrustum[CONE_FRUST_FRONT]), boxAABBCenter );

	float absExtent =	boxAABBExtent[0] * abs( coneFrustum[CONE_FRUST_FRONT].a ) + 
						boxAABBExtent[1] * abs( coneFrustum[CONE_FRUST_FRONT].b ) + 
						boxAABBExtent[2] * abs( coneFrustum[CONE_FRUST_FRONT].c );


	if ( boxPlaneDist >= absExtent || boxPlaneDist <= -( absExtent + coneHeight ) )
		return false;


	for ( BYTE i = CONE_FRUST_FRONT + 1; i < 5; ++i)
	{
		float boxPlaneDist = D3DXPlaneDotCoord( &(coneFrustum[i]), boxAABBCenter );

		float absExtent =	boxAABBExtent[0] * abs( coneFrustum[i].a ) + 
							boxAABBExtent[1] * abs( coneFrustum[i].b ) + 
							boxAABBExtent[2] * abs( coneFrustum[i].c );

		if ( boxPlaneDist >= absExtent )
			return false;
	}

	return true;
};
////

bool intersectFunc( BoundingCone* _cone, BoundingBox* _box)
{
	_cone->update();
	_box->update();

	
	const D3DXVECTOR3* boxCenter = _box->getCenter();
	const D3DXVECTOR3* boxAxis = _box->getAxis();
	const float* boxExtent = _box->getAxisExtent();
	const D3DXPLANE* coneFrustum = _cone->getFrustum();
	float coneHeight = _cone->getHeight();


	float boxPlaneDist = D3DXPlaneDotCoord( &(coneFrustum[CONE_FRUST_FRONT]), boxCenter );

	float absExtent =	boxExtent[0] * abs(	D3DXPlaneDotNormal( &(coneFrustum[CONE_FRUST_FRONT]), &(boxAxis[0]) ) ) + 
						boxExtent[1] * abs(	D3DXPlaneDotNormal( &(coneFrustum[CONE_FRUST_FRONT]), &(boxAxis[1]) ) ) + 
						boxExtent[2] * abs(	D3DXPlaneDotNormal( &(coneFrustum[CONE_FRUST_FRONT]), &(boxAxis[2]) ) );

	if ( boxPlaneDist >= absExtent || boxPlaneDist <= -( absExtent + coneHeight ) )
		return false;


	for ( BYTE i = CONE_FRUST_FRONT + 1; i < 5; ++i)
	{
		float boxPlaneDist = D3DXPlaneDotCoord( &(coneFrustum[i]), boxCenter );

		float absExtent =	boxExtent[0] * abs( D3DXPlaneDotNormal( &(coneFrustum[i]), &(boxAxis[0]) ) ) + 
							boxExtent[1] * abs(	D3DXPlaneDotNormal( &(coneFrustum[i]), &(boxAxis[1]) ) ) + 
							boxExtent[2] * abs(	D3DXPlaneDotNormal( &(coneFrustum[i]), &(boxAxis[2]) ) );

		if ( boxPlaneDist >= absExtent )
			return false;
	}

	return true;
};
////

bool intersectFunc( BoundingCone* _cone, BoundingSphere* _sphere )
{
	_cone->update();
	_sphere->update();


	const D3DXVECTOR3* coneDirection = _cone->getDirection();
	float coneHeight = _cone->getHeight();

	float sphereRadius = _sphere->getRadius();
	float sphereRadiusSq = _sphere->getRadiusSquared();


	D3DXVECTOR3 CmV;
	D3DXVec3Subtract( &CmV, _sphere->getCenter(), _cone->getApex() );
	float e1 = D3DXVec3Dot( &CmV, coneDirection );
	if ( e1 >= ( coneHeight + sphereRadius ) )
		return false;

	D3DXVECTOR3 D = CmV + ( sphereRadius * _cone->getInvAngleSine() ) * *coneDirection;
	float DSqrLen = D3DXVec3LengthSq( &D );
	float e = D3DXVec3Dot( &D, coneDirection );

	if ( e > 0.0f && ( e * e ) > ( DSqrLen * _cone->getAngleCosineSquared() ) )
	{
		DSqrLen = D3DXVec3LengthSq( &CmV );
		if ( e1 < 0.0f && ( e1 * e1 ) > ( DSqrLen * _cone->getAngleSineSquared() ) )
		{
			return DSqrLen < sphereRadiusSq;
		}

		return true;
	}

	return false;
};
////



bool intersectFunc( BoundingFrustum* _frustum1, BoundingFrustum* _frustum2 )
{
	_frustum1->update();
	_frustum2->update();


	float frustum1Height1 = _frustum1->getHeight1();
	float frustum1Height2 = _frustum1->getHeight2();

	const D3DXVECTOR3* frustum1Direction = _frustum1->getDirection();
	const D3DXVECTOR3* frustum1Up = _frustum1->getUp();
	const D3DXVECTOR3* frustum1Right = _frustum1->getRight();


	D3DXVECTOR3 points[8];
	float tmp;

	D3DXVECTOR3 baseCenter ( frustum1Direction->x * frustum1Height1, frustum1Direction->y * frustum1Height1, frustum1Direction->z * frustum1Height1 );
	D3DXVECTOR3 upv = *frustum1Up * 0.5f * _frustum1->getFocalLengthVertical() * frustum1Height1;
	D3DXVECTOR3 riv = *frustum1Right * 0.5f * _frustum1->getFocalLengthHorizontal() * frustum1Height1;


	points[0] = baseCenter + upv;
	points[1] = points[0] - riv;
	points[0] += riv;

	points[2] = baseCenter - upv;
	points[3] = points[2] - riv;
	points[2] += riv;


	baseCenter.x = frustum1Direction->x * frustum1Height2;
	baseCenter.y = frustum1Direction->y * frustum1Height2;
	baseCenter.z = frustum1Direction->z * frustum1Height2;
	upv = *frustum1Up * 0.5f * _frustum1->getBaseHeight();
	riv = *frustum1Right * 0.5f * _frustum1->getBaseWidth();


	points[4] = baseCenter + upv;
	points[5] = points[4] - riv;
	points[4] += riv;

	points[6] = baseCenter - upv;
	points[7] = points[6] - riv;
	points[6] += riv;



	const D3DXPLANE* frustum2Planes = _frustum2->getPlanes();


	bool flag = false;
	bool flag2 = false;
	float dist;
	float mFrustum2Height = - _frustum2->getHeight();


	for ( BYTE j = 0; j < 8; ++j )
	{
		dist = D3DXPlaneDotCoord( &(frustum2Planes[FRUSTUM_FRONT]), &(points[j]) );

		if ( dist < 0.0f )
			flag |= true;

		if ( dist > mFrustum2Height )
			flag2 |= true;

		if ( flag && flag2 )
			break;

	}

	if ( !flag || !flag2 )
		return false;


	for ( BYTE i = 2; i < 6; ++i )
	{
		flag = false;

		for ( BYTE j = 0; j < 8; ++j )
		{
			dist = D3DXPlaneDotCoord( &(frustum2Planes[i]), &(points[j]) );

			if ( dist < 0.0f )
			{
				flag = true;
				break;
			}
		}

		if ( !flag )
			return false;
	}

	return true;
};
////


bool intersectFunc( BoundingFrustum* _frustum, BoundingCone* _cone )
{
	_frustum->update();
	_cone->update();


	const D3DXVECTOR3* coneApex = _cone->getApex();
	const D3DXVECTOR3* coneDirection = _cone->getDirection();
	float coneHeight = _cone->getHeight();
	float coneRadius = _cone->getRadius();

	D3DXVECTOR3 coneEnd = *coneApex + ( coneHeight * *coneDirection );

	const D3DXPLANE* frustumPlanes = _frustum->getPlanes();

	float dist;

	for ( BYTE i = 0; i < 6; ++i )
	{
		dist = D3DXPlaneDotCoord( &(frustumPlanes[i]) , &coneEnd );

		if ( dist > coneHeight )
			return false;

		if ( dist > coneRadius )
		{
			dist = D3DXPlaneDotCoord( &(frustumPlanes[i]), coneApex );
			if ( dist > 0.0f )
				return false;
		}
	}

	return true;
};


/*
bool intersectFunc( BoundingFrustum* _frustum, BoundingCone* _cone )
{
	_frustum->update();
	_cone->update();


	const D3DXPLANE* frustumPlanes = _frustum->getPlanes();

	const D3DXVECTOR3* coneDirection = _cone->getDirection();
	float coneRadius = _cone->getRadius();

	float capsuleExtent = 0.5f * ( coneRadius + _cone->getHeight() );
	D3DXVECTOR3 capsuleCenter = *_cone->getApex() + capsuleExtent * *coneDirection;


	float dist1 =	frustumPlanes[FRUSTUM_FRONT].a * capsuleCenter.x + 
					frustumPlanes[FRUSTUM_FRONT].b * capsuleCenter.y + 
					frustumPlanes[FRUSTUM_FRONT].c * capsuleCenter.z + 
					frustumPlanes[FRUSTUM_FRONT].d;

	float dist2 =	frustumPlanes[FRUSTUM_FRONT].a * coneDirection->x + 
					frustumPlanes[FRUSTUM_FRONT].b * coneDirection->y + 
					frustumPlanes[FRUSTUM_FRONT].c * coneDirection->z;

	dist1 -= capsuleExtent * abs(dist2);
	if ( dist1 >= coneRadius || dist1 <= - ( coneRadius - _frustum->getHeight() ) )
		return false;


	for ( BYTE i = 2; i < 6; ++i )
	{
		dist1 =	frustumPlanes[i].a * capsuleCenter.x + 
				frustumPlanes[i].b * capsuleCenter.y + 
				frustumPlanes[i].c * capsuleCenter.z + 
				frustumPlanes[i].d;

		dist2 =	frustumPlanes[i].a * coneDirection->x + 
				frustumPlanes[i].b * coneDirection->y + 
				frustumPlanes[i].c * coneDirection->z;

		if ( ( dist1 - capsuleExtent * abs(dist2) ) >= coneRadius )
			return false;
	}

	return true;
};
*/


bool intersectFunc( BoundingFrustum* _frustum, BoundingCapsule* _capsule)
{
	_frustum->update();
	_capsule->update();


	const D3DXPLANE* frustumPlanes = _frustum->getPlanes();

	const D3DXVECTOR3* capsuleCenter = _capsule->getCenter();
	const D3DXVECTOR3* capsuleLine = _capsule->getDirection();
	float capsuleExtent = _capsule->getExtent();
	float capsuleRadius = _capsule->getRadius();


	float dist1 = D3DXPlaneDotCoord( &(frustumPlanes[FRUSTUM_FRONT]), capsuleCenter );
	float dist2 = D3DXPlaneDotNormal( &(frustumPlanes[FRUSTUM_FRONT]), capsuleLine );

	if ( ( dist1 - capsuleExtent * abs(dist2) ) >= capsuleRadius || ( dist1 - capsuleExtent * abs(dist2) ) <= - ( capsuleRadius - _frustum->getHeight() ) )
			return false;


	for ( BYTE i = 2; i < 6; ++i )
	{
		dist1 = D3DXPlaneDotCoord( &(frustumPlanes[i]), capsuleCenter );
		dist2 = D3DXPlaneDotNormal( &(frustumPlanes[i]), capsuleLine );

		if ( ( dist1 - capsuleExtent * abs(dist2) ) >= capsuleRadius )
			return false;
	}

	return true;
};
////

bool intersectFunc( BoundingFrustum* _frustum, BoundingBoxAABB* _boxAABB)
{
	_boxAABB->update();
	_frustum->update();


	const D3DXPLANE* frustumPlanes = _frustum->getPlanes();

	const D3DXVECTOR3* boxAABBCenter = _boxAABB->getCenter();
	const float* boxAABBExtent = _boxAABB->getAxisExtent();


	float boxPlaneDist = D3DXPlaneDotCoord( &(frustumPlanes[FRUSTUM_FRONT]), boxAABBCenter );

	float absExtent =	abs( frustumPlanes[FRUSTUM_FRONT].a * boxAABBExtent[0] ) + 
						abs( frustumPlanes[FRUSTUM_FRONT].b * boxAABBExtent[1] ) + 
						abs( frustumPlanes[FRUSTUM_FRONT].c * boxAABBExtent[2] );

	if ( boxPlaneDist >= absExtent || - ( boxPlaneDist + _frustum->getHeight() ) >= absExtent )
		return false;


	for ( BYTE i = FRUSTUM_BACK + 1; i < 6; ++i)
	{
		boxPlaneDist = D3DXPlaneDotCoord( &(frustumPlanes[i]), boxAABBCenter );

		absExtent =	abs( boxAABBExtent[0] * frustumPlanes[i].a ) + 
					abs( boxAABBExtent[1] * frustumPlanes[i].b ) + 
					abs( boxAABBExtent[2] * frustumPlanes[i].c );

		if ( boxPlaneDist >= absExtent )
			return false;
	}

	return true;
};
////

bool intersectFunc( BoundingFrustum* _frustum, BoundingBox* _box)
{
	_box->update();
	_frustum->update();


	const D3DXPLANE* frustumPlanes = _frustum->getPlanes();

	const D3DXVECTOR3* boxCenter = _box->getCenter();
	const D3DXVECTOR3* boxAxis = _box->getAxis();
	const float* boxExtent = _box->getAxisExtent();


	float boxPlaneDist = D3DXPlaneDotCoord( &(frustumPlanes[FRUSTUM_FRONT]), boxCenter );

	float absExtent =	boxExtent[0] * abs( D3DXPlaneDotNormal( &(frustumPlanes[FRUSTUM_FRONT]), &(boxAxis[0]) ) ) + 
						boxExtent[1] * abs( D3DXPlaneDotNormal( &(frustumPlanes[FRUSTUM_FRONT]), &(boxAxis[1]) ) ) + 
						boxExtent[2] * abs( D3DXPlaneDotNormal( &(frustumPlanes[FRUSTUM_FRONT]), &(boxAxis[2]) ) );

	if ( boxPlaneDist >= absExtent || - ( boxPlaneDist + _frustum->getHeight() ) >= absExtent )
		return false;


	for ( BYTE i = FRUSTUM_BACK + 1; i < 6; ++i)
	{
		boxPlaneDist = D3DXPlaneDotCoord( &(frustumPlanes[i]), boxCenter );

		absExtent =	boxExtent[0] * abs( D3DXPlaneDotNormal( &(frustumPlanes[i], &(boxAxis[0]) ) + 
					boxExtent[1] * abs( D3DXPlaneDotNormal( &(frustumPlanes[i], &(boxAxis[1]) ) + 
					boxExtent[2] * abs( D3DXPlaneDotNormal( &(frustumPlanes[i], &(boxAxis[2]) );

		if ( boxPlaneDist >= absExtent )
			return false;
	}

	return true;
};
////

bool intersectFunc( BoundingFrustum* _frustum, BoundingSphere* _sphere)
{
	_frustum->update();
	_sphere->update();


	const D3DXPLANE* frustumPlanes = _frustum->getPlanes();

	const D3DXVECTOR3* sphereCenter = _sphere->getCenter();
	float sphereRadius = _sphere->getRadius();


	float dist = D3DXPlaneDotCoord( &(frustumPlanes[FRUSTUM_FRONT]), sphereCenter );

	if ( dist >= sphereRadius || dist <= - ( sphereRadius - _frustum->getHeight() ) )
		return false;


	for ( BYTE i = 2; i < 6; ++i )
	{
		dist =	D3DXPlaneDotCoord( &(frustumPlanes[i]), sphereCenter );

		if ( dist >= sphereRadius )
			return false;
	}

	return true;
};
////


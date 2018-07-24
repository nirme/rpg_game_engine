#include "Geometry_Bounding_Frustum.h"



BoundingFrustum::BoundingFrustum( WorldPosition* _p_worldPosition, float _height1, float _height2, float _angleHorizontal, float _angleVertical, D3DXVECTOR3* _apex ) : BoundingVolume( _p_worldPosition )
{
	if ( _apex )
		apex = *_apex;
	else
		ZeroMemory( &apex, sizeof(D3DXVECTOR3) );

	height1 = _height1;
	height2 = _height2;
	angleHorizontal = _angleHorizontal;
	angleVertical = _angleVertical;

	angleHorizontalTan = tan( angleHorizontal * 0.5f );
	angleVerticalTan = tan( angleVertical * 0.5f );

	baseWidth = angleHorizontalTan * height2 * 2.0f;
	baseHeight = angleVerticalTan * height2 * 2.0f;


	focalLengthHoriz = 1.0f / angleHorizontalTan;
	focalLengthVert = 1.0f / angleVerticalTan;
	mfLrdHoriz = 1.0f / sqrt( focalLengthHoriz * focalLengthHoriz  + 1.0f );
	mfLrdVert = 1.0f / sqrt( focalLengthVert * focalLengthVert  + 1.0f );


	D3DXPlaneFromPointNormal( &( frustum[FRUSTUM_FRONT] ),	&D3DXVECTOR3( apex.x, apex.y, apex.z + height2 ),&D3DXVECTOR3( 0.0f, 0.0f, +1.0f ) );
	D3DXPlaneFromPointNormal( &( frustum[FRUSTUM_BACK] ),	&D3DXVECTOR3( apex.x, apex.y, apex.z + height1 ),&D3DXVECTOR3( 0.0f, 0.0f, -1.0f ) );
	D3DXPlaneFromPointNormal( &( frustum[FRUSTUM_LEFT] ),	&apex,	&D3DXVECTOR3( -focalLengthHoriz * mfLrdHoriz, 0.0f, -mfLrdHoriz ) );
	D3DXPlaneFromPointNormal( &( frustum[FRUSTUM_RIGHT] ),	&apex,	&D3DXVECTOR3( +focalLengthHoriz * mfLrdHoriz, 0.0f, -mfLrdHoriz ) );
	D3DXPlaneFromPointNormal( &( frustum[FRUSTUM_TOP] ),	&apex,	&D3DXVECTOR3( 0.0f, +focalLengthVert * mfLrdVert, -mfLrdVert ) );
	D3DXPlaneFromPointNormal( &( frustum[FRUSTUM_BOTTOM] ),	&apex,	&D3DXVECTOR3( 0.0f, -focalLengthVert * mfLrdVert, -mfLrdVert ) );


	dw_worldPositionTimestamp = p_worldPosition->getWorldPositionTimestamp();
	dw_worldPositionTimestamp--;

	update();
};


BoundingFrustum::~BoundingFrustum()
{};


bool BoundingFrustum::intersectRay( Ray* _ray, float* _distance )
{
	update();

	float tNear = -10000000000.0f;
	float tFar =   10000000000.0f;

	const D3DXVECTOR3* rayDirection = _ray->getDirection();
	const D3DXVECTOR3* rayOrigin = _ray->getOrigin();

	float Vd, Vn, t;

	for ( BYTE i = 0; i < 6; ++i )
	{
		Vn = cashedFrustum[i].a * rayOrigin->x + cashedFrustum[i].b * rayOrigin->y + cashedFrustum[i].c * rayOrigin->z + cashedFrustum[i].d;
		Vd = cashedFrustum[i].a * rayDirection->x + cashedFrustum[i].b * rayDirection->y + cashedFrustum[i].c * rayDirection->z;

		t = - Vn / Vd;

		if ( Vd > 0.0f )
		{
			if ( t < 0.0f )
				return false;

			if ( t < tFar )
			{
				tFar = t;
				if ( tNear > tFar )
					return false;
			}

			continue;
		}

		if ( Vd < 0.0f )
		{
			if ( t > tNear )
			{
				tNear = t;
				if ( tNear > tFar )
					return false;
			}

			continue;
		}

		if (Vn > 0.0f)
			return false;
	}

	if (_distance)
		*_distance = tNear;

	return true;
};


bool BoundingFrustum::intersectPoint( const D3DXVECTOR3* _point )
{
	update();


	for ( BYTE i = 0; i < 6; ++i )
	{
		if ( _point->x * cashedFrustum[i].a + _point->y * cashedFrustum[i].b + _point->z * cashedFrustum[i].c >= -cashedFrustum[i].d )
			return false;
	}

	return true;
};


float BoundingFrustum::distanceFromPointSquared( const D3DXVECTOR3* _point )
{
	update();


	float dRation = cashedHeight2 / cashedHeight1;

	D3DXVECTOR3 diff = *_point - cashedApex;
	D3DXVECTOR3 test = D3DXVECTOR3(	diff.x * cashedRight.x + diff.y * cashedRight.y + diff.z * cashedRight.z, 
									diff.x * cashedUp.x + diff.y * cashedUp.y + diff.z * cashedUp.z, 
									diff.x * cashedDirection.x + diff.y * cashedDirection.y + diff.z * cashedDirection.z );

	if ( test.x < 0.0f )
		test.x = -test.x;

	if ( test.y < 0.0f )
		test.y = -test.y;

	// Frustum derived parameters.
	float rmin = cashedHeight1 * angleHorizontalTan;
	float rmax = dRation * rmin;
	float umin = cashedHeight1 * angleVerticalTan;
	float umax = dRation * umin;
	float rminSqr = rmin * rmin;
	float dminSqr = cashedHeight1 * cashedHeight1;
	float minRDDot = rminSqr + dminSqr;
	float minUDDot = umin * umin + dminSqr;
	float minRUDDot = rminSqr + minUDDot;

	// Algorithm computes closest point in all cases by determining in which
	// Voronoi region of the vertices, edges, and faces of the frustum that
	// the test point lives.
	D3DXVECTOR3 closest;
	float rDot, uDot, rdDot, udDot, rudDot, t;
	if ( test.z >= cashedHeight2 )
	{
		if ( test.x <= rmax )
		{
			if ( test.y <= umax )
			{
				diff.z = test.z - cashedHeight2;
				return ( diff.z * diff.z );
			}

			diff.y = test.y - umax;
			diff.z = test.z - cashedHeight2;
			return ( diff.y * diff.y + diff.z * diff.z );

		}

		if ( test.y <= umax )
		{
			diff.x = test.x - rmax;
			diff.z = test.z - cashedHeight2;
			return ( diff.x * diff.x + diff.z * diff.z );
		}

		diff.x = test.x - rmax;
		diff.y = test.y - umax;
		diff.z = test.z - cashedHeight2;
		return ( diff.x * diff.x + diff.y * diff.y + diff.z * diff.z );

	}

	if ( test.z <= cashedHeight1 )
	{
		if ( test.x <= rmin )
		{
			if ( test.y <= umin )
			{
				diff.z = test.z - cashedHeight1;
				return ( diff.z * diff.z );
			}

			udDot = umin * test.y + cashedHeight1 * test.z;
			if ( udDot >= ( dRation * minUDDot ) )
			{
				diff.y = test.y - umax;
				diff.z = test.z - cashedHeight2;
				return ( diff.y * diff.y + diff.z * diff.z );
			}

			if ( udDot >= minUDDot )
			{
				t = ( cashedHeight1 * test.y - umin * test.z ) / minUDDot;
				diff.y = t * cashedHeight1;
				diff.z = - t * umin;
				return ( diff.y * diff.y + diff.z * diff.z );
			}

			diff.y = test.y - umin;
			diff.z = test.z - cashedHeight1;
			return ( diff.y * diff.y + diff.z * diff.z );

		}

		if ( test.y <= umin )
		{
			rdDot = rmin * test.x + cashedHeight1 * test.z;
			if ( rdDot >= ( dRation * minRDDot ) )
			{
				diff.x = test.x - rmax;
				diff.z = test.z - cashedHeight2;
				return ( diff.x * diff.x + diff.z * diff.z );
			}

			if ( rdDot >= minRDDot )
			{
				t = ( cashedHeight1 * test.x - rmin * test.z ) / minRDDot;
				diff.x = t * cashedHeight1;
				diff.z = - t * rmin;
				return ( diff.x * diff.x + diff.z * diff.z );
			}

			diff.x = test.x - rmin;
			diff.z = test.z - cashedHeight1;
			return ( diff.x * diff.x + diff.z * diff.z );

		}

		rudDot = rmin * test.x + umin * test.y + cashedHeight1 * test.z;
		if ( ( umin * rudDot - minRUDDot * test.y ) >= 0.0f )
		{
			rdDot = rmin * test.x + cashedHeight1 * test.z;

			if ( rdDot >= ( dRation * minRDDot ) )
			{
				diff.x = test.x - rmax;
				diff.z = test.z - cashedHeight2;
				return ( diff.x * diff.x + diff.z * diff.z );
			}

			if ( rdDot >= minRDDot )
			{
				t = ( cashedHeight1 * test.x - rmin * test.z ) / minRDDot;
				diff.x = t * cashedHeight1;
				diff.z = - t * rmin;
				return ( diff.x * diff.x + diff.z * diff.z );
			}

			diff.x = test.x - rmin;
			diff.z = test.z - cashedHeight1;
			return ( diff.x * diff.x + diff.z * diff.z );

		}

		if ( ( rmin * rudDot - minRUDDot * test.x ) >= 0.0f )
		{
			udDot = umin * test.y + cashedHeight1 * test.z;

			if ( udDot >= ( dRation * minUDDot ) )
			{
				diff.y = test.y - umax;
				diff.z = test.z - cashedHeight2;
				return ( diff.y * diff.y + diff.z * diff.z );
			}

			if ( udDot >= minUDDot )
			{
				t = ( cashedHeight1 * test.y - umin * test.z ) / minUDDot;
				diff.y = t * cashedHeight1;
				diff.z = - t * umin;
				return ( diff.y * diff.y + diff.z * diff.z );
			}

			diff.y = test.y - umin;
			diff.z = test.z - cashedHeight1;
			return ( diff.y * diff.y + diff.z * diff.z );

		}

		if ( rudDot >= ( dRation * minRUDDot ) )
		{
			diff.x = test.x - rmax;
			diff.y = test.y - umax;
			diff.z = test.z - cashedHeight2;
			return ( diff.x * diff.x + diff.y * diff.y + diff.z * diff.z );
		}

		if ( rudDot >= minRUDDot )
		{
			t = rudDot / minRUDDot;
			diff.x = test.x - ( t * rmin );
			diff.y = test.y - ( t * umin );
			diff.z = test.z - ( t * cashedHeight1 );
			return ( diff.x * diff.x + diff.y * diff.y + diff.z * diff.z );
		}

		diff.x = test.x - rmin;
		diff.y = test.y - umin;
		diff.z = test.z - cashedHeight1;
		return ( diff.x * diff.x + diff.y * diff.y + diff.z * diff.z );

	}

	rDot = cashedHeight1 * test.x - rmin * test.z;
	uDot = cashedHeight1 * test.y - umin * test.z;
	if ( rDot <= 0.0f )
	{
		if ( uDot <= 0.0f )
		{
			return 0.0f;
		}

		udDot = umin * test.y + cashedHeight1 * test.z;
		if ( udDot >= ( dRation * minUDDot ) )
		{
			diff.y = test.y - umax;
			diff.z = test.z - cashedHeight2;
			return ( diff.y * diff.y + diff.z * diff.z );
		}

		t = uDot / minUDDot;
		diff.y = t * cashedHeight1;
		diff.z = - t * umin;
		return ( diff.y * diff.y + diff.z * diff.z );

	}

	if ( uDot <= 0.0f )
	{
		rdDot = rmin * test.x + cashedHeight1 * test.z;
		if ( rdDot >= ( dRation * minRDDot ) )
		{
			diff.x = test.x - rmax;
			diff.z = test.z - cashedHeight2;
			return ( diff.x * diff.x + diff.z * diff.z );
		}

		t = rDot / minRDDot;
		diff.x = t * cashedHeight1;
		diff.z = - t * rmin;
		return ( diff.x * diff.x + diff.z * diff.z );

	}

	rudDot = rmin * test.x + umin * test.y + cashedHeight1 * test.z;
	if ( ( umin * rudDot - minRUDDot * test.y ) >= 0.0f )
	{
		rdDot = rmin * test.x + cashedHeight1 * test.z;
		if ( rdDot >= ( dRation * minRDDot ) )
		{
			diff.x = test.x - rmax;
			diff.z = test.z - cashedHeight2;
			return ( diff.x * diff.x + diff.z * diff.z );
		}

		t = rDot / minRDDot;
		diff.x = t * cashedHeight1;
		diff.z = - t * rmin;
		return ( diff.x * diff.x + diff.z * diff.z );

	}

	if ( ( rmin * rudDot - minRUDDot * test.x ) >= 0.0f )
	{
		udDot = umin * test.y + cashedHeight1 * test.z;
		if ( udDot >= ( dRation * minUDDot ) )
		{
			diff.y = test.y - umax;
			diff.z = test.z - cashedHeight2;
			return ( diff.y * diff.y + diff.z * diff.z );
		}

		t = uDot / minUDDot;
		diff.y = t * cashedHeight1;
		diff.z = - t * umin;
		return ( diff.y * diff.y + diff.z * diff.z );

	}

	if ( rudDot >= ( dRation * minRUDDot ) )
	{
		diff.x = test.x - rmax;
		diff.y = test.y - umax;
		diff.z = test.z - cashedHeight2;
		return ( diff.x * diff.x + diff.y * diff.y + diff.z * diff.z );
	}

	t = rudDot / minRUDDot;
	diff.x = test.x - ( t * rmin );
	diff.y = test.y - ( t * umin );
	diff.z = test.z - ( t * cashedHeight1 );
	return ( diff.x * diff.x + diff.y * diff.y + diff.z * diff.z );

};


float BoundingFrustum::distanceFromPoint( const D3DXVECTOR3* _point )
{
	return sqrt( distanceFromPointSquared( _point ) );
};


bool BoundingFrustum::distanceFromPointLT( const D3DXVECTOR3* _point, float _testValue )
{
	return distanceFromPointSquared( _point ) < _testValue * _testValue;
};

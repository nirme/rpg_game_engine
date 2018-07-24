#include "Geometry_Bounding_Cone.h"
#include "Geometry_Bounding_Cone_X.h"
#include "Geometry_Bounding_Cone_Y.h"



BoundingCone::BoundingCone( WorldPosition* _p_worldPosition ) : BoundingVolume( _p_worldPosition )
{};


BoundingCone::BoundingCone( WorldPosition* _p_worldPosition, float _height, float _angle, D3DXVECTOR3* _apex ) : BoundingVolume( _p_worldPosition )
{
	if ( _apex )
		apex = *_apex;
	else
		ZeroMemory( &apex, sizeof(D3DXVECTOR3) );

	height = _height;
	angle = _angle;

	angleSin = sin( angle );
	angleCon = cos( angle );
	angleTan = tan( angle );

	cashedAngleSineSquared = angleSin * angleSin;
	cashedInvAngleSine = 1.0f / angleSin;
	cashedAngleCosineSquared = angleCon * angleCon;


	float focalLength = 1.0f / angleTan;
	mfLrd = -1.0f / sqrt( focalLength * focalLength  + 1.0f );
	focalLengthDfLrd = focalLength * mfLrd;


	D3DXPlaneFromPointNormal( &(frustum[CONE_FRUST_FRONT]), &D3DXVECTOR3( apex.x, apex.y, apex.z + height ), &D3DXVECTOR3( 0.0f, 0.0f, 1.0f ) );
	D3DXPlaneFromPointNormal( &(frustum[CONE_FRUST_LEFT]), &apex, &D3DXVECTOR3( +focalLengthDfLrd, 0.0f, mfLrd ) );
	D3DXPlaneFromPointNormal( &(frustum[CONE_FRUST_RIGHT]), &apex, &D3DXVECTOR3( -focalLengthDfLrd, 0.0f, mfLrd ) );
	D3DXPlaneFromPointNormal( &(frustum[CONE_FRUST_TOP]), &apex, &D3DXVECTOR3( 0.0f, -focalLengthDfLrd, mfLrd ) );
	D3DXPlaneFromPointNormal( &(frustum[CONE_FRUST_BOTTOM]), &apex, &D3DXVECTOR3( 0.0f, +focalLengthDfLrd, mfLrd ) );


	dw_worldPositionTimestamp = dw_wpFrustumTimestamp = p_worldPosition->getWorldPositionTimestamp();
	dw_worldPositionTimestamp--;

	update();
	getFrustum();
};


BoundingCone::~BoundingCone()
{};


const D3DXPLANE* BoundingCone::getFrustum()
{
	DWORD stamp;
	if ( ( stamp = p_worldPosition->getWorldPositionTimestamp() ) != dw_wpFrustumTimestamp )
	{
		const D3DXMATRIX* mx = p_worldPosition->getWorldPosition();

		for ( BYTE i = 0; i < 5; ++i )
			D3DXPlaneNormalize( &(cashedFrustum[i]), D3DXPlaneTransform( &(cashedFrustum[i]), &(frustum[i]), mx ) );

		dw_wpFrustumTimestamp = stamp;
	}

	return cashedFrustum;
};


bool BoundingCone::intersectRay( Ray* _ray, float* _distance )
{
	update();
	const D3DXVECTOR3* lineOrigin = _ray->getOrigin();
	const D3DXVECTOR3* lineDirection = _ray->getDirection();

	float AdD = cashedDirection.x * lineDirection->x + cashedDirection.y * lineDirection->y + cashedDirection.z * lineDirection->z;
	D3DXVECTOR3 E = *lineOrigin - cashedApex;

	float AdE = cashedDirection.x * E.x + cashedDirection.y * E.y + cashedDirection.z * E.z;
	float DdE = lineDirection->x * E.x + lineDirection->y * E.y + lineDirection->z * E.z;

	float EdE = E.x * E.x + E.y * E.y + E.z * E.z;
	float c2 = AdD * AdD - cashedAngleCosineSquared;
	float c1 = AdD * AdE - cashedAngleCosineSquared * DdE;
	float c0 = AdE * AdE - cashedAngleCosineSquared * EdE;
	float dot;

	// Solve the quadratic.  Keep only those X for which Dot(A,X-V) >= 0.
	if (abs(c2) >= EPSILON)
	{
		// c2 != 0
		float discr = c1*c1 - c0*c2;
		if (discr > EPSILON)
		{
			// Q(t) = 0 has two distinct real-valued roots.  However, one or
			// both of them might intersect the portion of the double-sided
			// cone "behind" the vertex.  We are interested only in those
			// intersections "in front" of the vertex.
			float root = sqrt(discr);
			float invC2 = ((float)1)/c2;

			BYTE flag = 0;

			float t1 = ( -c1 - root ) * invC2;
			if ( t1 >= 0.0f )
			{
				E = ( *lineOrigin + t1 * *lineDirection ) - cashedApex;
				dot = E.x * cashedDirection.x + E.y * cashedDirection.y + E.z * cashedDirection.z;
				if ( dot > 0.0f )
				{
					flag += 0x01;
				}
			}

			float t2 = ( -c1 + root ) * invC2;
			if ( t2 >= 0.0f )
			{
				E = ( *lineOrigin + t2 * *lineDirection ) - cashedApex;
				dot = E.x * cashedDirection.x + E.y * cashedDirection.y + E.z * cashedDirection.z;
				if ( dot > 0.0f )
				{
					flag += 0x10;
				}
			}

			if ( flag == 0x11 )
			{
				if (_distance)
					*_distance = min( t1, t2 );
				return true;
			}

			if ( flag )
			{
				if ( AdD > 0.0f )
				{
					if (_distance)
						*_distance = ( flag | 0x01 ) ? t1 : t2;

					return true;
				}

				float tp =	cashedFrustum[CONE_FRUST_FRONT].a * lineOrigin->x + 
							cashedFrustum[CONE_FRUST_FRONT].b * lineOrigin->y + 
							cashedFrustum[CONE_FRUST_FRONT].c * lineOrigin->z + 
							cashedFrustum[CONE_FRUST_FRONT].d;

				if ( tp < 0.0f )
					return false;

				if (_distance)
				{
					*_distance = tp / -( cashedFrustum[CONE_FRUST_FRONT].a * lineDirection->x + 
										 cashedFrustum[CONE_FRUST_FRONT].b * lineDirection->y + 
										 cashedFrustum[CONE_FRUST_FRONT].c * lineDirection->z);
				}

				return true;
			}

			return false;
		}

		if ( abs( discr ) < EPSILON )
		{
			if (_distance)
			{
				if ( AdD > 0.0f )
					*_distance = sqrt( E.x * E.x + E.y * E.y + E.z * E.z );
				else
					*_distance = sqrt( E.x * E.x + E.y * E.y + E.z * E.z ) - cashedHeight;
			}

			return true;
		}

		return false;
	}

	if (abs(c1) >= EPSILON)
	{
		float t = 0.5f * c0 / c1;
		E = ( *lineOrigin - t * *lineDirection ) - cashedApex;
		dot = E.x * cashedDirection.x + E.y * cashedDirection.y + E.z * cashedDirection.z;
		if ( dot > 0.0f )
		{
			if ( AdD > 0.0f )
			{
				if (_distance)
					*_distance = t;

				return true;
			}

			float tp =	cashedFrustum[CONE_FRUST_FRONT].a * lineOrigin->x + 
						cashedFrustum[CONE_FRUST_FRONT].b * lineOrigin->y + 
						cashedFrustum[CONE_FRUST_FRONT].c * lineOrigin->z + 
						cashedFrustum[CONE_FRUST_FRONT].d;

			if ( tp < 0.0f )
				return false;

			if (_distance)
			{
				*_distance = tp / -( cashedFrustum[CONE_FRUST_FRONT].a * lineDirection->x + 
									 cashedFrustum[CONE_FRUST_FRONT].b * lineDirection->y + 
									 cashedFrustum[CONE_FRUST_FRONT].c * lineDirection->z);
			}

			return true;
		}
	}

	return false;
};


bool BoundingCone::intersectPoint( const D3DXVECTOR3* _point )
{
	update();


	float capsuleExtent = 0.5f * ( cashedRadius + cashedHeight );

	D3DXVECTOR3 delta = *_point - ( cashedApex + capsuleExtent * cashedDirection );

	float cosDist = delta.x * cashedDirection.x + delta.y * cashedDirection.y + delta.z * cashedDirection.z;

	if ( abs( cosDist ) > capsuleExtent )
	{
		if ( cosDist > 0.0f )
			delta += capsuleExtent * cashedDirection;
		else
			delta -= capsuleExtent * cashedDirection;

		return delta.x * delta.x + delta.y * delta.y + delta.z * delta.z < cashedRadius * cashedRadius;
	}

	return delta.x * delta.x + delta.y * delta.y + delta.z * delta.z - cosDist * cosDist < cashedRadius * cashedRadius;
};


float BoundingCone::distanceFromPointSquared( const D3DXVECTOR3* _point )
{
	update();


	D3DXVECTOR3 delta1 = *_point - cashedApex;

	float delta1LengthSq = delta1.x * delta1.x + delta1.y * delta1.y + delta1.z * delta1.z;
	float cosDirDelta1 = delta1.x * cashedDirection.x + delta1.y * cashedDirection.y + delta1.z * cashedDirection.z;
	float cosDirDelta1Sq = cosDirDelta1 * cosDirDelta1 / delta1LengthSq;

	if ( cosDirDelta1 < 0.0f && cosDirDelta1Sq > angleSin * angleSin )
	{
		return delta1LengthSq;
	}


	float H = cashedHeight / ( angleCon * angleCon );

	D3DXVECTOR3 delta2 = *_point - ( cashedApex + H * cashedDirection );

	float delta2LengthSq = delta2.x * delta2.x + delta2.y * delta2.y + delta2.z * delta2.z;
	float cosDirDelta2 = delta2.x * cashedDirection.x + delta2.y * cashedDirection.y + delta2.z * cashedDirection.z;
	float cosDirDelta2Sq = cosDirDelta2 * cosDirDelta2 / delta2LengthSq;

	if ( cosDirDelta2 < 0.0f && cosDirDelta2Sq < angleSin * angleSin )
	{
		float pointFromLineDistSq = delta1LengthSq - cosDirDelta1 * cosDirDelta1;
		if ( pointFromLineDistSq <= cashedRadius * cashedRadius )
		{
			float dist = cosDirDelta1 - cashedHeight;
			return dist * dist;
		}

		float h1 = cosDirDelta1 - cashedHeight;
		float r1 = sqrt( pointFromLineDistSq ) - cashedRadius;

		return ( h1 * h1 + r1 * r1 );
	}

	float dist = ( sqrt( cosDirDelta1 - delta1LengthSq ) - ( ( cosDirDelta1 / cashedHeight ) * cashedRadius ) ) * angleCon;

	return dist * dist;
};


float BoundingCone::distanceFromPoint( const D3DXVECTOR3* _point )
{
	update();


	D3DXVECTOR3 delta1 = *_point - cashedApex;

	float delta1LengthSq = delta1.x * delta1.x + delta1.y * delta1.y + delta1.z * delta1.z;
	float cosDirDelta1 = delta1.x * cashedDirection.x + delta1.y * cashedDirection.y + delta1.z * cashedDirection.z;
	float cosDirDelta1Sq = cosDirDelta1 * cosDirDelta1 / delta1LengthSq;

	if ( cosDirDelta1 < 0.0f && cosDirDelta1Sq > angleSin * angleSin )
	{
		return sqrt( delta1LengthSq );
	}


	float H = cashedHeight / ( angleCon * angleCon );

	D3DXVECTOR3 delta2 = *_point - ( cashedApex + H * cashedDirection );

	float delta2LengthSq = delta2.x * delta2.x + delta2.y * delta2.y + delta2.z * delta2.z;
	float cosDirDelta2 = delta2.x * cashedDirection.x + delta2.y * cashedDirection.y + delta2.z * cashedDirection.z;
	float cosDirDelta2Sq = cosDirDelta2 * cosDirDelta2 / delta2LengthSq;

	if ( cosDirDelta2 < 0.0f && cosDirDelta2Sq < angleSin * angleSin )
	{
		float pointFromLineDistSq = delta1LengthSq - cosDirDelta1 * cosDirDelta1;
		if ( pointFromLineDistSq <= cashedRadius * cashedRadius )
		{
			return ( cosDirDelta1 - cashedHeight );
		}

		float h1 = cosDirDelta1 - cashedHeight;
		float r1 = sqrt( pointFromLineDistSq ) - cashedRadius;

		return sqrt( h1 * h1 + r1 * r1 );
	}

	return ( sqrt( cosDirDelta1 - delta1LengthSq ) - ( ( cosDirDelta1 / cashedHeight ) * cashedRadius ) ) * angleCon;
};


bool BoundingCone::distanceFromPointLT( const D3DXVECTOR3* _point, float _testValue )
{
	update();


	D3DXVECTOR3 delta1 = *_point - cashedApex;

	float delta1LengthSq = delta1.x * delta1.x + delta1.y * delta1.y + delta1.z * delta1.z;
	float cosDirDelta1 = delta1.x * cashedDirection.x + delta1.y * cashedDirection.y + delta1.z * cashedDirection.z;
	float cosDirDelta1Sq = cosDirDelta1 * cosDirDelta1 / delta1LengthSq;

	if ( cosDirDelta1 < 0.0f && cosDirDelta1Sq > angleSin * angleSin )
	{
		return delta1LengthSq < _testValue * _testValue;
	}


	float H = cashedHeight / ( angleCon * angleCon );

	D3DXVECTOR3 delta2 = *_point - ( cashedApex + H * cashedDirection );

	float delta2LengthSq = delta2.x * delta2.x + delta2.y * delta2.y + delta2.z * delta2.z;
	float cosDirDelta2 = delta2.x * cashedDirection.x + delta2.y * cashedDirection.y + delta2.z * cashedDirection.z;
	float cosDirDelta2Sq = cosDirDelta2 * cosDirDelta2 / delta2LengthSq;

	if ( cosDirDelta2 < 0.0f && cosDirDelta2Sq < angleSin * angleSin )
	{
		float pointFromLineDistSq = delta1LengthSq - cosDirDelta1 * cosDirDelta1;
		if ( pointFromLineDistSq <= cashedRadius * cashedRadius )
		{
			return ( cosDirDelta1 - cashedHeight ) < _testValue;
		}

		float h1 = cosDirDelta1 - cashedHeight;
		float r1 = sqrt( pointFromLineDistSq ) - cashedRadius;

		return ( h1 * h1 + r1 * r1 ) < _testValue * _testValue;
	}

	float dist = ( sqrt( cosDirDelta1 - delta1LengthSq ) - ( ( cosDirDelta1 / cashedHeight ) * cashedRadius ) ) * angleCon;

	return dist < _testValue;
};



BoundingConeX::BoundingConeX( WorldPosition* _p_worldPosition, float _height, float _angle, D3DXVECTOR3* _apex ) : BoundingCone( _p_worldPosition )
{
	if ( _apex )
		apex = *_apex;
	else
		ZeroMemory( &apex, sizeof(D3DXVECTOR3) );

	height = _height;
	angle = _angle;

	angleSin = sin( angle );
	angleCon = cos( angle );
	angleTan = tan( angle );

	cashedAngleSineSquared = angleSin * angleSin;
	cashedInvAngleSine = 1.0f / angleSin;
	cashedAngleCosineSquared = angleCon * angleCon;


	float focalLength = 1.0f / angleTan;
	mfLrd = -1.0f / sqrt( focalLength * focalLength  + 1.0f );
	focalLengthDfLrd = focalLength * mfLrd;


	D3DXPlaneFromPointNormal( &(frustum[CONE_FRUST_FRONT]), &D3DXVECTOR3( apex.x + height, apex.y, apex.z ), &D3DXVECTOR3( 1.0f, 0.0f, 0.0f ) );
	D3DXPlaneFromPointNormal( &(frustum[CONE_FRUST_LEFT]), &apex, &D3DXVECTOR3( mfLrd, 0.0f, -focalLengthDfLrd) );
	D3DXPlaneFromPointNormal( &(frustum[CONE_FRUST_RIGHT]), &apex, &D3DXVECTOR3( mfLrd, 0.0f, +focalLengthDfLrd) );
	D3DXPlaneFromPointNormal( &(frustum[CONE_FRUST_TOP]), &apex, &D3DXVECTOR3( mfLrd, -focalLengthDfLrd, 0.0f ) );
	D3DXPlaneFromPointNormal( &(frustum[CONE_FRUST_BOTTOM]), &apex, &D3DXVECTOR3( mfLrd, +focalLengthDfLrd, 0.0f ) );


	dw_worldPositionTimestamp = dw_wpFrustumTimestamp = p_worldPosition->getWorldPositionTimestamp();
	dw_worldPositionTimestamp--;

	update();
	getFrustum();
};


BoundingConeY::BoundingConeY( WorldPosition* _p_worldPosition, float _height, float _angle, D3DXVECTOR3* _apex ) : BoundingCone( _p_worldPosition )
{
	if ( _apex )
		apex = *_apex;
	else
		ZeroMemory( &apex, sizeof(D3DXVECTOR3) );

	height = _height;
	angle = _angle;

	angleSin = sin( angle );
	angleCon = cos( angle );
	angleTan = tan( angle );

	cashedAngleSineSquared = angleSin * angleSin;
	cashedInvAngleSine = 1.0f / angleSin;
	cashedAngleCosineSquared = angleCon * angleCon;


	float focalLength = 1.0f / angleTan;
	mfLrd = -1.0f / sqrt( focalLength * focalLength  + 1.0f );
	focalLengthDfLrd = focalLength * mfLrd;


	D3DXPlaneFromPointNormal( &(frustum[CONE_FRUST_FRONT]), &D3DXVECTOR3( apex.x, apex.y + height, apex.z ), &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );
	D3DXPlaneFromPointNormal( &(frustum[CONE_FRUST_LEFT]), &apex, &D3DXVECTOR3( 0.0f, +focalLengthDfLrd, mfLrd ) );
	D3DXPlaneFromPointNormal( &(frustum[CONE_FRUST_RIGHT]), &apex, &D3DXVECTOR3( 0.0f, -focalLengthDfLrd, mfLrd ) );
	D3DXPlaneFromPointNormal( &(frustum[CONE_FRUST_TOP]), &apex, &D3DXVECTOR3( +focalLengthDfLrd, 0.0f, mfLrd ) );
	D3DXPlaneFromPointNormal( &(frustum[CONE_FRUST_BOTTOM]), &apex, &D3DXVECTOR3( -focalLengthDfLrd, 0.0f, mfLrd ) );


	dw_worldPositionTimestamp = dw_wpFrustumTimestamp = p_worldPosition->getWorldPositionTimestamp();
	dw_worldPositionTimestamp--;

	update();
	getFrustum();
};


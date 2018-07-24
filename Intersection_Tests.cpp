#include "Intersection_Tests.h"




int intersectionSphereSphere(SphereDesc* _sphere1, SphereDesc* _sphere2)
{
	return intersectionSphereSphere(&(_sphere1->center), _sphere1->radius, &(_sphere2->center), _sphere2->radius);
};


bool intersectionSphereSphere(D3DXVECTOR3* _sphere1Center, float _sphere1Radius, D3DXVECTOR3* _sphere2Center, float _sphere2Radius)
{
	D3DXVECTOR3 dist = *_sphere1Center - * _sphere2Center;
	float distanceSq = dist.x * dist.x + dist.y * dist.y + dist.z * dist.z;
	if ( distanceSq < ( _sphere1Radius * _sphere1Radius + _sphere2Radius * _sphere2Radius ) )
		return 1;
	return 0;
};


int intersectionSpherePlane(SphereDesc* _sphere, D3DXPLANE* _plane)
{
	return intersectionSpherePlane( &(_sphere->center), _sphere->radius, _plane);
};


int intersectionSpherePlane(D3DXVECTOR3* _sphereCenter, float _sphereRadius, D3DXPLANE* _plane)
{
	float distance = _plane->a * _sphereCenter->x + _plane->b * _sphereCenter->y + _plane->c * _sphereCenter->z + _plane->d;

	if ( abs(distance) < _sphereRadius )
		return distance > 0.0f ? return 1 : -1;
	return 0;
};


bool sphereUnderPlane(SphereDesc* _sphere, D3DXPLANE* _plane)
{
	return sphereUnderPlane(&(_sphere->center), _sphere->radius, D3DXPLANE* _plane);
};


bool sphereUnderPlane(D3DXVECTOR3* _sphereCenter, float _sphereRadius, D3DXPLANE* _plane)
{
	float distance = _plane->a * _sphereCenter->x + _plane->b * _sphereCenter->y + _plane->c * _sphereCenter->z + _plane->d;
	return distance < 0.0f ? true : false;
};


bool sphereAbovePlane(SphereDesc* _sphere, D3DXPLANE* _plane)
{
	return sphereAbovePlane(&(_sphere->center), _sphere->radius, D3DXPLANE* _plane);
};


bool sphereAbovePlane(D3DXVECTOR3* _sphereCenter, float _sphereRadius, D3DXPLANE* _plane)
{
	float distance = _plane->a * _sphereCenter->x + _plane->b * _sphereCenter->y + _plane->c * _sphereCenter->z + _plane->d;
	return distance >= 0.0f ? true : false;
};


int intersectionConePlane(ConeDesc* _cone, D3DXPLANE* _plane)
{
	return intersectionConePlane(&(_cone->tip), &(_cone->axisDirection), _cone->radius, _cone->cosHalfAngle, _plane);
};


int intersectionConePlane(D3DXVECTOR3* _coneApex, D3DXVECTOR3* _coneAxisDirection, float _coneRadius, float _coneCosHalfAngle, D3DXPLANE* _plane)
{
	float distT = _plane->a * _coneApex->x + _plane->b * _coneApex->y + _plane->c * _coneApex->z + _plane->d;

	if ( abs(distT) > abs(_coneRadius) )
	{
		return distT >= 0.0f ? 1 : -1;
	}


	D3DXVECTOR3 surfaceDir;
	D3DXVec3Normalize(&surfaceDir, D3DXVec3Cross( &surfaceDir, D3DXVec3Cross( &surfaceDir, &D3DXVECTOR3(_plane->a, _plane->b, _plane->c), _coneAxisDirection ), _coneAxisDirection ));

	float c = _coneRadius / _coneCosHalfAngle;
	float r = pow( (c * c) - (_coneRadius * _coneRadius) , 0.5f);
	if (distT < 0.0f)
		r = -r;

	D3DXVECTOR3 pointQ = *_coneApex + *_coneAxisDirection * c + surfaceDir * r;

	float distQ = _plane->a * pointQ.x + _plane->b * pointQ.y + _plane->c * pointQ.z + _plane->d;


	return (distT * distQ) > 0.0f ? ( ( distT >= 0.0f) ? 1 : -1 ) : 0 ;
};



bool coneUnderPlane(ConeDesc* _cone, D3DXPLANE* _plane)
{
	return coneUnderPlane(&(_cone->tip), &(_cone->axisDirection), _cone->radius, _cone->cosHalfAngle, _plane);
};


bool coneUnderPlane(D3DXVECTOR3* _coneApex, D3DXVECTOR3* _coneAxisDirection, float _coneRadius, float _coneCosHalfAngle, D3DXPLANE* _plane)
{
	float distT = _plane->a * _coneApex->x + _plane->b * _coneApex->y + _plane->c * _coneApex->z + _plane->d;

	if ( abs(distT) > abs(_coneRadius) )
	{
		return distT > 0.0f ? false : true;
	}


	D3DXVECTOR3 surfaceDir;
	D3DXVec3Normalize(&surfaceDir, D3DXVec3Cross( &surfaceDir, D3DXVec3Cross( &surfaceDir, &D3DXVECTOR3(_plane->a, _plane->b, _plane->c), _coneAxisDirection ), _coneAxisDirection ));

	float c = _coneRadius / _coneCosHalfAngle;
	float r = pow( (c * c) - (_coneRadius * _coneRadius) , 0.5f);
	if (distT < 0.0f)
		r = -r;

	D3DXVECTOR3 pointQ = *_coneApex + *_coneAxisDirection * c + surfaceDir * r;

	float distQ = _plane->a * pointQ.x + _plane->b * pointQ.y + _plane->c * pointQ.z + _plane->d;


	return ( distT > 0.0f && distQ > 0.0f ) ? false : true;
};


bool coneAbovePlane(ConeDesc* _cone, D3DXPLANE* _plane)
{
	return coneAbovePlane(&(_cone->tip), &(_cone->axisDirection), _cone->radius, _cone->cosHalfAngle, _plane);
};


bool coneAbovePlane(D3DXVECTOR3* _coneApex, D3DXVECTOR3* _coneAxisDirection, float _coneRadius, float _coneCosHalfAngle, D3DXPLANE* _plane)
{
	float distT = _plane->a * _coneApex->x + _plane->b * _coneApex->y + _plane->c * _coneApex->z + _plane->d;

	if ( abs(distT) > abs(_coneRadius) )
	{
		return distT >= 0.0f ? true : false;
	}


	D3DXVECTOR3 surfaceDir;
	D3DXVec3Normalize(&surfaceDir, D3DXVec3Cross( &surfaceDir, D3DXVec3Cross( &surfaceDir, &D3DXVECTOR3(_plane->a, _plane->b, _plane->c), _coneAxisDirection ), _coneAxisDirection ));

	float c = _coneRadius / _coneCosHalfAngle;
	float r = pow( (c * c) - (_coneRadius * _coneRadius) , 0.5f);
	if (distT < 0.0f)
		r = -r;

	D3DXVECTOR3 pointQ = *_coneApex + *_coneAxisDirection * c + surfaceDir * r;

	float distQ = _plane->a * pointQ.x + _plane->b * pointQ.y + _plane->c * pointQ.z + _plane->d;


	return ( distT < 0.0f && distQ < 0.0f ) ? false : true;
};



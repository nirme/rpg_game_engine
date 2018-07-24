#pragma once

#ifndef _INTERSECTION_TESTS
#define _INTERSECTION_TESTS

#include "utility.h"



struct SphereDesc
{
	D3DXVECTOR3 center;
	float radius;
};


struct ConeDesc
{
	D3DXVECTOR3 tip;
	D3DXVECTOR3 axisDirection;
	float radius;
	float cosHalfAngle;
};




int intersectionSphereSphere(SphereDesc* _sphere1, SphereDesc* _sphere2);
bool intersectionSphereSphere(D3DXVECTOR3* _sphere1Center, float _sphere1Radius, D3DXVECTOR3* _sphere2Center, float _sphere2Radius);

int intersectionSpherePlane(SphereDesc* _sphere, D3DXPLANE* _plane);
int intersectionSpherePlane(D3DXVECTOR3* _sphereCenter, float _sphereRadius, D3DXPLANE* _plane);

bool sphereUnderPlane(SphereDesc* _sphere, D3DXPLANE* _plane);
bool sphereUnderPlane(D3DXVECTOR3* _sphereCenter, float _sphereRadius, D3DXPLANE* _plane);

bool sphereAbovePlane(SphereDesc* _sphere, D3DXPLANE* _plane);
bool sphereAbovePlane(D3DXVECTOR3* _sphereCenter, float _sphereRadius, D3DXPLANE* _plane);


int intersectionConePlane(ConeDesc* _cone, D3DXPLANE* _plane);
int intersectionConePlane(D3DXVECTOR3* _coneApex, D3DXVECTOR3* _coneAxisDirection, float _coneRadius, float _coneCosHalfAngle, D3DXPLANE* _plane);

bool coneUnderPlane(ConeDesc* _cone, D3DXPLANE* _plane);
bool coneUnderPlane(D3DXVECTOR3* _coneApex, D3DXVECTOR3* _coneAxisDirection, float _coneRadius, float _coneCosHalfAngle, D3DXPLANE* _plane);

bool coneAbovePlane(ConeDesc* _cone, D3DXPLANE* _plane);
bool coneAbovePlane(D3DXVECTOR3* _coneApex, D3DXVECTOR3* _coneAxisDirection, float _coneRadius, float _coneCosHalfAngle, D3DXPLANE* _plane);







#endif //_INTERSECTION_TESTS

#pragma once

#ifndef _GEOMETRY_BOUNDING_SPHERE
#define _GEOMETRY_BOUNDING_SPHERE

#include "utility.h"
#include "Geometry_Bounding_Volume.h"



class BoundingCapsule;
class BoundingBox;
class BoundingBoxAABB;
class BoundingCone;

class BoundingFrustum;



class BoundingSphere : public BoundingVolume
{
protected:
	D3DXVECTOR3 center;
	float radius;

	D3DXVECTOR3 cashedCenter;
	float cashedRadius;
	float cashedRadiusSq;


public:

	BoundingSphere( WorldPosition* _p_worldPosition, float _radius, D3DXVECTOR3* _center = NULL );

	virtual ~BoundingSphere();


	inline const D3DXVECTOR3* getCenter()
	{
		return &cashedCenter;
	};

	inline float getRadius()
	{
		return cashedRadius;
	};

	inline float getRadiusSquared()
	{
		return cashedRadiusSq;
	};


	inline virtual void update()
	{
		DWORD stamp;
		if ( ( stamp = p_worldPosition->getWorldPositionTimestamp() ) != dw_worldPositionTimestamp )
		{
			D3DXVec3TransformCoord(&cashedCenter, &center, p_worldPosition->getWorldPosition());
			cashedRadius = radius * p_worldPosition->getWorldScale();
			cashedRadiusSq = cashedRadius * cashedRadius;

			AABBCenter.x = cashedCenter.x;
			AABBCenter.y = cashedCenter.y;
			AABBCenter.z = cashedCenter.z;
			v_AABBExtent[0] = v_AABBExtent[1] = v_AABBExtent[2] = cashedRadius;

			dw_worldPositionTimestamp = stamp;
		}
	};


	virtual bool intersect(BoundingSphere* _sphere)
	{
		return intersectFunc(this, _sphere);
	};

	virtual bool intersect(BoundingCapsule* _capsule)
	{
		return intersectFunc(_capsule, this);
	};

	virtual bool intersect(BoundingBox* _box)
	{
		return intersectFunc(_box, this);
	};

	virtual bool intersect(BoundingBoxAABB* _boxAABB)
	{
		return intersectFunc(_boxAABB, this);
	};

	virtual bool intersect(BoundingCone* _cone)
	{
		return intersectFunc(_cone, this);
	};

	virtual bool intersect(BoundingFrustum* _frustum)
	{
		return intersectFunc(_frustum, this);
	};


	virtual bool intersect(BoundingVolume* _volume)
	{
		return _volume->intersect(this);
	};


	virtual bool intersectRay( Ray* _ray, float* _distance = NULL );

	virtual bool intersectPoint( const D3DXVECTOR3* _point );

	virtual float distanceFromPointSquared( const D3DXVECTOR3* _point );
	virtual float distanceFromPoint( const D3DXVECTOR3* _point );
	virtual bool distanceFromPointLT( const D3DXVECTOR3* _point, float _testValue );

};


#endif //_GEOMETRY_BOUNDING_SPHERE

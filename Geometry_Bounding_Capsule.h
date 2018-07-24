#pragma once

#ifndef _GEOMETRY_BOUNDING_CAPSULE
#define _GEOMETRY_BOUNDING_CAPSULE

#include "utility.h"
#include "Geometry_Bounding_Volume.h"



class BoundingSphere;
class BoundingBox;
class BoundingBoxAABB;
class BoundingCone;

class BoundingFrustum;



class BoundingCapsule : public BoundingVolume
{
protected:
	D3DXVECTOR3 center;
	float extent;
	float radius;

	D3DXVECTOR3 cashedCenter;
	D3DXVECTOR3 cashedDirection;
	float cashedExtent;
	float cashedRadius;


public:


	BoundingCapsule( WorldPosition* _p_worldPosition, float _extent, float _radius, D3DXVECTOR3* _center = NULL );

	virtual ~BoundingCapsule();


	inline const D3DXVECTOR3* getCenter()
	{
		return &cashedCenter;
	};

	inline const D3DXVECTOR3* getDirection()
	{
		return &cashedDirection;
	};

	inline float getExtent()
	{
		return cashedExtent;
	};

	inline float getRadius()
	{
		return cashedRadius;
	};


	inline virtual void update()
	{
		DWORD stamp;
		if ( ( stamp = p_worldPosition->getWorldPositionTimestamp() ) != dw_worldPositionTimestamp )
		{
			const D3DXMATRIX* mx = p_worldPosition->getWorldPosition();
			D3DXVec3TransformCoord( &cashedCenter, &center, mx );
			D3DXVec3Normalize( &cashedDirection, reinterpret_cast <const D3DXVECTOR3*> ( &(mx->_21) ) );

			cashedExtent = p_worldPosition->getWorldScale();

			cashedRadius = cashedExtent * radius;
			cashedExtent *= extent;

			AABBCenter = center;
			v_AABBExtent[0] = abs( cashedDirection.x * cashedExtent ) + cashedRadius;
			v_AABBExtent[1] = abs( cashedDirection.y * cashedExtent ) + cashedRadius;
			v_AABBExtent[2] = abs( cashedDirection.z * cashedExtent ) + cashedRadius;


			dw_worldPositionTimestamp = stamp;
		}
	};


	virtual bool intersect(BoundingSphere* _sphere)
	{
		return intersectFunc(this, _sphere);
	};

	virtual bool intersect(BoundingCapsule* _capsule)
	{
		return intersectFunc(this, _capsule);
	};

	virtual bool intersect(BoundingBox* _box)
	{
		return intersectFunc(this, _box);
	};

	virtual bool intersect(BoundingBoxAABB* _boxAABB)
	{
		return intersectFunc(this, _boxAABB);
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


#endif //_GEOMETRY_BOUNDING_CAPSULE

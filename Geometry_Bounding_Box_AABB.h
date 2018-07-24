#pragma once

#ifndef _GEOMETRY_BOUNDING_BOX_AABB
#define _GEOMETRY_BOUNDING_BOX_AABB

#include "utility.h"
#include "Geometry_Bounding_Volume.h"



class BoundingSphere;
class BoundingCapsule;
class BoundingBox;
class BoundingCone;

class BoundingFrustum;



class BoundingBoxAABB : public BoundingVolume
{
protected:

	D3DXVECTOR3 center;
	float v_axisExtent[3];


public:

	BoundingBoxAABB( WorldPosition* _p_worldPosition, float _axisXExtent, float _axisYExtent, float _axisZExtent, D3DXVECTOR3* _center = NULL );

	virtual ~BoundingBoxAABB();

	inline const D3DXVECTOR3* getCenter()
	{
		return &AABBCenter;
	};

	inline const float* getAxisExtent()
	{
		return v_AABBExtent;
	};


	inline virtual void update()
	{
		DWORD stamp;
		if ( ( stamp = p_worldPosition->getWorldPositionTimestamp() ) != dw_worldPositionTimestamp )
		{
			D3DXVec3TransformCoord(&AABBCenter, &center, p_worldPosition->getWorldPosition());

			float scale = p_worldPosition->getWorldScale();
			v_AABBExtent[0] = v_axisExtent[0] * scale;
			v_AABBExtent[1] = v_axisExtent[1] * scale;
			v_AABBExtent[2] = v_axisExtent[2] * scale;


			dw_worldPositionTimestamp = stamp;
		}
	};


	virtual bool intersect( BoundingSphere* _sphere )
	{
		return intersectFunc(this, _sphere);
	};

	virtual bool intersect( BoundingCapsule* _capsule )
	{
		return intersectFunc(_capsule, this);
	};

	virtual bool intersect( BoundingBox* _box )
	{
		return intersectFunc(this, _box);
	};

	virtual bool intersect( BoundingBoxAABB* _boxAABB )
	{
		return intersectAABB(_boxAABB);
	};

	virtual bool intersect( BoundingCone* _cone )
	{
		return intersectFunc(_cone, this);
	};

	virtual bool intersect( BoundingFrustum* _frustum )
	{
		return intersectFunc(_frustum, this);
	};


	virtual bool intersect( BoundingVolume* _volume )
	{
		return _volume->intersect(this);
	};


	virtual bool intersectRay( Ray* _ray, float* _distance = NULL );

	virtual bool intersectPoint( const D3DXVECTOR3* _point )
	{
		return intersectAABBPoint( _point );
	};

	virtual float distanceFromPointSquared( const D3DXVECTOR3* _point )
	{
		return distanceAABBFromPointSquared( _point );
	};

	virtual float distanceFromPoint( const D3DXVECTOR3* _point )
	{
		return sqrt( distanceAABBFromPointSquared( _point ) );
	};

	virtual bool distanceFromPointLT( const D3DXVECTOR3* _point, float _testValue )
	{
		return distanceAABBFromPointSquared( _point ) < _testValue * _testValue;
	};

};


#endif //_GEOMETRY_BOUNDING_BOX_AABB

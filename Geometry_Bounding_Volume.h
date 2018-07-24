#pragma once

#ifndef _GEOMETRY_BOUNDING_VOLUME
#define _GEOMETRY_BOUNDING_VOLUME

#include "utility.h"
#include "Core_World_Position_v3.h"

#include "Geometry_Ray.h"



class BoundingSphere;
class BoundingCapsule;
class BoundingBox;
class BoundingBoxAABB;
class BoundingCone;


class BoundingFrustum;



class BoundingVolume
{
protected:
	WorldPosition* p_worldPosition;
	DWORD dw_worldPositionTimestamp;

	D3DXVECTOR3 AABBCenter;
	float v_AABBExtent[3];


public:

	BoundingVolume( WorldPosition* _p_worldPosition );
	virtual ~BoundingVolume();

	void setWorldPosition( WorldPosition* _p_worldPosition );
	WorldPosition* getWorldPosition();

	inline const D3DXVECTOR3* getAABBCenter()
	{
		return &AABBCenter;
	};

	inline const float* getAABBExtent()
	{
		return v_AABBExtent;
	};


	bool intersectAABB( BoundingVolume* _volume );
	bool intersectAABBRay( Ray* _ray, float* _distance );

	inline virtual void update() = 0;

	virtual bool intersect( BoundingSphere* _sphere ) = 0;
	virtual bool intersect( BoundingCapsule* _capsule ) = 0;
	virtual bool intersect( BoundingBox* _box ) = 0;
	virtual bool intersect( BoundingBoxAABB* _boxAABB ) = 0;
	virtual bool intersect( BoundingCone* _cone ) = 0;
	virtual bool intersect( BoundingFrustum* _frustum ) = 0;

	virtual bool intersect( BoundingVolume* _volume ) = 0;

	inline bool intersectWithPreTest( BoundingVolume* _volume )
	{
		if (intersectAABB( _volume ) )
			return intersect( _volume );
		return false;
	};


	virtual bool intersectRay( Ray* _ray, float* _distance ) = 0;

	inline bool intersectRayWithPreTest( Ray* _ray, float* _distance = NULL )
	{
		if ( intersectAABBRay( _ray, NULL ) )
			return intersectRay( _ray, _distance );
		return false;
	};


	bool intersectAABBPoint( const D3DXVECTOR3* _point );
	float distanceAABBFromPointSquared( const D3DXVECTOR3* _point );


	inline float distanceAABBFromPoint( const D3DXVECTOR3* _point )
	{
		return sqrt( distanceAABBFromPointSquared( _point ) );
	};


	virtual bool intersectPoint( const D3DXVECTOR3* _point ) = 0;


	inline bool intersectPointWithPreTest( const D3DXVECTOR3* _point )
	{
		if ( intersectAABBPoint( _point ) )
			return intersectPoint( _point );
		return false;
	};


	virtual float distanceFromPointSquared( const D3DXVECTOR3* _point ) = 0;
	virtual float distanceFromPoint( const D3DXVECTOR3* _point ) = 0;


	bool distanceAABBFromPointLT( const D3DXVECTOR3* _point, float _testValue );


	virtual bool distanceFromPointLT( const D3DXVECTOR3* _point, float _testValue ) = 0;

};



// INTERSECTION FUNCTIONS

bool intersectFunc( BoundingSphere* _sphere1, BoundingSphere* _sphere2 );

bool intersectFunc( BoundingBox* _box1, BoundingBox* _box2 );
bool intersectFunc( BoundingBox* _box, BoundingSphere* _sphere );

//bool intersectFunc( BoundingBoxAABB* _boxAABB1, BoundingBoxAABB* _boxAABB2);
bool intersectFunc( BoundingBoxAABB* _boxAABB, BoundingBox* _box);
bool intersectFunc( BoundingBoxAABB* _boxAABB, BoundingSphere* _sphere );

bool intersectFunc( BoundingCapsule* _capsule1, BoundingCapsule* _capsule2 );
bool intersectFunc( BoundingCapsule* _capsule, BoundingBoxAABB* _boxAABB);
bool intersectFunc( BoundingCapsule* _capsule, BoundingBox* _box);
bool intersectFunc( BoundingCapsule* _capsule, BoundingSphere* _sphere );

bool intersectFunc( BoundingCone* _cone1, BoundingCone* _cone2 );
bool intersectFunc( BoundingCone* _cone, BoundingCapsule* _capsule);
bool intersectFunc( BoundingCone* _cone, BoundingBoxAABB* _boxAABB);
bool intersectFunc( BoundingCone* _cone, BoundingBox* _box);
bool intersectFunc( BoundingCone* _cone, BoundingSphere* _sphere );

bool intersectFunc( BoundingFrustum* _frustum1, BoundingFrustum* _frustum2 );
bool intersectFunc( BoundingFrustum* _frustum, BoundingCone* _cone);
bool intersectFunc( BoundingFrustum* _frustum, BoundingCapsule* _capsule);
bool intersectFunc( BoundingFrustum* _frustum, BoundingBoxAABB* _boxAABB);
bool intersectFunc( BoundingFrustum* _frustum, BoundingBox* _box);
bool intersectFunc( BoundingFrustum* _frustum, BoundingSphere* _sphere);


#endif //_GEOMETRY_BOUNDING_VOLUME

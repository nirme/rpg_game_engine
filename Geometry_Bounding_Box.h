#pragma once

#ifndef _GEOMETRY_BOUNDING_BOX
#define _GEOMETRY_BOUNDING_BOX

#include "utility.h"
#include "Geometry_Bounding_Volume.h"



class BoundingSphere;
class BoundingCapsule;
class BoundingBoxAABB;
class BoundingCone;

class BoundingFrustum;



class BoundingBox : public BoundingVolume
{
protected:

	D3DXVECTOR3 center;
	float v_axisExtent[3];


	D3DXVECTOR3 cashedCenter;
	D3DXVECTOR3 v_cashedAxis[3];
	float v_cashedAxisExtent[3];


public:

	BoundingBox( WorldPosition* _p_worldPosition, float _axisXExtent, float _axisYExtent, float _axisZExtent, D3DXVECTOR3* _center = NULL );

	virtual ~BoundingBox();


	inline const D3DXVECTOR3* getCenter()
	{
		return &cashedCenter;
	};

	inline const D3DXVECTOR3* getAxis()
	{
		return v_cashedAxis;
	};

	inline const float* getAxisExtent()
	{
		return v_cashedAxisExtent;
	};


	inline virtual void update()
	{
		if ( DWORD stamp = p_worldPosition->getWorldPositionTimestamp() != dw_worldPositionTimestamp )
		{
			const D3DXMATRIX* mx = p_worldPosition->getWorldPosition();
			D3DXVec3TransformCoord( &cashedCenter, &center, mx );

			D3DXVec3Normalize( &(v_cashedAxis[0]), reinterpret_cast <const D3DXVECTOR3*> ( &(mx->_11) ) );
			//D3DXVec3Normalize( &(v_cashedAxis[1]), reinterpret_cast <const D3DXVECTOR3*> ( &(mx->_21) ) );
			D3DXVec3Normalize( &(v_cashedAxis[2]), reinterpret_cast <const D3DXVECTOR3*> ( &(mx->_31) ) );

			D3DXVec3Cross( &(v_cashedAxis[1]), &(v_cashedAxis[2]), &(v_cashedAxis[0]) );


			float scale = p_worldPosition->getWorldScale();

			v_cashedAxisExtent[0] = v_axisExtent[0] * scale;
			v_cashedAxisExtent[1] = v_axisExtent[1] * scale;
			v_cashedAxisExtent[2] = v_axisExtent[2] * scale;

			AABBCenter = cashedCenter;

			v_AABBExtent[0] =	abs( v_cashedAxis[0].x * v_cashedAxisExtent[0] ) + 
								abs( v_cashedAxis[1].x * v_cashedAxisExtent[1] ) + 
								abs( v_cashedAxis[2].x * v_cashedAxisExtent[2] );

			v_AABBExtent[1] =	abs( v_cashedAxis[0].y * v_cashedAxisExtent[0] ) + 
								abs( v_cashedAxis[1].y * v_cashedAxisExtent[1] ) + 
								abs( v_cashedAxis[2].y * v_cashedAxisExtent[2] );

			v_AABBExtent[2] =	abs( v_cashedAxis[0].z * v_cashedAxisExtent[0] ) + 
								abs( v_cashedAxis[1].z * v_cashedAxisExtent[1] ) + 
								abs( v_cashedAxis[2].z * v_cashedAxisExtent[2] );


			dw_worldPositionTimestamp = stamp;
		}
	};


	virtual bool intersect( BoundingSphere* _sphere )
	{
		return intersectFunc( this, _sphere );
	};

	virtual bool intersect( BoundingCapsule* _capsule )
	{
		return intersectFunc( _capsule, this );
	};

	virtual bool intersect( BoundingBox* _box )
	{
		return intersectFunc( this, _box );
	};

	virtual bool intersect( BoundingBoxAABB* _boxAABB )
	{
		return intersectFunc( _boxAABB, this );
	};

	virtual bool intersect( BoundingCone* _cone )
	{
		return intersectFunc( _cone, this );
	};

	virtual bool intersect( BoundingFrustum* _frustum )
	{
		return intersectFunc( _frustum, this );
	};


	virtual bool intersect( BoundingVolume* _volume )
	{
		return _volume->intersect( this );
	};


	virtual bool intersectRay( Ray* _ray, float* _distance = NULL );


	virtual bool intersectPoint( const D3DXVECTOR3* _point );

	virtual float distanceFromPointSquared( const D3DXVECTOR3* _point );
	virtual float distanceFromPoint( const D3DXVECTOR3* _point );
	virtual bool distanceFromPointLT( const D3DXVECTOR3* _point, float _testValue );

};


#endif //_GEOMETRY_BOUNDING_BOX

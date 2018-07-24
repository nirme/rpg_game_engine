#pragma once

#ifndef _GEOMETRY_BOUNDING_CONE
#define _GEOMETRY_BOUNDING_CONE

#include "utility.h"
#include "Geometry_Bounding_Volume.h"



class BoundingSphere;
class BoundingCapsule;
class BoundingBox;
class BoundingBoxAABB;

class BoundingFrustum;



#define		CONE_FRUST_FRONT		0
#define		CONE_FRUST_LEFT			1
#define		CONE_FRUST_RIGHT		2
#define		CONE_FRUST_TOP			3
#define		CONE_FRUST_BOTTOM		4


class BoundingCone : public BoundingVolume
{
protected:

	D3DXVECTOR3 apex;
	//D3DXVECTOR3 direction; (0.0, 0.0, 1.0)
	float height;
	float angle;
	float angleSin;
	float angleCon;
	float angleTan;


	D3DXVECTOR3 cashedApex;
	D3DXVECTOR3 cashedDirection;
	float cashedHeight;
	float cashedRadius;
	float cashedHeightSquared;
	float cashedAngleSineSquared;
	float cashedInvAngleSine;
	float cashedAngleCosineSquared;


	//float focalLength;// = 1.0f / tan( angle );
	float mfLrd;// = 1.0f / sqrt( focalLength * focalLength  + 1.0f );
	float focalLengthDfLrd;// = focalLength * mfLrd;
	//planeNormals
	// front on z+
	// front = (0.0f, 0.0f, 1.0f);
	// back = (0.0f, 0.0f, -1.0f);
	// left   = (  focalLength * mfLrd, 0.0f, -mfLrd );
	// right  = ( -focalLength * mfLrd, 0.0f, -mfLrd );
	// top    = ( 0.0f, -focalLength * mfLrd, -mfLrd );
	// bottom = ( 0.0f,  focalLength * mfLrd, -mfLrd ;)
	//
	// float d = -( a * x + b * y + c * z );

	D3DXPLANE frustum[5];
	D3DXPLANE cashedFrustum[5];

	DWORD dw_wpFrustumTimestamp;


public:


	BoundingCone( WorldPosition* _p_worldPosition );
	BoundingCone( WorldPosition* _p_worldPosition, float _height, float _angle, D3DXVECTOR3* _apex = NULL );

	~BoundingCone();


	inline const D3DXVECTOR3* getApex()
	{
		return &cashedApex;
	};

	inline const D3DXVECTOR3* getDirection()
	{
		return &cashedDirection;
	};

	inline float getHeight()
	{
		return cashedHeight;
	};

	inline float getRadius()
	{
		return cashedRadius;
	};

	inline float getHeightSquared()
	{
		return cashedHeightSquared;
	};

	inline float getAngleSin()
	{
		return angleSin;
	};

	inline float getAngleCos()
	{
		return angleCon;
	};

	inline float getAngleTan()
	{
		return angleTan;
	};

	inline float getAngleSineSquared()
	{
		return cashedAngleSineSquared;
	};

	inline float getInvAngleSine()
	{
		return cashedInvAngleSine;
	};

	inline float getAngleCosineSquared()
	{
		return cashedAngleCosineSquared;
	};

	const D3DXPLANE* getFrustum();



	inline virtual void update()
	{
		DWORD stamp;
		if ( ( stamp = p_worldPosition->getWorldPositionTimestamp() ) != dw_worldPositionTimestamp )
		{
			const D3DXMATRIX* mx = p_worldPosition->getWorldPosition();

			D3DXVec3TransformCoord(&cashedApex, &apex, mx);

			D3DXVec3Normalize( &cashedDirection, reinterpret_cast <const D3DXVECTOR3*> ( &(mx->_31) ) );

			float scale = p_worldPosition->getWorldScale();
			cashedHeight = scale * height;
			cashedRadius = cashedHeight * angleTan;
			cashedHeightSquared = cashedHeight * cashedHeight;


			// AABB recalc
			D3DXVECTOR3 baseCenter = cashedApex + cashedHeight * cashedDirection;
			D3DXVECTOR3 max, min;

			float absDist;
			float tmpDist;

			absDist = cashedRadius * ( abs( mx->_11 ) + abs( mx->_21 ) );
			max.x = cashedApex.x > ( tmpDist = baseCenter.x + absDist ) ? cashedApex.x : tmpDist;
			min.x = cashedApex.x < ( tmpDist = baseCenter.x - absDist ) ? cashedApex.x : tmpDist;

			absDist = cashedRadius * ( abs( mx->_12 ) + abs( mx->_22 ) );
			max.y = cashedApex.y > ( tmpDist = baseCenter.y + absDist ) ? cashedApex.y : tmpDist;
			min.y = cashedApex.y < ( tmpDist = baseCenter.y - absDist ) ? cashedApex.y : tmpDist;

			absDist = cashedRadius * ( abs( mx->_13 ) + abs( mx->_23 ) );
			max.z = cashedApex.z > ( tmpDist = baseCenter.z + absDist ) ? cashedApex.z : tmpDist;
			min.z = cashedApex.z < ( tmpDist = baseCenter.z - absDist ) ? cashedApex.z : tmpDist;

			v_AABBExtent[0] = 0.5f * (max.x - min.x);
			v_AABBExtent[1] = 0.5f * (max.y - min.y);
			v_AABBExtent[2] = 0.5f * (max.z - min.z);
			AABBCenter.x = min.x + v_AABBExtent[0];
			AABBCenter.y = min.y + v_AABBExtent[1];
			AABBCenter.z = min.z + v_AABBExtent[2];


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
		return intersectFunc(this, _cone);
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


#endif //_GEOMETRY_BOUNDING_CONE

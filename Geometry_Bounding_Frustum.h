#pragma once

#ifndef _GEOMETRY_BOUNDING_FRUSTUM
#define _GEOMETRY_BOUNDING_FRUSTUM

#include "utility.h"
#include "Geometry_Bounding_Volume.h"


#define		FRUSTUM_FRONT		0
#define		FRUSTUM_BACK		1
#define		FRUSTUM_LEFT		2
#define		FRUSTUM_RIGHT		3
#define		FRUSTUM_TOP			4
#define		FRUSTUM_BOTTOM		5



class BoundingSphere;
class BoundingCapsule;
class BoundingBox;
class BoundingBoxAABB;
class BoundingCone;


class BoundingFrustum : public BoundingVolume
{
protected:

	D3DXVECTOR3 apex;
	//D3DXVECTOR3 direction; (1,0,0)
	//D3DXVECTOR3 up; (0,1,0)
	//D3DXVECTOR3 right; (0,0,1)
	float height1;
	float height2;
	float baseWidth; //horizontal
	float baseHeight; //vertical
	float angleHorizontal;
	float angleVertical;

	float angleHorizontalTan;
	float angleVerticalTan;


	float focalLengthHoriz;// = 1.0f / tan( angle );
	float focalLengthVert;// = 1.0f / tan( angle );
	float mfLrdHoriz;// = 1.0f / sqrt( focalLengthHoriz * focalLengthHoriz  + 1.0f );
	float mfLrdVert;// = 1.0f / sqrt( focalLengthVert * focalLengthVert  + 1.0f );

	// front  = +direction
	// back   = -direction
	// left   = +focalLengthHoriz * mfLrdHoriz, 0.0f, -mfLrdHoriz
	// right  = -focalLengthHoriz * mfLrdHoriz, 0.0f, -mfLrdHoriz
	// top    = 0.0f, -focalLengthVert * mfLrdVert, -mfLrdVert
	// bottom = 0.0f,  focalLengthVert * mfLrdVert, -mfLrdVert
//	D3DXPLANE cashedLocalFrustum[6];


	D3DXVECTOR3 cashedApex;
	D3DXVECTOR3 cashedDirection;
	D3DXVECTOR3 cashedUp;
	D3DXVECTOR3 cashedRight;
	float cashedHeight;
	float cashedHeight1;
	float cashedHeight2;
	float cashedBaseWidth;
	float cashedBaseHeight;

	D3DXPLANE frustum[6];
	D3DXPLANE cashedFrustum[6];


public:

	BoundingFrustum( WorldPosition* _p_worldPosition, float _height1, float _height2, float _angleHorizontal, float _angleVertical, D3DXVECTOR3* _apex = NULL );

	virtual ~BoundingFrustum();



	inline float getFocalLengthHorizontal()
	{
		return focalLengthHoriz;
	};

	inline float getFocalLengthVertical()
	{
		return focalLengthVert;
	};


	inline float getAngleHorizontalTangent()
	{
		return angleHorizontalTan;
	};

	inline float angleVerticalTangent()
	{
		return angleVerticalTan;
	};


	inline const D3DXVECTOR3* getApex()
	{
		return &cashedApex;
	};

	inline const D3DXVECTOR3* getDirection()
	{
		return &cashedDirection;
	};

	inline const D3DXVECTOR3* getUp()
	{
		return &cashedUp;
	};

	inline const D3DXVECTOR3* getRight()
	{
		return &cashedRight;
	};

	inline float getHeight()
	{
		return cashedHeight;
	};

	inline float getHeight1()
	{
		return cashedHeight1;
	};

	inline float getHeight2()
	{
		return cashedHeight2;
	};

	inline float getBaseWidth()
	{
		return cashedBaseWidth;
	};

	inline float getBaseHeight()
	{
		return cashedBaseHeight;
	};

	inline const D3DXPLANE* getPlanes()
	{
		return cashedFrustum;
	};


	inline virtual void update()
	{
		DWORD stamp;
		if ( ( stamp = p_worldPosition->getWorldPositionTimestamp() ) != dw_worldPositionTimestamp )
		{
			const D3DXMATRIX* mx = p_worldPosition->getWorldPosition();
			D3DXVec3Normalize(&cashedDirection, reinterpret_cast <const D3DXVECTOR3*> ( &(mx->_31) ));
			D3DXVec3Normalize(&cashedUp, reinterpret_cast <const D3DXVECTOR3*> ( &(mx->_21) ));

			//cross( cashedUp , cashedDirection )
			cashedRight.x = cashedUp.y * cashedDirection.z - cashedUp.z * cashedDirection.y;
			cashedRight.y = cashedUp.z * cashedDirection.x - cashedUp.x * cashedDirection.z;
			cashedRight.z = cashedUp.x * cashedDirection.y - cashedUp.y * cashedDirection.x;

			D3DXVec3TransformCoord(&cashedApex, &apex, mx);

			for (BYTE i = 0; i < 6; ++i)
				D3DXPlaneNormalize( &(cashedFrustum[i]), D3DXPlaneTransform( &(cashedFrustum[i]), &(frustum[i]), mx ) );

			float scale = p_worldPosition->getWorldScale();
			cashedHeight1 = scale * height1;
			cashedHeight2 = scale * height2;
			cashedHeight = cashedHeight2 - cashedHeight1;
			cashedBaseWidth = scale * baseWidth;
			cashedBaseHeight = scale * baseHeight;

			float cashedBase1HalfWidth = angleHorizontalTan * height1;
			float cashedBase1HalfHeight = angleVerticalTan * height1;


			D3DXVECTOR3 max, min, max2, min2;
			D3DXVECTOR3 absRight, absUp;

			D3DXVECTOR3 center = cashedApex + cashedHeight2 * cashedDirection;

			absRight.x = abs( cashedRight.x );
			absRight.y = abs( cashedRight.y );
			absRight.z = abs( cashedRight.z );
			absUp.x = abs( cashedUp.x );
			absUp.y = abs( cashedUp.y );
			absUp.z = abs( cashedUp.z );

			float tmpRight = 0.5f * cashedBaseWidth;
			float tmpUp = 0.5f * cashedBaseHeight;

			absRight = tmpRight * absRight + tmpUp * cashedUp;

			min = center - absRight;
			max = center + absRight;


			center = cashedApex + cashedHeight1 * cashedDirection;

			float ratio = cashedHeight1 / cashedHeight2;
			float tmp;

			min2 = center - ratio * absRight;
			max2 = center + ratio * absRight;

			tmp = center.x + ratio * absRight.x;
			if ( max.x < tmp )	max.x = tmp;
			tmp = center.y + ratio * absRight.y;
			if ( max.y < tmp )	max.y = tmp;
			tmp = center.z + ratio * absRight.z;
			if ( max.z < tmp )	max.z = tmp;

			tmp = center.x - ratio * absRight.x;
			if ( min.x > tmp )	min.x = tmp;
			tmp = center.y - ratio * absRight.y;
			if ( min.y > tmp )	min.y = tmp;
			tmp = center.z - ratio * absRight.z;
			if ( min.z > tmp )	min.z = tmp;


			v_AABBExtent[0] = ( max.x - min.x ) * 0.5f;
			v_AABBExtent[1] = ( max.y - min.y ) * 0.5f;
			v_AABBExtent[2] = ( max.z - min.z ) * 0.5f;
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
		return intersectFunc(this, _frustum);
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


#endif //_GEOMETRY_BOUNDING_FRUSTUM

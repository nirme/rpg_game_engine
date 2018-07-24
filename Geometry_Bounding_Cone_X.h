#pragma once

#ifndef _GEOMETRY_BOUNDING_CONE_X
#define _GEOMETRY_BOUNDING_CONE_X

#include "utility.h"
#include "Geometry_Bounding_Volume.h"

#include "Geometry_Bounding_Cone.h"


class BoundingConeX : public BoundingCone
{
public:


	BoundingConeX( WorldPosition* _p_worldPosition, float _height, float _angle, D3DXVECTOR3* _apex = NULL );

	inline virtual void update()
	{
		DWORD stamp;
		if ( ( stamp = p_worldPosition->getWorldPositionTimestamp() ) != dw_worldPositionTimestamp )
		{
			const D3DXMATRIX* mx = p_worldPosition->getWorldPosition();

			D3DXVec3TransformCoord(&cashedApex, &apex, mx);

			D3DXVec3Normalize( &cashedDirection, reinterpret_cast <const D3DXVECTOR3*> ( &(mx->_11) ) );
			float scale = p_worldPosition->getWorldScale();
			cashedHeight = scale * height;
			cashedRadius = cashedHeight * angleTan;
			cashedHeightSquared = cashedHeight * cashedHeight;


			// AABB recalc
			D3DXVECTOR3 baseCenter = cashedApex + cashedHeight * cashedDirection;
			D3DXVECTOR3 max, min;

			float absDist;
			float tmpDist;

			absDist = cashedRadius * ( abs( mx->_31 ) + abs( mx->_21 ) );
			max.x = cashedApex.x > ( tmpDist = baseCenter.x + absDist ) ? cashedApex.x : tmpDist;
			min.x = cashedApex.x < ( tmpDist = baseCenter.x - absDist ) ? cashedApex.x : tmpDist;

			absDist = cashedRadius * ( abs( mx->_32 ) + abs( mx->_22 ) );
			max.y = cashedApex.y > ( tmpDist = baseCenter.y + absDist ) ? cashedApex.y : tmpDist;
			min.y = cashedApex.y < ( tmpDist = baseCenter.y - absDist ) ? cashedApex.y : tmpDist;

			absDist = cashedRadius * ( abs( mx->_33 ) + abs( mx->_23 ) );
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
};

#endif //_GEOMETRY_BOUNDING_CONE_X

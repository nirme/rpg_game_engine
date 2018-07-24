#pragma once

#ifndef _GEOMETRY_BOUNDING_CAPSULE_X
#define _GEOMETRY_BOUNDING_CAPSULE_X

#include "utility.h"
#include "Geometry_Bounding_Volume.h"

#include "Geometry_Bounding_Capsule.h"


class BoundingCapsuleX : public BoundingCapsule
{
public:

	BoundingCapsuleX( WorldPosition* _p_worldPosition, float _extent, float _radius, D3DXVECTOR3* _center = NULL );

	inline virtual void update()
	{
		DWORD stamp;
		if ( ( stamp = p_worldPosition->getWorldPositionTimestamp() ) != dw_worldPositionTimestamp )
		{
			const D3DXMATRIX* mx = p_worldPosition->getWorldPosition();
			D3DXVec3TransformCoord( &cashedCenter, &center, mx );
			D3DXVec3Normalize( &cashedDirection, reinterpret_cast <const D3DXVECTOR3*> ( &(mx->_11) ) );

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
};


#endif //_GEOMETRY_BOUNDING_CAPSULE_X

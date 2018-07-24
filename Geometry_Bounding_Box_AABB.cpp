#include "Geometry_Bounding_Box_AABB.h"


BoundingBoxAABB::BoundingBoxAABB( WorldPosition* _p_worldPosition, float _axisXExtent, float _axisYExtent, float _axisZExtent, D3DXVECTOR3* _center ) : BoundingVolume( _p_worldPosition )
{
	if ( _center )
		center = *_center;
	else
		ZeroMemory( &center, sizeof(D3DXVECTOR3) );

	v_axisExtent[0] = _axisXExtent;
	v_axisExtent[1] = _axisYExtent;
	v_axisExtent[2] = _axisZExtent;

	dw_worldPositionTimestamp = p_worldPosition->getWorldPositionTimestamp();
	dw_worldPositionTimestamp--;

	update();
};


BoundingBoxAABB::~BoundingBoxAABB()
{};


bool BoundingBoxAABB::intersectRay( Ray* _ray, float* _distance )
{
	return intersectAABBRay( _ray, _distance );
};


#pragma once

#ifndef _AUTO_NODE_LOADER
#define _AUTO_NODE_LOADER


#include "utility.h"
#include "Core_Defines.h"

#include "Geometry.h"
#include "Core_Camera.h"


#include "Geo_Node.h"
#include "Octree_Node.h"



#define		AUTO_NODE_LOADER_DISTANCE		100.0f


class AutoNodeLoader
{
protected:

	Camera* pCamera;
	D3DXVECTOR3* pCameraPosition;

	float minDistanceFromGeoNode;
	float maxDistanceFromGeoNode;

	float minDistanceFromOctreeNode;
	float maxDistanceFromOctreeNode;


public:

	AutoNodeLoader() :	pCamera( NULL ), 
						pCameraPosition( NULL ), 
						minDistanceFromGeoNode( AUTO_NODE_LOADER_DISTANCE ), 
						maxDistanceFromGeoNode( AUTO_NODE_LOADER_DISTANCE ), 
						minDistanceFromOctreeNode( AUTO_NODE_LOADER_DISTANCE ), 
						maxDistanceFromOctreeNode( AUTO_NODE_LOADER_DISTANCE )
	{};

	AutoNodeLoader( Camera* _pCamera, float _minMaxDistanceFromGeoNode, float _minMaxDistanceFromOctreeNode ) : 
		pCamera( _pCamera ), 
		pCameraPosition( NULL ), 
		minDistanceFromGeoNode( _minMaxDistanceFromGeoNode ), 
		maxDistanceFromGeoNode( _minMaxDistanceFromGeoNode ), 
		minDistanceFromOctreeNode( _minMaxDistanceFromOctreeNode ), 
		maxDistanceFromOctreeNode( _minMaxDistanceFromOctreeNode )
	{
		pCameraPosition = pCamera->getPosition();
	};

	AutoNodeLoader( Camera* _pCamera, float _minDistanceFromGeoNode, float _maxDistanceFromGeoNode, float _minDistanceFromOctreeNode, float _maxDistanceFromOctreeNode ) : 
		pCamera( _pCamera ), 
		pCameraPosition( NULL ), 
		minDistanceFromGeoNode( _minDistanceFromGeoNode ), 
		maxDistanceFromGeoNode( _maxDistanceFromGeoNode ), 
		minDistanceFromOctreeNode( _minDistanceFromOctreeNode ), 
		maxDistanceFromOctreeNode( _maxDistanceFromOctreeNode )
	{
		pCameraPosition = pCamera->getPosition();
	};


	void setCamera( Camera* _pCamera )
	{
		pCamera = _pCamera;
		pCameraPosition = pCamera->getPosition();
	};



	inline bool isOutsideBoundary( BoundingVolume* _pVolume )
	{
		return !( _pVolume->distanceFromPointLT( pCameraPosition, maxDistanceFromGeoNode ) );
	};

	inline bool isInsideBoundary( BoundingVolume* _pVolume )
	{
		return _pVolume->distanceFromPointLT( pCameraPosition, minDistanceFromGeoNode );
	};


	inline bool isOutsideBoundary( BoundingBoxAABB* _pVolume )
	{
		return !( _pVolume->distanceAABBFromPointLT( pCameraPosition, maxDistanceFromOctreeNode ) );
	};

	inline bool isInsideBoundary( BoundingBoxAABB* _pVolume )
	{
		return _pVolume->distanceAABBFromPointLT( pCameraPosition, minDistanceFromOctreeNode );
	};


};


#endif //_AUTO_NODE_LOADER

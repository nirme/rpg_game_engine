#pragma once

#ifndef _GEO_NODE
#define _GEO_NODE


#include "utility.h"
#include "Core_Defines.h"

#include "Base_Node.h"
#include "Core_World_Position_v3.h"

#include "Geometry.h"


class GeoNode : public BaseNode
{
protected:

	WorldPosition* pWorldPosition;
	BoundingVolume* pBoundingVolume;


public:

	GeoNode(string const & _name, BaseNode* _parentNode = NULL) : BaseNode(_name, _parentNode) {};
	GeoNode(string const & _name, WorldPosition* _pWorldPosition, BoundingVolume* _boundingVolume, BaseNode* _parentNode = NULL);
	virtual ~GeoNode();

	void setWorldPosition(WorldPosition* _pWorldPosition);
	WorldPosition* getWorldPosition();

	void setBoundingVolume( BoundingVolume* _boundingVolume);
	const BoundingVolume* getBoundingSphere();


	virtual void update()
	{
		return 0;
	};

};



#endif //_GEO_NODE

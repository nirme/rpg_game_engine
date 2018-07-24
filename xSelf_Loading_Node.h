#pragma once

#ifndef _SELF_LOADING_NODE
#define _SELF_LOADING_NODE


#include "utility.h"
#include "Core_Defines.h"

#include "Base_Node.h"
#include "Geo_Node.h"


class SelfLoadingNode : public GeoNode
{
protected:

public:
	GeoNode(string const & _name, BaseNode* _parentNode = NULL) : BaseNode(_name, _parentNode) {};
	GeoNode(string const & _name, WorldPosition* _pWorldPosition = NULL, SphereDesc* _boundingSphere = NULL, BaseNode* _parentNode = NULL);
	virtual ~GeoNode();

	virtual void setWorldPosition(WorldPosition* _pWorldPosition);
	virtual WorldPosition* getWorldPosition();

	virtual void setBoundingSphere(SphereDesc* _boundingSphere);
	virtual SphereDesc* getBoundingSphere();

	virtual int updateNode()
	{
		return 0;
	};
};



#endif //_SELF_LOADING_NODE

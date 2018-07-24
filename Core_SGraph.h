#pragma once

#ifndef _CORE_SGRAPH
#define _CORE_SGRAPH

#include "utility.h"
#include "Core_World_Position.h"


class SG_node;
class BoundingVolume;

class SG_node
{
protected:
	SG_node* parent;
	vector <SG_node*> childrens;

	bool isTriMesh;

	WorldPosition worldPosition;
	BoundingVolume* boundingVolume;

public:
	SG_node(SG_node* _parent, WorldPosition _worldPosition, BoundingVolume* _boundingVolume, bool _isTriMesh = true)
	{
		isTriMesh = _isTriMesh;;
		parent = _parent;
		childrens.clear();
		worldPosition = _worldPosition;
		boundingVolume = _boundingVolume;
	}
	virtual ~SG_node()
	{
		vector <SG_node*>::iterator it;
		for (it=childrens.begin();it!=childrens.end();it++)
			delete (*it);
		childrens.clear();
	}
	SG_node* getParent()
	{
		return parent;
	}
	DWORD getChildNumber()
	{
		return childrens.size();
	}
	SG_node* getChild(DWORD _index)
	{
		return childrens[_index];
	}
	void addChild(SG_node* _child)
	{
		childrens.push_back(_child);
	}
	void setBVolume(BoundingVolume* _boundingVolume)
	{
		boundingVolume = _boundingVolume;
	}

	BoundingVolume* getBVolume()
	{
		return boundingVolume;
	}

    inline D3DXMATRIX* getMatrix()
	{
		return worldPosition.getMatrix();
	}
	inline WorldPosition* getWorldPosition()
	{
		return &worldPosition;
	}
};

class BoundingVolume
{
public:
	D3DXVECTOR3 center;
	float radius;
};


#endif //_CORE_SGRAPH
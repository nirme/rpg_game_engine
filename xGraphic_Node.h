#pragma once

#ifndef _GRAPHIC_NODE
#define _GRAPHIC_NODE


#include "utility.h"
#include "Core_Defines.h"

#include "Geo_Node.h"
#include "Core_Graphic.h"
#include "Core_Graphic_Resource.h"
#include "Core_Graphic_Object.h"

#include "Camera_Node.h"
#include "Light_Node.h"


class GraphicNode : public GeoNode
{
protected:
	GraphicObject* graphicObject;

public:
	GraphicNode(const string _name, BaseNode* _parentNode = NULL);
	GraphicNode(const string _name, GraphicObject* _graphicObject, BoundingSphere* _boundingVolume, WorldPosition* _pWorldPosition = NULL, BoundingSphere* _volume = NULL, BaseNode* _parentNode = NULL);
	virtual ~GraphicNode();

	virtual void setWorldPosition(WorldPosition* _pWorldPosition);

	virtual int updateNode(float _timeDelta = 0.0f);


	int setGraphicObject(GraphicObject* _graphicObject)
	{
		SAFE_DELETE(graphicObject);
		graphicObject = graphicObject;
		return 0;
	};

	GraphicObject* getGraphicObject();

};


#endif //_GRAPHIC_NODE
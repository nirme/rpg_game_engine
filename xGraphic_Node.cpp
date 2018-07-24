#include "Graphic_Node.h"



	
GraphicNode::GraphicNode(string const & _name, BaseNode* _parentNode) : GeoNode(_name, _parentNode)
{
};

GraphicNode::GraphicNode(string const & _name, GraphicObject* _graphicObject, BoundingSphere* _boundingVolume, WorldPosition* _pWorldPosition, BoundingSphere* _volume, BaseNode* _parentNode) : GeoNode(_name, _pWorldPosition, _volume, _parentNode)
{
	graphicObject = _graphicObject;
	boundingVolume = _boundingVolume;
	graphicObject->setPosition(pWorldPosition->getWorldPosition());
};

GraphicNode::~GraphicNode()
{
	SAFE_RELEASE(graphicObject);
};

void GraphicNode::setWorldPosition(WorldPosition* _pWorldPosition)
{
	SAFE_DELETE(pWorldPosition);
	pWorldPosition = _pWorldPosition;
	if (!pWorldPosition)
		pWorldPosition = new WorldPosition;
	volume->setPosition(pWorldPosition);

	if (GeoNode* parent = dynamic_cast<GeoNode*>(getParent()))
	{
		pWorldPosition->attachTo(parent->getWorldPosition());
	}

};

int GraphicNode::updateNode(float _timeDelta)
{
	if (graphicObject->isLoaded())
		return 0;
	return 1;
};

GraphicObject* GraphicNode::getGraphicObject()
{
	return graphicObject;
};


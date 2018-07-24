#include "Geo_Node.h"



GeoNode::GeoNode(string _name, WorldPosition* _pWorldPosition, SphereDesc* _boundingSphere, BaseNode* _parentNode) : BaseNode(_name, _parentNode)
{
	pWorldPosition = _pWorldPosition;
	boundingSphere = _boundingSphere;

	if (!pWorldPosition)
		pWorldPosition = new WorldPosition;

	if (!boundingSphere)
		boundingSphere = new SphereDesc;

	GeoNode* parent = NULL;
	parent = dynamic_cast<GeoNode*>(_parentNode);
	if (parent)
	{
		pWorldPosition->attachTo(parent->getWorldPosition());
	}
};

GeoNode::~GeoNode()
{
	delete pWorldPosition;
	delete volume;
};

void GeoNode::setWorldPosition(WorldPosition* _pWorldPosition)
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

WorldPosition* GeoNode::getWorldPosition()
{
	return pWorldPosition;
};

void GeoNode::setBoundingSphere(SphereDesc* _boundingSphere)
{
	SAFE_DELETE(boundingSphere);
	boundingSphere = _boundingSphere;
	if (!boundingSphere)
		boundingSphere = new SphereDesc;
};

SphereDesc* GeoNode::getBoundingVolume()
{
	return boundingSphere;
};



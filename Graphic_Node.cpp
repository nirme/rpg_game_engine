#include "Graphic_Node.h"




void GraphicNode::updateWorldPositionLinks()
{
	pGraphicObject->setWorldPosition( pWorldPosition );
};


void GraphicNode::setGraphicObject( GraphicObject* _pGraphicObject )
{
	SAFE_RELEASE( pGraphicObject );
	pGraphicObject = _pGraphicObject;
	pGraphicObject->setWorldPosition( pWorldPosition );
};


void GraphicNode::setup( const char* _name, BaseNode* _pParent, WorldPosition* _pWorldPosition, GraphicObject* _pGraphicObject, BoundingVolume* _pBoundingVolume, USHORT _childrenNumber, BaseNode* const* _v_children )
{
	setup( _name, _pParent, _pWorldPosition, _pBoundingVolume, _childrenNumber, _v_children );

	SAFE_RELEASE( pGraphicObject );
	pGraphicObject = _pGraphicObject;

	updateWorldPositionLinks();
};


void GraphicNode::setup( const string &_name, BaseNode* _pParent, WorldPosition* _pWorldPosition, GraphicObject* _pGraphicObject, BoundingVolume* _pBoundingVolume, USHORT _childrenNumber, BaseNode* const* _v_children )
{
	setup( _name, _pParent, _pWorldPosition, _pBoundingVolume, _childrenNumber, _v_children );

	SAFE_RELEASE( pGraphicObject );
	pGraphicObject = _pGraphicObject;

	updateWorldPositionLinks();
};


void GraphicNode::setup( const char* _name, BaseNode* _pParent, WorldPosition* _pWorldPosition, GraphicObject* _pGraphicObject, BoundingVolume* _pBoundingVolume, USHORT _childrenNumber, char* const* _v_childrenNames )
{
	setup( _name, _pParent, _pWorldPosition, _pBoundingVolume, _childrenNumber, _v_childrenNames );

	SAFE_RELEASE( pGraphicObject );
	pGraphicObject = _pGraphicObject;

	updateWorldPositionLinks();
};


void GraphicNode::setup( const string &_name, BaseNode* _pParent, WorldPosition* _pWorldPosition, GraphicObject* _pGraphicObject, BoundingVolume* _pBoundingVolume, USHORT _childrenNumber, string* const _v_childrenNames )
{
	setup( _name, _pParent, _pWorldPosition, _pBoundingVolume, _childrenNumber, _v_childrenNames );

	SAFE_RELEASE( pGraphicObject );
	pGraphicObject = _pGraphicObject;

	updateWorldPositionLinks();
};


GraphicNode::GraphicNode() : GeoNode(), pGraphicObject(NULL)
{};


GraphicNode::GraphicNode( const char* _name, BaseNode* _pParent, WorldPosition* _pWorldPosition, GraphicObject* _pGraphicObject, BoundingVolume* _pBoundingVolume, USHORT _childrenNumber = 0, BaseNode* const* _v_children = NULL ) : 
	GeoNode( _name, _pParent, _pWorldPosition, _pBoundingVolume, _childrenNumber, _v_children )
{
	pGraphicObject = _pGraphicObject;
	updateWorldPositionLinks();
};


GraphicNode::GraphicNode( const string &_name, BaseNode* _pParent, WorldPosition* _pWorldPosition, GraphicObject* _pGraphicObject, BoundingVolume* _pBoundingVolume, USHORT _childrenNumber = 0, BaseNode* const* _v_children = NULL ) : 
	GeoNode( _name, _pParent, _pWorldPosition, _pBoundingVolume, _childrenNumber, _v_children )
{
	pGraphicObject = _pGraphicObject;
	updateWorldPositionLinks();
};


GraphicNode::GraphicNode( const char* _name, BaseNode* _pParent, WorldPosition* _pWorldPosition, GraphicObject* _pGraphicObject, BoundingVolume* _pBoundingVolume, USHORT _childrenNumber, char* const* _v_childrenNames ) : 
	GeoNode( _name, _pParent, _pWorldPosition, _pBoundingVolume, _childrenNumber, _v_childrenNames )
{
	pGraphicObject = _pGraphicObject;
	updateWorldPositionLinks();
};


GraphicNode::GraphicNode( const string &_name, BaseNode* _pParent, WorldPosition* _pWorldPosition, GraphicObject* _pGraphicObject, BoundingVolume* _pBoundingVolume, USHORT _childrenNumber, string* const _v_childrenNames ) : 
	GeoNode( _name, _pParent, _pWorldPosition, _pBoundingVolume, _childrenNumber, _v_childrenNames )
{
	pGraphicObject = _pGraphicObject;
	updateWorldPositionLinks();
};


GraphicNode::~GraphicNode()
{
	SAFE_RELEASE( pGraphicObject );
};


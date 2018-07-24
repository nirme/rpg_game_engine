#include "Animated_Node.h"




void AnimatedNode::setup( const char* _name, BaseNode* _pParent, WorldPosition* _pWorldPosition, GraphicObject* _pGraphicObject, BoundingVolume* _pBoundingVolume, USHORT _childrenNumber, BaseNode* const* _v_children )
{
	setup( _name, _pParent, _pWorldPosition, _pBoundingVolume, _childrenNumber, _v_children );

	SAFE_RELEASE( pGraphicObject );
	pGraphicObject = _pGraphicObject;
	pAnimationInstance = graphicObject->getAnimationInfo();

	updateWorldPositionLinks();
};


void AnimatedNode::setup( const string &_name, BaseNode* _pParent, WorldPosition* _pWorldPosition, GraphicObject* _pGraphicObject, BoundingVolume* _pBoundingVolume, USHORT _childrenNumber, BaseNode* const* _v_children )
{
	setup( _name, _pParent, _pWorldPosition, _pBoundingVolume, _childrenNumber, _v_children );

	SAFE_RELEASE( pGraphicObject );
	pGraphicObject = _pGraphicObject;
	pAnimationInstance = graphicObject->getAnimationInfo();

	updateWorldPositionLinks();
};


void AnimatedNode::setup( const char* _name, BaseNode* _pParent, WorldPosition* _pWorldPosition, GraphicObject* _pGraphicObject, BoundingVolume* _pBoundingVolume, USHORT _childrenNumber, char* const* _v_childrenNames )
{
	setup( _name, _pParent, _pWorldPosition, _pBoundingVolume, _childrenNumber, _v_childrenNames );

	SAFE_RELEASE( pGraphicObject );
	pGraphicObject = _pGraphicObject;
	pAnimationInstance = graphicObject->getAnimationInfo();

	updateWorldPositionLinks();
};


void AnimatedNode::setup( const string &_name, BaseNode* _pParent, WorldPosition* _pWorldPosition, GraphicObject* _pGraphicObject, BoundingVolume* _pBoundingVolume, USHORT _childrenNumber, string* const _v_childrenNames )
{
	setup( _name, _pParent, _pWorldPosition, _pBoundingVolume, _childrenNumber, _v_childrenNames );

	SAFE_RELEASE( pGraphicObject );
	pGraphicObject = _pGraphicObject;
	pAnimationInstance = graphicObject->getAnimationInfo();

	updateWorldPositionLinks();
};




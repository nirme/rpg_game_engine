#include "Geo_Node.h"


//GeoNode::


void GeoNode::setWorldPosition( WorldPosition* _pWorldPosition )
{
	WorldPosition* parentPosition = pWorldPosition->getAttachedPosition();
	SAFE_DELETE( pWorldPosition );
	pWorldPosition = _pWorldPosition;
	if ( pBoundingVolume )
		pBoundingVolume->setWorldPosition( pWorldPosition );

	pWorldPosition->attachTo( parentPosition );

	updateWorldPositionLinks();
};


void GeoNode::setBoundingVolume( BoundingVolume* _pBoundingVolume )
{
	SAFE_DELETE( pBoundingVolume );
	pBoundingVolume = _pBoundingVolume;
	if ( pBoundingVolume )
		pBoundingVolume->setWorldPosition( pWorldPosition );
};



void GeoNode::setup( const char* _name, BaseNode* _pParent, WorldPosition* _pWorldPosition, BoundingVolume* _pBoundingVolume, USHORT _childrenNumber, BaseNode* const* _v_children )
{
	setup( _name, _pParent, _childrenNumber, _v_children );

	SAFE_DELETE( pWorldPosition );
	pWorldPosition = _pWorldPosition;

	SAFE_DELETE( pBoundingVolume );
	pBoundingVolume = _pBoundingVolume;
	if (pBoundingVolume)
		pBoundingVolume->setWorldPosition( pWorldPosition );

	setParentPosition();
};


void GeoNode::setup( const string &_name, BaseNode* _pParent, WorldPosition* _pWorldPosition, BoundingVolume* _pBoundingVolume, USHORT _childrenNumber, BaseNode* const* _v_children )
{
	setup( _name, _pParent, _childrenNumber, _v_children );

	SAFE_DELETE( pWorldPosition );
	pWorldPosition = _pWorldPosition;

	SAFE_DELETE( pBoundingVolume );
	pBoundingVolume = _pBoundingVolume;
	if (pBoundingVolume)
		pBoundingVolume->setWorldPosition( pWorldPosition );

	setParentPosition();
};


void GeoNode::setup( const char* _name, BaseNode* _pParent, WorldPosition* _pWorldPosition, BoundingVolume* _pBoundingVolume, USHORT _childrenNumber, char* const* _v_childrenNames )
{
	setup( _name, _pParent, _childrenNumber, _v_childrenNames );

	SAFE_DELETE( pWorldPosition );
	pWorldPosition = _pWorldPosition;

	SAFE_DELETE( pBoundingVolume );
	pBoundingVolume = _pBoundingVolume;
	if (pBoundingVolume)
		pBoundingVolume->setWorldPosition( pWorldPosition );

	setParentPosition();
};


void GeoNode::setup( const string &_name, BaseNode* _pParent, WorldPosition* _pWorldPosition, BoundingVolume* _pBoundingVolume, USHORT _childrenNumber, string* const _v_childrenNames )
{
	setup( _name, _pParent, _childrenNumber, _v_childrenNames );

	SAFE_DELETE( pWorldPosition );
	pWorldPosition = _pWorldPosition;

	SAFE_DELETE( pBoundingVolume );
	pBoundingVolume = _pBoundingVolume;
	if (pBoundingVolume)
		pBoundingVolume->setWorldPosition( pWorldPosition );

	setParentPosition();
};



GeoNode::GeoNode() : BaseNode(), pWorldPosition(NULL), pBoundingVolume(NULL)
{};


GeoNode::GeoNode( const char* _name, BaseNode* _pParent, WorldPosition* _pWorldPosition, BoundingVolume* _pBoundingVolume, USHORT _childrenNumber, BaseNode* const* _v_children ) : BaseNode( _name, _pParent, _childrenNumber, _v_children )
{
	pWorldPosition = _pWorldPosition;
	pBoundingVolume = _pBoundingVolume;
	pBoundingVolume->setWorldPosition( pWorldPosition );
	setParentPosition();
};


GeoNode::GeoNode( const string &_name, BaseNode* _pParent, WorldPosition* _pWorldPosition, BoundingVolume* _pBoundingVolume, USHORT _childrenNumber, BaseNode* const* _v_children ) : BaseNode( _name, _pParent, _childrenNumber, _v_children )
{
	pWorldPosition = _pWorldPosition;
	pBoundingVolume = _pBoundingVolume;
	pBoundingVolume->setWorldPosition( pWorldPosition );
	setParentPosition();
};


GeoNode::GeoNode( const char* _name, BaseNode* _pParent, WorldPosition* _pWorldPosition, BoundingVolume* _pBoundingVolume, USHORT _childrenNumber, char* const* _v_childrenNames ) : BaseNode( _name, _pParent, _childrenNumber, _v_childrenNames )
{
	pWorldPosition = _pWorldPosition;
	pBoundingVolume = _pBoundingVolume;
	pBoundingVolume->setWorldPosition( pWorldPosition );
	setParentPosition();
};


GeoNode::GeoNode( const string &_name, BaseNode* _pParent, WorldPosition* _pWorldPosition, BoundingVolume* _pBoundingVolume, USHORT _childrenNumber, string* const _v_childrenNames ) : BaseNode( _name, _pParent, _childrenNumber, _v_childrenNames )
{
	pWorldPosition = _pWorldPosition;
	pBoundingVolume = _pBoundingVolume;
	pBoundingVolume->setWorldPosition( pWorldPosition );
	setParentPosition();
};


GeoNode::~GeoNode()
{
	SAFE_DELETE( pWorldPosition );
	SAFE_DELETE( pBoundingVolume );
};


int GeoNode::restrucNode()
{
	setParentPosition();
};

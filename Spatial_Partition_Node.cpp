#include "Spatial_Partition_Node.h"



void SpatialPartitionNode::setWorldPosition( WorldPosition* _pWorldPosition )
{
	WorldPosition* parentPosition = pWorldPosition->getAttachedPosition();
	SAFE_DELETE( pWorldPosition );
	pWorldPosition = _pWorldPosition;
	pWorldPosition->attachTo( parentPosition );
	pBoundingBox->setWorldPosition( pWorldPosition );
};


void SpatialPartitionNode::setBoundingBox( BoundingBoxAABB* _pBoundingBox )
{
	SAFE_DELETE( pBoundingBox );
	pBoundingBox = _pBoundingBox;
	pBoundingBox->setWorldPosition( pWorldPosition );
};


void SpatialPartitionNode::setup( const char* _name, BaseNode* _pParent, WorldPosition* _pWorldPosition, BoundingBoxAABB* _pBoundingBox, USHORT _childrenNumber, BaseNode* const* _v_children )
{
	setup( _name, _pParent, _childrenNumber, _v_children );

	SAFE_DELETE( pWorldPosition );
	pWorldPosition = _pWorldPosition;

	SAFE_DELETE( pBoundingBox );
	pBoundingBox = _pBoundingBox;
	pBoundingBox->setWorldPosition( pWorldPosition );
	setParentPosition();
};


void SpatialPartitionNode::setup( const string &_name, BaseNode* _pParent, WorldPosition* _pWorldPosition, BoundingBoxAABB* _pBoundingBox, USHORT _childrenNumber, BaseNode* const* _v_children )
{
	setup( _name, _pParent, _childrenNumber, _v_children );

	SAFE_DELETE( pWorldPosition );
	pWorldPosition = _pWorldPosition;

	SAFE_DELETE( pBoundingBox );
	pBoundingBox = _pBoundingBox;
	pBoundingBox->setWorldPosition( pWorldPosition );
	setParentPosition();
};


void SpatialPartitionNode::setup( const char* _name, BaseNode* _pParent, WorldPosition* _pWorldPosition, BoundingBoxAABB* _pBoundingBox, USHORT _childrenNumber, char* const* _v_childrenNames )
{
	setup( _name, _pParent, _childrenNumber, _v_childrenNames );

	SAFE_DELETE( pWorldPosition );
	pWorldPosition = _pWorldPosition;

	SAFE_DELETE( pBoundingBox );
	pBoundingBox = _pBoundingBox;
	pBoundingBox->setWorldPosition( pWorldPosition );
	setParentPosition();
};


void SpatialPartitionNode::setup( const string &_name, BaseNode* _pParent, WorldPosition* _pWorldPosition, BoundingBoxAABB* _pBoundingBox, USHORT _childrenNumber, string* const _v_childrenNames )
{
	setup( _name, _pParent, _childrenNumber, _v_childrenNames );

	SAFE_DELETE( pWorldPosition );
	pWorldPosition = _pWorldPosition;

	SAFE_DELETE( pBoundingBox );
	pBoundingBox = _pBoundingBox;
	pBoundingBox->setWorldPosition( pWorldPosition );
	setParentPosition();
};


SpatialPartitionNode::SpatialPartitionNode() : BaseNode(), pWorldPosition(NULL), pBoundingBox(NULL)
{};


SpatialPartitionNode::SpatialPartitionNode( const char* _name, BaseNode* _pParent, WorldPosition* _pWorldPosition, BoundingBoxAABB* _pBoundingBox, USHORT _childrenNumber, BaseNode* const* _v_children ) : BaseNode( _name, _pParent, _childrenNumber, _v_children )
{
	pWorldPosition = _pWorldPosition;
	pBoundingBox = _pBoundingBox;
	pBoundingBox->setWorldPosition( pWorldPosition );
	setParentPosition();
};


SpatialPartitionNode::SpatialPartitionNode( const string &_name, BaseNode* _pParent, WorldPosition* _pWorldPosition, BoundingBoxAABB* _pBoundingBox, USHORT _childrenNumber, BaseNode* const* _v_children ) : BaseNode( _name, _pParent, _childrenNumber, _v_children )
{
	pWorldPosition = _pWorldPosition;
	pBoundingBox = _pBoundingBox;
	pBoundingBox->setWorldPosition( pWorldPosition );
	setParentPosition();
};


SpatialPartitionNode::SpatialPartitionNode( const char* _name, BaseNode* _pParent, WorldPosition* _pWorldPosition, BoundingBoxAABB* _pBoundingBox, USHORT _childrenNumber, char* const* _v_childrenNames ) : BaseNode( _name, _pParent, _childrenNumber, _v_childrenNames )
{
	pWorldPosition = _pWorldPosition;
	pBoundingBox = _pBoundingBox;
	pBoundingBox->setWorldPosition( pWorldPosition );
	setParentPosition();
};


SpatialPartitionNode::SpatialPartitionNode( const string &_name, BaseNode* _pParent, WorldPosition* _pWorldPosition, BoundingBoxAABB* _pBoundingBox, USHORT _childrenNumber, string* const _v_childrenNames ) : BaseNode( _name, _pParent, _childrenNumber, _v_childrenNames )
{
	pWorldPosition = _pWorldPosition;
	pBoundingBox = _pBoundingBox;
	pBoundingBox->setWorldPosition( pWorldPosition );
	setParentPosition();
};


SpatialPartitionNode::~SpatialPartitionNode()
{
	SAFE_DELETE( pWorldPosition );
	SAFE_DELETE( pBoundingBox );
};


int SpatialPartitionNode::update()
{
	setParentPosition();
};




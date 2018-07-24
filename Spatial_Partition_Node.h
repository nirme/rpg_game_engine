#pragma once

#ifndef _SPATIAL_PARTITION_NODE
#define _SPATIAL_PARTITION_NODE


#include "utility.h"
#include "Core_Defines.h"

#include "Base_Node.h"
#include "Geometry.h"




class SpatialPartitionNode : public BaseNode
{
protected:

	WorldPosition* pWorldPosition;
	BoundingBoxAABB* pBoundingBox;


	inline void setParentPosition()
	{
		BaseNode* par = pParent;
		SpatialPartitionNode* part = NULL;

		while ( par )
		{
			if ( part = dynamic_cast <SpatialPartitionNode*> (par) )
			{
				pWorldPosition->attachTo( part->getWorldPosition() );
				return;
			};

			par = par->getParent();
		}
	};


public:

	void setWorldPosition( WorldPosition* _pWorldPosition );
	void setBoundingBox( BoundingBoxAABB* _pBoundingBox );

	inline WorldPosition* getWorldPosition()	{	return pWorldPosition;	};
	inline BoundingBoxAABB* getBoundingBox()	{	return pBoundingBox;	};


	void setup(	const char* _name, 
				BaseNode* _pParent, 
				WorldPosition* _pWorldPosition, 
				BoundingBoxAABB* _pBoundingBox, 
				USHORT _childrenNumber = 0, 
				BaseNode* const* _v_children = NULL );

	void setup(	const string &_name, 
				BaseNode* _pParent, 
				WorldPosition* _pWorldPosition, 
				BoundingBoxAABB* _pBoundingBox, 
				USHORT _childrenNumber = 0, 
				BaseNode* const* _v_children = NULL );

	void setup(	const char* _name, 
				BaseNode* _pParent, 
				WorldPosition* _pWorldPosition, 
				BoundingBoxAABB* _pBoundingBox, 
				USHORT _childrenNumber, 
				char* const* _v_childrenNames );

	void setup(	const string &_name, 
				BaseNode* _pParent, 
				WorldPosition* _pWorldPosition, 
				BoundingBoxAABB* _pBoundingBox, 
				USHORT _childrenNumber, 
				string* const _v_childrenNames );


	SpatialPartitionNode();

	SpatialPartitionNode(	const char* _name, 
							BaseNode* _pParent, 
							WorldPosition* _pWorldPosition, 
							BoundingBoxAABB* _pBoundingBox, 
							USHORT _childrenNumber = 0, 
							BaseNode* const* _v_children = NULL );

	SpatialPartitionNode(	const string &_name, 
							BaseNode* _pParent, 
							WorldPosition* _pWorldPosition, 
							BoundingBoxAABB* _pBoundingBox, 
							USHORT _childrenNumber = 0, 
							BaseNode* const* _v_children = NULL );

	SpatialPartitionNode(	const char* _name, 
							BaseNode* _pParent, 
							WorldPosition* _pWorldPosition, 
							BoundingBoxAABB* _pBoundingBox, 
							USHORT _childrenNumber, 
							char* const* _v_childrenNames );

	SpatialPartitionNode(	const string &_name, 
							BaseNode* _pParent, 
							WorldPosition* _pWorldPosition, 
							BoundingBoxAABB* _pBoundingBox, 
							USHORT _childrenNumber, 
							string* const _v_childrenNames );

	virtual ~SpatialPartitionNode();


	virtual int update();





};


#endif //_SPATIAL_PARTITION_NODE
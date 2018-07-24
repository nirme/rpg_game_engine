#pragma once

#ifndef _GRAPHIC_NODE
#define _GRAPHIC_NODE


#include "utility.h"
#include "Core_Defines.h"

#include "Geo_Node.h"
#include "Renderable_Node.h"

#include "Core_Graphic_Object.h"



class GraphicNode : public RenderableNode
{
protected:

	GraphicObject* pGraphicObject;

public:

	virtual void updateWorldPositionLinks();
	virtual void setGraphicObject( GraphicObject* _pGraphicObject );

	inline GraphicObject* getGraphicObject()	{	return pGraphicObject;	};


	virtual void setup(	const char* _name, 
						BaseNode* _pParent, 
						WorldPosition* _pWorldPosition, 
						GraphicObject* _pGraphicObject, 
						BoundingVolume* _pBoundingVolume, 
						USHORT _childrenNumber = 0, 
						BaseNode* const* _v_children = NULL );

	virtual void setup(	const string &_name, 
						BaseNode* _pParent, 
						WorldPosition* _pWorldPosition, 
						GraphicObject* _pGraphicObject, 
						BoundingVolume* _pBoundingVolume, 
						USHORT _childrenNumber = 0, 
						BaseNode* const* _v_children = NULL );

	virtual void setup(	const char* _name, 
						BaseNode* _pParent, 
						WorldPosition* _pWorldPosition, 
						GraphicObject* _pGraphicObject, 
						BoundingVolume* _pBoundingVolume, 
						USHORT _childrenNumber, 
						char* const* _v_childrenNames );

	virtual void setup(	const string &_name, 
						BaseNode* _pParent, 
						WorldPosition* _pWorldPosition, 
						GraphicObject* _pGraphicObject, 
						BoundingVolume* _pBoundingVolume, 
						USHORT _childrenNumber, 
						string* const _v_childrenNames );


	GraphicNode();

	GraphicNode(	const char* _name, 
					BaseNode* _pParent, 
					WorldPosition* _pWorldPosition, 
					GraphicObject* _pGraphicObject, 
					BoundingVolume* _pBoundingVolume, 
					USHORT _childrenNumber = 0, 
					BaseNode* const* _v_children = NULL );

	GraphicNode(	const string &_name, 
					BaseNode* _pParent, 
					WorldPosition* _pWorldPosition, 
					GraphicObject* _pGraphicObject, 
					BoundingVolume* _pBoundingVolume, 
					USHORT _childrenNumber = 0, 
					BaseNode* const* _v_children = NULL );

	GraphicNode(	const char* _name, 
					BaseNode* _pParent, 
					WorldPosition* _pWorldPosition, 
					GraphicObject* _pGraphicObject, 
					BoundingVolume* _pBoundingVolume, 
					USHORT _childrenNumber, 
					char* const* _v_childrenNames );

	GraphicNode(	const string &_name, 
					BaseNode* _pParent, 
					WorldPosition* _pWorldPosition, 
					GraphicObject* _pGraphicObject, 
					BoundingVolume* _pBoundingVolume, 
					USHORT _childrenNumber, 
					string* const _v_childrenNames );

	~GraphicNode();


	// not needed
	//virtual int updateNode( TIMETYPE _timeDelta );

	virtual int render( RenderVisitor* _pRenderVisitor )
	{
		return _pRenderVisitor->render( pGraphicObject );
	};


};

#endif //_GRAPHIC_NODE
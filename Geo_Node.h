#pragma once

#ifndef _GEO_NODE
#define _GEO_NODE


#include "utility.h"
#include "Core_Defines.h"

#include "Base_Node.h"

#include "Core_World_Position_v3.h"

#include "Geometry.h"




class GeoNode : public BaseNode
{
protected:

	WorldPosition* pWorldPosition;
	BoundingVolume* pBoundingVolume;


	inline void setParentPosition()
	{
		BaseNode* par = pParent;
		GeoNode* geo = NULL;

		while ( par )
		{
			if ( geo = dynamic_cast <GeoNode*> (par) )
			{
				pWorldPosition->attachTo( geo->getWorldPosition() );
				return;
			};

			par = par->getParent();
		}
	};


public:

	inline virtual void setParent( BaseNode* _pParent )
	{
		pParent = _pParent;
		setParentPosition();
	};


	virtual void updateWorldPositionLinks();

	void setWorldPosition( WorldPosition* _pWorldPosition );
	void setBoundingVolume( BoundingVolume* _pBoundingVolume );

	inline WorldPosition* getWorldPosition()	{	return pWorldPosition;	};
	inline BoundingVolume* getBoundingVolume()	{	return pBoundingVolume;	};


	void setup(	const char* _name, 
				BaseNode* _pParent, 
				WorldPosition* _pWorldPosition, 
				BoundingVolume* _pBoundingVolume, 
				USHORT _childrenNumber = 0, 
				BaseNode* const* _v_children = NULL );

	void setup(	const string &_name, 
				BaseNode* _pParent, 
				WorldPosition* _pWorldPosition, 
				BoundingVolume* _pBoundingVolume, 
				USHORT _childrenNumber = 0, 
				BaseNode* const* _v_children = NULL );

	void setup(	const char* _name, 
				BaseNode* _pParent, 
				WorldPosition* _pWorldPosition, 
				BoundingVolume* _pBoundingVolume, 
				USHORT _childrenNumber, 
				char* const* _v_childrenNames );

	void setup(	const string &_name, 
				BaseNode* _pParent, 
				WorldPosition* _pWorldPosition, 
				BoundingVolume* _pBoundingVolume, 
				USHORT _childrenNumber, 
				string* const _v_childrenNames );


	GeoNode();

	GeoNode(	const char* _name, 
				BaseNode* _pParent, 
				WorldPosition* _pWorldPosition, 
				BoundingVolume* _pBoundingVolume, 
				USHORT _childrenNumber = 0, 
				BaseNode* const* _v_children = NULL );

	GeoNode(	const string &_name, 
				BaseNode* _pParent, 
				WorldPosition* _pWorldPosition, 
				BoundingVolume* _pBoundingVolume, 
				USHORT _childrenNumber = 0, 
				BaseNode* const* _v_children = NULL );

	GeoNode(	const char* _name, 
				BaseNode* _pParent, 
				WorldPosition* _pWorldPosition, 
				BoundingVolume* _pBoundingVolume, 
				USHORT _childrenNumber, 
				char* const* _v_childrenNames );

	GeoNode(	const string &_name, 
				BaseNode* _pParent, 
				WorldPosition* _pWorldPosition, 
				BoundingVolume* _pBoundingVolume, 
				USHORT _childrenNumber, 
				string* const _v_childrenNames );

	virtual ~GeoNode();


	virtual int restrucNode();

};


#endif //_GEO_NODE

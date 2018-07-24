#pragma once

#ifndef _PARTICLE_SYSTEM_NODE
#define _PARTICLE_SYSTEM_NODE


#include "utility.h"
#include "Core_Defines.h"

#include "Geo_Node.h"
#include "Renderable_Node.h"

#include "Particle_System_v4.h"




class ParticleSystemNode : RenderableNode
{
protected:

	ParticleSystem_v4* pParticleSystem;

	bool updateOnRender;
	float timeDelta;


public:

	virtual void updateWorldPositionLinks();

	virtual void setParticleSystem( ParticleSystem_v4* _pParticleSystem );
	void setUpdateOnRender( bool _updateOnRender );

	inline ParticleSystem_v4* getParticleSystem()	{	return pParticleSystem;	};
	inline bool getUpdateOnRender()					{	return updateOnRender;	};


	virtual void setup(	const char* _name, 
						BaseNode* _pParent, 
						WorldPosition* _pWorldPosition, 
						ParticleSystem_v4* _pParticleSystem, 
						bool _updateOnRender, 
						BoundingVolume* _pBoundingVolume, 
						USHORT _childrenNumber = 0, 
						BaseNode* const* _v_children = NULL );

	virtual void setup(	const string &_name, 
						BaseNode* _pParent, 
						WorldPosition* _pWorldPosition, 
						ParticleSystem_v4* _pParticleSystem, 
						bool _updateOnRender, 
						BoundingVolume* _pBoundingVolume, 
						USHORT _childrenNumber = 0, 
						BaseNode* const* _v_children = NULL );

	virtual void setup(	const char* _name, 
						BaseNode* _pParent, 
						WorldPosition* _pWorldPosition, 
						ParticleSystem_v4* _pParticleSystem, 
						bool _updateOnRender, 
						BoundingVolume* _pBoundingVolume, 
						USHORT _childrenNumber, 
						char* const* _v_childrenNames );

	virtual void setup(	const string &_name, 
						BaseNode* _pParent, 
						WorldPosition* _pWorldPosition, 
						ParticleSystem_v4* _pParticleSystem, 
						bool _updateOnRender, 
						BoundingVolume* _pBoundingVolume, 
						USHORT _childrenNumber, 
						string* const _v_childrenNames );



	ParticleSystemNode() : GeoNode(), pParticleSystem(NULL), updateOnRender(false), timeDelta(0.0f)
	{};


	ParticleSystemNode(	const char* _name, 
						BaseNode* _pParent, 
						WorldPosition* _pWorldPosition, 
						ParticleSystem_v4* _pParticleSystem, 
						bool _updateOnRender, 
						BoundingVolume* _pBoundingVolume, 
						USHORT _childrenNumber = 0, 
						BaseNode* const* _v_children = NULL );

	ParticleSystemNode(	const string &_name, 
						BaseNode* _pParent, 
						WorldPosition* _pWorldPosition, 
						ParticleSystem_v4* _pParticleSystem, 
						bool _updateOnRender, 
						BoundingVolume* _pBoundingVolume, 
						USHORT _childrenNumber = 0, 
						BaseNode* const* _v_children = NULL );

	ParticleSystemNode(	const char* _name, 
						BaseNode* _pParent, 
						WorldPosition* _pWorldPosition, 
						ParticleSystem_v4* _pParticleSystem, 
						bool _updateOnRender, 
						BoundingVolume* _pBoundingVolume, 
						USHORT _childrenNumber, 
						char* const* _v_childrenNames );

	ParticleSystemNode(	const string &_name, 
						BaseNode* _pParent, 
						WorldPosition* _pWorldPosition, 
						ParticleSystem_v4* _pParticleSystem, 
						bool _updateOnRender, 
						BoundingVolume* _pBoundingVolume, 
						USHORT _childrenNumber, 
						string* const _v_childrenNames );

	~ParticleSystemNode();


	virtual int updateNode( TIMETYPE _timeDelta );

	virtual int render( RenderVisitor* _pRenderVisitor );


};

#endif //_PARTICLE_SYSTEM_NODE

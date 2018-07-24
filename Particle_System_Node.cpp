#include "Particle_System_Node.h"




void ParticleSystemNode::updateWorldPositionLinks()
{
	pParticleSystem->setWorldPosition( pWorldPosition );
};


void ParticleSystemNode::setParticleSystem( ParticleSystem_v4* _pParticleSystem )
{
	SAFE_DELETE( pParticleSystem );
	pParticleSystem = _pParticleSystem;
	updateWorldPositionLinks();
};


void ParticleSystemNode::setUpdateOnRender( bool _updateOnRender )
{
	updateOnRender = _updateOnRender;
};



void ParticleSystemNode::setup(	const char* _name, BaseNode* _pParent, WorldPosition* _pWorldPosition, ParticleSystem_v4* _pParticleSystem, bool _updateOnRender, BoundingVolume* _pBoundingVolume, USHORT _childrenNumber = 0, BaseNode* const* _v_children = NULL )
{
	setup( _name, _pParent, _pWorldPosition, _pBoundingVolume, _childrenNumber, _v_children );

	SAFE_DELETE( pParticleSystem );
	pParticleSystem = _pParticleSystem;
	pParticleSystem->setWorldPosition( pWorldPosition );

	updateOnRender = _updateOnRender;

	updateWorldPositionLinks();
};


void ParticleSystemNode::setup(	const string &_name, BaseNode* _pParent, WorldPosition* _pWorldPosition, ParticleSystem_v4* _pParticleSystem, bool _updateOnRender, BoundingVolume* _pBoundingVolume, USHORT _childrenNumber = 0, BaseNode* const* _v_children = NULL )
{
	setup( _name, _pParent, _pWorldPosition, _pBoundingVolume, _childrenNumber, _v_children );

	SAFE_DELETE( pParticleSystem );
	pParticleSystem = _pParticleSystem;
	pParticleSystem->setWorldPosition( pWorldPosition );

	updateOnRender = _updateOnRender;

	updateWorldPositionLinks();
};


void ParticleSystemNode::setup(	const char* _name, BaseNode* _pParent, WorldPosition* _pWorldPosition, ParticleSystem_v4* _pParticleSystem, bool _updateOnRender, BoundingVolume* _pBoundingVolume, USHORT _childrenNumber, char* const* _v_childrenNames )
{
	setup( _name, _pParent, _pWorldPosition, _pBoundingVolume, _childrenNumber, _v_childrenNames );

	SAFE_DELETE( pParticleSystem );
	pParticleSystem = _pParticleSystem;
	pParticleSystem->setWorldPosition( pWorldPosition );

	updateOnRender = _updateOnRender;

	updateWorldPositionLinks();
};


void ParticleSystemNode::setup(	const string &_name, BaseNode* _pParent, WorldPosition* _pWorldPosition, ParticleSystem_v4* _pParticleSystem, bool _updateOnRender, BoundingVolume* _pBoundingVolume, USHORT _childrenNumber, string* const _v_childrenNames )
{
	setup( _name, _pParent, _pWorldPosition, _pBoundingVolume, _childrenNumber, _v_childrenNames );

	SAFE_DELETE( pParticleSystem );
	pParticleSystem = _pParticleSystem;
	pParticleSystem->setWorldPosition( pWorldPosition );

	updateOnRender = _updateOnRender;

	updateWorldPositionLinks();
};



ParticleSystemNode::ParticleSystemNode() : GeoNode(), pParticleSystem(NULL), updateOnRender(false), timeDelta(0.0f)
{};


ParticleSystemNode::ParticleSystemNode(	const char* _name, BaseNode* _pParent, WorldPosition* _pWorldPosition, ParticleSystem_v4* _pParticleSystem, bool _updateOnRender, BoundingVolume* _pBoundingVolume, USHORT _childrenNumber = 0, BaseNode* const* _v_children = NULL ) : 
	GeoNode( _name, _pParent, _pWorldPosition, _pBoundingVolume, _childrenNumber, _v_children )
{
	timeDelta = 0.0f;

	pParticleSystem = _pParticleSystem;
	pParticleSystem->setWorldPosition( pWorldPosition );

	updateOnRender = _updateOnRender;

	updateWorldPositionLinks();
};


ParticleSystemNode::ParticleSystemNode( const string &_name, BaseNode* _pParent, WorldPosition* _pWorldPosition, ParticleSystem_v4* _pParticleSystem, bool _updateOnRender, BoundingVolume* _pBoundingVolume, USHORT _childrenNumber = 0, BaseNode* const* _v_children = NULL ) : 
	GeoNode( _name, _pParent, _pWorldPosition, _pBoundingVolume, _childrenNumber, _v_children )
{
	timeDelta = 0.0f;

	pParticleSystem = _pParticleSystem;
	pParticleSystem->setWorldPosition( pWorldPosition );

	updateOnRender = _updateOnRender;

	updateWorldPositionLinks();
};


ParticleSystemNode::ParticleSystemNode( const char* _name, BaseNode* _pParent, WorldPosition* _pWorldPosition, ParticleSystem_v4* _pParticleSystem, bool _updateOnRender, BoundingVolume* _pBoundingVolume, USHORT _childrenNumber, char* const* _v_childrenNames ) : 
	GeoNode( _name, _pParent, _pWorldPosition, _pBoundingVolume, _childrenNumber, _v_childrenNames )
{
	timeDelta = 0.0f;

	pParticleSystem = _pParticleSystem;
	pParticleSystem->setWorldPosition( pWorldPosition );

	updateOnRender = _updateOnRender;

	updateWorldPositionLinks();
};


ParticleSystemNode::ParticleSystemNode( const string &_name, BaseNode* _pParent, WorldPosition* _pWorldPosition, ParticleSystem_v4* _pParticleSystem, bool _updateOnRender, BoundingVolume* _pBoundingVolume, USHORT _childrenNumber, string* const _v_childrenNames ) : 
	GeoNode( _name, _pParent, _pWorldPosition, _pBoundingVolume, _childrenNumber, _v_childrenNames )
{
	timeDelta = 0.0f;

	pParticleSystem = _pParticleSystem;
	pParticleSystem->setWorldPosition( pWorldPosition );

	updateOnRender = _updateOnRender;

	updateWorldPositionLinks();
};


ParticleSystemNode::~ParticleSystemNode()
{
	SAFE_DELETE( pParticleSystem );
};



int ParticleSystemNode::updateNode( TIMETYPE _timeDelta )
{
	if ( updateOnRender )
	{
		timeDelta = _timeDelta;
		return 0;
	}

	return pParticleSystem->update( timeDelta );
};


int ParticleSystemNode::render( RenderVisitor* _pRenderVisitor )
{
	if ( updateOnRender )
		if ( ( int res = pParticleSystem->update( timeDelta, _pRenderVisitor->getCamera() ) ) < 0 )
			return res;

	return _pRenderVisitor->render( pParticleSystem );
};


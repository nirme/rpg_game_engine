#pragma once

#ifndef _PARTICLE_SYSTEM_NODE
#define _PARTICLE_SYSTEM_NODE


#include "utility.h"
#include "Core_Defines.h"

#include "Geo_Node.h"
#include "Core_Graphic.h"
#include "Core_Graphic_Resource.h"
#include "Core_Graphic_Object.h"

#include "Camera_Node.h"
#include "Light_Node.h"


class ParticleSystemNode : public GeoNode
{
private:
	bool psSelfMoving;
	ParticleSystem_v4* particleSystem;

public:
	ParticleSystemNode(string const & _name, BaseNode* _parentNode = NULL) : GeoNode(_name, _parentNode) {};
	ParticleSystemNode(string const & _name, WorldPosition* _pWorldPosition = NULL, BoundingSphere* _volume = NULL, BaseNode* _parentNode = NULL) : (_name, _pWorldPosition, _volume, _parentNode)
	{
	};


};

#endif //_PARTICLE_SYSTEM_NODE
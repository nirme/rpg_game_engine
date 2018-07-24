#pragma once

#ifndef _CORE_SCENE_GRAPH
#define _CORE_SCENE_GRAPH


#include "Core_Defines.h"
#include "Core_World_Position_v3.h"
#include "Core_Bounding_Volume.h"

#include "Base_Node.h"
#include "Base_Nodes_Loading_Functions.h"

#include "Core_Graphic.h"
#include "Core_Sound.h"

class SceneGraph
{
protected:
	BaseNode* sceneRoot;
	BaseNode* actorsRoot;
	BaseNode* dynamicObjectsRoot;
	CameraNode* cameraNode;




};



#endif //_CORE_SCENE_GRAPH
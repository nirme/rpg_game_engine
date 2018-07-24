#pragma once

#ifndef _LIGHT_NODE
#define _LIGHT_NODE


#include "utility.h"
#include "Core_Defines.h"

#include "Geo_Node.h"
#include "Core_Graphic.h"
#include "Core_Light.h"



class LightNode : public GeoNode
{
protected:
	LightResource* pLightResource;
	LIGHTSTRUC* pLight;

	DWORD positionUpdateTimestamp;

	static float minimalUsableDistancePointLight;
	static float minimalUsableDistanceSpotLight;

	bool lightTypeSupported;
	static list<LIGHTSTRUC*> l_lights;
	list<LIGHTSTRUC*>::iterator it_lightIterator;

	virtual int setNodeLight();

public:
	LightNode(const string _name, LightResource* _lightResource, WorldPosition* _pWorldPosition = NULL, BaseNode* _parentNode = NULL);
	~LightNode();
	virtual int updateNode();
	bool lightLoaded();
	bool lightSupported();
	static list<LIGHTSTRUC*>* getLightList();
	LIGHTSTRUC* getLightStruct();
	DWORD getLightShaderData(D3DXVECTOR4* vecArray, DWORD size);
	int getLightType();
	D3DXVECTOR3* getLightPosition();
};


#endif //_LIGHT_NODE
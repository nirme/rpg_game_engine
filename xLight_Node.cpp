#include "Light_Node.h"



float LightNode::minimalUsableDistancePointLight;
float LightNode::minimalUsableDistanceSpotLight;
list<LIGHTSTRUC*> LightNode::l_lights;



int LightNode::setNodeLight()
{
	if (pLight || !pLightResource->isLoaded())
		return 1;
	pLight = new LIGHTSTRUC;
	memcpy(pLight, pLightResource->getResourcePointer(), sizeof(LIGHTSTRUC));

	volume->setVolumeRadius(pLight->_range);

	UINT type = pLightResource->getLightType();
	if (type == LIGHT_SPOT)
	{
		pLight->_spot_direction.w = 0.0f;
		D3DXVec3TransformNormal((D3DXVECTOR3*)&(pLight->_spot_direction), &(LightResource::getBaseDirection()), pWorldPosition->getWorldPosition());
	}
	else if (type == LIGHT_DIRECTIONAL)
		D3DXVec3TransformNormal((D3DXVECTOR3*)&(pLight->_position), &(LightResource::getBaseDirection()), pWorldPosition->getWorldPosition());
	else
		pLight->_position = D3DXVECTOR4(*(pWorldPosition->getWorldPositionVector()), 1.0f);

	positionUpdateTimestamp = pWorldPosition->getWorldPositionTimestamp();

	DWORD slt = Graphic::getSupportedLightTypes();
	if (slt >= 3 || (slt == 2 && type != LIGHT_SPOT))
	{
		it_lightIterator = l_lights.insert(l_lights.end(), pLight);
		lightTypeSupported = true;
	}

	return 0;
};


LightNode::LightNode(const string _name, LightResource* _lightResource, WorldPosition* _pWorldPosition, BaseNode* _parentNode ) : GeoNode(_name, _pWorldPosition, NULL, _parentNode), pLightResource(_lightResource)
{
	positionUpdateTimestamp = 0;
	lightTypeSupported = false;
	if (!pLightResource)
		pLightResource = LightResource::getResource(0);
	setNodeLight();
};

LightNode::~LightNode()
{
	if (pLight && lightTypeSupported)
	{
		lightTypeSupported = false;
		l_lights.erase(it_lightIterator);
	}

	SAFE_DELETE(pLight);
	SAFE_RELEASE(pLightResource);
};

int LightNode::updateNode()
{
	if (!pLight)
	{
		setNodeLight();
		return 0;
	}

	if (positionUpdateTimestamp == pWorldPosition->getWorldPositionTimestamp())
		return 0;

	UINT type = pLightResource->getLightType();
	if (type == LIGHT_SPOT)
		D3DXVec3TransformNormal((D3DXVECTOR3*)&(pLight->_spot_direction), &(LightResource::getBaseDirection()), pWorldPosition->getWorldPosition());
	else if (type == LIGHT_DIRECTIONAL)
		D3DXVec3TransformNormal((D3DXVECTOR3*)&(pLight->_position), &(LightResource::getBaseDirection()), pWorldPosition->getWorldPosition());
	else
		pLight->_position = D3DXVECTOR4(*(pWorldPosition->getWorldPositionVector()), 1.0f);

	positionUpdateTimestamp = pWorldPosition->getWorldPositionTimestamp();
	return 0;
};

bool LightNode::lightLoaded()
{
	if (!pLight)
		setNodeLight();
	return pLight ? true : false;
};

bool LightNode::lightSupported()
{
	return lightTypeSupported;
};

list<LIGHTSTRUC*>* LightNode::getLightList()
{
	return &l_lights;
};

LIGHTSTRUC* LightNode::getLightStruct()
{
	return pLight;
};

DWORD LightNode::getLightShaderData(D3DXVECTOR4* vecArray, DWORD size)
{
	if (!pLight && setNodeLight())
		return 1;
	if (!lightTypeSupported)
		return 2;

	memcpy(vecArray, pLight->light_full, size);
	return 0;
};

int LightNode::getLightType()
{
	if (!pLight && setNodeLight())
		return -1;

	return pLightResource->getLightType();
};

D3DXVECTOR3* LightNode::getLightPosition()
{
	if (!pLight && setNodeLight())
		return NULL;
	return (D3DXVECTOR3*) &(pLight->_position);
};


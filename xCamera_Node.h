#pragma once

#ifndef _CAMERA_NODE
#define _CAMERA_NODE


#include "utility.h"
#include "Core_Defines.h"

#include "Geo_Node.h"
#include "Core_Camera.h"



class CameraNode : public GeoNode
{
public:
	typedef struct FRUSTUM
	{
		union
		{
			struct
			{
				D3DXPLANE planeNear;
				D3DXPLANE planeFar;
				D3DXPLANE planeRight;
				D3DXPLANE planeDown;
				D3DXPLANE planeLeft;
				D3DXPLANE planeUp;
			};
			struct
			{
				D3DXPLANE _p[6];
			};
		};
	};

protected:
	Camera* pCamera;
	float farViewDistance;
	float nearViewDistance;

	bool useFarPlane;
	FRUSTUM normalizedFrustum;
	FRUSTUM cashedFrustum;

	float cashedFOVVertical;
	float cashedFOVHorizontal;

	bool lookAtAttachedPosition;
	GeoNode* pAttachedNode;

	void setupFrustum(float _farViewDistance, float _nearViewDistance);

public:
	CameraNode(	string _name,
				Camera* _pCamera,
				float _farViewDistance = FLT_MAX,
				float _nearViewDistance = 0.0f,
				WorldPosition* _pWorldPosition = NULL,
				BoundingSphere* _volume = NULL,
				BaseNode* _parentNode = NULL);

	virtual ~CameraNode()
	{
		delete pCamera;
	};

	virtual void setWorldPosition(WorldPosition* _pWorldPosition);

	virtual int updateNode();

	float getFarViewDistance();
	void setFarViewDistance(float _farViewDistance = FLT_MAX);
	float getNearViewDistance();
	void setNearViewDistance(float _nearViewDistance = 0.0f);
	void setViewDistance(float _farViewDistance = FLT_MAX, float _nearViewDistance = 0.0f);

	Camera* getCamera();
	void setCamera(Camera* _pCamera);

	bool setLookAtAttachedPosition(bool _lookAt, GeoNode* _pAttachedNode = NULL);
	bool getLookAtAttachedPosition();

//SETUP FUNCTION CHECKING BASIC VALUME CONTAINMENT

	int nodeInsideFrustum(GeoNode* _node);
};



#endif //_CAMERA_NODE

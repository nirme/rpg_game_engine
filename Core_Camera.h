#pragma once

#ifndef _CORE_CAMERA
#define _CORE_CAMERA

#include "utility.h"
#include "Core_World_Position_v3.h"
#include "Core_Bounding_Volume.h"
#include "Core_Logger.h"





class Camera
{
protected:
	float FOVVertical;
	float FOVHorizontal;
	float aspectRatio;
	float zNearViewPlane;
	float zFarViewPlane;

	float cashedXScale;
	float cashedYScale;

	D3DXVECTOR3* positionVector;
	D3DXVECTOR3* lookAtVector;
	D3DXVECTOR3 normalizedFacingVector;
	D3DXVECTOR3 lookAtPointVector;
	D3DXVECTOR3 normalizedUpDirection;
	D3DXVECTOR3 normalizedRightDirection;

	D3DXMATRIX viewMatrix;
	D3DXMATRIX viewMatrixInverted;
	D3DXMATRIX billboardingSphericalMatrix;
	D3DXMATRIX billboardingCylindricalMatrix;
	D3DXMATRIX projectionMatrix;
	D3DXMATRIX viewProjectionMatrix;

	WorldPosition* pCameraPosition;

	DWORD timestamp;



	D3DXMATRIX* setProjectionMatrix(float _xScale, float _yScale, float zNVPlane, float zFVPlane);

public:

	Camera(	WorldPosition* _pCameraPosition,
			D3DXVECTOR3 _upDirection,
			float _horizontalFOV,
			float _aspectRatio,
			float _zNearViewPlane = 1.0f,
			float _zFarViewPlane = 10000.0f);

	~Camera(){};

	D3DXMATRIX* updateViewMatrix();

	D3DXMATRIX* setProjectionMatrixFOVLH(float _verticalFOV, float _horizontalFOV, float _zNearViewPlane = 1.0f, float _zFarViewPlane = 10000.0f);
	D3DXMATRIX* setProjectionMatrixFOVHorizontalLH(float _horizontalFOV, float _aspectRatio, float _zNearViewPlane = 1.0f, float _zFarViewPlane = 10000.0f);
	D3DXMATRIX* setProjectionMatrixFOVVerticalLH(float _verticalFOV, float _aspectRatio, float _zNearViewPlane = 1.0f, float _zFarViewPlane = 10000.0f);

	WorldPosition* getCameraWorldPosition();
	void setCameraWorldPosition(WorldPosition* _position);

	D3DXMATRIX* getViewMatrix();
	D3DXMATRIX* getViewInverseMatrix();
	D3DXMATRIX* getBillboardingSphericalMatrix();
	D3DXMATRIX* getBillboardingCylindricalMatrix();
	D3DXMATRIX* getProjectionMatrix();
	D3DXMATRIX* getViewProjectionMatrix();

	float getFOVVertical();
	float getFOVVerticalDegrees();
	float getFOVHorizontal();
	float getFOVHorizontalDegrees();
	void setFOVVertical(float _FOVVertical);
	void setFOVHorizontal(float _FOVHorizontal);
	float getAspectRatio();

	float getNearViewPlaneDistance();
	void setNearViewPlaneDistance(float _zNearViewPlane);
	float getFarViewPlaneDistance();
	void setFarViewPlaneDistance(float _zFarViewPlane);
	void setupLookAtVector(D3DXVECTOR3* _lookAtVec = NULL);


	D3DXVECTOR3* getPosition();
	D3DXVECTOR3* getNormalizedFacingVector();
	D3DXVECTOR3* getNormalizedUpDirection();
	D3DXVECTOR3* getNormalizedRightDirection();
	void setNormalizedUpDirection(D3DXVECTOR3 _normalizedUpDirection);

	inline DWORD getTimestamp()
	{
		return timestamp;
	};
};



#endif //_CORE_CAMERA
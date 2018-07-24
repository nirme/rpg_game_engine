#include "Core_Camera.h"



D3DXMATRIX* Camera::setProjectionMatrix(float _xScale, float _yScale, float zNVPlane, float zFVPlane)
{
	cashedXScale = _xScale;
	cashedYScale = _yScale;
	zNearViewPlane = zNVPlane;
	zFarViewPlane = zFVPlane;

	ZeroMemory(&projectionMatrix, sizeof(D3DXMATRIX));

	projectionMatrix._11 = cashedXScale;
	projectionMatrix._22 = cashedYScale;
	projectionMatrix._33 = zFarViewPlane / (zFarViewPlane - zNearViewPlane);
	projectionMatrix._34 = 1.0f;
	projectionMatrix._43 = -1.0f * zNearViewPlane * zFarViewPlane / (zFarViewPlane - zNearViewPlane);

	viewProjectionMatrix = viewMatrix * projectionMatrix;

	zNearViewPlane = zNVPlane;
	zFarViewPlane = zFVPlane;

	timestamp++;

	return &projectionMatrix;
};


Camera::Camera(WorldPosition* _pCameraPosition, D3DXVECTOR3 _upDirection, float _horizontalFOV, float _aspectRatio, float _zNearViewPlane, float _zFarViewPlane) : positionVector(NULL), lookAtVector(NULL), pCameraPosition(_pCameraPosition), timestamp(0)
{
	ZeroMemory(&normalizedFacingVector, sizeof(D3DXVECTOR3));
	ZeroMemory(&lookAtPointVector, sizeof(D3DXVECTOR3));
	D3DXVec3Normalize(&normalizedUpDirection, &_upDirection);
	ZeroMemory(&viewMatrix, sizeof(D3DXMATRIX));
	ZeroMemory(&viewMatrixInverted, sizeof(D3DXMATRIX));
	ZeroMemory(&billboardingSphericalMatrix, sizeof(D3DXMATRIX));
	ZeroMemory(&projectionMatrix, sizeof(D3DXMATRIX));
	ZeroMemory(&viewProjectionMatrix, sizeof(D3DXMATRIX));


	updateViewMatrix();
	setProjectionMatrixFOVHorizontalLH(_horizontalFOV, _aspectRatio, _zNearViewPlane, _zFarViewPlane);
};


D3DXMATRIX* Camera::updateViewMatrix()
{
	positionVector = pCameraPosition->getWorldPositionVector();
	if (!lookAtVector)
	{
		D3DXMATRIX* wm = pCameraPosition->getWorldPosition();
		// instead of transforming normals
		normalizedFacingVector = D3DXVECTOR3(wm->_31, wm->_32, wm->_33);
		normalizedUpDirection = D3DXVECTOR3(wm->_21, wm->_22, wm->_23);
	}
	else
		D3DXVec3Normalize(&normalizedFacingVector, &(*lookAtVector - *positionVector));

	D3DXVec3Normalize(&normalizedRightDirection, D3DXVec3Cross(&normalizedRightDirection, &normalizedUpDirection, &normalizedFacingVector));
	D3DXVECTOR3 yaxis;
	D3DXVec3Normalize(&yaxis, D3DXVec3Cross(&yaxis, &normalizedFacingVector, &normalizedRightDirection));

	/* Setup view matrix */
	viewMatrix._11 = normalizedRightDirection.x;
	viewMatrix._12 = yaxis.x;
	viewMatrix._13 = normalizedFacingVector.x;
	viewMatrix._14 = 0.0f;
	viewMatrix._21 = normalizedRightDirection.y;
	viewMatrix._22 = yaxis.y;
	viewMatrix._23 = normalizedFacingVector.y;
	viewMatrix._24 = 0.0f;
	viewMatrix._31 = normalizedRightDirection.z;
	viewMatrix._32 = yaxis.z;
	viewMatrix._33 = normalizedFacingVector.z;
	viewMatrix._34 = 0.0f;
	viewMatrix._41 = -1.0f * D3DXVec3Dot(&xaxis, positionVector);
	viewMatrix._42 = -1.0f * D3DXVec3Dot(&yaxis, positionVector);
	viewMatrix._43 = -1.0f * D3DXVec3Dot(&normalizedFacingVector, positionVector);
	viewMatrix._44 = 1.0f;



	// Setup inverted view matrix 
	viewMatrixInverted._11 = normalizedRightDirection.x;
	viewMatrixInverted._12 = normalizedRightDirection.y;
	viewMatrixInverted._13 = normalizedRightDirection.z;
	viewMatrixInverted._14 = 0.0f;
	viewMatrixInverted._21 = yaxis.x;
	viewMatrixInverted._22 = yaxis.y;
	viewMatrixInverted._23 = yaxis.z;
	viewMatrixInverted._24 = 0.0f;
	viewMatrixInverted._31 = normalizedFacingVector.x;
	viewMatrixInverted._32 = normalizedFacingVector.y;
	viewMatrixInverted._33 = normalizedFacingVector.z;
	viewMatrixInverted._34 = 0.0f;
	viewMatrixInverted._41 = positionVector->x;
	viewMatrixInverted._42 = positionVector->y;
	viewMatrixInverted._43 = positionVector->z;
	viewMatrixInverted._44 = 1.0f;


	// Recalculate matrix components - don't want billboarding matrix to rotate models by z axis
	D3DXVECTOR3 xaxis;
	D3DXVec3Normalize(&xaxis, D3DXVec3Cross(&normalizedRightDirection, &D3DXVECTOR3(0.0f, 1.0f, 0.0f), &normalizedFacingVector));
	D3DXVec3Normalize(&yaxis, D3DXVec3Cross(&yaxis, &normalizedFacingVector, &xaxis));

	// Setup inverted view matrix 
	billboardingSphericalMatrix._11 = xaxis.x;
	billboardingSphericalMatrix._12 = xaxis.y;
	billboardingSphericalMatrix._13 = xaxis.z;
	billboardingSphericalMatrix._14 = 0.0f;
	billboardingSphericalMatrix._21 = yaxis.x;
	billboardingSphericalMatrix._22 = yaxis.y;
	billboardingSphericalMatrix._23 = yaxis.z;
	billboardingSphericalMatrix._24 = 0.0f;
	billboardingSphericalMatrix._31 = normalizedFacingVector.x;
	billboardingSphericalMatrix._32 = normalizedFacingVector.y;
	billboardingSphericalMatrix._33 = normalizedFacingVector.z;
	billboardingSphericalMatrix._34 = 0.0f;
	billboardingSphericalMatrix._41 = positionVector->x;
	billboardingSphericalMatrix._42 = positionVector->y;
	billboardingSphericalMatrix._43 = positionVector->z;
	billboardingSphericalMatrix._44 = 1.0f;



	// Setup inverted view matrix 
	billboardingCylindricalMatrix._11 = xaxis.x;
	billboardingCylindricalMatrix._12 = xaxis.y;
	billboardingCylindricalMatrix._13 = xaxis.z;
	billboardingCylindricalMatrix._14 = 0.0f;
	billboardingCylindricalMatrix._21 = 0.0f;
	billboardingCylindricalMatrix._22 = 1.0f;
	billboardingCylindricalMatrix._23 = 0.0f;
	billboardingCylindricalMatrix._24 = 0.0f;
	billboardingCylindricalMatrix._31 = normalizedFacingVector.x;
	billboardingCylindricalMatrix._32 = normalizedFacingVector.y;
	billboardingCylindricalMatrix._33 = normalizedFacingVector.z;
	billboardingCylindricalMatrix._34 = 0.0f;
	billboardingCylindricalMatrix._41 = positionVector->x;
	billboardingCylindricalMatrix._42 = positionVector->y;
	billboardingCylindricalMatrix._43 = positionVector->z;
	billboardingCylindricalMatrix._44 = 1.0f;


	/* Setup view X projection matrix */
	viewProjectionMatrix = viewMatrix * projectionMatrix;

	timestamp++;

	return &viewMatrix;
};

D3DXMATRIX* Camera::setProjectionMatrixFOVLH(float _verticalFOV, float _horizontalFOV, float _zNearViewPlane, float _zFarViewPlane)
{
	FOVVertical = _verticalFOV;
	FOVHorizontal = _horizontalFOV;
	aspectRatio = tan(_horizontalFOV / 2) / tan(_verticalFOV / 2);

	float xScale = 1.0f / tan(_horizontalFOV / 2.0f);
	float yScale = 1.0f / tan(_verticalFOV / 2.0f);

	return setProjectionMatrix(xScale, yScale, _zNearViewPlane, _zFarViewPlane);
};

D3DXMATRIX* Camera::setProjectionMatrixFOVHorizontalLH(float _horizontalFOV, float _aspectRatio, float _zNearViewPlane, float _zFarViewPlane)
{
	float xScale = 1.0f / tan(_horizontalFOV / 2.0f);
	float yScale = xScale * _aspectRatio;

	FOVVertical = 2.0f * atan(1 / (1 / _aspectRatio * tan(xScale)));
	FOVHorizontal = _horizontalFOV;
	aspectRatio = _aspectRatio;

	return setProjectionMatrix(xScale, yScale, _zNearViewPlane, _zFarViewPlane);
};

D3DXMATRIX* Camera::setProjectionMatrixFOVVerticalLH(float _verticalFOV, float _aspectRatio, float _zNearViewPlane, float _zFarViewPlane)
{
	float yScale = 1.0f / tan(_verticalFOV / 2.0f);
	float xScale = yScale / _aspectRatio;

	FOVVertical = _verticalFOV;
	FOVHorizontal = 2.0f * atan(1 / (_aspectRatio * tan(yScale)));
	aspectRatio = _aspectRatio;

	return setProjectionMatrix(xScale, yScale, _zNearViewPlane, _zFarViewPlane);
};


WorldPosition* Camera::getCameraWorldPosition()
{
	return pCameraPosition;
};

void Camera::setCameraWorldPosition(WorldPosition* _position)
{
	pCameraPosition = _position;
};

D3DXMATRIX* Camera::getViewMatrix()
{
	return &viewMatrix;
};

D3DXMATRIX* Camera::getViewInverseMatrix()
{
	return &viewMatrixInverted;
};

D3DXMATRIX* Camera::getBillboardingSphericalMatrix()
{
	return &billboardingSphericalMatrix;
};

D3DXMATRIX* Camera::getBillboardingCylindricalMatrix()
{
	return &billboardingCylindricalMatrix;
};

D3DXMATRIX* Camera::getProjectionMatrix()
{
	return &projectionMatrix;
};

D3DXMATRIX* Camera::getViewProjectionMatrix()
{
	return &viewProjectionMatrix;
};

float Camera::getFOVVertical()
{
	return FOVVertical;
};

float Camera::getFOVVerticalDegrees()
{
	return D3DXToDegree(FOVVertical);
};


float Camera::getFOVHorizontal()
{
	return FOVHorizontal;
};

float Camera::getFOVHorizontalDegrees()
{
	return D3DXToDegree(FOVHorizontal);
};

void Camera::setFOVVertical(float _FOVVertical)
{
	setProjectionMatrix(cashedXScale, 1.0f / tan(_FOVVertical / 2.0f), zNearViewPlane, zFarViewPlane);
};

void Camera::setFOVHorizontal(float _FOVHorizontal)
{
	setProjectionMatrix(1.0f / tan(_FOVHorizontal / 2.0f), cashedYScale, zNearViewPlane, zFarViewPlane);
};

float Camera::getAspectRatio()
{
	return aspectRatio;
};

float Camera::getNearViewPlaneDistance()
{
	return zNearViewPlane;
};

void Camera::setNearViewPlaneDistance(float _zNearViewPlane)
{
	zNearViewPlane = _zNearViewPlane;
	setProjectionMatrix(cashedXScale, cashedYScale, zNearViewPlane, zFarViewPlane);
};

float Camera::getFarViewPlaneDistance()
{
	return zFarViewPlane;
};

void Camera::setFarViewPlaneDistance(float _zFarViewPlane)
{
	zFarViewPlane = _zFarViewPlane;
	setProjectionMatrix(cashedXScale, cashedYScale, zNearViewPlane, zFarViewPlane);
};

void Camera::setupLookAtVector(D3DXVECTOR3* _lookAtVec)
{
	lookAtVector = _lookAtVec;
};


D3DXVECTOR3* Camera::getPosition()
{
	return pCameraPosition->getWorldPositionVector();
};

D3DXVECTOR3* Camera::getNormalizedFacingVector()
{
	return &normalizedFacingVector;
};

D3DXVECTOR3* Camera::getNormalizedUpDirection()
{
	return &normalizedUpDirection;
};

D3DXVECTOR3* getNormalizedRightDirection()
{
	return &normalizedRightDirection;
};

void Camera::setNormalizedUpDirection(D3DXVECTOR3 _normalizedUpDirection)
{
	normalizedUpDirection = _normalizedUpDirection;
};

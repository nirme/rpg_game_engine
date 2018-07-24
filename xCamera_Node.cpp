#include "Camera_Node.h"



CameraNode::CameraNode(string _name, Camera* _pCamera, float _farViewDistance, float _nearViewDistance, WorldPosition* _pWorldPosition, BoundingSphere* _volume, BaseNode* _parentNode) : GeoNode(_name, _pWorldPosition, _volume, _parentNode)
{
	pCamera = _pCamera;
	if (_pWorldPosition)
	{
		pCamera->setCameraWorldPosition(_pWorldPosition);
		pWorldPosition = _pWorldPosition;
	}
	else
	{
		pWorldPosition = pCamera->getCameraWorldPosition();
	}
	farViewDistance = _farViewDistance;
	nearViewDistance = _nearViewDistance;

	useFarPlane = false;
	ZeroMemory(&normalizedFrustum, sizeof(FRUSTUM));
	ZeroMemory(&cashedFrustum, sizeof(FRUSTUM));
	cashedFOVVertical = 0.0f;
	cashedFOVHorizontal = 0.0f;

	pAttachedNode = NULL;
	lookAtAttachedPosition = false;

	if (pCamera)
		setupFrustum(farViewDistance, nearViewDistance);
};


void CameraNode::setupFrustum(float _farViewDistance, float _nearViewDistance)
{
	farViewDistance = 100.0f;
	if (_farViewDistance >= FLT_MAX)
		useFarPlane = false;
	else
	{
		farViewDistance = _farViewDistance;
		useFarPlane = true;
	}

	nearViewDistance = 0.0f;
	if (!(_nearViewDistance < 0.0f))
		nearViewDistance = _nearViewDistance;

	cashedFOVVertical = pCamera->getFOVVertical();
	cashedFOVHorizontal = pCamera->getFOVHorizontal();
	float vertFOV = cashedFOVVertical + EPSILON;
	float horizFOV = cashedFOVHorizontal + EPSILON;

	D3DXVECTOR3 cameraPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 viewVector = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	D3DXVECTOR3 maxA, maxB, maxC, maxD;
	maxA.x = maxB.x = maxC.x = maxD.x = tan(horizFOV / 2) * farViewDistance;
	maxA.y = maxB.y = maxC.y = maxD.y = tan(vertFOV / 2) * farViewDistance;
	maxA.z = maxB.z = maxC.z = maxD.z = farViewDistance;
	maxB.y *= -1;
	maxC.x *= -1;
	maxC.y *= -1;
	maxD.x *= -1;

	D3DXPlaneFromPointNormal(&(normalizedFrustum.planeFar), &(D3DXVECTOR3(0.0f, 0.0f, farViewDistance)), &(D3DXVECTOR3(0.0f, 0.0f, -1.0f)));
	D3DXPlaneFromPointNormal(&(normalizedFrustum.planeNear), &(D3DXVECTOR3(0.0f, 0.0f, nearViewDistance)), &(D3DXVECTOR3(0.0f, 0.0f, 1.0f)));
	D3DXPlaneFromPoints(&(normalizedFrustum.planeRight), &maxA, &cameraPos, &maxB);
	D3DXPlaneFromPoints(&(normalizedFrustum.planeDown), &maxB, &cameraPos, &maxC);
	D3DXPlaneFromPoints(&(normalizedFrustum.planeLeft), &maxC, &cameraPos, &maxD);
	D3DXPlaneFromPoints(&(normalizedFrustum.planeUp), &maxD, &cameraPos, &maxA);

	D3DXPlaneNormalize(&(normalizedFrustum.planeNear), &(normalizedFrustum.planeNear));
	D3DXPlaneNormalize(&(normalizedFrustum.planeFar), &(normalizedFrustum.planeFar));
	D3DXPlaneNormalize(&(normalizedFrustum.planeRight), &(normalizedFrustum.planeRight));
	D3DXPlaneNormalize(&(normalizedFrustum.planeDown), &(normalizedFrustum.planeDown));
	D3DXPlaneNormalize(&(normalizedFrustum.planeLeft), &(normalizedFrustum.planeLeft));
	D3DXPlaneNormalize(&(normalizedFrustum.planeUp), &(normalizedFrustum.planeUp));
};

void setWorldPosition(WorldPosition* _pWorldPosition)
{
	SAFE_DELETE(pWorldPosition);
	pWorldPosition = _pWorldPosition;
	if (!pWorldPosition)
		pWorldPosition = new WorldPosition;
	volume->setPosition(pWorldPosition);

	if (GeoNode* parent = dynamic_cast<GeoNode*>(getParent()))
	{
		pWorldPosition->attachTo(parent->getWorldPosition());
	}

	pCamera->setCameraWorldPosition(pWorldPosition);
};

int CameraNode::updateNode()
{
	if (cashedFOVVertical != pCamera->getFOVVertical() || cashedFOVHorizontal != pCamera->getFOVHorizontal())
	{
		setupFrustum(farViewDistance, nearViewDistance);
	}

	pCamera->updateViewMatrix();

	D3DXMATRIX viewInvTrans;
	D3DXMatrixTranspose(&viewInvTrans, pCamera->getViewInverseMatrix());
	D3DXPlaneTransformArray(cashedFrustum._p, sizeof(D3DXPLANE), normalizedFrustum._p, sizeof(D3DXPLANE), &viewInvTrans, 6);

	return 0;
};

float CameraNode::getFarViewDistance()
{
	return farViewDistance;
};

void CameraNode::setFarViewDistance(float _farViewDistance)
{
	setupFrustum(_farViewDistance, nearViewDistance);
};

float CameraNode::getNearViewDistance()
{
	return nearViewDistance;
};

void CameraNode::setNearViewDistance(float _nearViewDistance)
{
	setupFrustum(farViewDistance, _nearViewDistance);
};

void CameraNode::setViewDistance(float _farViewDistance, float _nearViewDistance)
{
	setupFrustum(_farViewDistance, _nearViewDistance);
};

Camera* CameraNode::getCamera()
{
	return pCamera;
};

void CameraNode::setCamera(Camera* _pCamera)
{
	if (pCamera != _pCamera)
		setupFrustum(farViewDistance, nearViewDistance);
	pCamera = _pCamera;
};

bool CameraNode::setLookAtAttachedPosition(bool _lookAt, GeoNode* _pAttachedNode)
{
	pAttachedNode = _pAttachedNode;

	if (_lookAt && pAttachedNode)
		lookAtAttachedPosition = true;
	else
		lookAtAttachedPosition = false;

	if (lookAtAttachedPosition)
		pCamera->setupLookAtVector(_pAttachedNode->getWorldPosition()->getWorldPositionVector());
	else
		pCamera->setupLookAtVector(NULL);

	return lookAtAttachedPosition;
};

bool CameraNode::getLookAtAttachedPosition()
{
	return lookAtAttachedPosition;
};

int CameraNode::nodeInsideFrustum(GeoNode* _node)
{
	BoundingSphere* vol = NULL;

	if (!(vol = _node->getBoundingVolume()))
		return -1;
	if (!vol->getPosition())
		return -2;

	D3DXVECTOR3 volpos = vol->getVolumePosition();
	float radius = vol->getVolumeRadius() * -1.0f;
	if (radius >= D3DXPlaneDotCoord(&(cashedFrustum.planeNear), &volpos))
		return 0;

	if (useFarPlane && radius >= D3DXPlaneDotCoord(&(cashedFrustum.planeFar), &volpos))
		return 0;

	if (radius >= D3DXPlaneDotCoord(&(cashedFrustum.planeRight), &volpos))
		return 0;

	if (radius >= D3DXPlaneDotCoord(&(cashedFrustum.planeLeft), &volpos))
		return 0;

	if (radius >= D3DXPlaneDotCoord(&(cashedFrustum.planeUp), &volpos))
		return 0;

	if (radius >= D3DXPlaneDotCoord(&(cashedFrustum.planeDown), &volpos))
		return 0;

	return 1;
};

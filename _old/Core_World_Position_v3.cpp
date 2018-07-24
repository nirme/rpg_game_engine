#include "Core_World_Position_v3.h"



WorldPosition::WorldPosition()
{
	ZeroMemory(&v_rotation, sizeof(D3DXVECTOR3));
	scale = 1.0f;
	ZeroMemory(&v_position, sizeof(D3DXVECTOR3));

	D3DXMatrixIdentity(&mx_rotation);
	D3DXMatrixIdentity(&mx_scale);
	D3DXMatrixIdentity(&mx_position);

	b_cashedLocalActual = false;
	D3DXMatrixIdentity(&mx_cashedLocal);

	dw_attachedPositionLastUpdateTimestamp = 0;
	wp_attachedPosition = NULL;

	dw_worldPositionLastUpdateTimestamp = 1;

	b_cashedWorldActual = false;
	D3DXMatrixIdentity(&mx_cashedWorld);

	b_cashedFacingActual = false;
	ZeroMemory(&v_cashedFacing, sizeof(D3DXVECTOR3));


	b_cashedWorldNoRotationActual = false;
	D3DXMatrixIdentity(&mx_cashedWorldNoRotation);

	b_bilboarding = false;
	mx_viewInverse = NULL;
};

WorldPosition::WorldPosition( const D3DXVECTOR3* _v_rotation, float _scale, const D3DXVECTOR3* _v_position, WorldPosition* _wp_attachedPosition, const D3DXMATRIX* _mx_viewInverse, bool _b_bilboarding)
{
	if ( _v_rotation )
		v_rotation = *_v_rotation;
	else
		ZeroMemory( &v_rotation, sizeof(D3DXVECTOR3) );

	scale = _scale;

	if ( v_position )
		v_position = *_v_position;
	else
		ZeroMemory( &v_position, sizeof(D3DXVECTOR3) );

	D3DXMatrixIdentity(&mx_rotation);
	D3DXMatrixIdentity(&mx_scale);
	D3DXMatrixIdentity(&mx_position);

	b_cashedLocalActual = false;
	D3DXMatrixIdentity(&mx_cashedLocal);

	dw_attachedPositionLastUpdateTimestamp = 0;
	wp_attachedPosition = _wp_attachedPosition;

	dw_worldPositionLastUpdateTimestamp = 1;

	b_cashedWorldActual = false;
	D3DXMatrixIdentity(&mx_cashedWorld);

	b_cashedFacingActual = false;
	ZeroMemory(&v_cashedFacing, sizeof(D3DXVECTOR3));

	b_cashedWorldNoRotationActual = false;
	D3DXMatrixIdentity(&mx_cashedWorldNoRotation);

	b_bilboarding = false;
	mx_viewInverse = _mx_viewInverse;
	if (mx_viewInverse && _b_bilboarding)
		b_bilboarding = true;
};

void WorldPosition::setCashedNotActual()
{
	b_cashedLocalActual = false;
	b_cashedWorldActual = false;
	b_cashedFacingActual = false;
	b_cashedWorldNoRotationActual = false;
};

const D3DXMATRIX* WorldPosition::getWorldPosition()
{
	if (!b_bilboarding)
	{
		if (!b_cashedLocalActual)
		{
			D3DXMatrixRotationYawPitchRoll(&mx_rotation, v_rotation.y, v_rotation.x, v_rotation.z);
			D3DXMatrixScaling(&mx_scale, scale, scale, scale);
			D3DXMatrixTranslation(&mx_position, v_position.x, v_position.y, v_position.z);
			mx_cashedLocal = (mx_rotation * mx_scale) * mx_position;
			b_cashedLocalActual = true;
			if (!wp_attachedPosition)
			{
				mx_cashedWorld = mx_cashedLocal;
				b_cashedWorldActual = true;
				return &mx_cashedWorld;
			}
		}

		if (wp_attachedPosition && (wp_attachedPosition->getWorldPositionTimestamp() != dw_worldPositionLastUpdateTimestamp || !b_cashedWorldActual))
		{
			mx_cashedWorld = (*(wp_attachedPosition->getWorldPosition())) * mx_cashedLocal;
			dw_worldPositionLastUpdateTimestamp = wp_attachedPosition->getWorldPositionTimestamp();
			b_cashedWorldActual = true;
		}
	}
	else
	{
		D3DXMatrixScaling(&mx_scale, scale, scale, scale);
		D3DXMatrixTranslation(&mx_position, v_position.x, v_position.y, v_position.z);
		mx_cashedLocal = ((*mx_viewInverse) * mx_scale) * mx_position;
		b_cashedLocalActual = true;
		if (!wp_attachedPosition)
		{
			mx_cashedWorld = mx_cashedLocal;
			b_cashedWorldActual = true;
			return &mx_cashedWorld;
		}

		if (wp_attachedPosition && (wp_attachedPosition->getWorldPositionTimestamp() != dw_worldPositionLastUpdateTimestamp || !b_cashedWorldActual))
		{
			mx_cashedWorld = *(wp_attachedPosition->getWorldPosition());
			float vs = wp_attachedPosition->getWorldScale();
			mx_cashedWorld = D3DXMATRIX(vs,		0.0f,	0.0f,	0.0f,
										0.0f,	vs,		0.0f,	0.0f,
										0.0f,	0.0f,	vs,		0.0f,
										mx_cashedWorld._41, mx_cashedWorld._42, mx_cashedWorld._43, mx_cashedWorld._44)
							* mx_cashedLocal;

			dw_worldPositionLastUpdateTimestamp = wp_attachedPosition->getWorldPositionTimestamp();
			b_cashedWorldActual = true;
		}
	}

	return &mx_cashedWorld;
};

const D3DXVECTOR3* WorldPosition::getWorldPositionVector()
{
	getWorldPosition();
	D3DXVECTOR3* vec = reinterpret_cast<D3DXVECTOR3*>(&(mx_cashedWorld._41));
	return vec;
};

const D3DXMATRIX* WorldPosition::getLocalPosition()
{
	if (!b_cashedLocalActual)
	{
		D3DXMatrixScaling(&mx_scale, scale, scale, scale);
		D3DXMatrixTranslation(&mx_position, v_position.x, v_position.y, v_position.z);

		if (!b_bilboarding)
			mx_cashedLocal = ((*mx_viewInverse) * mx_scale) * mx_position;
		else
			mx_cashedLocal = (mx_rotation * mx_scale) * mx_position;

		b_cashedLocalActual = true;
	}

	return &mx_cashedLocal;
};

const D3DXMATRIX* WorldPosition::getLocalRotationMatrix()
{
	if (!b_cashedLocalActual && !b_bilboarding)
		D3DXMatrixRotationYawPitchRoll(&mx_rotation, v_rotation.y, v_rotation.x, v_rotation.z);

	return (!b_bilboarding) ? &mx_rotation : mx_viewInverse;
};

const D3DXMATRIX* WorldPosition::getLocalScaleMatrix()
{
	if (!b_cashedLocalActual)
		D3DXMatrixScaling(&mx_scale, scale, scale, scale);

	return &mx_scale;
};

const D3DXMATRIX* WorldPosition::getLocalPositionMatrix()
{
	if (!b_cashedLocalActual)
		D3DXMatrixTranslation(&mx_position, v_position.x, v_position.y, v_position.z);

	return &mx_position;
};

void WorldPosition::attachTo(WorldPosition* _parentPosition)
{
	if (wp_attachedPosition != _parentPosition)
		setCashedNotActual();
	wp_attachedPosition = _parentPosition;
};

WorldPosition* WorldPosition::getAttachedPosition()
{
	return wp_attachedPosition;
};

void WorldPosition::setViewInverse(D3DXMATRIX* _mx_viewInverse)
{
	mx_viewInverse = _mx_viewInverse;
	if (!mx_viewInverse && b_bilboarding)
	{
		setCashedNotActual();
		b_bilboarding = false;
	}
};

bool WorldPosition::setBilboarding(bool _b_bilboarding)
{
	if (!mx_viewInverse)
	{
		if (_b_bilboarding)
			setCashedNotActual();
		return b_bilboarding = false;
	};

	if (b_bilboarding != _b_bilboarding)
		setCashedNotActual();
	return b_bilboarding = _b_bilboarding;
};

bool WorldPosition::setBilboarding(bool _b_bilboarding, D3DXMATRIX* _mx_viewInverse)
{
	mx_viewInverse = _mx_viewInverse;
	if (!mx_viewInverse)
	{
		if (_b_bilboarding)
			setCashedNotActual();
		return b_bilboarding = false;
	}

	if (b_bilboarding != _b_bilboarding)
		setCashedNotActual();
	return b_bilboarding = _b_bilboarding;
};


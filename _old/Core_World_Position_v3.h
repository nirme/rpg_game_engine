#pragma once

#ifndef _CORE_WORLD_POSITION_V3
#define _CORE_WORLD_POSITION_V3

#include "utility.h"



class WorldPosition
{
	
protected:
	D3DXVECTOR3 v_rotation;
	float scale;
	D3DXVECTOR3 v_position;

	D3DXMATRIX mx_rotation;
	D3DXMATRIX mx_scale;
	D3DXMATRIX mx_position;

	bool b_cashedLocalActual;
	D3DXMATRIX mx_cashedLocal;

	bool b_cashedLocalNoRotationActual;
	D3DXMATRIX mx_cashedLocalNoRotation;

	DWORD dw_attachedPositionLastUpdateTimestamp;
	WorldPosition* wp_attachedPosition;

	DWORD dw_worldPositionLastUpdateTimestamp;

	bool b_cashedWorldActual;
	D3DXMATRIX mx_cashedWorld;


	bool b_cashedFacingActual;
	D3DXVECTOR3 v_cashedFacing;


	bool b_cashedWorldNoRotationActual;
	D3DXMATRIX mx_cashedWorldNoRotation;

	bool b_bilboarding;
	const D3DXMATRIX* mx_viewInverse;



	D3DXMATRIX* getWorldPositionNoRotation();



public:

	WorldPosition();
	WorldPosition(	D3DXVECTOR3* _v_rotation,
					float _scale,
					D3DXVECTOR3* _v_position,
					WorldPosition* _wp_attachedPosition = NULL,
					D3DXMATRIX* _mx_viewInverse = NULL,
					bool _b_bilboarding = false);

	void setCashedNotActual();

	inline DWORD getWorldPositionTimestamp()
	{
		return dw_worldPositionLastUpdateTimestamp;
	};

	const D3DXMATRIX* getWorldPosition();
	const D3DXVECTOR3* getWorldPositionVector();
	const D3DXMATRIX* getLocalPosition();

	inline D3DXVECTOR3*	getPosition()	{	return &v_position;		};
	inline float		getPositionX()	{	return v_position.x;	};
	inline float		getPositionY()	{	return v_position.y;	};
	inline float		getPositionZ()	{	return v_position.z;	};

	inline D3DXVECTOR3*	getRotation()	{	return &v_rotation;		};
	inline float		getRotationX()	{	return v_rotation.x;	};
	inline float		getRotationY()	{	return v_rotation.y;	};
	inline float		getRotationZ()	{	return v_rotation.z;	};

	inline float		getScale()		{	return scale;		};


	inline void setPosition( D3DXVECTOR3* _v_position )	{	v_position = *_v_position;		setCashedNotActual();	};
	inline void setPositionX( float _v_positionX )		{	v_position.x = _v_positionX;	setCashedNotActual();	};
	inline void setPositionY( float _v_positionY )		{	v_position.y = _v_positionY;	setCashedNotActual();	};
	inline void setPositionZ( float _v_positionZ )		{	v_position.z = _v_positionZ;	setCashedNotActual();	};

	inline void setRotation( D3DXVECTOR3* _v_rotation )	{	v_rotation = *_v_rotation;		setCashedNotActual();	};
	inline void setRotationX( float _v_rotationX )		{	v_rotation.x = _v_rotationX;	setCashedNotActual();	};
	inline void setRotationY( float _v_rotationY )		{	v_rotation.y = _v_rotationY;	setCashedNotActual();	};
	inline void setRotationZ( float _v_rotationZ )		{	v_rotation.z = _v_rotationZ;	setCashedNotActual();	};

	inline void setScale( float _scale )				{	scale = _scale;					setCashedNotActual();	};


	const D3DXMATRIX* getLocalRotationMatrix();
	const D3DXMATRIX* getLocalScaleMatrix();
	const D3DXMATRIX* getLocalPositionMatrix();
	void attachTo( WorldPosition* _parentPosition );
	WorldPosition* getAttachedPosition();

	float getWorldScale()
	{
		return wp_attachedPosition ? (scale * wp_attachedPosition->getWorldScale()) : scale;
	}


	const D3DXVECTOR3* getFacingDirection()
	{
		if (!b_cashedFacingActual)
		{
			const D3DXMATRIX* mx = getWorldPosition();
			D3DXVec3Normalize(&v_cashedFacing, &D3DXVECTOR3(mx->_31, mx->_32, mx->_33));
			b_cashedFacingActual = true;
		}
		return &v_cashedFacing;
	};


	void setViewInverse( const D3DXMATRIX* _mx_viewInverse );
	bool setBilboarding( bool _b_bilboarding );
	bool setBilboarding( bool _b_bilboarding, D3DXMATRIX* _mx_viewInverse );

};

#endif //_CORE_WORLD_POSITION_V3

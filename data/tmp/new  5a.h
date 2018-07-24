#pragma once

#ifndef _WORLD_POSITION_V4
#define _WORLD_POSITION_V4

#include "utility.h"


const D3DXVECTOR3 axisX = D3DXVECTOR3( 1.0f, 0.0f, 0.0f );
const D3DXVECTOR3 axisY = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
const D3DXVECTOR3 axisZ = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );

const D3DXVECTOR3 const * v3_facingDirectionNormalised = &axisX;


class WorldPosition;

class WorldPositionHR;


class WorldPosition
{
protected:

	float f_scale;
	float f_oneDividedByScale;
	D3DXVECTOR3 v3_position;
	D3DXQUATERNION q_rotation;

	bool b_cashedLocalTransformationActual;
	D3DXMATRIX mx_cashedLocalTransformation;
	DWORD dw_cashedLocalTransformationTimestamp;


	virtual void updateTransformationMatrix()
	{
		if ( !b_cashedLocalTransformationActual )
		{
			D3DXMatrixAffineTransformation( &mx_cashedLocalTransformation, f_scale, NULL, q_rotation, v3_position );
			b_cashedLocalTransformationActual = true;
		}
	};

public:

	WorldPosition() : f_scale(1.0f), f_oneDividedByScale(1.0f), b_cashedLocalTransformationActual(false), dw_cashedLocalTransformationTimestamp(-1)
	{
		v3_position = D3DXVECTOR3( 0.0f, 0.0f, 0.0f )
		D3DXQuaternionIdentity( &q_rotation );
		D3DXMatrixIdentity( &mx_rotation );
		D3DXMatrixIdentity( &mx_position );
		WorldPosition::updateTransformationMatrix();
	};

	
	virtual DWORD getTransformationTimestamp()
	{
		updateTransformationMatrix();
		return dw_cashedLocalTransformationTimestamp;
	};


	void setScale( float _f_scale )
	{
		f_scale = _f_scale;
		f_oneDividedByScale = 1.0f / f_scale;
		b_cashedLocalTransformationActual = false;
		++dw_cashedLocalTransformationTimestamp;
	};

	void setTranslation( const D3DXVECTOR3& _v3_position )
	{
		v3_position = _v3_position;
		b_cashedLocalTransformationActual = false;
		++dw_cashedLocalTransformationTimestamp;
	};


	void setTranslationX( const float& _f_positionX )
	{
		v3_position.x = _f_positionX;
		b_cashedLocalTransformationActual = false;
		++dw_cashedLocalTransformationTimestamp;
	};

	void setTranslationY( const float& _f_positionY )
	{
		v3_position.y = _f_positionY;
		b_cashedLocalTransformationActual = false;
		++dw_cashedLocalTransformationTimestamp;
	};

	void setTranslationZ( const float& _f_positionZ )
	{
		v3_position.z = _f_positionZ;
		b_cashedLocalTransformationActual = false;
		++dw_cashedLocalTransformationTimestamp;
	};


	void setRotation( const D3DXVECTOR3& _v3_rotation )
	{
		D3DXQuaternionNormalize( &q_rotation, D3DXQuaternionRotationYawPitchRoll( &q_rotation, _v3_rotation.y, _v3_rotation.x, _v3_rotation.z ) );
		b_cashedLocalTransformationActual = false;
		++dw_cashedLocalTransformationTimestamp;
	};

	void setRotation( const D3DXQUATERNION& _q_rotation )
	{
		D3DXQuaternionNormalize( &q_rotation, &_q_rotation );
		b_cashedLocalTransformationActual = false;
		++dw_cashedLocalTransformationTimestamp;
	};


	void translate( const D3DXVECTOR3& _v3_position )
	{
		v3_position += _v3_position;
		b_cashedLocalTransformationActual = false;
		++dw_cashedLocalTransformationTimestamp;
	};

	void translateX( const float& _f_positionX )
	{
		v3_position.x += _f_positionX;
		b_cashedLocalTransformationActual = false;
		++dw_cashedLocalTransformationTimestamp;
	};

	void translateY( const float& _f_positionY )
	{
		v3_position.y += _f_positionY;
		b_cashedLocalTransformationActual = false;
		++dw_cashedLocalTransformationTimestamp;
	};

	void translateZ( const float& _f_positionZ )
	{
		v3_position.z += _f_positionZ;
		b_cashedLocalTransformationActual = false;
		++dw_cashedLocalTransformationTimestamp;
	};


	void translateOnLocalAxis( const D3DXVECTOR3& _v3_position )
	{
		updateTransformationMatrix();
		D3DXVECTOR3 v3_tmp;
		D3DXVec3TransformNormal( &v3_tmp, &_v3_position, &mx_cashedLocalTransformation );
		v3_position += v3_tmp * f_oneDividedByScale;
		b_cashedLocalTransformationActual = false;
		++dw_cashedLocalTransformationTimestamp;
	};

	void translateXOnLocalAxis( const float& _f_positionX )
	{
		updateTransformationMatrix();
		D3DXVECTOR3 v3_tmp( mx_cashedLocalTransformation.m[0] );
		v3_position += v3_tmp * f_oneDividedByScale;
		b_cashedLocalTransformationActual = false;
		++dw_cashedLocalTransformationTimestamp;
	};

	void translateYOnLocalAxis( const float& _f_positionX )
	{
		updateTransformationMatrix();
		D3DXVECTOR3 v3_tmp( mx_cashedLocalTransformation.m[0] );
		v3_position += v3_tmp * f_oneDividedByScale;
		b_cashedLocalTransformationActual = false;
		++dw_cashedLocalTransformationTimestamp;
	};

	void translateZOnLocalAxis( const float& _f_positionX )
	{
		updateTransformationMatrix();
		D3DXVECTOR3 v3_tmp( mx_cashedLocalTransformation.m[0] );
		v3_position += v3_tmp * f_oneDividedByScale;
		b_cashedLocalTransformationActual = false;
		++dw_cashedLocalTransformationTimestamp;
	};


	void rotate( const D3DXVECTOR3& _v3_rotation )
	{
		D3DXQUATERNION q_tmp;
		D3DXQuaternionNormalize( &q_rotation, D3DXQuaternionMultiply( &q_rotation, &q_rotation, D3DXQuaternionNormalize( &q_tmp, D3DXQuaternionRotationYawPitchRoll( &q_rotation, _v3_rotation.y, _v3_rotation.x, _v3_rotation.z ) ) );
		b_cashedLocalTransformationActual = false;
		++dw_cashedLocalTransformationTimestamp;
	};

	void rotate( const D3DXQUATERNION& _q_rotation )
	{
		D3DXQuaternionMultiply( &q_rotation, &q_rotation, _q_rotation );
		b_cashedLocalTransformationActual = false;
		++dw_cashedLocalTransformationTimestamp;
	};

	void rotateX( const float& _f_rotationAngle )
	{
		D3DXQUATERNION q_tmpRotation;
		D3DXQuaternionMultiply( &q_rotation, &q_rotation, D3DXQuaternionRotationAxis( &q_tmpRotation, &axisX, _f_rotationAngle ) );
		b_cashedLocalTransformationActual = false;
		++dw_cashedLocalTransformationTimestamp;
	};

	void rotateY( const float& _f_rotationAngle )
	{
		D3DXQUATERNION q_tmpRotation;
		D3DXQuaternionMultiply( &q_rotation, &q_rotation, D3DXQuaternionRotationAxis( &q_tmpRotation, &axisY, _f_rotationAngle ) );
		b_cashedLocalTransformationActual = false;
		++dw_cashedLocalTransformationTimestamp;
	};

	void rotateZ( const float& _f_rotationAngle )
	{
		D3DXQUATERNION q_tmpRotation;
		D3DXQuaternionMultiply( &q_rotation, &q_rotation, D3DXQuaternionRotationAxis( &q_tmpRotation, &axisZ, _f_rotationAngle ) );
		b_cashedLocalTransformationActual = false;
		++dw_cashedLocalTransformationTimestamp;
	};



	const D3DXMATRIX* getLocalTransformationMatrix()
	{
		updateTransformationMatrix();
		return &mx_cashedLocalTransformation;
	};


	virtual const D3DXMATRIX* getTransformationMatrix()
	{
		updateTransformationMatrix();
		return &mx_cashedLocalTransformation;
	};


	inline float getScale()	{	return f_scale;	};

	inline D3DXVECTOR3& getPosition()	{	return D3DXVECTOR3( v3_position );	};

	inline D3DXQUATERNION& getRotation()	{	return D3DXQUATERNION( q_rotation );	};

	D3DXVECTOR3& getFacing()
	{
		D3DXVECTOR3 v3_tmp;
		D3DXVec3TransformNormal( &v3_tmp, v3_facingDirectionNormalised, getTransformationMatrix() );

		return v3_tmp;
	};
};



class WorldPositionHR
{
protected:

	bool b_isAttached;

	WorldPosition* wp_attachedPosition;

	D3DXMATRIX* mx_attachedPosition;
	DWORD* dw_attachedPositionMatrixTimestamp;

	DWORD dw_attachedPositionLastUpdateTimestamp;


	D3DXMATRIX mx_cashedTransformation;
	bool b_isCashedTransformationActual;
	DWORD dw_cashedTransformationTimestamp;


	virtual void updateTransformationMatrix()
	{
		getTransformationTimestamp();

		if ( !b_isCashedTransformationActual )
		{
			WorldPosition::updateTransformationMatrix();

			if ( b_isAttached )
			{
				if ( wp_attachedPosition )
				{
					dw_attachedPositionLastUpdateTimestamp = wp_attachedPosition->getTransformationTimestamp();
					D3DXMatrixMultiply( &mx_cashedTransformation, wp_attachedPosition->getTransformationMatrix(), &mx_cashedLocalTransformation );
				}
				else
				{
					dw_attachedPositionLastUpdateTimestamp = *dw_attachedPositionMatrixTimestamp;
					D3DXMatrixMultiply( &mx_cashedTransformation, mx_attachedPosition, &mx_cashedLocalTransformation );
				}
			}
			else
				mx_cashedTransformation = mx_cashedLocalTransformation;

			b_isCashedTransformationActual = true;
		}
	};


public:

	WorldPositionHR() : WorldPosition(), b_isAttached(bool), wp_attachedPosition(NULL), mx_attachedPosition(NULL), dw_attachedPositionMatrixTimestamp(NULL), dw_attachedPositionLastUpdateTimestamp(-1), b_isCashedTransformationActual(false), dw_cashedTransformationTimestamp(0)
	{
		D3DXMatrixIdentity( &mx_cashedTransformation );
		updateTransformationMatrix();
	};

	virtual DWORD getTransformationTimestamp()
	{
		if ( !b_isCashedTransformationActual )
			return dw_cashedTransformationTimestamp;

		if ( b_isAttached )
		{
			if ( wp_attachedPosition )
			{
				if ( dw_attachedPositionLastUpdateTimestamp != wp_attachedPosition->getTransformationTimestamp() )
				{
					b_isCashedTransformationActual = false;
					return ++dw_cashedTransformationTimestamp;
				}
			}
			else
			{
				if ( dw_attachedPositionLastUpdateTimestamp != *dw_attachedPositionMatrixTimestamp )
				{
					b_isCashedTransformationActual = false;
					return ++dw_cashedTransformationTimestamp;
				}
			}
		}

		if ( !b_cashedLocalTransformationActual )
		{
			b_isCashedTransformationActual = false;
			return ++dw_cashedTransformationTimestamp;
		}

		return dw_cashedTransformationTimestamp;
	};


	virtual const D3DXMATRIX* getTransformationMatrix()
	{
		updateTransformationMatrix();
		return &mx_cashedTransformation;
	};


	void attach( WorldPosition* _wp_attachedPosition )
	{
		b_isAttached = true;
		wp_attachedPosition = _wp_attachedPosition;
		dw_attachedPositionLastUpdateTimestamp = wp_attachedPosition->getTransformationTimestamp();
		--dw_attachedPositionLastUpdateTimestamp;
		mx_attachedPosition = NULL;
		dw_attachedPositionMatrixTimestamp = NULL;
		b_isCashedTransformationActual = false;
		++dw_cashedTransformationTimestamp;
	};

	void attach( D3DXMATRIX* _mx_attachedPosition, DWORD* _dw_attachedPositionMatrixTimestamp )
	{
		b_isAttached = true;
		wp_attachedPosition = NULL;
		mx_attachedPosition = _mx_attachedPosition;
		dw_attachedPositionMatrixTimestamp = _dw_attachedPositionMatrixTimestamp;
		dw_attachedPositionLastUpdateTimestamp = *dw_attachedPositionMatrixTimestamp;
		--dw_attachedPositionLastUpdateTimestamp;
		b_isCashedTransformationActual = false;
		++dw_cashedTransformationTimestamp;
	};


	void dettach()
	{
		b_isAttached = false;
		wp_attachedPosition = NULL;
		mx_attachedPosition = NULL;
		dw_attachedPositionMatrixTimestamp = NULL;
		dw_attachedPositionLastUpdateTimestamp = -1;
		b_isCashedTransformationActual = false;
		++dw_cashedTransformationTimestamp;
	};


	inline bool isAttached()	{	return b_isAttached;	};

	inline bool isAttachedByWorldPosition()		{	return b_isAttached ? ( wp_attachedPosition ? true : false ) : false;	};
	inline bool isAttachedByMatrixPointer()		{	return b_isAttached ? ( mx_attachedPosition ? true : false ) : false;	};
	
	inline WorldPosition* getAttachedWorldPosition()		{	return wp_attachedPosition;	};
	inline const D3DXMATRIX* getAttachedMatrixPointer()		{	return mx_attachedPosition;	};
};



#endif //_WORLD_POSITION_V4

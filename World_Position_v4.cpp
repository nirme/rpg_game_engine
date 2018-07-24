#include "World_Position_v4.h"




void WorldPosition::updateTransformationMatrix()
{
	if ( !b_cashedLocalTransformationActual )
	{
		D3DXMatrixAffineTransformation( &mx_cashedLocalTransformation, f_scale, NULL, q_rotation, v3_position );
		b_cashedLocalTransformationActual = true;
	}
};


WorldPosition::WorldPosition() : f_scale(1.0f), f_oneDividedByScale(1.0f), b_cashedLocalTransformationActual(false), dw_cashedLocalTransformationTimestamp(-1)
{
	v3_position = D3DXVECTOR3( 0.0f, 0.0f, 0.0f )
	D3DXQuaternionIdentity( &q_rotation );
	D3DXMatrixIdentity( &mx_rotation );
	D3DXMatrixIdentity( &mx_position );
	WorldPosition::updateTransformationMatrix();
};


DWORD WorldPosition::getTransformationTimestamp()
{
	updateTransformationMatrix();
	return dw_cashedLocalTransformationTimestamp;
};


void WorldPosition::setScale( float _f_scale )
{
	f_scale = _f_scale;
	f_oneDividedByScale = 1.0f / f_scale;
	b_cashedLocalTransformationActual = false;
	++dw_cashedLocalTransformationTimestamp;
};


void WorldPosition::setTranslation( const D3DXVECTOR3& _v3_position )
{
	v3_position = _v3_position;
	b_cashedLocalTransformationActual = false;
	++dw_cashedLocalTransformationTimestamp;
};


void WorldPosition::setTranslationX( const float& _f_positionX )
{
	v3_position.x = _f_positionX;
	b_cashedLocalTransformationActual = false;
	++dw_cashedLocalTransformationTimestamp;
};


void WorldPosition::setTranslationY( const float& _f_positionY )
{
	v3_position.y = _f_positionY;
	b_cashedLocalTransformationActual = false;
	++dw_cashedLocalTransformationTimestamp;
};


void WorldPosition::setTranslationZ( const float& _f_positionZ )
{
	v3_position.z = _f_positionZ;
	b_cashedLocalTransformationActual = false;
	++dw_cashedLocalTransformationTimestamp;
};


void WorldPosition::setRotation( const D3DXVECTOR3& _v3_rotation )
{
	D3DXQuaternionNormalize( &q_rotation, D3DXQuaternionRotationYawPitchRoll( &q_rotation, _v3_rotation.y, _v3_rotation.x, _v3_rotation.z ) );
	b_cashedLocalTransformationActual = false;
	++dw_cashedLocalTransformationTimestamp;
};


void WorldPosition::setRotation( const D3DXQUATERNION& _q_rotation )
{
	D3DXQuaternionNormalize( &q_rotation, &_q_rotation );
	b_cashedLocalTransformationActual = false;
	++dw_cashedLocalTransformationTimestamp;
};


void WorldPosition::translate( const D3DXVECTOR3& _v3_position )
{
	v3_position += _v3_position;
	b_cashedLocalTransformationActual = false;
	++dw_cashedLocalTransformationTimestamp;
};


void WorldPosition::translateX( const float& _f_positionX )
{
	v3_position.x += _f_positionX;
	b_cashedLocalTransformationActual = false;
	++dw_cashedLocalTransformationTimestamp;
};


void WorldPosition::translateY( const float& _f_positionY )
{
	v3_position.y += _f_positionY;
	b_cashedLocalTransformationActual = false;
	++dw_cashedLocalTransformationTimestamp;
};


void WorldPosition::translateZ( const float& _f_positionZ )
{
	v3_position.z += _f_positionZ;
	b_cashedLocalTransformationActual = false;
	++dw_cashedLocalTransformationTimestamp;
};


void WorldPosition::translateOnLocalAxis( const D3DXVECTOR3& _v3_position )
{
	updateTransformationMatrix();
	D3DXVECTOR3 v3_tmp;
	D3DXVec3TransformNormal( &v3_tmp, &_v3_position, &mx_cashedLocalTransformation );
	v3_position += v3_tmp * f_oneDividedByScale;
	b_cashedLocalTransformationActual = false;
	++dw_cashedLocalTransformationTimestamp;
};


void WorldPosition::translateXOnLocalAxis( const float& _f_positionX )
{
	updateTransformationMatrix();
	D3DXVECTOR3 v3_tmp( mx_cashedLocalTransformation.m[0] );
	v3_position += v3_tmp * f_oneDividedByScale;
	b_cashedLocalTransformationActual = false;
	++dw_cashedLocalTransformationTimestamp;
};


void WorldPosition::translateYOnLocalAxis( const float& _f_positionX )
{
	updateTransformationMatrix();
	D3DXVECTOR3 v3_tmp( mx_cashedLocalTransformation.m[0] );
	v3_position += v3_tmp * f_oneDividedByScale;
	b_cashedLocalTransformationActual = false;
	++dw_cashedLocalTransformationTimestamp;
};


void WorldPosition::translateZOnLocalAxis( const float& _f_positionX )
{
	updateTransformationMatrix();
	D3DXVECTOR3 v3_tmp( mx_cashedLocalTransformation.m[0] );
	v3_position += v3_tmp * f_oneDividedByScale;
	b_cashedLocalTransformationActual = false;
	++dw_cashedLocalTransformationTimestamp;
};


void WorldPosition::rotate( const D3DXVECTOR3& _v3_rotation )
{
	D3DXQUATERNION q_tmp;
	D3DXQuaternionNormalize( &q_rotation, D3DXQuaternionMultiply( &q_rotation, &q_rotation, D3DXQuaternionNormalize( &q_tmp, D3DXQuaternionRotationYawPitchRoll( &q_rotation, _v3_rotation.y, _v3_rotation.x, _v3_rotation.z ) ) );
	b_cashedLocalTransformationActual = false;
	++dw_cashedLocalTransformationTimestamp;
};


void WorldPosition::rotate( const D3DXQUATERNION& _q_rotation )
{
	D3DXQuaternionMultiply( &q_rotation, &q_rotation, _q_rotation );
	b_cashedLocalTransformationActual = false;
	++dw_cashedLocalTransformationTimestamp;
};


void WorldPosition::rotateX( const float& _f_rotationAngle )
{
	D3DXQUATERNION q_tmpRotation;
	D3DXQuaternionMultiply( &q_rotation, &q_rotation, D3DXQuaternionRotationAxis( &q_tmpRotation, &axisX, _f_rotationAngle ) );
	b_cashedLocalTransformationActual = false;
	++dw_cashedLocalTransformationTimestamp;
};


void WorldPosition::rotateY( const float& _f_rotationAngle )
{
	D3DXQUATERNION q_tmpRotation;
	D3DXQuaternionMultiply( &q_rotation, &q_rotation, D3DXQuaternionRotationAxis( &q_tmpRotation, &axisY, _f_rotationAngle ) );
	b_cashedLocalTransformationActual = false;
	++dw_cashedLocalTransformationTimestamp;
};


void WorldPosition::rotateZ( const float& _f_rotationAngle )
{
	D3DXQUATERNION q_tmpRotation;
	D3DXQuaternionMultiply( &q_rotation, &q_rotation, D3DXQuaternionRotationAxis( &q_tmpRotation, &axisZ, _f_rotationAngle ) );
	b_cashedLocalTransformationActual = false;
	++dw_cashedLocalTransformationTimestamp;
};


const D3DXMATRIX* WorldPosition::getLocalTransformationMatrix()
{
	updateTransformationMatrix();
	return &mx_cashedLocalTransformation;
};


const D3DXMATRIX* WorldPosition::getTransformationMatrix()
{
	updateTransformationMatrix();
	return &mx_cashedLocalTransformation;
};


D3DXVECTOR3& WorldPosition::getFacing()
{
	D3DXVECTOR3 v3_tmp;
	D3DXVec3Normalize( &v3_tmp, D3DXVec3TransformNormal( &v3_tmp, v3_facingDirectionNormalised, getTransformationMatrix() ) );
	return v3_tmp;
};



void WorldPositionHR::updateTransformationMatrix()
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


WorldPositionHR::WorldPositionHR() : WorldPosition(), b_isAttached(bool), wp_attachedPosition(NULL), mx_attachedPosition(NULL), dw_attachedPositionMatrixTimestamp(NULL), dw_attachedPositionLastUpdateTimestamp(-1), b_isCashedTransformationActual(false), dw_cashedTransformationTimestamp(0)
{
	D3DXMatrixIdentity( &mx_cashedTransformation );
	updateTransformationMatrix();
};


DWORD WorldPositionHR::getTransformationTimestamp()
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


const D3DXMATRIX* WorldPositionHR::getTransformationMatrix()
{
	updateTransformationMatrix();
	return &mx_cashedTransformation;
};


void WorldPositionHR::attach( WorldPosition* _wp_attachedPosition )
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


void WorldPositionHR::attach( D3DXMATRIX* _mx_attachedPosition, DWORD* _dw_attachedPositionMatrixTimestamp )
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


void WorldPositionHR::dettach()
{
	b_isAttached = false;
	wp_attachedPosition = NULL;
	mx_attachedPosition = NULL;
	dw_attachedPositionMatrixTimestamp = NULL;
	dw_attachedPositionLastUpdateTimestamp = -1;
	b_isCashedTransformationActual = false;
	++dw_cashedTransformationTimestamp;
};


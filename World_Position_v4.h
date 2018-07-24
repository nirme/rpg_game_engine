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


	virtual void updateTransformationMatrix();

public:

	WorldPosition();
	
	virtual DWORD getTransformationTimestamp();

	void setScale( float _f_scale );
	inline float getScale()	{	return f_scale;	};

	void setTranslation( const D3DXVECTOR3& _v3_position );
	void setTranslationX( const float& _f_positionX );
	void setTranslationY( const float& _f_positionY );
	void setTranslationZ( const float& _f_positionZ );

	void setRotation( const D3DXVECTOR3& _v3_rotation );
	void setRotation( const D3DXQUATERNION& _q_rotation );

	void translate( const D3DXVECTOR3& _v3_position );
	void translateX( const float& _f_positionX );
	void translateY( const float& _f_positionY );
	void translateZ( const float& _f_positionZ );

	void translateOnLocalAxis( const D3DXVECTOR3& _v3_position );
	void translateXOnLocalAxis( const float& _f_positionX );
	void translateYOnLocalAxis( const float& _f_positionX );
	void translateZOnLocalAxis( const float& _f_positionX );

	void rotate( const D3DXVECTOR3& _v3_rotation );
	void rotate( const D3DXQUATERNION& _q_rotation );
	void rotateX( const float& _f_rotationAngle );
	void rotateY( const float& _f_rotationAngle );
	void rotateZ( const float& _f_rotationAngle );

	const D3DXMATRIX* getLocalTransformationMatrix();
	virtual const D3DXMATRIX* getTransformationMatrix();

	inline D3DXVECTOR3& getPosition()	{	return D3DXVECTOR3( v3_position );	};

	inline D3DXQUATERNION& getRotation()	{	return D3DXQUATERNION( q_rotation );	};

	D3DXVECTOR3& getFacing();
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


	virtual void updateTransformationMatrix();


public:

	WorldPositionHR();

	virtual DWORD getTransformationTimestamp();
	virtual const D3DXMATRIX* getTransformationMatrix();

	void attach( WorldPosition* _wp_attachedPosition );
	void attach( D3DXMATRIX* _mx_attachedPosition, DWORD* _dw_attachedPositionMatrixTimestamp );
	void dettach();


	inline bool isAttached()	{	return b_isAttached;	};

	inline bool isAttachedByWorldPosition()		{	return b_isAttached ? ( wp_attachedPosition ? true : false ) : false;	};
	inline bool isAttachedByMatrixPointer()		{	return b_isAttached ? ( mx_attachedPosition ? true : false ) : false;	};
	
	inline WorldPosition* getAttachedWorldPosition()		{	return wp_attachedPosition;	};
	inline const D3DXMATRIX* getAttachedMatrixPointer()		{	return mx_attachedPosition;	};
};



#endif //_WORLD_POSITION_V4

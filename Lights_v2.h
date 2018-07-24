#pragma once

#ifndef _LIGHTS_V2
#define _LIGHTS_V2

#include "utility.h"
#include "World_Position_v4.h"



#define		LIGHT_TYPE_UNDEFINED		0x00
#define		LIGHT_TYPE_DIRECTIONAL		0x01
#define		LIGHT_TYPE_OMNI				0x02
#define		LIGHT_TYPE_SPOT				0x03


// LIGHT STRUCT 
// it's packed in array of floats / length is dividable by 4 - size of D3DXVECTOR4.
// array has variable length
//
// attenuation note: 0 - constant, 1 - linear, 2 - quadratic
// color diffuse note: RGB, light type (int)
// color specular note: RGB, empty (user defined)
//
// +---------------------------+--------------------------------+
// |  LIGHT TYPE: DIRECTIONAL  |  SIZE: 12x float / 3x VECTOR4  |
// +------------------+--------+--------------+-----------------+
// |    Variable      |  Position (flt/VEC4) |  Size (flt/VEC4) |
// +------------------+----------------------+------------------+
// |  direction       |    0 / 0             |    4 / 1         |
// +------------------+----------------------+------------------+
// |  color diffuse   |    4 / 1(0)          |    3 / 0(3)      |
// +------------------+----------------------+------------------+
// |  light type      |    7 / 1(3)          |    1 / 0(1)      |
// +------------------+----------------------+------------------+
// |  color specular  |    8 / 2(0)          |    3 / 0(3)      |
// +------------------+----------------------+------------------+
// |  empty 1         |    11 / 2(3)         |    1 / 0(1)      |
// +------------------+----------------------+------------------+
//
//
// +-----------------------+------------------------------------+
// |  LIGHT TYPE: OMNI     |  SIZE: 16x float / 4x VECTOR4      |
// +------------------+----+-----------------+------------------+
// |    Variable      |  Position (flt/VEC4) |  Size (flt/VEC4) |
// +------------------+----------------------+------------------+
// |  position        |    0 / 0             |    4 / 1         |
// +------------------+----------------------+------------------+
// |  color diffuse   |    4 / 1(0)          |    3 / 0(3)      |
// +------------------+----------------------+------------------+
// |  light type      |    7 / 1(3)          |    1 / 0(1)      |
// +------------------+----------------------+------------------+
// |  color specular  |    8 / 2(0)          |    3 / 0(3)      |
// +------------------+----------------------+------------------+
// |  empty 1         |    11 / 2(3)         |    1 / 0(1)      |
// +------------------+----------------------+------------------+
// |  range           |    12 / 3(0)         |    1 / 0(1)      |
// +------------------+----------------------+------------------+
// |  attenuation 0   |    13 / 3(1)         |    1 / 0(1)      |
// +------------------+----------------------+------------------+
// |  attenuation 1   |    14 / 3(2)         |    1 / 0(1)      |
// +------------------+----------------------+------------------+
// |  attenuation 2   |    15 / 3(3)         |    1 / 0(1)      |
// +------------------+----------------------+------------------+
//
//
// +-----------------------+------------------------------------+
// |  LIGHT TYPE: SPOT     |  SIZE: 24x float / 6x D3DXVECTOR4  |
// +------------------+----+-----------------+------------------+
// |    Variable      |  Position (flt/VEC4) |  Size (flt/VEC4) |
// +------------------+----------------------+------------------+
// |  position        |    0 / 0             |    4 / 1         |
// +------------------+----------------------+------------------+
// |  color diffuse   |    4 / 1(0)          |    3 / 0(3)      |
// +------------------+----------------------+------------------+
// |  light type      |    7 / 1(3)          |    1 / 0(1)      |
// +------------------+----------------------+------------------+
// |  color specular  |    8 / 2(0)          |    3 / 0(3)      |
// +------------------+----------------------+------------------+
// |  empty 1         |    11 / 2(3)         |    1 / 0(1)      |
// +------------------+----------------------+------------------+
// |  range           |    12 / 3(0)         |    1 / 0(1)      |
// +------------------+----------------------+------------------+
// |  attenuation 0   |    13 / 3(1)         |    1 / 0(1)      |
// +------------------+----------------------+------------------+
// |  attenuation 1   |    14 / 3(2)         |    1 / 0(1)      |
// +------------------+----------------------+------------------+
// |  attenuation 2   |    15 / 3(3)         |    1 / 0(1)      |
// +------------------+----------------------+------------------+
// |  cos( hotspot )  |    16 / 4(0)         |    1 / 0(1)      |
// +------------------+----------------------+------------------+
// |  cos( cutoff )   |    17 / 4(1)         |    1 / 0(1)      |
// +------------------+----------------------+------------------+
// |  falloff         |    18 / 4(2)         |    1 / 0(1)      |
// +------------------+----------------------+------------------+
// |  empty 2         |    19 / 4(3)         |    1 / 0(1)      |
// +------------------+----------------------+------------------+
// |  direction       |    20 / 5            |    4 / 1         |
// +------------------+----------------------+------------------+
//


class Light;
class LightDirectional;
class LightOmniDirectional;
class LightSpot;

bool light_compare ( const Light* first, const Light* second);
bool light_compare ( const Light& first, const Light& second);


class Light
{
private:

	float* v4_lightDataArray;
	D3DXVECTOR4* v4_lightDataVector;
	UINT ui_lightDataVectorLength;


	void setupLightArray( BYTE _bt_vector4Number );


protected:

	inline void setLightDataInArray( BYTE _bt_position, float _fl_value )
	{
		if ( _bt_position >= ( ui_lightDataVectorLength * 4 ) )
			throw ApplicationException( "Light::setLightDataInArray called with invalid position" );
		v4_lightDataArray[_bt_position] = _fl_value;
	};

	inline void setLightDataInArray( BYTE _bt_position, int _i_value )
	{
		if ( _bt_position >= ( ui_lightDataVectorLength * 4 ) )
			throw ApplicationException( "Light::setLightDataInArray called with invalid position" );
		v4_lightDataArray[_bt_position] = _i_value;
	};

	inline void setLightDataInArray( BYTE _bt_position, bool _bl_value )
	{
		if ( _bt_position >= ( ui_lightDataVectorLength * 4 ) )
			throw ApplicationException( "Light::setLightDataInArray called with invalid position" );
		v4_lightDataArray[_bt_position] = _bl_value ? 1.0f : 0.0f;
	};


	inline void setLightDataInVector( BYTE _bt_position, const D3DXVECTOR4* _v4_value )
	{
		if ( _bt_position >= ui_lightDataVectorLength )
			throw ApplicationException( "Light::setLightDataInArray called with invalid position" );
		v4_lightDataVector[_bt_position] = *_v4_value;
	};

	inline void setLightDataInVector( BYTE _bt_position, const D3DXVECTOR3* _v3_value )
	{
		if ( _bt_position >= ui_lightDataVectorLength )
			throw ApplicationException( "Light::setLightDataInArray called with invalid position" );
		v4_lightDataVector[_bt_position].x = _v3_value->x;
		v4_lightDataVector[_bt_position].y = _v3_value->y;
		v4_lightDataVector[_bt_position].z = _v3_value->z;
	};

	inline void setLightDataInVector( BYTE _bt_position, const D3DXCOLOR* _xc_value )
	{
		if ( _bt_position >= ui_lightDataVectorLength )
			throw ApplicationException( "Light::setLightDataInArray called with invalid position" );
		v4_lightDataVector[_bt_position].x = _xc_value->r;
		v4_lightDataVector[_bt_position].y = _xc_value->g;
		v4_lightDataVector[_bt_position].z = _xc_value->b;
	};

	inline void setLightDataInVector( BYTE _bt_position, BYTE _bt_vectorPosition, float _fl_value )
	{
		setLightDataInArray( _bt_position * 4 + _bt_vectorPosition, _fl_value );
	};

	inline void setLightDataInVector( BYTE _bt_position, BYTE _bt_vectorPosition, int _i_value )
	{
		setLightDataInArray( _bt_position * 4 + _bt_vectorPosition, _i_value );
	};

	inline void setLightDataInVector( BYTE _bt_position, BYTE _bt_vectorPosition, bool _bl_value )
	{
		setLightDataInArray( _bt_position * 4 + _bt_vectorPosition, _bl_value );
	};



	BYTE bt_priority;
	const BYTE bt_lightType;
	bool b_generateShadows;

	D3DXCOLOR xc_color_diffuse; //RGBA
	D3DXCOLOR xc_color_specular; //RGBA

	WorldPosition* wp_transform;
	DWORD dw_transformTimestamp;


	Light( BYTE _bt_lightType, BYTE _bt_vector4Number, BYTE _bt_priority, bool _b_generateShadows, WorldPosition* _wp_transform = NULL );
	const D3DXMATRIX* updateWorldPosition();


public:

	virtual ~Light();

	virtual void update() = 0;

	inline WorldPosition* getWorldPosition()	{	return wp_transform;	};
	void setWorldPosition( WorldPosition* _wp_transform );

	inline D3DXCOLOR getColorDiffuse()	{	return xc_color_diffuse;	};
	void setColorDiffuse( const D3DXCOLOR& _xc_color );

	inline D3DXCOLOR getColorSpecular()	{	return xc_color_specular;	};
	void setColorSpecular( const D3DXCOLOR& _xc_color );

	const D3DXVECTOR4* getLightDataVector( UINT* _pui_lightDataVectorLength );


	inline BYTE getPriority()									{	return bt_priority;	};
	inline void setPriority( BYTE _bt_priority )				{	bt_priority = _bt_priority;	};

	inline BYTE getLightType()									{	return bt_lightType;	};

	inline bool getShadowsGeneration()							{	return b_generateShadows;	};
	inline void setShadowsGeneration( bool _b_generateShadows )	{	b_generateShadows = _b_generateShadows;	};


	inline bool operator > (Light const & b)
	{
		if (lightType == b.lightType)
			return priority > b.priority;
		return lightType > b.lightType;
	};
};


inline bool light_compare ( const Light* first, const Light* second)
{
	return (*first) < (*second);
};

inline bool light_compare ( const Light& first, const Light& second)
{
	return first < second;
};



class LightDirectional : public Light
{
protected:

public:

	LightDirectional( BYTE _bt_priority, bool _b_generateShadows, WorldPosition* _wp_transform = NULL );

	virtual void update();
};


class LightOmniDirectional : public Light
{
protected:

	float fl_range;
	float fl_attenuation0;
	float fl_attenuation1;
	float fl_attenuation2;

public:

	LightOmniDirectional( BYTE _bt_priority, bool _b_generateShadows, float _fl_range, float _fl_attenuation0, float _fl_attenuation1, float _fl_attenuation2, WorldPosition* _wp_transform = NULL );

	virtual void update();


	void setRange( float _fl_range );

	inline float getRange()	{	return fl_range;	};


	void setAttenuation( float _fl_attenuation0, float _fl_attenuation1, float _fl_attenuation2 );
	void setAttenuation( float _fl_attenuationLinearity );
	void setAttenuationLinear();
	void setAttenuationQuadratic();
	void getAttenuation( float* _fl_attenuation0, float* _fl_attenuation1, float* _fl_attenuation2 );

};


class LightSpot : public Light
{
protected:

	float fl_range;
	float fl_attenuation0;
	float fl_attenuation1;
	float fl_attenuation2;

	float fl_cosHotspot;
	float ft_cosCutoff;
	float ft_falloff;


public:

	virtual void update();

	void setRange( float _fl_range );
	inline float getRange()	{	return fl_range;	};

	void setAttenuation( float _fl_attenuation0, float _fl_attenuation1, float _fl_attenuation2 );
	void setAttenuation( float _fl_attenuationLinearity );
	void setAttenuationLinear();
	void setAttenuationQuadratic();
	void getAttenuation( float* _fl_attenuation0, float* _fl_attenuation1, float* _fl_attenuation2 );

	void setHotspot( float _fl_hotspotAngle );
	inline float getHotspot()	{	return acos( fl_cosHotspot );	};

	void setCutoff( float _fl_cutoffAngle );
	inline float getCutoff()	{	return acos( ft_cosCutoff );	};

	void setFalloff( float _ft_falloff );
	inline float getFalloff()	{	return ft_falloff;	};
};


#endif //_LIGHTS_V2

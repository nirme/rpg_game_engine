#include "Lights_v2.h"



void Light::setupLightArray( BYTE _bt_vector4Number )
{
	SAFE_RELEASE_ARRAY( v4_lightDataArray );

	v4_lightDataArray = new float [ _ui_vector4Number * 4 ];
	v4_lightDataVector = reinterpret_cast <D3DXVECTOR4*> ( v4_lightDataArray );
	ui_lightDataVectorLength = _ui_vector4Number;
};


Light::Light( BYTE _bt_lightType, BYTE _bt_vector4Number, BYTE _bt_priority, bool _b_generateShadows, WorldPosition* _wp_transform ) : v4_lightDataArray(NULL), v4_lightDataVector(NULL), ui_lightDataVectorLength(0), bt_lightType(_bt_lightType), bt_priority(_bt_priority), b_generateShadows(_b_generateShadows), wp_transform(_wp_transform), xc_color_diffuse((DWORD)0), xc_color_specular((DWORD)0)
{
	setupLightArray( _bt_vector4Number );
};


const Light::D3DXMATRIX* updateWorldPosition()
{
	if ( wp_transform )
	{
		DWORD dw_timestamp = wp_transform->getTransformationTimestamp();
		if ( dw_transformTimestamp != dw_timestamp )
		{
			dw_transformTimestamp = dw_timestamp;
			return wp_transform->getTransformationMatrix();
		}
	}
	return NULL;
};


virtual Light::~Light()
{
	SAFE_DELETE_ARRAY( v4_lightDataArray );
};


void Light::setWorldPosition( WorldPosition* _wp_transform )
{
	wp_transform = _wp_transform;
	dw_transformTimestamp = wp_transform ? ( wp_transform->getTransformationTimestamp() - 1 ) : -1;
	update();
};


void Light::setColorDiffuse( const D3DXCOLOR& _xc_color )
{
	xc_color_diffuse = _xc_color;
	setLightDataInVector( 1, xc_color_diffuse );
};


void Light::setColorSpecular( const D3DXCOLOR& _xc_color )
{
	xc_color_specular = _xc_color;
	setLightDataInVector( 2, xc_color_specular );
};


const D3DXVECTOR4* Light::getLightDataVector( UINT* _pui_lightDataVectorLength )
{
	update();
	if ( _pui_lightDataVectorLength )
		*_pui_lightDataVectorLength = ui_lightDataVectorLength;
	return v4_lightDataVector;
};




LightDirectional::LightDirectional( BYTE _bt_priority, bool _b_generateShadows, WorldPosition* _wp_transform ) : Light( LIGHT_TYPE_DIRECTIONAL, 3, _bt_priority, _b_generateShadows, _wp_transform )
{
	setLightDataInArray( 7, LIGHT_TYPE_DIRECTIONAL );
	setLightDataInArray( 11, 0 );
	update();
};


virtual void LightDirectional::update()
{
	const D3DXMATRIX* mx_world = updateWorldPosition();
	if ( mx_world )
	{
		D3DXVECTOR3 v3_tmp;
		setLightDataInVector( 0, D3DXVec3Normalize( &v3_tmp, D3DXVec3TransformNormal( &v3_tmp, v3_facingDirectionNormalised, mx_world ) ) );
	}
};



LightOmniDirectional::LightOmniDirectional( BYTE _bt_priority, bool _b_generateShadows, float _fl_range, float _fl_attenuation0, float _fl_attenuation1, float _fl_attenuation2, WorldPosition* _wp_transform ) : Light( LIGHT_TYPE_OMNI, 4, _bt_priority, _b_generateShadows, _wp_transform ), fl_range(_fl_range), fl_attenuation0(_fl_attenuation0), fl_attenuation1(_fl_attenuation1), fl_attenuation2(_fl_attenuation2)
{
	setLightDataInArray( 7, LIGHT_TYPE_OMNI );
	setLightDataInArray( 11, 0.0f );
	setLightDataInArray( 12, fl_range );
	setLightDataInArray( 13, fl_attenuation0 );
	setLightDataInArray( 14, fl_attenuation1 );
	setLightDataInArray( 15, fl_attenuation2 );

	update();
};


virtual void LightOmniDirectional::update()
{
	const D3DXMATRIX* mx_world = updateWorldPosition();
	if ( mx_world )
	{
		D3DXVECTOR3 v3_tmp;
		D3DXVec3TransformCoord( &v3_tmp, &D3DXVECTOR3( 0.0f, 0.0f. 0.0f ), mx_world );
		setLightDataInVector( 0, &D3DXVECTOR4( v3_tmp, 1.0f ) );
		// use a little hack assuming the matrix is affine if we multiply a vector (1,0,0,0) by it we'll get (mx[0][0], mx[0][1], mx[0][2], mx[0][3]) = its perfect to find out total scale
		setLightDataInArray( 12, fl_range * D3DXVec4Length( &D3DXVECTOR4( mx_world->m[0] ) ) );
	}
};



void LightOmniDirectional::setRange( float _fl_range )
{
	fl_range = _fl_range;
	const D3DXMATRIX* mx_world = wp_transform->getTransformationMatrix();
	setLightDataInArray( 12, fl_range * D3DXVec4Length( &D3DXVECTOR4( mx_world->m[0] ) ) );
};


void LightOmniDirectional::setAttenuation( float _fl_attenuation0, float _fl_attenuation1, float _fl_attenuation2 )
{
	setLightDataInArray( 13, fl_attenuation0 = _fl_attenuation0 );
	setLightDataInArray( 14, fl_attenuation1 = _fl_attenuation1 );
	setLightDataInArray( 15, fl_attenuation2 = _fl_attenuation2 );
};


void LightOmniDirectional::setAttenuation( float _fl_attenuationLinearity )
{
	_fl_attenuationLinearity = saturate(_fl_attenuationLinearity);
	setAttenuation( 0.0f, _fl_attenuationLinearity, 1.0f - _fl_attenuationLinearity );
};


void LightOmniDirectional::setAttenuationLinear()
{
	setAttenuation( 0.0f, 1.0f, 0.0f );
};


void LightOmniDirectional::setAttenuationQuadratic()
{
	setAttenuation( 0.0f, 0.0f, 1.0f );
};


void LightOmniDirectional::getAttenuation( float* _fl_attenuation0, float* _fl_attenuation1, float* _fl_attenuation2 )
{
	if ( _fl_attenuation0 )		*_fl_attenuation0 = fl_attenuation0;
	if ( _fl_attenuation1 )		*_fl_attenuation1 = fl_attenuation1;
	if ( _fl_attenuation2 )		*_fl_attenuation2 = fl_attenuation2;
};



virtual void LightSpot::update()
{
	const D3DXMATRIX* mx_world = updateWorldPosition();
	if ( mx_world )
	{
		D3DXVECTOR3 v3_tmp;
		D3DXVec3TransformCoord( &v3_tmp, &D3DXVECTOR3( 0.0f, 0.0f. 0.0f ), mx_world );
		setLightDataInVector( 0, &D3DXVECTOR4( v3_tmp, 1.0f ) );
		setLightDataInArray( 12, fl_range * D3DXVec4Length( &D3DXVECTOR4( mx_world->m[0] ) ) );
		setLightDataInVector( 5, D3DXVec3Normalize( &v3_tmp, D3DXVec3TransformNormal( &v3_tmp, v3_facingDirectionNormalised, mx_world ) ) );
	}
};


void LightSpot::setRange( float _fl_range )
{
	fl_range = _fl_range;
	const D3DXMATRIX* mx_world = wp_transform->getTransformationMatrix();
	setLightDataInArray( 12, fl_range * D3DXVec4Length( &D3DXVECTOR4( mx_world->m[0] ) ) );
};


void LightSpot::setAttenuation( float _fl_attenuation0, float _fl_attenuation1, float _fl_attenuation2 )
{
	fl_attenuation0 = _fl_attenuation0;
	fl_attenuation1 = _fl_attenuation1;
	fl_attenuation2 = _fl_attenuation2;

	setLightDataInArray( 13, fl_attenuation0 );
	setLightDataInArray( 14, fl_attenuation1 );
	setLightDataInArray( 15, fl_attenuation2 );
};


void LightSpot::setAttenuation( float _fl_attenuationLinearity )
{
	setAttenuation( 0.0f, _fl_attenuationLinearity, 1.0f - _fl_attenuationLinearity );
};


void LightSpot::setAttenuationLinear()
{
	setAttenuation( 0.0f, 1.0f, 0.0f );
};


void LightSpot::setAttenuationQuadratic()
{
	setAttenuation( 0.0f, 0.0f, 1.0f );
};


void LightSpot::getAttenuation( float* _fl_attenuation0, float* _fl_attenuation1, float* _fl_attenuation2 )
{
	if ( _fl_attenuation0 )		*_fl_attenuation0 = fl_attenuation0;
	if ( _fl_attenuation1 )		*_fl_attenuation1 = fl_attenuation1;
	if ( _fl_attenuation2 )		*_fl_attenuation2 = fl_attenuation2;
};


void LightSpot::setHotspot( float _fl_hotspotAngle )
{
	fl_cosHotspot = cos( clip( _fl_hotspotAngle, 0.0f, D3DX_PI * 0.5f ) );
	setLightDataInArray( 16, fl_cosHotspot );
};


void LightSpot::setCutoff( float _fl_cutoffAngle )
{
	ft_cosCutoff = cos( clip( _fl_cutoffAngle, 0.0f, D3DX_PI * 0.5f ) );
	setLightDataInArray( 17, ft_cosCutoff );
};


void LightSpot::setFalloff( float _ft_falloff )
{
	ft_falloff = _ft_falloff;
	setLightDataInArray( 18, ft_falloff );
};


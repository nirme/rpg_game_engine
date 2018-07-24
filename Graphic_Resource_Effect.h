#pragma once

#ifndef _GRAPHIC_RESOURCE_EFFECT
#define _GRAPHIC_RESOURCE_EFFECT

#include "utility.h"
#include "Template_Basic_Resource.h"
#include "Core_Defines.h"

#include "Core_Light.h"

#include "Base_Exceptions.h"



class BaseEffect;
class SystemEffect;
class StandardEffect;
class SkinnedEffect;
class LightedEffect;
class SkinnedLightedEffect;



class BaseEffect
{
protected:
	struct Technique
	{
		const D3DXHANDLE h_handle;
		DWORD dw_passCount;
		string str_name;

		Technique( D3DXHANDLE _h_handle, DWORD _dw_passCount, const string& _str_name );
		Technique( D3DXHANDLE _h_handle, DWORD _dw_passCount, LPCSTR _cstr_name );
	};

	struct Variable
	{
		const D3DXHANDLE h_handle;
		D3DXPARAMETER_CLASS dx_class;
		D3DXPARAMETER_TYPE dx_type;
		string str_name;
		string str_semantic;

		Variable( D3DXHANDLE _h_handle, D3DXPARAMETER_CLASS _dx_class, D3DXPARAMETER_TYPE _dx_type, const string& _str_name, const string& _str_semantic );
		Variable( D3DXHANDLE _h_handle, D3DXPARAMETER_CLASS _dx_class, D3DXPARAMETER_TYPE _dx_type, LPCSTR _cstr_name, LPCSTR _cstr_semantic );
	};

private:
	typedef		map < const string*, BaseEffect* , p_scompare >	BaseEffectMap;
	static BaseEffectMap m_baseEffectMap;

protected:

	static BaseEffect* (*requestFunction)( string* );
	#ifdef(MULTI_THREADED_LOADING)
		static mutable boost::mutex mx_baseEffectMapMutex;
	#endif //MULTI_THREADED_LOADING


	string str_id;
	UINT ui_refCount;

	ID3DXEffect* dx_pEffect;
	D3DXHANDLE h_defaultTechnique;
	DWORD dw_defaultTechniquePassCount;


public:

	BaseEffect( const string& _str_id, ID3DXEffect* _dx_pEffect, const string& _str_defaultTechniqueName );

protected:

	virtual ~BaseEffect();


public:

	inline const string* getId()	{	return &str_id;	};
	inline UINT getRefCount()	{	return ui_refCount;	};

	inline void addRef()		{	++ui_refCount;	};
	//inline void operator++()	{	++ui_refCount;	};
	//inline void operator++(int)	{	++ui_refCount;	};

	void release();

	static BaseEffect* getBaseEffect( const string& _str_id );

	inline const ID3DXEffect* getDXEffect()				{	return dx_pEffect;	};
	inline const D3DXHANDLE getDefaultTechnique()		{	return h_defaultTechnique;	};
	inline const DWORD getDefaultTechniquePassCount()	{	return dw_defaultTechniquePassCount;	};

	virtual void reset();

};


class SystemEffect : public BaseEffect
{
protected:

	vector<Technique> v_techniques;
	vector<Variable> v_variables;


public:

	SystemEffect( const string& _str_id, ID3DXEffect* _dx_pEffect, const string& _str_defaultTechniqueName );

	UINT getTechniqueIndexByName( const string& _str_name );

	const Technique* getTechniqueDesc( UINT _ui_index );
	const Technique* getTechniqueDescByName( const string& _str_name );

	const D3DXHANDLE getTechniqueHandle( UINT _ui_index );
	const D3DXHANDLE getTechniqueHandleByName( const string& _str_name );

	UINT getVariableIndexByName( const string& _str_name );
	UINT getVariableIndexBySemantic( const string& _str_semantic );

	const Variable* getVariableDesc( UINT _ui_index );
	const Variable* getVariableDescByName( const string& _str_name );
	const Variable* getVariableDescBySemantic( const string& _str_semantic );

	const D3DXHANDLE getVariableHandle( UINT _ui_index );
	const D3DXHANDLE getVariableHandleByName( const string& _str_name );
	const D3DXHANDLE getVariableHandleBySemantic( const string& _str_semantic );

	void setTechnique( UINT _ui_index );

};


class StandardEffect : public BaseEffect
{
protected:

	D3DXHANDLE h_mxWorld;
	D3DXHANDLE h_mxView;
	D3DXHANDLE h_mxProjection;

	D3DXHANDLE h_mxViewProjection;
	D3DXHANDLE h_mxViewInverse;

	UINT ui_frameTimeStamp;

	vector<D3DXHANDLE> v_textureHandles;

	D3DXHANDLE h_v3MrtMaterial;
	D3DXHANDLE h_flSpecularIntensity;
	D3DXHANDLE h_flSpecularPower;
	D3DXHANDLE h_flEmissiveFactor;

	vector<Variable> v_variables;


public:

	StandardEffect( const string& _str_id, ID3DXEffect* _dx_pEffect, const string& _str_defaultTechniqueName );

	void setViewProjection( const D3DXMATRIX* _pMx_view, const D3DXMATRIX* _pMx_projection, const D3DXMATRIX* _pMx_viewProjection, const D3DXMATRIX* _pMx_viewInverse, UINT _ui_frameTimeStamp );
	void setViewProjection( const D3DXMATRIX* _pMx_view, const D3DXMATRIX* _pMx_projection, const D3DXMATRIX* _pMx_viewProjection, const D3DXMATRIX* _pMx_viewInverse );

	void setWorld( const D3DXMATRIX* _pMx_world );
	void setTexture( BYTE _b_index, const IDirect3DTexture9* _pTx_texture );
	void setMaterial( const D3DXVECTOR3* _pV3_material );
	void setMaterial( float _fl_specularIntensity, float _fl_specularPower, float _fl_emissiveFactor );

	UINT getVariableIndexByName( const string& _str_name );
	UINT getVariableIndexBySemantic( const string& _str_semantic );

	const Variable* getVariableDesc( UINT _ui_index );

	void setValue( UINT _ui_index, const void* _vp_data, UINT _ui_size );

	inline void commitChanges()
	{
		if ( FAILED( HRESULT hr = dx_pEffect->CommitChanges() ) )
			throw D3DX9Exception( "StandardEffect::commitChanges", "ID3DXEffect::CommitChanges", hr );
	};

};


class SkinnedEffect : public virtual StandardEffect
{
protected:

	UINT ui_totalPaletteSize;
	D3DXHANDLE h_mxSkinningPalette;
	D3DXHANDLE h_iVertexInfluencesNumber;


public:

	SkinnedEffect( const string& _str_id, ID3DXEffect* _dx_pEffect, const string& _str_defaultTechniqueName );

	inline UINT getTotalPaletteSize()	{	return ui_totalPaletteSize;	};

	void setSkinningPalette( const D3DXMATRIX* _pMx_skinningPalette, UINT _ui_skinningPaletteLength, INT _i_vertexInfluencesNumber );
	void setSkinningPalette( const D3DXMATRIX** _ppMx_skinningPalette, UINT _ui_skinningPaletteLength, INT _i_vertexInfluencesNumber );

};


class LightedEffect : public virtual StandardEffect
{
protected:

	D3DXHANDLE h_v4LightAmbient;
	UINT ui_totalLightsVectorSize;
	D3DXHANDLE h_v4LightsVector;
	D3DXHANDLE h_iLightsNumbers;

	UINT ui_frameLightTimeStamp;


public:

	LightedEffect( const string& _str_id, ID3DXEffect* _dx_pEffect, const string& _str_defaultTechniqueName );

	inline UINT getTotalLightsVectorSize()	{	return ui_totalLightsVectorSize;	};

	void setLights( const D3DXVECTOR4* _pV4_lightAmbient, const D3DXVECTOR4* _pV4_lightsVector, UINT _ui_lightsVectorLength, int _i_lightsNumber, UINT _ui_frameLightTimeStamp );
	void setLights( const D3DXVECTOR4* _pV4_lightAmbient, const D3DXVECTOR4* _pV4_lightsVector, UINT _ui_lightsVectorLength, int _i_lightsNumber );

	void setLights( const D3DXVECTOR4* _pV4_lightAmbient, const Light* const * _ppL_lights, int _i_lightsNumber, UINT _ui_frameLightTimeStamp );
	void setLights( const D3DXVECTOR4* _pV4_lightAmbient, const Light* const * _ppL_lights, int _i_lightsNumber );

};


class SkinnedLightedEffect : public virtual SkinnedEffect, public virtual LightedEffect
{
public:

	SkinnedLightedEffect( const string& _str_id, ID3DXEffect* _dx_pEffect, const string& _str_defaultTechniqueName );
};



#endif //_GRAPHIC_RESOURCE_EFFECT

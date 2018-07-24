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


		Technique( D3DXHANDLE _h_handle, DWORD _dw_passCount, const string& _str_name ) : h_handle(_h_handle), dw_passCount(_dw_passCount), str_name(_str_name)
		{};

		Technique( D3DXHANDLE _h_handle, DWORD _dw_passCount, LPCSTR _cstr_name ) : h_handle(_h_handle), dw_passCount(_dw_passCount)
		{
			str_name = string(_cstr_name);
		};
	};

	struct Variable
	{
		const D3DXHANDLE h_handle;
		D3DXPARAMETER_CLASS dx_class;
		D3DXPARAMETER_TYPE dx_type;
		string str_name;
		string str_semantic;


		Variable( D3DXHANDLE _h_handle, D3DXPARAMETER_CLASS _dx_class, D3DXPARAMETER_TYPE _dx_type, const string& _str_name, const string& _str_semantic ) : h_handle(_h_handle), dx_class(_dx_class), dx_type(_dx_type), str_name(_str_name), str_semantic(_str_semantic)
		{};

		Variable( D3DXHANDLE _h_handle, D3DXPARAMETER_CLASS _dx_class, D3DXPARAMETER_TYPE _dx_type, LPCSTR _cstr_name, LPCSTR _cstr_semantic ) : h_handle(_h_handle), dx_class(_dx_class), dx_type(_dx_type)
		{
			str_name = string(_cstr_name);
			str_semantic = string(_cstr_semantic);
		};
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

	BaseEffect( const string& _str_id, ID3DXEffect* _dx_pEffect, const string& _str_defaultTechniqueName ) : str_id(_str_id), ui_refCount(1), dx_pEffect(_dx_pEffect), dw_defaultTechniquePassCount(0)
	{
		HRESULT hr = S_OK;

		if ( _str_defaultTechniqueName.length() )
		{
			if ( !( h_defaultTechnique = dx_pEffect->GetTechniqueByName( _str_defaultTechniqueName.c_str() ) ) )
				throw ResourceErrorException( "BaseEffect", _str_id.c_str(), "technique not found" );

			if ( FAILED( hr = dx_pEffect->ValidateTechnique( h_defaultTechnique ) ) )
				throw D3DX9Exception( "BaseEffect::BaseEffect", "ID3DXEffect::ValidateTechnique", hr );
		}
		else
		{
			if ( FAILED( hr = dx_pEffect->FindNextValidTechnique( NULL, &h_defaultTechnique ) ) )
				throw D3DX9Exception( "BaseEffect::BaseEffect", "ID3DXEffect::FindNextValidTechnique", hr );

			if ( !h_defaultTechnique )
				throw ResourceErrorException( "BaseEffect", _str_id.c_str(), "no valid technique found" );
		}

		D3DXTECHNIQUE_DESC dx_techniqueDesc;
		if ( FAILED( hr = dx_pEffect->GetTechniqueDesc( h_defaultTechnique, &dx_techniqueDesc ) ) )
			throw D3DX9Exception( "BaseEffect::BaseEffect", "ID3DXEffect::GetTechniqueDesc", hr );

		dw_defaultTechniquePassCount = dx_techniqueDesc.Passes;

		pair< BaseEffectMap::iterator, bool > pMB_insertResult = m_baseEffectMap.insert( BaseEffectMap::value_type( &str_id, this ) );
		if ( !pMB_insertResult.second )
			throw ResourceDuplicateException( "BaseEffect", _str_id.c_str() );
	};

protected:

	virtual ~BaseEffect()
	{
		m_baseEffectMap.erase( &str_id );
		IDX_SAFE_RELEASE( dx_pEffect );
	};


public:

	inline const string* getId()	{	return &str_id;	};
	inline UINT getRefCount()	{	return ui_refCount;	};

	inline void addRef()		{	++ui_refCount;	};
	//inline void operator++()	{	++ui_refCount;	};
	//inline void operator++(int)	{	++ui_refCount;	};

	void release()
	{
		--ui_refCount;
		if ( !ui_refCount )
			delete this;
	};

	static BaseEffect* getBaseEffect( const string& _str_id )
	{
		#ifdef(MULTI_THREADED_LOADING)
			boost::mutex::scoped_lock lock( mx_baseEffectMapMutex );
		#endif //MULTI_THREADED_LOADING

		BaseEffectMap::iterator it = m_baseEffectMap.find( &_str_id );
		
		if ( it != m_baseEffectMap.end() )
		{
			it->second->addRef();
			return it->second;
		}

		BaseEffect* eff_pRequestedEffect = requestFunction( &_str_id );
		return eff_pRequestedEffect;
	};

	inline const ID3DXEffect* getDXEffect()				{	return dx_pEffect;	};
	inline const D3DXHANDLE getDefaultTechnique()		{	return h_defaultTechnique;	};
	inline const DWORD getDefaultTechniquePassCount()	{	return dw_defaultTechniquePassCount;	};

	virtual void reset()
	{
		HRESULT hr = dx_pEffect->SetTechnique( h_defaultTechnique );
		if ( FAILED( hr ) )
			throw D3DX9Exception( "BaseEffect::reset", "ID3DXEffect::SetTechnique", hr );
	};

};


class SystemEffect : public BaseEffect
{
protected:

	vector<Technique> v_techniques;
	vector<Variable> v_variables;


public:

	SystemEffect( const string& _str_id, ID3DXEffect* _dx_pEffect, const string& _str_defaultTechniqueName ) : BaseEffect( _str_id, _dx_pEffect, _str_defaultTechniqueName )
	{
		HRESULT hr = S_OK;
		UINT i = 0;
		D3DXHANDLE h_tmpHandle = NULL;
		D3DXTECHNIQUE_DESC dx_tmpTechniqueDesc;
		D3DXPARAMETER_DESC dx_tmpParameterDesc;
		
		while ( h_tmpHandle = dx_pEffect->GetTechnique( i++ ) )
		{
			if ( FAILED( hr = dx_pEffect->GetTechniqueDesc( h_tmpHandle, &dx_tmpTechniqueDesc ) ) )
				throw D3DX9Exception( "SystemEffect::SystemEffect", "ID3DXEffect::GetTechniqueDesc", hr );

			v_techniques.push_back( Technique( h_tmpHandle, dx_tmpTechniqueDesc.Passes, dx_tmpTechniqueDesc.Name ) );
		}

		i = 0;
		while ( h_tmpHandle = dx_pEffect->GetParameter( NULL, i++ ) )
		{
			if ( FAILED( hr = dx_pEffect->GetParameterDesc( h_tmpHandle, &dx_tmpParameterDesc ) ) )
				throw D3DX9Exception( "SystemEffect::SystemEffect", "ID3DXEffect::GetParameterDesc", hr );

			v_variables.push_back( Variable( h_tmpHandle, dx_tmpParameterDesc.Class, dx_tmpParameterDesc.Type, dx_tmpParameterDesc.Name, dx_tmpParameterDesc.Semantic ) );
		}
	};

	UINT getTechniqueIndexByName( const string& _str_name )
	{
		for ( UINT i = 0; i < v_techniques.size(); ++i )
		{
			if ( !v_techniques[i].str_name.compare( _str_name ) )
				return i;
		}
		throw ResourceErrorException( "SystemEffect", str_id.c_str(), "technique not found" );
	};

	const Technique* getTechniqueDesc( UINT _ui_index )
	{
		return &(v_techniques[_ui_index]);
	};

	const Technique* getTechniqueDescByName( const string& _str_name )
	{
		for ( UINT i = 0; i < v_techniques.size(); ++i )
		{
			if ( !v_techniques[i].str_name.compare( _str_name ) )
				return &(v_techniques[i]);
		}
		throw ResourceErrorException( "SystemEffect", str_id.c_str(), "technique not found" );
	};

	const D3DXHANDLE getTechniqueHandle( UINT _ui_index )
	{
		return v_techniques[_ui_index].h_handle;
	};

	const D3DXHANDLE getTechniqueHandleByName( const string& _str_name )
	{
		for ( UINT i = 0; i < v_techniques.size(); ++i )
		{
			if ( !v_techniques[i].str_name.compare( _str_name ) )
				return v_techniques[i].h_handle;
		}
		throw ResourceErrorException( "SystemEffect", str_id.c_str(), "technique not found" );
	};

	UINT getVariableIndexByName( const string& _str_name )
	{
		for ( UINT i = 0; i < v_variables.size(); ++i )
		{
			if ( !v_variables[i].str_name.compare( _str_name ) )
				return i;
		}
		throw ResourceErrorException( "SystemEffect", str_id.c_str(), "technique not found" );
	};

	UINT getVariableIndexBySemantic( const string& _str_semantic )
	{
		for ( UINT i = 0; i < v_variables.size(); ++i )
		{
			if ( !v_variables[i].str_semantic.compare( _str_semantic ) )
				return i;
		}
		throw ResourceErrorException( "SystemEffect", str_id.c_str(), "technique not found" );
	};

	const Variable* getVariableDesc( UINT _ui_index )
	{
		return &(v_variables[_ui_index]);
	};

	const Variable* getVariableDescByName( const string& _str_name )
	{
		for ( UINT i = 0; i < v_variables.size(); ++i )
		{
			if ( !v_variables[i].str_name.compare( _str_name ) )
				return &(v_variables[i]);
		}
		throw ResourceErrorException( "SystemEffect", str_id.c_str(), "technique not found" );
	};

	const Variable* getVariableDescBySemantic( const string& _str_semantic )
	{
		for ( UINT i = 0; i < v_variables.size(); ++i )
		{
			if ( !v_variables[i].str_semantic.compare( _str_semantic ) )
				return &(v_variables[i]);
		}
		throw ResourceErrorException( "SystemEffect", str_id.c_str(), "technique not found" );
	};

	const D3DXHANDLE getVariableHandle( UINT _ui_index )
	{
		return v_variables[_ui_index].h_handle;
	};

	const D3DXHANDLE getVariableHandleByName( const string& _str_name )
	{
		for ( UINT i = 0; i < v_variables.size(); ++i )
		{
			if ( !v_variables[i].str_name.compare( _str_name ) )
				return v_variables[i].h_handle;
		}
		throw ResourceErrorException( "SystemEffect", str_id.c_str(), "technique not found" );
	};

	const D3DXHANDLE getVariableHandleBySemantic( const string& _str_semantic )
	{
		for ( UINT i = 0; i < v_variables.size(); ++i )
		{
			if ( !v_variables[i].str_semantic.compare( _str_semantic ) )
				return v_variables[i].h_handle;
		}
		throw ResourceErrorException( "SystemEffect", str_id.c_str(), "technique not found" );
	};

	void setTechnique( UINT _ui_index )
	{
		if ( FAILED( hr = dx_pEffect->SetTechnique( v_techniques[_ui_index].h_handle ) ) )
			throw D3DX9Exception( "SystemEffect::setTechnique", "ID3DXEffect::SetTechnique", hr );
	};

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

	StandardEffect( const string& _str_id, ID3DXEffect* _dx_pEffect, const string& _str_defaultTechniqueName ) : BaseEffect( _str_id, _dx_pEffect, _str_defaultTechniqueName ), h_mxWorld(NULL), h_mxView(NULL), h_mxProjection(NULL), h_mxViewProjection(NULL), h_mxViewInverse(NULL), ui_frameTimeStamp(-1), h_v3MrtMaterial(NULL), h_flSpecularIntensity(NULL), h_flSpecularPower(NULL), h_flEmissiveFactor(NULL)
	{
		v_textureHandles.resize(8);

		HRESULT hr = S_OK;
		UINT i = 0;
		D3DXHANDLE h_tmpHandle = NULL;
		D3DXPARAMETER_DESC dx_tmpParameterDesc;

		while ( h_tmpHandle = dx_pEffect->GetParameter( NULL, i++ ) )
		{
			if ( FAILED( hr = dx_pEffect->GetParameterDesc( h_tmpHandle, &dx_tmpParameterDesc ) ) )
				throw D3DX9Exception( "StandardEffect::StandardEffect", "ID3DXEffect::GetParameterDesc", hr );


			string tmpSemantic = dx_tmpParameterDesc.Semantic;

			if ( !tmpSemantic.compare( "WORLD" ) )					{	h_mxWorld = h_tmpHandle;			continue;	}
			if ( !tmpSemantic.compare( "VIEW" ) )					{	h_mxView = h_tmpHandle;				continue;	}
			if ( !tmpSemantic.compare( "PROJECTION" ) )				{	h_mxProjection = h_tmpHandle;		continue;	}
			if ( !tmpSemantic.compare( "VIEWPROJECTION" ) )			{	h_mxViewProjection = h_tmpHandle;	continue;	}
			if ( !tmpSemantic.compare( "VIEWINVERSE" ) )			{	h_mxViewInverse = h_tmpHandle;		continue;	}


			string tmpName = dx_tmpParameterDesc.Name;

			if ( !tmpName.compare( EFFECT_VAR_TEXTURE_0 ) )			{	v_textureHandles[0] = h_tmpHandle;	continue;	}
			if ( !tmpName.compare( EFFECT_VAR_TEXTURE_1 ) )			{	v_textureHandles[1] = h_tmpHandle;	continue;	}
			if ( !tmpName.compare( EFFECT_VAR_TEXTURE_2 ) )			{	v_textureHandles[2] = h_tmpHandle;	continue;	}
			if ( !tmpName.compare( EFFECT_VAR_TEXTURE_3 ) )			{	v_textureHandles[3] = h_tmpHandle;	continue;	}
			if ( !tmpName.compare( EFFECT_VAR_TEXTURE_4 ) )			{	v_textureHandles[4] = h_tmpHandle;	continue;	}
			if ( !tmpName.compare( EFFECT_VAR_TEXTURE_5 ) )			{	v_textureHandles[5] = h_tmpHandle;	continue;	}
			if ( !tmpName.compare( EFFECT_VAR_TEXTURE_6 ) )			{	v_textureHandles[6] = h_tmpHandle;	continue;	}
			if ( !tmpName.compare( EFFECT_VAR_TEXTURE_7 ) )			{	v_textureHandles[7] = h_tmpHandle;	continue;	}


			if ( !tmpName.compare( EFFECT_VAR_V3_MRT_MATERIAL ) )		{	h_v3MrtMaterial = h_tmpHandle;			continue;	}
			if ( !tmpName.compare( EFFECT_VAR_FL_SPECULAR_INTENSITY ) )	{	h_flSpecularIntensity = h_tmpHandle;	continue;	}
			if ( !tmpName.compare( EFFECT_VAR_FL_SPECULAR_POWER ) )		{	h_flSpecularPower = h_tmpHandle;		continue;	}
			if ( !tmpName.compare( EFFECT_VAR_FL_EMISSIVE_FACTOR ) )	{	h_flEmissiveFactor = h_tmpHandle;		continue;	}


			v_variables.push_back( Variable( h_tmpHandle, dx_tmpParameterDesc.Class, dx_tmpParameterDesc.Type, dx_tmpParameterDesc.Name, dx_tmpParameterDesc.Semantic ) );
		}


		int i = v_textureHandles.size();
		while ( ! v_textureHandles[ i-- ] );
		v_textureHandles.resize( i + 1 );

		if ( h_v3MrtMaterial )
		{
			h_flSpecularIntensity = dx_pEffect->GetParameterElement( h_v3MrtMaterial, 0 );
			h_flSpecularPower = dx_pEffect->GetParameterElement( h_v3MrtMaterial, 1 );
			h_flEmissiveFactor = dx_pEffect->GetParameterElement( h_v3MrtMaterial, 2 );
		}
	};

	void setViewProjection( const D3DXMATRIX* _pMx_view, const D3DXMATRIX* _pMx_projection, const D3DXMATRIX* _pMx_viewProjection, const D3DXMATRIX* _pMx_viewInverse, UINT _ui_frameTimeStamp )
	{
		if ( ui_frameTimeStamp != _ui_frameTimeStamp )
		{
			HRESULT hr = S_OK;
			if ( FAILED( hr = dx_pEffect->SetMatrix( h_mxView, _pMx_view ) ) || 
				 FAILED( hr = dx_pEffect->SetMatrix( h_mxProjection, _pMx_projection ) ) || 
				 FAILED( hr = dx_pEffect->SetMatrix( h_mxViewProjection, _pMx_viewProjection ) ) || 
				 FAILED( hr = dx_pEffect->SetMatrix( h_mxViewInverse, _pMx_viewInverse ) ) )
				throw D3DX9Exception( "StandardEffect::setViewProjection", "ID3DXEffect::SetMatrix", hr );

			ui_frameTimeStamp = _ui_frameTimeStamp;
		}
	};

	void setViewProjection( const D3DXMATRIX* _pMx_view, const D3DXMATRIX* _pMx_projection, const D3DXMATRIX* _pMx_viewProjection, const D3DXMATRIX* _pMx_viewInverse )
	{
		HRESULT hr = S_OK;
		if ( FAILED( hr = dx_pEffect->SetMatrix( h_mxView, _pMx_view ) ) || 
			 FAILED( hr = dx_pEffect->SetMatrix( h_mxProjection, _pMx_projection ) ) || 
			 FAILED( hr = dx_pEffect->SetMatrix( h_mxViewProjection, _pMx_viewProjection ) ) || 
			 FAILED( hr = dx_pEffect->SetMatrix( h_mxViewInverse, _pMx_viewInverse ) ) )
			throw D3DX9Exception( "StandardEffect::setViewProjection", "ID3DXEffect::SetMatrix", hr );
	};

	void setWorld( const D3DXMATRIX* _pMx_world )
	{
		HRESULT hr = dx_pEffect->SetMatrix( h_mxWorld, _pMx_world );
		if ( FAILED( hr ) )
			throw D3DX9Exception( "StandardEffect::setWorld", "ID3DXEffect::SetMatrix", hr );
	};

	void setTexture( BYTE _b_index, const IDirect3DTexture9* _pTx_texture )
	{
		HRESULT hr = dx_pEffect->SetTexture( v_textureHandles[_b_index], _pTx_texture );
		if ( FAILED( hr ) )
			throw D3DX9Exception( "StandardEffect::setTexture", "ID3DXEffect::SetTexture", hr );
	};

	void setMaterial( const D3DXVECTOR3* _pV3_material )
	{
		if ( h_v3MrtMaterial )
		{
			HRESULT hr = dx_pEffect->SetValue( h_v3MrtMaterial, _pV3_material, D3DX_DEFAULT );
			if ( FAILED( hr ) )
				throw D3DX9Exception( "StandardEffect::setMaterial", "ID3DXEffect::SetValue", hr );
		}
		else
		{
			HRESULT hr;
			if ( FAILED( hr = dx_pEffect->SetFloat( h_flSpecularIntensity, _pV3_material.x ) ) || 
				 FAILED( hr = dx_pEffect->SetFloat( h_flSpecularPower, _pV3_material.y ) ) || 
				 FAILED( hr = dx_pEffect->SetFloat( h_flEmissiveFactor, _pV3_material.z ) ) )
				throw D3DX9Exception( "StandardEffect::setMaterial", "ID3DXEffect::SetFloat", hr );
		}
	};

	void setMaterial( float _fl_specularIntensity, float _fl_specularPower, float _fl_emissiveFactor )
	{
		HRESULT hr;
		if ( FAILED( hr = dx_pEffect->SetFloat( h_flSpecularIntensity, _fl_specularIntensity ) ) || 
			 FAILED( hr = dx_pEffect->SetFloat( h_flSpecularPower, _fl_specularPower ) ) || 
			 FAILED( hr = dx_pEffect->SetFloat( h_flEmissiveFactor, _fl_emissiveFactor ) ) )
			throw D3DX9Exception( "StandardEffect::setMaterial", "ID3DXEffect::SetFloat", hr );
	};

	UINT getVariableIndexByName( const string& _str_name )
	{
		for ( UINT i = 0; i < v_variables.size(); ++i )
		{
			if ( !v_variables[i].str_name.compare( _str_name ) )
				return i;
		}
		throw ResourceErrorException( "SystemEffect", str_id.c_str(), "technique not found" );
	};

	UINT getVariableIndexBySemantic( const string& _str_semantic )
	{
		for ( UINT i = 0; i < v_variables.size(); ++i )
		{
			if ( !v_variables[i].str_semantic.compare( _str_semantic ) )
				return i;
		}
		throw ResourceErrorException( "SystemEffect", str_id.c_str(), "technique not found" );
	};

	const Variable* getVariableDesc( UINT _ui_index )
	{
		return &(v_variables[_ui_index]);
	};

	void setValue( UINT _ui_index, const void* _vp_data, UINT _ui_size )
	{
		HRESULT hr;
		if ( FAILED( hr = dx_pEffect->SetValue( v_variables[_ui_index].h_handle, _vp_data, _ui_size ) ) )
			throw D3DX9Exception( "StandardEffect::setMaterial", "ID3DXEffect::SetValue", hr );
	};

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

	SkinnedEffect( const string& _str_id, ID3DXEffect* _dx_pEffect, const string& _str_defaultTechniqueName ) : StandardEffect( _str_id, _dx_pEffect, _str_defaultTechniqueName ), ui_totalPaletteSize(0), h_mxSkinningPalette(NULL), h_iVertexInfluencesNumber(NULL)
	{
		vector<Variable>::iterator it_variable = v_variables.begin();

		while ( it_variable != v_variables.end() )
		{
			if ( !(*it_variable).str_name.compare( EFFECT_VAR_TOTAL_PALETTE_SIZE ) )
			{
				if ( FAILED( hr = dx_pEffect->GetInt( (*it_variable).h_handle, &ui_totalPaletteSize ) ) )
					throw D3DX9Exception( "SkinnedEffect::SkinnedEffect", "ID3DXEffect::GetInt", hr );
				it_variable = v_variables.erase( it_variable );
				continue;
			}

			if ( !(*it_variable).str_name.compare( EFFECT_VAR_SKINNING_PALETTE ) )
			{
				h_mxSkinningPalette = (*it_variable).h_handle;
				it_variable = v_variables.erase( it_variable );
				continue;
			}

			if ( !(*it_variable).str_name.compare( EFFECT_VAR_VERT_INFL_NUMBER ) )
			{
				h_iVertexInfluencesNumber = (*it_variable).h_handle;
				it_variable = v_variables.erase( it_variable );
				continue;
			}

			++it_variable;
		}
	};

	inline UINT getTotalPaletteSize()	{	return ui_totalPaletteSize;	};

	void setSkinningPalette( const D3DXMATRIX* _pMx_skinningPalette, UINT _ui_skinningPaletteLength, INT _i_vertexInfluencesNumber )
	{
		HRESULT hr = S_OK;

		if ( FAILED( hr = dx_pEffect->SetMatrixArray( h_mxSkinningPalette, _pMx_skinningPalette, _ui_skinningPaletteLength ) ) )
			throw D3DX9Exception( "SkinnedEffect::setSkinningPalette", "ID3DXEffect::SetMatrixArray", hr );

		if ( FAILED( hr = dx_pEffect->SetInt( h_uiVertexInfluencesNumber, _i_vertexInfluencesNumber ) ) )
			throw D3DX9Exception( "SkinnedEffect::setSkinningPalette", "ID3DXEffect::SetInt", hr );
	};

	void setSkinningPalette( const D3DXMATRIX** _ppMx_skinningPalette, UINT _ui_skinningPaletteLength, INT _i_vertexInfluencesNumber )
	{
		HRESULT hr = S_OK;

		if ( FAILED( hr = dx_pEffect->SetMatrixPointerArray( h_mxSkinningPalette, _ppMx_skinningPalette, _ui_skinningPaletteLength ) ) )
			throw D3DX9Exception( "SkinnedEffect::setSkinningPalette", "ID3DXEffect::SetMatrixPointerArray", hr );

		if ( FAILED( hr = dx_pEffect->SetInt( h_uiVertexInfluencesNumber, _i_vertexInfluencesNumber ) ) )
			throw D3DX9Exception( "SkinnedEffect::setSkinningPalette", "ID3DXEffect::SetInt", hr );
	};

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

	LightedEffect( const string& _str_id, ID3DXEffect* _dx_pEffect, const string& _str_defaultTechniqueName ) : StandardEffect( _str_id, _dx_pEffect, _str_defaultTechniqueName ), h_v4LightAmbient(NULL), ui_totalLightsVectorSize(0), h_v4LightsVector(NULL), h_iLightsNumbers(NULL), ui_frameLightTimeStamp(-1)
	{
		vector<Variable>::iterator it_variable = v_variables.begin();

		while ( it_variable != v_variables.end() )
		{
			if ( !(*it_variable).str_name.compare( EFFECT_VAR_V4_LIGHT_AMBIENT ) )
			{
				h_v4LightAmbient = (*it_variable).h_handle;
				it_variable = v_variables.erase( it_variable );
				continue;
			}

			if ( !(*it_variable).str_name.compare( EFFECT_VAR_LIGHTS_VECTOR_SIZE ) )
			{
				if ( FAILED( hr = dx_pEffect->GetInt( (*it_variable).h_handle, &ui_totalLightsVectorSize ) ) )
					throw D3DX9Exception( "SkinnedEffect::SkinnedEffect", "ID3DXEffect::GetInt", hr );
				it_variable = v_variables.erase( it_variable );
				continue;
			}

			if ( !(*it_variable).str_name.compare( EFFECT_VAR_V4_LIGHTS_VECTOR ) )
			{
				h_v4LightsVector = (*it_variable).h_handle;
				it_variable = v_variables.erase( it_variable );
				continue;
			}

			if ( !(*it_variable).str_name.compare( EFFECT_VAR_LIGHTS_NUMBER ) )
			{
				h_iLightsNumbers = (*it_variable).h_handle;
				it_variable = v_variables.erase( it_variable );
				continue;
			}

			++it_variable;
		}
	};

	inline UINT getTotalLightsVectorSize()	{	return ui_totalLightsVectorSize;	};

	void setLights( const D3DXVECTOR4* _pV4_lightAmbient, const D3DXVECTOR4* _pV4_lightsVector, UINT _ui_lightsVectorLength, int _i_lightsNumber, UINT _ui_frameLightTimeStamp )
	{
		if ( ui_frameLightTimeStamp != _ui_frameLightTimeStamp )
		{
			HRESULT hr = S_OK;

			if ( FAILED( hr = dx_pEffect->SetVector( h_v4LightAmbient, _pV4_lightAmbient ) ) )
				throw D3DX9Exception( "LightedEffect::setLights", "ID3DXEffect::SetVector", hr );

			if ( _i_lightsNumber )
			{
				if ( ui_totalLightsVectorSize > _ui_lightsVectorLength )
					throw ApplicationException( "LightedEffect::setLights lights vector too big" );

				if ( FAILED( hr = dx_pEffect->SetVectorArray( h_v4LightsVector, _pV4_lightsVector, _ui_lightsVectorLength ) ) )
					throw D3DX9Exception( "LightedEffect::setLights", "ID3DXEffect::SetVectorArray", hr );
			}

			if ( FAILED( hr = dx_pEffect->SetInt( h_iLightsNumbers, _i_lightsNumber ) ) )
				throw D3DX9Exception( "LightedEffect::setLights", "ID3DXEffect::SetInt", hr );

			ui_frameLightTimeStamp = _ui_frameLightTimeStamp;
		}
	};

	void setLights( const D3DXVECTOR4* _pV4_lightAmbient, const D3DXVECTOR4* _pV4_lightsVector, UINT _ui_lightsVectorLength, int _i_lightsNumber )
	{
		HRESULT hr = S_OK;

		if ( FAILED( hr = dx_pEffect->SetVector( h_v4LightAmbient, _pV4_lightAmbient ) ) )
			throw D3DX9Exception( "LightedEffect::setLights", "ID3DXEffect::SetVector", hr );

		if ( _i_lightsNumber )
		{
			if ( ui_totalLightsVectorSize > _ui_lightsVectorLength )
				throw ApplicationException( "LightedEffect::setLights lights vector too big" );

			if ( FAILED( hr = dx_pEffect->SetVectorArray( h_v4LightsVector, _pV4_lightsVector, _ui_lightsVectorLength ) ) )
				throw D3DX9Exception( "LightedEffect::setLights", "ID3DXEffect::SetVectorArray", hr );
		}

		if ( FAILED( hr = dx_pEffect->SetInt( h_iLightsNumbers, _i_lightsNumber ) ) )
			throw D3DX9Exception( "LightedEffect::setLights", "ID3DXEffect::SetInt", hr );
	};

	void setLights( const D3DXVECTOR4* _pV4_lightAmbient, const Light* const * _ppL_lights, int _i_lightsNumber, UINT _ui_frameLightTimeStamp )
	{
		if ( ui_frameLightTimeStamp != _ui_frameLightTimeStamp )
		{
			HRESULT hr = S_OK;

			if ( FAILED( hr = dx_pEffect->SetVector( h_v4LightAmbient, _pV4_lightAmbient ) ) )
				throw D3DX9Exception( "LightedEffect::setLights", "ID3DXEffect::SetVector", hr );

			UINT ui_arrayLength = 0;
			UINT ui_setupLength = 0;
			D3DXVECTOR4* pV4_lightArray = NULL;
			int i_lightsNumber;
			for ( i_lightsNumber = 0; i_lightsNumber < _i_lightsNumber; ++i_lightsNumber )
			{
				pV4_lightArray = _ppL_lights[i_lightsNumber]->getLightArray( &ui_arrayLength );
				if ( ui_setupLength + ui_arrayLength > ui_lightsVectorSize )	continue;

				if ( FAILED( hr = dx_pEffect->SetRawValue( h_v4LightsVector, pV4_lightArray, ui_setupLength * sizeof(D3DXVECTOR4), ui_arrayLength * sizeof(D3DXVECTOR4) ) ) )
					throw D3DX9Exception( "LightedEffect::setLights", "ID3DXEffect::SetRawValue", hr );

				ui_setupLength += ui_arrayLength;
			}

			if ( FAILED( hr = dx_pEffect->SetInt( h_iLightsNumbers, i_lightsNumber ) ) )
				throw D3DX9Exception( "LightedEffect::setLights", "ID3DXEffect::SetInt", hr );

			ui_frameLightTimeStamp = _ui_frameLightTimeStamp;
		}
	};

	void setLights( const D3DXVECTOR4* _pV4_lightAmbient, const Light* const * _ppL_lights, int _i_lightsNumber )
	{
		HRESULT hr = S_OK;

		if ( FAILED( hr = dx_pEffect->SetVector( h_v4LightAmbient, _pV4_lightAmbient ) ) )
			throw D3DX9Exception( "LightedEffect::setLights", "ID3DXEffect::SetVector", hr );

		UINT ui_arrayLength = 0;
		UINT ui_setupLength = 0;
		D3DXVECTOR4* pV4_lightArray = NULL;
		int i_lightsNumber;
		for ( i_lightsNumber = 0; i_lightsNumber < _i_lightsNumber; ++i_lightsNumber )
		{
			pV4_lightArray = _ppL_lights[i_lightsNumber]->getLightArray( &ui_arrayLength );
			if ( ui_setupLength + ui_arrayLength > ui_lightsVectorSize )	continue;

			if ( FAILED( hr = dx_pEffect->SetRawValue( h_v4LightsVector, pV4_lightArray, ui_setupLength * sizeof(D3DXVECTOR4), ui_arrayLength * sizeof(D3DXVECTOR4) ) ) )
				throw D3DX9Exception( "LightedEffect::setLights", "ID3DXEffect::SetRawValue", hr );

			ui_setupLength += ui_arrayLength;
		}

		if ( FAILED( hr = dx_pEffect->SetInt( h_iLightsNumbers, i_lightsNumber ) ) )
			throw D3DX9Exception( "LightedEffect::setLights", "ID3DXEffect::SetInt", hr );
	};

};


class SkinnedLightedEffect : public virtual SkinnedEffect, public virtual LightedEffect
{
public:

	SkinnedLightedEffect( const string& _str_id, ID3DXEffect* _dx_pEffect, const string& _str_defaultTechniqueName ) : StandardEffect( _str_id, _dx_pEffect, _str_defaultTechniqueName ), SkinnedEffect( _str_id, _dx_pEffect, _str_defaultTechniqueName ), LightedEffect( _str_id, _dx_pEffect, _str_defaultTechniqueName )
	{};
};



#endif //_GRAPHIC_RESOURCE_EFFECT

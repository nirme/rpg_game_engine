#pragma once

#ifndef _RESOURCE_GRAPHIC_TECHNIQUE
#define _RESOURCE_GRAPHIC_TECHNIQUE

#include "utility.h"

#include "Core_Defines.h"
#include "d3d9types.h"
#include "Base_Exceptions.h"

#include "Resource_Graphic_Shader_Constant_Description.h"
#include "Graphic_Device_State_Manager.h"



namespace Renderer
{
	//Forward declarations
	class Renderer;
	class PixelShaderContainer;
	class VertexShaderContainer;



	class PixelShaderContainer
	{
		friend Technique;

	private:
		Renderer* pR_weakPtrRenderer;

		string str_name;
		IDirect3DPixelShader9* pD9PS_pixelShader;
		vector<ShaderConstantDescription*> v_pSCD_shaderConstants;

		UINT ui_refCount;

		bool b_loaded;


		~PixelShaderContainer();

	public:

		PixelShaderContainer( Renderer* _pR_weakPtrRenderer, const string& _str_name );
		PixelShaderContainer( Renderer* _pR_weakPtrRenderer, const string& _str_name, IDirect3DPixelShader9* _pD9PS_pixelShader, UINT _ui_constDescNumber, const ShaderConstantDescription** _pSCD_desc );

		void setup( IDirect3DPixelShader9* _pD9PS_pixelShader, UINT _ui_constDescNumber, const ShaderConstantDescription** _pSCD_desc );

		const string* getNamePtr();

		void addRef();
		void release();
	};



	class VertexShaderContainer
	{
		friend Technique;

	private:
		Renderer* pR_weakPtrRenderer;

		string str_name;
		IDirect3DVertexShader9* pD9PS_vertexShader;
		vector<ShaderConstantDescription*> v_pSCD_shaderConstants;

		UINT ui_refCount;

		bool b_loaded;


		~VertexShaderContainer();

	public:

		VertexShaderContainer( Renderer* _pR_weakPtrRenderer, const string& _str_name );
		VertexShaderContainer( Renderer* _pR_weakPtrRenderer, const string& _str_name, IDirect3DVertexShader9* _pD9PS_vertexShader, UINT _ui_constDescNumber, const ShaderConstantDescription** _pSCD_desc );

		void setup( IDirect3DVertexShader9* _pD9PS_vertexShader, UINT _ui_constDescNumber, const ShaderConstantDescription** _pSCD_desc );

		const string* getNamePtr();

		void addRef();
		void release();
	};



	struct Pass
	{
		VertexShaderContainer* pVSC_vertexShader;
		PixelShaderContainer* pPSC_pixelShader;

		vector<RENDER_STATE> v_RS_renderStates;
		vector<SAMPLER_DEFINITION> v_SD_samplersDefinition;


		Pass( VertexShaderContainer* _pVSC_vertexShader, PixelShaderContainer* _pPSC_pixelShader, UINT _ui_renderStatesNumber, const RENDER_STATE* _pRS_renderStates, UINT _ui_samplersDefinitionsNumber, const SAMPLER_DEFINITION* _pSD_samplersDefinition ) : pVSC_vertexShader( _pVSC_vertexShader ), pPSC_pixelShader( _pPSC_pixelShader );
		~Pass();
	};



	class Technique
	{
		friend Renderer;

		typedef		vector< ShaderConstantDescription* >		ShaderVariableVector;
		typedef		map< string*, ShaderConstantDescription*, p_scompare >		ShaderVariableMap;

	private:
		Renderer* pR_weakPtrRenderer;

		string str_name;

		vector < Pass* > v_pPasses;

		ShaderVariableVector svv_shaderConstants;
		ShaderVariableMap svm_shaderConstants;

		UINT ui_refCount;
		bool b_loaded;

		UINT ui_renderQueueIndex;


		~Technique();

	public:

		Technique( Renderer* _pR_weakPtrRenderer, const string& _str_name, UINT _ui_renderQueueIndex );
		Technique( Renderer* _pR_weakPtrRenderer, const string& _str_name, UINT _ui_renderQueueIndex, UINT _ui_passesNumber, const Pass** _p_ppPasses );

		void setup( UINT _ui_passesNumber, const Pass** _p_ppPasses );

		UINT getPassesNumber();
		void beginPass( UINT _ui_pass );

		const string* getNamePtr();
		void addRef();
		void release();

		inline bool isLoaded()				{	return b_loaded;	};
		inline UINT getRenderQueueIndex()	{	return ui_renderQueueIndex;	};

		ShaderConstantDescription* getShaderVariableByName( const string& _str_name );
		ShaderConstantDescription* getShaderVariableByIndex( UINT _ui_index );
	};

}


#endif //_RESOURCE_GRAPHIC_TECHNIQUE

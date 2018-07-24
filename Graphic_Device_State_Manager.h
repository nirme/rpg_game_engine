#pragma once

#ifndef _GRAPHIC_DEVICE_STATE_MANAGER
#define _GRAPHIC_DEVICE_STATE_MANAGER

#include "utility.h"

#include "Core_Defines.h"
#include "d3d9types.h"
#include "Base_Exceptions.h"
#include "Resource_Graphic_Shader_Constant_Description.h"




namespace Renderer
{
	//Forward declarations
	class DeviceStateManager;


	#define		RENDER_STATES_NUMBER		103
	#define		SAMPLER_STATES_NUMBER		13
	//#define	SAMPLERS_NUMBER		16
	// we have 16 samplers but only 8 textures can be attached to them per pass
	// will need to modify it in the future:
	// - add full 16 samplers / 8 textures usage
	// - add 4 sampler as it can be used by vertex shader from VS3.0 - pretty much useless without geometry shader -  great for tesselation on geometry shader / height maps on VS
	#define		SAMPLERS_NUMBER				8

	// dx9 provide up to 16 streams but using more than 2 is very rare and might decrease rendering efficiency
	#define		STREAMS_NUMBER				4


	struct RENDER_STATE
	{
		D3DRENDERSTATETYPE state;
		DWORD value;
	};

	struct SAMPLER_STATE
	{
		D3DSAMPLERSTATETYPE state;
		DWORD value;
	};

	struct SAMPLER_DEFINITION
	{
		DWORD dw_samplerIndex;
		union
		{
			DWORD dw_values[ SAMPLER_STATES_NUMBER ];
			struct
			{
				DWORD dw_addressu;
				DWORD dw_addressv;
				DWORD dw_addressw;
				DWORD dw_bordercolor;
				DWORD dw_magfilter;
				DWORD dw_minfilter;
				DWORD dw_mipfilter;
				DWORD dw_mipmaplodbias;
				DWORD dw_maxmiplevel;
				DWORD dw_maxanisotropy;
				DWORD dw_srgbtexture;
				DWORD dw_elementindex;
				DWORD dw_dmapoffset;
			};
		};

		bool b_valuesUsed[ SAMPLER_STATES_NUMBER ];
	};



	class DeviceStateManager
	{
		typedef struct STREAM_DEFINITION
		{
		public:
			UINT ui_streamNumber;

			IDirect3DVertexBuffer9* pDxVB_streamData;

			UINT ui_stride;
			UINT ui_frequencyParameter;
			bool b_geometryIndexed;
			bool b_instanceData;
			UINT ui_frequencySetValue;
		};

	private:

		IDirect3DDevice9* pD3D9_device;

		static const BYTE by_renderStateUsedIndexes[ /*RENDER_STATES_NUMBER*/ ];

		// base state setup at start
		DWORD pDw_renderStateBaseValues[ 255 ];
		bool pB_renderStateUsedStates[ 255 ];
		DWORD ppDw_samplerStateBaseValues[ SAMPLERS_NUMBER ][ SAMPLER_STATES_NUMBER ];


		// changes of states before applying state change
		// render states
		DWORD pDw_renderStateModificationValues[ 255 ];
		list<BYTE>::iterator plBtIt_renderStateModificationFlag[ 255 ];
		list<BYTE> lBt_renderStateModifiedIndexes;

		// sampler states
		DWORD ppDw_samplerStateModificationValues[ SAMPLERS_NUMBER ][ SAMPLER_STATES_NUMBER ];
		list< BYTE >::iterator plBtIt_samplerModifiedIndexes[ SAMPLERS_NUMBER ];
		list< BYTE >::iterator pplBtIt_samplerStateModifiedIndexes[ SAMPLERS_NUMBER ][ SAMPLER_STATES_NUMBER ];
		list< BYTE > lBt_samplerModifiedIndexes;
		list< BYTE > pLBt_samplerStateModifiedIndexes[ SAMPLERS_NUMBER ];


		// actual state
		// render states
		DWORD dw_renderStateActualValues[ 255 ];
		// sampler states
		DWORD ppDw_samplerStateActualValues[ SAMPLERS_NUMBER ][ SAMPLER_STATES_NUMBER ];

		// samplers textures
		IDirect3DBaseTexture9* pDxTx_samplerActualTexture[ SAMPLERS_NUMBER ];

		// render pipe states
		IDirect3DVertexShader9* pDxVS_vertexShaderActual;
		IDirect3DPixelShader9* pDxPS_pixelShaderActual;


		// rendered data state
		// Rendering vertices declaration
		IDirect3DVertexDeclaration9* pDxVD_vertexDeclarationActual;
		// Rendering vertices indexes state
		IDirect3DIndexBuffer9* pDxIB_indexDataActual;
		// rendered streams state
		STREAM_DEFINITION pSD_StreamsDefinitionActual[ STREAMS_NUMBER ];


		// shader constants settings
		typedef void ( DeviceStateManager::*ApplySCMethod ) ( const ShaderConstantDescription* /*_pSCD_constDesc*/, const void* /*_pV_value*/ );
		typedef void ( DeviceStateManager::*ApplySCMethodAd ) ( const ShaderConstantDescription* /*_pSCD_constDesc*/, const void* /*_pV_value*/, UINT /*_ui_offset*/, UINT /*_ui_lengthVec4f / BOOL num*/ );

		typedef HRESULT ( __stdcall IDirect3DDevice9::*SetShaderConstantF ) ( UINT /*StartRegister*/, const float* /*pConstantData*/, UINT /*Vector4fCount*/);
		typedef HRESULT ( __stdcall IDirect3DDevice9::*SetShaderConstantI ) ( UINT /*StartRegister*/, const int* /*pConstantData*/, UINT /*Vector4fCount*/);
		typedef HRESULT ( __stdcall IDirect3DDevice9::*SetShaderConstantB ) ( UINT /*StartRegister*/, const BOOL* /*pConstantData*/, UINT /*BoolCount*/);

		static const ApplySCMethod applySCMethods[ 2 ][ 3 ];
		static const ApplySCMethodAd applySCMethodAd[ 2 ][ 3 ];

		static const SetShaderConstantF applySCMethodsF[ /*2*/ ];
		static const SetShaderConstantI applySCMethodsI[ /*2*/ ];
		static const SetShaderConstantB applySCMethodsB[ /*2*/ ];


	public:

		DeviceStateManager( IDirect3DDevice9* _pD3D9_device );
		~DeviceStateManager();

		void setBaseRenderState( D3DRENDERSTATETYPE _dxRst_renderStateType, DWORD _dw_value );
		void setBaseSamplerState( BYTE _bt_samplerIndex, D3DSAMPLERSTATETYPE _dxSst_samplerStateType, DWORD _dw_value );

		void pushRenderState( BYTE _bt_state, DWORD _dw_value );
		void pushRenderStates( BYTE _bt_statesNumber, const RENDER_STATE* _pRS_values );

		void pushSamplerState( DWORD _dw_sampler, D3DSAMPLERSTATETYPE _dSS_state, DWORD _dw_value );
		void pushSamplerStates( const SAMPLER_DEFINITION* _pSD_samplerDef );

		void applyRenderStatesToActual( BYTE _bt_statesNumber, const RENDER_STATE* _pRS_values );
		void applySamplerStatesToActual( const SAMPLER_DEFINITION* _pSD_samplerDef );

		void applyRenderStatesToBase( BYTE _bt_statesNumber, const RENDER_STATE* _pRS_values );
		void applySamplerStatesToBase( const SAMPLER_DEFINITION* _pSD_samplerDef );

		void applyRenderState( BYTE _bt_state, DWORD _dw_value );
		void applySamplerState( DWORD _dw_sampler, D3DSAMPLERSTATETYPE _dSS_state, DWORD _dw_value );

		void applyRenderStateToActual();
		void applySamplerStateToActual();

		void applyRenderStateToBase();
		void applySamplerStateToBase();

		void resetRenderState();
		void resetSamplerState( DWORD _dw_sampler );
		void resetSamplersState();


		void clearQueue();


		void applyRenderPipeState( IDirect3DVertexShader9* _pDxVS_vertexShader, IDirect3DPixelShader9* _pDxPS_pixelShader );
		void applyRenderPipeVSState( IDirect3DVertexShader9* _pDxVS_vertexShader );
		void applyRenderPipePSState( IDirect3DPixelShader9* _pDxPS_pixelShader );

		void applyTexture( DWORD _dw_sampler, IDirect3DBaseTexture9* _pDxTx_texture );


	private:

		void applyVertexShaderConstantFloat( const ShaderConstantDescription* _pSCD_constDesc, const void* _pV_value );
		void applyVertexShaderConstantInt( const ShaderConstantDescription* _pSCD_constDesc, const void* _pV_value );
		void applyVertexShaderConstantBool( const ShaderConstantDescription* _pSCD_constDesc, const void* _pV_value );
		void applyPixelShaderConstantFloat( const ShaderConstantDescription* _pSCD_constDesc, const void* _pV_value );
		void applyPixelShaderConstantInt( const ShaderConstantDescription* _pSCD_constDesc, const void* _pV_value );
		void applyPixelShaderConstantBool( const ShaderConstantDescription* _pSCD_constDesc, const void* _pV_value );

		void applyVertexShaderConstantFloat( const ShaderConstantDescription* _pSCD_constDesc, const void* _pV_value, UINT _ui_offset, UINT _ui_lengthVec4f );
		void applyVertexShaderConstantInt( const ShaderConstantDescription* _pSCD_constDesc, const void* _pV_value, UINT _ui_offset, UINT _ui_lengthVec4f );
		void applyVertexShaderConstantBool( const ShaderConstantDescription* _pSCD_constDesc, const void* _pV_value, UINT _ui_offset, UINT _ui_lengthVec4f );
		void applyPixelShaderConstantFloat( const ShaderConstantDescription* _pSCD_constDesc, const void* _pV_value, UINT _ui_offset, UINT _ui_lengthVec4f );
		void applyPixelShaderConstantInt( const ShaderConstantDescription* _pSCD_constDesc, const void* _pV_value, UINT _ui_offset, UINT _ui_lengthVec4f );
		void applyPixelShaderConstantBool( const ShaderConstantDescription* _pSCD_constDesc, const void* _pV_value, UINT _ui_offset, UINT _ui_lengthVec4f );


	public:

		void applyShaderConstantFloat( const ShaderConstantDescription* _pSCD_constDesc, const float* _pF_value );
		void applyShaderConstantFloat( const ShaderConstantDescription* _pSCD_constDesc, const float* _pF_value, UINT _ui_offset, UINT _ui_lengthVec4f );

		void applyShaderConstantInt( const ShaderConstantDescription* _pSCD_constDesc, const int* _pI_value );
		void applyShaderConstantInt( const ShaderConstantDescription* _pSCD_constDesc, const int* _pI_value, UINT _ui_offset, UINT _ui_lengthVec4f );

		void applyShaderConstantBool( const ShaderConstantDescription* _pSCD_constDesc, const BOOL* _pB_value );

		inline void applyShaderConstant( const ShaderConstantDescription* _pSCD_constDesc, const void* _pV_value )
			{	(this->* applySCMethods[ _pSCD_constDesc->bt_shaderType ][ _pSCD_constDesc->bt_operationType ])( _pSCD_constDesc, _pV_value );	};
		inline void applyShaderConstant( const ShaderConstantDescription* _pSCD_constDesc, const void* _pV_value, UINT _ui_offset, UINT _ui_lengthVec4f )
			{	(this->* applySCMethodAd[ _pSCD_constDesc->bt_shaderType ][ _pSCD_constDesc->bt_operationType ])( _pSCD_constDesc, _pV_value, _ui_offset, _ui_lengthVec4f );	};


		void applyVertexDeclaration( IDirect3DVertexDeclaration9* _pDxVD_vertexDeclaration );

		void applyIndexData( IDirect3DIndexBuffer9* _pDxIB_indexData );

		void resetStreamData( UINT _ui_streamNumber );
		void applyStreamData( UINT _ui_streamNumber, IDirect3DVertexBuffer9* _pDxVB_streamData, UINT _ui_stride, UINT _ui_frequencyParameter = 1, bool _b_geometryIndexed = true, bool _b_instanceData = false );

		void applyGeometry( IDirect3DVertexDeclaration9* _pDxVD_vertexDeclaration, UINT _ui_streamsNumber, IDirect3DVertexBuffer9** _pDxVB_streamData, UINT* _ui_streamStride );
		void applyIndexedGeometry( IDirect3DVertexDeclaration9* _pDxVD_vertexDeclaration, IDirect3DIndexBuffer9* _pDxIB_indexData, UINT _ui_streamsNumber, IDirect3DVertexBuffer9** _pDxVB_streamData, UINT* _ui_streamStride );

		void applyInstancedGeometry( IDirect3DVertexDeclaration9* _pDxVD_vertexDeclaration, UINT _ui_instancesNumber, UINT _ui_streamsNumber, IDirect3DVertexBuffer9** _pDxVB_streamData, UINT* _ui_streamStride, bool* _b_geometryData );
		void applyInstancedIndexedGeometry( IDirect3DVertexDeclaration9* _pDxVD_vertexDeclaration, UINT _ui_instancesNumber, IDirect3DIndexBuffer9* _pDxIB_indexData, UINT _ui_streamsNumber, IDirect3DVertexBuffer9** _pDxVB_streamData, UINT* _ui_streamStride, bool* _b_geometryData );

		void purgeRenderPipeState();
	};

}


#endif //_GRAPHIC_DEVICE_STATE_MANAGER
#include "Resource_Graphic_Technique.h"
#include "Graphic_Renderer.h"



namespace Renderer
{

	PixelShaderContainer::~PixelShaderContainer()
	{
		IDX_SAFE_RELEASE( pD9PS_pixelShader );

		for ( UINT i = 0; i < v_pSCD_shaderConstants.size(); ++i )
		{
			SAFE_RELEASE( v_pSCD_shaderConstants[ i ] );
		}
	};


	PixelShaderContainer::PixelShaderContainer( Renderer* _pR_weakPtrRenderer, const string& _str_name ) : pR_weakPtrRenderer( _pR_weakPtrRenderer ), str_name( _str_name ), pD9PS_pixelShader( NULL ), ui_refCount( 1 ), b_loaded( false )
	{};


	PixelShaderContainer::PixelShaderContainer( Renderer* _pR_weakPtrRenderer, const string& _str_name, IDirect3DPixelShader9* _pD9PS_pixelShader, UINT _ui_constDescNumber, const ShaderConstantDescription** _pSCD_desc ) : pR_weakPtrRenderer( _pR_weakPtrRenderer ), str_name( _str_name ), pD9PS_pixelShader( _pD9PS_pixelShader ), ui_refCount( 1 )
	{
		v_pSCD_shaderConstants.resize( _ui_constDescNumber );
		for ( UINT i = 0; i < _ui_constDescNumber; ++i )
		{
			v_pSCD_shaderConstants[ i ] = _pSCD_desc[ i ];
		}

		b_loaded = _pD9PS_pixelShader ? true : false;
	};


	void PixelShaderContainer::setup( IDirect3DPixelShader9* _pD9PS_pixelShader, UINT _ui_constDescNumber, const ShaderConstantDescription** _pSCD_desc )
	{
		if ( b_loaded )
			throw ApplicationException( "PixelShaderContainer::setup error - resource %s already exist", str_name );
		if ( !_pD9PS_pixelShader )
			throw ApplicationException( "PixelShaderContainer::setup error - atempting to setup empty resource %s", str_name );

		pD9PS_pixelShader = _pD9PS_pixelShader;

		v_pSCD_shaderConstants.resize( _ui_constDescNumber );
		for ( UINT i = 0; i < _ui_constDescNumber; ++i )
		{
			v_pSCD_shaderConstants[ i ] = _pSCD_desc[ i ];
		}

		b_loaded = true;
	};


	const string* PixelShaderContainer::getNamePtr()
	{
		return &str_name;
	};


	void PixelShaderContainer::addRef()
	{
		++ui_refCount;
	};


	void PixelShaderContainer::release()
	{
		if ( !( --us_refCount ) )
		{
			pR_weakPtrRenderer->release( this );
			delete this;
		}
	};



	VertexShaderContainer::~VertexShaderContainer()
	{
		IDX_SAFE_RELEASE( pD9PS_vertexShader );

		for ( UINT i = 0; i < v_pSCD_shaderConstants.size(); ++i )
		{
			SAFE_RELEASE( v_pSCD_shaderConstants[ i ] );
		}
	};


	VertexShaderContainer::VertexShaderContainer( Renderer* _pR_weakPtrRenderer, const string& _str_name ) : pR_weakPtrRenderer( _pR_weakPtrRenderer ), str_name( _str_name ), pD9PS_pixelShader( NULL ), ui_refCount( 1 ), b_loaded( false )
	{};


	VertexShaderContainer::VertexShaderContainer( Renderer* _pR_weakPtrRenderer, const string& _str_name, IDirect3DVertexShader9* _pD9PS_vertexShader, UINT _ui_constDescNumber, const ShaderConstantDescription** _pSCD_desc ) : pR_weakPtrRenderer( _pR_weakPtrRenderer ), str_name( _str_name ), pD9PS_vertexShader( _pD9PS_vertexShader ), ui_refCount( 1 )
	{
		v_pSCD_shaderConstants.resize( _ui_constDescNumber );
		for ( UINT i = 0; i < _ui_constDescNumber; ++i )
		{
			v_pSCD_shaderConstants[ i ] = _pSCD_desc[ i ];
		}

		b_loaded = _pD9PS_vertexShader ? true : false;
	};


	void VertexShaderContainer::setup( IDirect3DVertexShader9* _pD9PS_vertexShader, UINT _ui_constDescNumber, const ShaderConstantDescription** _pSCD_desc )
	{
		if ( b_loaded )
			throw ApplicationException( "VertexShaderContainer::setup error - resource %s already exist", str_name );
		if ( !_pD9PS_vertexShader )
			throw ApplicationException( "VertexShaderContainer::setup error - atempting to setup empty resource %s", str_name );

		pD9PS_vertexShader = _pD9PS_vertexShader;

		v_pSCD_shaderConstants.resize( _ui_constDescNumber );
		for ( UINT i = 0; i < _ui_constDescNumber; ++i )
		{
			v_pSCD_shaderConstants[ i ] = _pSCD_desc[ i ];
		}

		b_loaded = true;
	};


	const string* VertexShaderContainer::getNamePtr()
	{
		return &str_name;
	};


	void VertexShaderContainer::addRef()
	{
		++ui_refCount;
	};


	void VertexShaderContainer::release()
	{
		if ( !( --us_refCount ) )
		{
			pR_weakPtrRenderer->release( this );
			delete this;
		}
	};



	Pass::Pass( VertexShaderContainer* _pVSC_vertexShader, PixelShaderContainer* _pPSC_pixelShader, UINT _ui_renderStatesNumber, const RENDER_STATE* _pRS_renderStates, UINT _ui_samplersDefinitionsNumber, const SAMPLER_DEFINITION* _pSD_samplersDefinition ) : pVSC_vertexShader( _pVSC_vertexShader ), pPSC_pixelShader( _pPSC_pixelShader )
	{
		v_RS_renderStates.resize( _ui_renderStatesNumber );
		for ( UINT i = 0; i < _ui_renderStatesNumber; ++i )
		{
			v_RS_renderStates[ i ] = _pRS_renderStates[ i ];
		}

		v_SD_samplersDefinition.resize( _ui_samplersDefinitionsNumber );
		for ( UINT i = 0; i < _ui_samplersDefinitionsNumber; ++i )
		{
			v_SD_samplersDefinition[ i ] = _pSD_samplersDefinition[ i ];
		}
	};


	Pass::~Pass()
	{
		SAFE_RELEASE( pVSC_vertexShader );
		SAFE_RELEASE( pPSC_pixelShader );
	};



	Technique::~Technique()
	{
		for ( UINT i = 0; i < v_pPasses.size(); ++i )
		{
			SAFE_DELETE( v_pPasses );
		}
	};


	Technique::Technique( Renderer* _pR_weakPtrRenderer, const string& _str_name, UINT _ui_renderQueueIndex ) : pR_weakPtrRenderer( _pR_weakPtrRenderer ), str_name( _str_name ), ui_refCount( 1 ), b_loaded( false ), ui_renderQueueIndex( _ui_renderQueueIndex )
	{};


	Technique::Technique( Renderer* _pR_weakPtrRenderer, const string& _str_name, UINT _ui_renderQueueIndex, UINT _ui_passesNumber, const Pass** _p_ppPasses ) : pR_weakPtrRenderer( _pR_weakPtrRenderer ), str_name( _str_name )ui_refCount( 1 ), ui_renderQueueIndex( _ui_renderQueueIndex )
	{
		v_pPasses.resize( _ui_passesNumber );

		for ( UINT i = 0; i < _ui_passesNumber; ++i )
		{
			v_pPasses[ i ] = _p_ppPasses[ i ];

			for ( UINT j = 0; j < v_pPasses[ i ]->pVSC_vertexShader->v_pSCD_shaderConstants.size(); ++j )
			{
				ShaderConstantDescription* tmpSCD = v_pPasses[ i ]->pVSC_vertexShader->v_pSCD_shaderConstants[ j ];
				m_pSTRpSCD_shaderConstants.insert( map< string*, ShaderConstantDescription* >::value_type( tmpSCD->getNamePtr(), tmpSCD ) );
			}

			for ( UINT j = 0; j < v_pPasses[ i ]->pPSC_pixelShader->v_pSCD_shaderConstants.size(); ++j )
			{
				ShaderConstantDescription* tmpSCD = v_pPasses[ i ]->pPSC_pixelShader->v_pSCD_shaderConstants[ j ];
				m_pSTRpSCD_shaderConstants.insert( map< string*, ShaderConstantDescription* >::value_type( tmpSCD->getNamePtr(), tmpSCD ) );
			}
		}

		b_loaded = ( _ui_passesNumber & _p_ppPasses ) ? true : false;
	};


	void Technique::setup( UINT _ui_passesNumber, const Pass** _p_ppPasses )
	{
		if ( b_loaded )
			throw ApplicationException( "Technique::setup error - resource %s already exist", str_name );
		if ( !_ui_passesNumber || !_p_ppPasses )
			throw ApplicationException( "Technique::setup error - atempting to setup empty resource %s", str_name );

		v_pPasses.resize( _ui_passesNumber );

		for ( UINT i = 0; i < _ui_passesNumber; ++i )
		{
			v_pPasses[ i ] = _p_ppPasses[ i ];

			for ( UINT j = 0; j < v_pPasses[ i ]->pVSC_vertexShader->v_pSCD_shaderConstants.size(); ++j )
			{
				ShaderConstantDescription* tmpSCD = v_pPasses[ i ]->pVSC_vertexShader->v_pSCD_shaderConstants[ j ];
				m_pSTRpSCD_shaderConstants.insert( map< string*, ShaderConstantDescription* >::value_type( tmpSCD->getNamePtr(), tmpSCD ) );
			}

			for ( UINT j = 0; j < v_pPasses[ i ]->pPSC_pixelShader->v_pSCD_shaderConstants.size(); ++j )
			{
				ShaderConstantDescription* tmpSCD = v_pPasses[ i ]->pPSC_pixelShader->v_pSCD_shaderConstants[ j ];
				m_pSTRpSCD_shaderConstants.insert( map< string*, ShaderConstantDescription* >::value_type( tmpSCD->getNamePtr(), tmpSCD ) );
			}
		}

		b_loaded = true;
	};


	UINT Technique::getPassesNumber()
	{
		return v_pPasses.size();
	};


	void Technique::beginPass( UINT _ui_pass )
	{
		DeviceStateManager* pDSM_manager = pR_weakPtrRenderer->getDeviceStateManager();
		Pass* reqPass = v_pPasses[ _ui_pass ];

		pDSM_manager->applyRenderStatesToActual( reqPass->v_RS_renderStates.size(), &( reqPass->v_RS_renderStates[ 0 ] ) );
		for ( UINT i = 0; i < reqPass->v_SD_samplersDefinition.size(); ++i )
		{
			pDSM_manager->applySamplerStatesToActual( &( reqPass->v_SD_samplersDefinition[ i ] ) );
		}

		pDSM_manager->applyRenderPipeState( reqPass->pVSC_vertexShader->pD9PS_vertexShader, reqPass->pPSC_pixelShader->pD9PS_pixelShader );
	};


	const string* Technique::getNamePtr()
	{
		return &str_name;
	};


	void Technique::addRef()
	{
		++ui_refCount;
	};


	void Technique::release()
	{
		if ( ! ( --ui_refCount ) )
		{
			// remember to allways use mutex lock while dealing with system resources
			pR_weakPtrRenderer->release( this );
			delete this;
		}
	};


	ShaderConstantDescription* Technique::getShaderVariableByName( const string& _str_name )
	{
		ShaderVariableMap::iterator it = svm_shaderConstants->find( &_str_name );
		if ( it != svm_shaderConstants.end() )
		{
			it->second->addRef();
			return it->second;
		}
		return NULL;
	};


	ShaderConstantDescription* Technique::getShaderVariableByIndex( UINT _ui_index )
	{
		if ( _ui_index < svv_shaderConstants.size() )
		{
			svv_shaderConstants[ _ui_index ]->addRef();
			return svv_shaderConstants[ _ui_index ];
		}
		return NULL;
	};


}
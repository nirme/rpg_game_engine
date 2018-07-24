#include "Resource_Graphic_Mesh_Container.h"
#include "Graphic_Renderer.h"



namespace Renderer
{

	MeshContainer::~MeshContainer()
	{
		for ( UINT i = 0; i < ui_vertexBuffersNumber; ++i )
		{
			IDX_SAFE_RELEASE( pDxIB_vertexBuffer[ i ] );
		}

		SAFE_DELETE_ARRAY( pDxIB_vertexBuffer );
		SAFE_DELETE_ARRAY( ui_singleVertexSize );

		IDX_SAFE_RELEASE( pDxIB_indexBuffer );

		IDX_SAFE_RELEASE( pDxVD_tmpWeakPtrVertexDeclaration );

		SAFE_RELEASE( pVDC_vertexDeclContainer );

		msm_subsetsMap.clear();
		msv_subsetsVector.clear();
	};


	MeshContainer::MeshContainer( Renderer* _pR_weakPtrRenderer, const string& _str_name ) : pR_weakPtrRenderer( _pR_weakPtrRenderer ), str_name( _str_name ), ui_refCount( 1 ), b_loaded( false ), pDxIB_vertexBuffer( NULL ), ui_singleVertexSize( NULL ), ui_vertexBuffersNumber( 0 ), b_useExternalStream( false ), pDxIB_indexBuffer( NULL ), b_meshIndexed( false ), pVDC_vertexDeclContainer( NULL ), pDxVD_tmpWeakPtrVertexDeclaration( NULL ), dx9PT_renderOperation( 0 )
	{};


	MeshContainer::MeshContainer( Renderer* _pR_weakPtrRenderer, const string& _str_name, UINT _ui_vertexBuffersNumber, const IDirect3DVertexBuffer9** _pDxIB_vertexBuffer, const UINT* _ui_singleVertexSize, bool _b_useExternalStream, IDirect3DIndexBuffer9* _pDxIB_indexBuffer, VertexDeclarationContainer* _pVDC_vertexDeclContainer, D3DPRIMITIVETYPE _dx9PT_renderOperation, UINT _ui_subsetsNumber, const MeshSubset* _pMS_subsets ) : pR_weakPtrRenderer( _pR_weakPtrRenderer ), str_name( _str_name ), ui_refCount( 1 ), ui_vertexBuffersNumber( _ui_vertexBuffersNumber ), b_useExternalStream( _b_useExternalStream ), pDxIB_indexBuffer( _pDxIB_indexBuffer ), pVDC_vertexDeclContainer( _pVDC_vertexDeclContainer ), dx9PT_renderOperation( _dx9PT_renderOperation )
	{
		if ( ui_vertexBuffersNumber )
		{
			pDxIB_vertexBuffer = new IDirect3DVertexBuffer9* [ ui_vertexBuffersNumber ];
			ui_singleVertexSize = new UINT [ ui_vertexBuffersNumber ];

			for ( UINT i = 0; i < ui_vertexBuffersNumber; ++i )
			{
				pDxIB_vertexBuffer[ i ] = _pDxIB_vertexBuffer[ i ];
				ui_singleVertexSize[ i ] = _ui_singleVertexSize[ i ];
			}
		}
		else
		{
			pDxIB_vertexBuffer = NULL;
			ui_singleVertexSize = NULL;
		}

		b_meshIndexed = pDxIB_indexBuffer ? true : false;
		pDxVD_tmpWeakPtrVertexDeclaration = pVDC_vertexDeclContainer ? pVDC_vertexDeclContainer->getVertexDeclWeakPtr() : NULL;

		msv_subsetsVector.resize( _ui_subsetsNumber );

		for ( UINT i = 0; i < _ui_subsetsNumber; ++i )
		{
			msv_subsetsVector[ i ] = _pMS_subsets[ i ];
			msm_subsetsMap.insert( MeshSubsetsMap::value_type( &( msv_subsetsVector[ i ].str_name ), i ) );
		}

		b_loaded = ( ui_vertexBuffersNumber & pDxVD_tmpWeakPtrVertexDeclaration & _ui_subsetsNumber ) ? true : false;
	};


	void MeshContainer::setup(	UINT _ui_vertexBuffersNumber, const IDirect3DVertexBuffer9** _pDxIB_vertexBuffer, const UINT* _ui_singleVertexSize, bool _b_useExternalStream, IDirect3DIndexBuffer9* _pDxIB_indexBuffer, VertexDeclarationContainer* _pVDC_vertexDeclContainer, D3DPRIMITIVETYPE _dx9PT_renderOperation, UINT _ui_subsetsNumber, const MeshSubset* _pMS_subsets )
	{
		if ( b_loaded )
			throw ApplicationException( "MeshContainer::setup error - resource %s already exist", str_name );
		if ( !_ui_vertexBuffersNumber || !_pDxIB_vertexBuffer || !_pVDC_vertexDeclContainer || !_ui_subsetsNumber || !_pMS_subsets )
			throw ApplicationException( "MeshContainer::setup error - atempting to setup empty resource %s", str_name );

		ui_vertexBuffersNumber = _ui_vertexBuffersNumber;
		b_useExternalStream = _b_useExternalStream;

		pDxIB_vertexBuffer = new IDirect3DVertexBuffer9* [ ui_vertexBuffersNumber ];
		ui_singleVertexSize = new UINT [ ui_vertexBuffersNumber ];

		for ( UINT i = 0; i < ui_vertexBuffersNumber; ++i )
		{
			pDxIB_vertexBuffer[ i ] = _pDxIB_vertexBuffer[ i ];
			ui_singleVertexSize[ i ] = _ui_singleVertexSize[ i ];
		}

		pDxIB_indexBuffer = _pDxIB_indexBuffer;
		b_meshIndexed = pDxIB_indexBuffer ? true : false;

		pVDC_vertexDeclContainer = _pVDC_vertexDeclContainer;
		pDxVD_tmpWeakPtrVertexDeclaration = pVDC_vertexDeclContainer->getVertexDeclWeakPtr();

		dx9PT_renderOperation = _dx9PT_renderOperation;

		msv_subsetsVector.resize( _ui_subsetsNumber );
		for ( UINT i = 0; i < _ui_subsetsNumber; ++i )
		{
			msv_subsetsVector[ i ] = _pMS_subsets[ i ];
			msm_subsetsMap.insert( MeshSubsetsMap::value_type( &( msv_subsetsVector[ i ].str_name ), i ) );
		}

		b_loaded = true;
	};


	const string* MeshContainer::getNamePtr()
	{
		return &str_name;
	};


	void MeshContainer::addRef()
	{
		++ui_refCount;
	};


	void MeshContainer::release()
	{
		if ( ! ( --ui_refCount ) )
		{
			b_loaded = false;
			// remember to allways use mutex lock while dealing with system resources
			pR_weakPtrRenderer->release( this );
			delete this;
		}
	};


	UINT MeshContainer::getSubsetIndexByName( const string& _str_name )
	{
		MeshSubsetsMap::iterator it = msm_subsetsMap.find( &_str_name );
		if ( it != msm_subsetsMap.end() )
			return it->second;
		return MESH_SUBSET_INEXIST;
	};


	string& MeshContainer::getSubsetNameByIndex( UINT _ui_index )
	{
		if ( _ui_index < msv_subsetsVector.size() )
			return msv_subsetsVector[ _ui_index ].str_name;
		return string("");
	};

}

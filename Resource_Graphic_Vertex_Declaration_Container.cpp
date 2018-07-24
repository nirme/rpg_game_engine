#include "Resource_Graphic_Vertex_Declaration_Container.h"
#include "Graphic_Renderer.h"



namespace Renderer
{

	VertexDeclarationContainer::~VertexDeclarationContainer()
	{
		IDX_SAFE_RELEASE( pDxVD_vertexDeclaration );
		SAFE_DELETE_ARRAY( pDxVE_vertexElements );
	};


	VertexDeclarationContainer::VertexDeclarationContainer( Renderer* _pR_weakPtrRenderer, const string& _str_name ) : pR_weakPtrRenderer( _pR_weakPtrRenderer ), str_name( _str_name ), ui_refCount( 1 ), b_loaded( false ), pDxVE_vertexElements( NULL ), ui_vertexElementsNumber( 0 ), pDxVD_vertexDeclaration( NULL )
	{};


	VertexDeclarationContainer::VertexDeclarationContainer( Renderer* _pR_weakPtrRenderer, const string& _str_name, const D3DVERTEXELEMENT9* _pDxVE_vertexElements, UINT _ui_vertexElementsNumber ) : pR_weakPtrRenderer( _pR_weakPtrRenderer ), str_name( _str_name ), ui_refCount( 1 ), ui_vertexElementsNumber( _ui_vertexElementsNumber )
	{
		pDxVE_vertexElements = new D3DVERTEXELEMENT9 [ ui_vertexElementsNumber ];
		memcpy( pDxVE_vertexElements, _pDxVE_vertexElements, sizeof( D3DVERTEXELEMENT9 ) * ui_vertexElementsNumber );

		if ( FAILED( hr = pR_weakPtrRenderer->getDeviceWeakPtr()->CreateVertexDeclaration( _pDxVE_vertexElements, &pDxVD_vertexDeclaration ) ) )
			throw D3DX9Exception( "VertexDeclarationContainer::VertexDeclarationContainer", "IDirect3DDevice9::CreateVertexDeclaration", hr );

		b_loaded = true;
	};


	VertexDeclarationContainer::VertexDeclarationContainer( Renderer* _pR_weakPtrRenderer, const string& _str_name, IDirect3DVertexDeclaration9* _pDxVD_vertexDeclaration, const D3DVERTEXELEMENT9* _pDxVE_vertexElements, UINT _ui_vertexElementsNumber ) : pR_weakPtrRenderer( _pR_weakPtrRenderer ), str_name( _str_name ), ui_refCount( 1 ), ui_vertexElementsNumber( _ui_vertexElementsNumber ), pDxVD_vertexDeclaration( _pDxVD_vertexDeclaration )
	{
		if ( !_ui_vertexElementsNumber )
		{
			if ( FAILED( hr = pDxVD_vertexDeclaration->GetDeclaration( NULL, &ui_vertexElementsNumber ) ) )
				throw D3DX9Exception( "VertexDeclarationContainer::VertexDeclarationContainer", "IDirect3DDevice9::GetDeclaration", hr );
		}
		else
		{
			ui_vertexElementsNumber = _ui_vertexElementsNumber;
		}

		pDxVE_vertexElements = new D3DVERTEXELEMENT9 [ ui_vertexElementsNumber ];

		if ( _ui_vertexElementsNumber )
		{
			memcpy( pDxVE_vertexElements, _pDxVE_vertexElements, sizeof( D3DVERTEXELEMENT9 ) * ui_vertexElementsNumber );
		}
		else
		{
			if ( FAILED( hr = pDxVD_vertexDeclaration->GetDeclaration( &pDxVE_vertexElements, &ui_vertexElementsNumber ) ) )
				throw D3DX9Exception( "VertexDeclarationContainer::VertexDeclarationContainer", "IDirect3DDevice9::GetDeclaration", hr );
		}

		b_loaded = true;
	};


	void VertexDeclarationContainer::setup( const D3DVERTEXELEMENT9* _pDxVE_vertexElements, UINT _ui_vertexElementsNumber )
	{
		if ( b_loaded )
			throw ApplicationException( "VertexDeclarationContainer::setup error - resource %s already exist", str_name );
		if ( !_pDxVE_vertexElements || !_ui_vertexElementsNumber )
			throw ApplicationException( "VertexDeclarationContainer::setup error - atempting to setup empty resource %s", str_name );

		ui_vertexElementsNumber = _ui_vertexElementsNumber;
		pDxVE_vertexElements = new D3DVERTEXELEMENT9 [ ui_vertexElementsNumber ];
		memcpy( pDxVE_vertexElements, _pDxVE_vertexElements, sizeof( D3DVERTEXELEMENT9 ) * ui_vertexElementsNumber );

		if ( FAILED( hr = pR_weakPtrRenderer->getDeviceWeakPtr()->CreateVertexDeclaration( _pDxVE_vertexElements, &pDxVD_vertexDeclaration ) ) )
			throw D3DX9Exception( "VertexDeclarationContainer::VertexDeclarationContainer", "IDirect3DDevice9::CreateVertexDeclaration", hr );

		b_loaded = true;
	};


	void VertexDeclarationContainer::setup( IDirect3DVertexDeclaration9* _pDxVD_vertexDeclaration, const D3DVERTEXELEMENT9* _pDxVE_vertexElements, UINT _ui_vertexElementsNumber )
	{
		if ( b_loaded )
			throw ApplicationException( "VertexDeclarationContainer::setup error - resource %s already exist", str_name );
		if ( !_pDxVD_vertexDeclaration )
			throw ApplicationException( "VertexDeclarationContainer::setup error - atempting to setup empty resource %s", str_name );

		if ( !_ui_vertexElementsNumber )
		{
			if ( FAILED( hr = pDxVD_vertexDeclaration->GetDeclaration( NULL, &ui_vertexElementsNumber ) ) )
				throw D3DX9Exception( "VertexDeclarationContainer::VertexDeclarationContainer", "IDirect3DDevice9::GetDeclaration", hr );
		}
		else
		{
			ui_vertexElementsNumber = _ui_vertexElementsNumber;
		}

		pDxVE_vertexElements = new D3DVERTEXELEMENT9 [ ui_vertexElementsNumber ];

		if ( _ui_vertexElementsNumber )
		{
			memcpy( pDxVE_vertexElements, _pDxVE_vertexElements, sizeof( D3DVERTEXELEMENT9 ) * ui_vertexElementsNumber );
		}
		else
		{
			if ( FAILED( hr = pDxVD_vertexDeclaration->GetDeclaration( &pDxVE_vertexElements, &ui_vertexElementsNumber ) ) )
				throw D3DX9Exception( "VertexDeclarationContainer::VertexDeclarationContainer", "IDirect3DDevice9::GetDeclaration", hr );
		}

		b_loaded = true;
	};


	const string* VertexDeclarationContainer::getNamePtr()
	{
		return &str_name;
	};


	void VertexDeclarationContainer::addRef()
	{
		++ui_refCount;
	};


	void VertexDeclarationContainer::release()
	{
		if ( ! ( --ui_refCount ) )
		{
			b_loaded = false;
			// remember to allways use mutex lock while dealing with system resources
			pR_weakPtrRenderer->release( this );
			delete this;
		}
	};

}

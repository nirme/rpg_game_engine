#include "Resource_Graphic_Texture_Container.h"
#include "Graphic_Renderer.h"



namespace Renderer
{

	TextureContainer::~TextureContainer()
	{
		IDX_SAFE_RELEASE( pDxTx_texture );
	};


	TextureContainer::TextureContainer( Renderer* _pR_weakPtrRenderer, const string& _str_name ) : pR_weakPtrRenderer( _pR_weakPtrRenderer ), str_name( _str_name ), ui_refCount( 1 ), b_loaded( false ), pDxTx_texture( NULL )
	{};


	TextureContainer::TextureContainer( Renderer* _pR_weakPtrRenderer, const string& _str_name, IDirect3DTexture9* _pDxTx_texture ) : pR_weakPtrRenderer( _pR_weakPtrRenderer ), str_name( _str_name ), ui_refCount( 1 ), pDxTx_texture( _pDxTx_texture ), b_loaded( true )
	{};


	void TextureContainer::setup( IDirect3DTexture9* _pDxTx_texture )
	{
		if ( b_loaded )
			throw ApplicationException( "TextureContainer::setup error - resource %s already exist", str_name );
		if ( !_pDxTx_texture )
			throw ApplicationException( "TextureContainer::setup error - atempting to setup empty resource %s", str_name );

		pDxTx_texture = _pDxTx_texture;
		b_loaded = true;
	};


	const string* TextureContainer::getNamePtr()
	{
		return &str_name;
	};


	void TextureContainer::addRef()
	{
		++ui_refCount;
	};


	void TextureContainer::release()
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
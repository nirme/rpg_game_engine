#pragma once

#ifndef _RESOURCE_GRAPHIC_TEXTURE_CONTAINER
#define _RESOURCE_GRAPHIC_TEXTURE_CONTAINER

#include "utility.h"

#include "Core_Defines.h"
#include "d3d9types.h"
#include "Base_Exceptions.h"



namespace Renderer
{
	//Forward declarations
	class Renderer;
	class TextureContainer;


	class TextureContainer
	{
	private:
		Renderer* pR_weakPtrRenderer;

		string str_name;
		UINT ui_refCount;

		IDirect3DTexture9* pDxTx_texture;
		bool b_loaded;


		~TextureContainer();

	public:

		// c-tors should be called only by renderer containing map of resources
		TextureContainer( Renderer* _pR_weakPtrRenderer, const string& _str_name );
		TextureContainer( Renderer* _pR_weakPtrRenderer, const string& _str_name, IDirect3DTexture9* _pDxTx_texture );

		void setup( IDirect3DTexture9* _pDxTx_texture );

		const string* getNamePtr();

		void addRef();
		void release();

		inline bool isLoaded()	{	return b_loaded;	};

		inline IDirect3DTexture9* getTextureWeakPtr()	{	return pDxTx_texture;	};
	};
}



#endif //_RESOURCE_GRAPHIC_TEXTURE_CONTAINER
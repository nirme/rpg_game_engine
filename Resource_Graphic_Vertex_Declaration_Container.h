#pragma once

#ifndef _RESOURCE_GRAPHIC_VERTEX_DECLARATION_CONTAINER
#define _RESOURCE_GRAPHIC_VERTEX_DECLARATION_CONTAINER

#include "utility.h"

#include "Core_Defines.h"
#include "d3d9types.h"
#include "Base_Exceptions.h"


namespace Renderer
{
	//Forward declarations
	class Renderer;
	class VertexDeclarationContainer;


	class VertexDeclarationContainer
	{
	private:
		Renderer* pR_weakPtrRenderer;

		string str_name;
		UINT ui_refCount;

		D3DVERTEXELEMENT9* pDxVE_vertexElements;
		UINT ui_vertexElementsNumber;

		IDirect3DVertexDeclaration9* pDxVD_vertexDeclaration;

		bool b_loaded;

		~VertexDeclarationContainer();

	public:

		// c-tors should be called only by renderer containing map of resources
		VertexDeclarationContainer( Renderer* _pR_weakPtrRenderer, const string& _str_name );
		VertexDeclarationContainer( Renderer* _pR_weakPtrRenderer, const string& _str_name, const D3DVERTEXELEMENT9* _pDxVE_vertexElements, UINT _ui_vertexElementsNumber );
		VertexDeclarationContainer( Renderer* _pR_weakPtrRenderer, const string& _str_name, IDirect3DVertexDeclaration9* _pDxVD_vertexDeclaration, const D3DVERTEXELEMENT9* _pDxVE_vertexElements = NULL, UINT _ui_vertexElementsNumber = 0 );

		void setup( const D3DVERTEXELEMENT9* _pDxVE_vertexElements, UINT _ui_vertexElementsNumber );
		void setup( IDirect3DVertexDeclaration9* _pDxVD_vertexDeclaration, const D3DVERTEXELEMENT9* _pDxVE_vertexElements = NULL, UINT _ui_vertexElementsNumber = 0 );

		const string* getNamePtr();

		void addRef();
		void release();

		inline bool isLoaded()	{	return b_loaded;	};

		inline IDirect3DVertexDeclaration9* getVertexDeclWeakPtr()	{	return pDxVD_vertexDeclaration;	};
	};

}


#endif //_RESOURCE_GRAPHIC_VERTEX_DECLARATION_CONTAINER
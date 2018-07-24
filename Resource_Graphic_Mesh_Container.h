#pragma once

#ifndef _RESOURCE_GRAPHIC_MESH_CONTAINER
#define _RESOURCE_GRAPHIC_MESH_CONTAINER

#include "utility.h"

#include "Core_Defines.h"
#include "d3d9types.h"
#include "Base_Exceptions.h"
#include "Resource_Graphic_Vertex_Declaration_Container.h"


namespace Renderer
{
	//Forward declarations
	class Renderer;
	class MeshContainer;
	struct MeshSubset;


#define		MESH_SUBSET_INEXIST		(UINT)(-1)


	struct MeshSubset
	{
		string str_name;
		UINT ui_startVertex;
		UINT ui_primitiveCount;
	};


	class MeshContainer
	{
		friend Renderer;

		typedef		vector< MeshSubset >		MeshSubsetsVector;
		typedef		map< string*, UINT, p_scompare >		MeshSubsetsMap;

	private:
		Renderer* pR_weakPtrRenderer;

		string str_name;
		UINT ui_refCount;

		IDirect3DVertexBuffer9** pDxIB_vertexBuffer;
		UINT* ui_singleVertexSize;
		UINT ui_vertexBuffersNumber;
		bool b_useExternalStream;

		IDirect3DIndexBuffer9* pDxIB_indexBuffer;
		bool b_meshIndexed;

		VertexDeclarationContainer* pVDC_vertexDeclContainer;
		// tmp pointer to vertex declaration
		IDirect3DVertexDeclaration9* pDxVD_tmpWeakPtrVertexDeclaration;

		D3DPRIMITIVETYPE dx9PT_renderOperation;

		MeshSubsetsVector msv_subsetsVector;
		MeshSubsetsMap msm_subsetsMap;

		bool b_loaded;


		~MeshContainer();

	public:

		// c-tors should be called only by renderer containing map of resources
		MeshContainer(	Renderer* _pR_weakPtrRenderer, const string& _str_name );

		MeshContainer(	Renderer* _pR_weakPtrRenderer, 
						const string& _str_name, 
						UINT _ui_vertexBuffersNumber, 
						const IDirect3DVertexBuffer9** _pDxIB_vertexBuffer, 
						const UINT* _ui_singleVertexSize, 
						bool _b_useExternalStream, 
						IDirect3DIndexBuffer9* _pDxIB_indexBuffer, 
						VertexDeclarationContainer* _pVDC_vertexDeclContainer, 
						D3DPRIMITIVETYPE _dx9PT_renderOperation, 
						UINT _ui_subsetsNumber, 
						const MeshSubset* _pMS_subsets );


		void setup(	UINT _ui_vertexBuffersNumber, 
					const IDirect3DVertexBuffer9** _pDxIB_vertexBuffer, 
					const UINT* _ui_singleVertexSize, 
					bool _b_useExternalStream, 
					IDirect3DIndexBuffer9* _pDxIB_indexBuffer, 
					VertexDeclarationContainer* _pVDC_vertexDeclContainer, 
					D3DPRIMITIVETYPE _dx9PT_renderOperation, 
					UINT _ui_subsetsNumber, 
					const MeshSubset* _pMS_subsets );

		const string* getNamePtr();

		void addRef();
		void release();

		inline bool isLoaded()	{	return b_loaded;	};


		inline IDirect3DVertexBuffer9* getWeakPtrVertexBuffer( UINT _ui_stream )	{	return pDxIB_vertexBuffer[ _ui_stream ];	};
		inline UINT getSingleVertexSize( UINT _ui_stream )							{	return ui_singleVertexSize[ _ui_stream ];	};
		inline UINT getVertexBuffersNumber()										{	return ui_vertexBuffersNumber;	};
		inline bool getUsingExternalStream()										{	return b_useExternalStream;	};

		inline IDirect3DIndexBuffer9* getWeakPtrIndexBuffer()						{	return pDxIB_indexBuffer;	};
		inline bool isMeshIndexed()													{	return b_meshIndexed;	};

		inline VertexDeclarationContainer* getWeakPtrVertexDeclContainer()			{	return pVDC_vertexDeclContainer;	};
		inline IDirect3DVertexDeclaration9* getWeakPtrVertexDecl()					{	return pDxVD_tmpWeakPtrVertexDeclaration;	};

		inline D3DPRIMITIVETYPE getRenderOperation()								{	return dx9PT_renderOperation;	};


		UINT getSubsetIndexByName( const string& _str_name );
		string& getSubsetNameByIndex( UINT _ui_index );

	};

}


#endif //_RESOURCE_GRAPHIC_MESH_CONTAINER

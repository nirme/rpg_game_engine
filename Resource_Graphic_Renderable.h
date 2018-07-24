#pragma once

#ifndef _RESOURCE_GRAPHIC_RENDERABLE
#define _RESOURCE_GRAPHIC_RENDERABLE

#include "utility.h"

#include "Core_Defines.h"
#include "d3d9types.h"
#include "Base_Exceptions.h"

#include "Resource_Graphic_Shader_Constant_Description.h"
#include "Graphic_Device_State_Manager.h"
#include "Resource_Graphic_Technique.h"
#include "Resource_Graphic_Mesh_Container.h"
#include "Resource_Graphic_Texture_Container.h"

namespace Renderer
{
	//Forward declarations
	class Material;
	class Renderable;
	class Renderer;


/*
	class TextureContainer;
	class VertexBufferContainer;
		class VertexDeclarationContainer;
		V ( class IndexBufferContainer );
*/

/*
	class Material
	{
	};
*/


	class Renderable
	{
		friend Renderer;

		typedef		map<USHORT,Material*>				LOD_MATERIAL_MAP;
		typedef		map<USHORT,Material*>::iterator		LOD_MATERIAL_MAP_IT;









	protected:


		vector< ShaderConstantDescription* > v_SCD_constantsDescription;






		bool b_renderableIndexed;
		D3DPRIMITIVETYPE dx9PT_renderOperation;
		UINT ui_startIndex;
		UINT ui_primitiveCount;

		LOD_MATERIAL_MAP lmp_lodMap;





		IDirect3DVertexDeclaration9* pDx9VD_vertexDeclaration;

		IDirect3DVertexBuffer9* pDx9VB_vertices;
		IDirect3DIndexBuffer9* pDx9IB_indices;

		boost::shared_ptr<D3DXMATRIX> spMx_worldTransformation;


	public:

		Renderable()
		{};

		virtual ~Renderable()
		{
			IDirect3DVertexDeclaration9* pDx9VD_vertexDeclaration;

			IDirect3DVertexBuffer9* pDx9VB_vertices;
			IDirect3DIndexBuffer9* pDx9IB_indices;
		};








// LOD level
// material map
// technique map
// virtual void setDeviceState( DeviceStateManager* khjkjh
// virtual Real getSquaredViewDepth (const Camera *cam) const =0
// at push for render ( LOD LEVEL ) get back technique pointer and save in pack 
// {
//   *technique
//   LOD LEVEL
//   *Renderable
// }

	};

}


#endif _RESOURCE_GRAPHIC_RENDERABLE

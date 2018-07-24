#pragma once

#ifndef _RENDERABLE_NODE
#define _RENDERABLE_NODE


#include "utility.h"
#include "Core_Defines.h"

#include "Geo_Node.h"

#include "Render_Visitor.h"



class RenderableNode : public GeoNode
{
public:

	virtual int render( RenderVisitor* _pRenderVisitor ) = 0;
};

#endif //_RENDERABLE_NODE

#include "Render_Visitor.h"



RenderVisitor::RenderVisitor() : pRenderer(NULL), pCamera(NULL)
{};


int RenderVisitor::setRenderer( Renderer* _pRenderer )
{
	pRenderer = _pRenderer;
	pRenderer->setCamera( pCamera );
};


int RenderVisitor::setCamera( Camera* _pCamera )
{
	pCamera = _pCamera;
	pRenderer->setCamera( pCamera );
};


int RenderVisitor::setGUISystem( GUISystem* _pGuiSystem )
{
	pRenderer->setGUISystem( _pGuiSystem );
};


int RenderVisitor::setAmbient( DWORD _color )
{
	return pRenderer->setAmbient( _color );
};


int RenderVisitor::setAmbient( D3DXCOLOR _color )
{
	return pRenderer->setAmbient( _color );
};

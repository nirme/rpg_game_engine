#pragma once

#ifndef _CORE_RENDER_VISITOR
#define _CORE_RENDER_VISITOR

#include "utility.h"
#include "Core_Defines.h"

#include "Core_Camera.h"
#include "Core_Graphic_Object.h"
#include "Particle_System_v4.h"
#include "Core_Light.h"
#include "GUI_System.h"

#include "Core_Renderer.h"



class RenderVisitor
{
private:

	Renderer* pRenderer;
	Camera* pCamera;


public:

	RenderVisitor();


	inline Renderer* getRenderer()		{	return pRenderer;	};
	inline Camera* getCamera()			{	return pCamera;		};

	int setRenderer( Renderer* _pRenderer );

	int setCamera( Camera* _pCamera );
	int setGUISystem( GUISystem* _pGuiSystem );

	int setAmbient( DWORD _color );
	int setAmbient( D3DXCOLOR _color );

	inline int render( GraphicObject* _pGraphicObject )			{	return pRenderer->addChunksToRender( _pGraphicObject );		};
	inline int render( ParticleSystem_v4* _pParticleSystem )	{	return pRenderer->addParticles( _pParticleSystem );			};

	inline int render( Light* _pLight )							{	return pRenderer->addLight( _pLight );									};
	inline int renderShadow( GraphicObject* _pGraphicObject )	{	return pRenderer->addChunksToShadowRenderNoCheck( _pGraphicObject );	};

	inline int render( GUIElement* _p2dElements )				{	return pRenderer->add2DElement( _p2dElements );		};

};

#endif //_CORE_RENDER_VISITOR

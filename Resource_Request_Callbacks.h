#pragma once

#ifndef _RESOURCE_REQUEST_CALLBACKS
#define _RESOURCE_REQUEST_CALLBACKS

#include "utility.h"
#include "thread_safe_queue.h"

#include "Core_Logger.h"
#include "Core_Variable_Library.h"
#include "Core_Defines.h"

#include "Load_Functions.h"
#include "Base_Node.h"
#include "Loader_v2.h"






int resourceTexture(void* _ptr)
{
	return Loader::resourceLoad(RESOURCE_TEXTURE, _ptr);
};

int resourceMesh(void* _ptr)
{
	return Loader::resourceLoad(RESOURCE_MESH, _ptr);
};

int resourceEffect(void* _ptr)
{
	return Loader::resourceLoad(RESOURCE_EFFECT, _ptr);
};

int resourceSound(void* _ptr)
{
	return Loader::resourceLoad(RESOURCE_SOUNDDATA, _ptr);
};

int resourceBitmapFont(void* _ptr)
{
	return Loader::resourceLoad(RESOURCE_BITMAP_FONT, _ptr);
};

int resourceGDIFont(void* _ptr)
{
	return Loader::resourceLoad(RESOURCE_GDI_FONT, _ptr);
};



int (*Texture::requestFunction)(void*) =		&resourceTexture;
int (*Mesh::requestFunction)(void*) =			&resourceMesh;
int (*Effect::requestFunction)(void*) =			&resourceEffect;
int (*Sound::requestFunction)(void*) =			&resourceSound;
int (*BitmapFont::requestFunction)(void*) =		&resourceBitmapFont;
int (*GDIFont::requestFunction)(void*) =		&resourceGDIFont;















GraphicObject* requestGraphicObject(string _GO_name)
{
	GraphicObject* pGraphicObject = new GraphicObject;
	pGraphicObject->setName(_GO_name);
	pGraphicObject->onRequest();
	Loader::request(new LoaderRequestQueueItem(RESOURCE_GRAPHIC_OBJECT, pGraphicObject));
	return pGraphicObject;
};


void requestNode(BaseNode* _parent, string _nodeName, bool _loadRecursive)
{
	NodeRequestItem* nrItem = new NodeRequestItem(_parent, _nodeName, _loadRecursive);
	Loader::request(new LoaderRequestQueueItem(RESOURCE_NODE, nrItem));
};


void requestUserDefinedResource(UINT _loaderFunctionIndex, void* _userDefinedData, UINT _userDefinedDataSize)
{
	UserRequestItem* urItem = new UserRequestItem(_loaderFunctionIndex, _userDefinedData, _userDefinedDataSize);
	Loader::request(new LoaderRequestQueueItem(RESOURCE_OTHERS, urItem));
};












#endif //_RESOURCE_REQUEST_CALLBACKS

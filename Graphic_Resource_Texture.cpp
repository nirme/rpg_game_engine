#include "Graphic_Resource_Texture.h"



Resource <IDirect3DTexture9*>::ResourceMap Resource<IDirect3DTexture9*>::loadedResMap;


Texture::~Texture()
{
	IDX_SAFE_RELEASE(resource);
};


int Texture::setResource( IDirect3DTexture9* _texture )
{
	#ifdef(MULTI_THREADED_LOADING)
		boost::mutex::scoped_lock lock( mutex );
	#endif //MULTI_THREADED_LOADING

	if ( loaded )
		return;

	resource = _texture;
	loaded = ( resource != NULL ) ? true : false;

	#ifdef(MULTI_THREADED_LOADING)
		conditionVar.notify_all();
	#endif //MULTI_THREADED_LOADING

	return 0;
};


#ifndef RESOURCE_MAP_WITH_STRINGS
	Texture* Texture::getResource( UINT _id )
#else //RESOURCE_MAP_WITH_STRINGS
	Texture* Texture::getResource( string _id )
#endif //RESOURCE_MAP_WITH_STRINGS
{
	return reinterpret_cast <Texture*> ( getBaseResource( _id ) );
};


#ifdef( MULTI_THREADED_LOADING )
	#ifndef RESOURCE_MAP_WITH_STRINGS
		Texture* Texture::getResourceMultiThreaded( UINT _id )
	#else //RESOURCE_MAP_WITH_STRINGS
		Texture* Texture::getResourceMultiThreaded( string _id )
	#endif //RESOURCE_MAP_WITH_STRINGS
	{
		return reinterpret_cast <Texture*> ( getBaseResourceMultiThreaded( _id ) );
	};
#endif //MULTI_THREADED_LOADING


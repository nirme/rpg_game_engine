#pragma once

#ifndef _GRAPHIC_RESOURCE_TEXTURE
#define _GRAPHIC_RESOURCE_TEXTURE

#include "utility.h"
#include "Template_Basic_Resource.h"



class Texture;


//textures are mixed together with bump, normal, light maps etc...
class Texture : public Resource <IDirect3DTexture9*>
{
private:
	~Texture();

public:

	#ifndef RESOURCE_MAP_WITH_STRINGS
		Texture( UINT _id ) : Resource( _id, NULL ){};
	#else //RESOURCE_MAP_WITH_STRINGS
		Texture( string _id ) : Resource( _id, NULL ){};
	#endif //RESOURCE_MAP_WITH_STRINGS


	int setResource( IDirect3DTexture9* _texture );


	#ifndef RESOURCE_MAP_WITH_STRINGS
		static Texture* getResource( UINT _id );
	#else //RESOURCE_MAP_WITH_STRINGS
		static Texture* getResource( string _id );
	#endif //RESOURCE_MAP_WITH_STRINGS


	#ifdef( MULTI_THREADED_LOADING )
		#ifndef RESOURCE_MAP_WITH_STRINGS
			static Texture* getResourceMultiThreaded( UINT _id );
		#else //RESOURCE_MAP_WITH_STRINGS
			static Texture* getResourceMultiThreaded( string _id );
		#endif //RESOURCE_MAP_WITH_STRINGS
	#endif //MULTI_THREADED_LOADING



	int onLostDevice(){ return 0; };
	int onResetDevice(){ return 0; };
};


#endif //_GRAPHIC_RESOURCE_TEXTURE

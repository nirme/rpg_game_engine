
class GraphicObjectInfo;

class SkinnedGraphicObject
class ControllableSkinnedGraphicObject

class GraphicObjectInfo
{
protected:

	Model* pModel;
	vector <Material> vMaterialsUsed;
	vector <USHORT> vSubsetMaterialIndex;


	GraphicObject( const string& _name ) : refCount(0), pModel(NULL)
	{
		boost::mutex::scoped_lock lock( resourceMapMutex );
		ResourceMap::iterator it = resourceMap.find( _name );

		if ( it != resourceMap.end() )
			throw ApplicationException( "application tried to create duplicate GraphicObject object" );

		name = _name;
		resourceMap.insert( ResourceMap::value_type( &name, this ) )
	};

	~GraphicObject()
	{
		boost::mutex::scoped_lock lock( resourceMapMutex );
		resourceMap.erase( resourceMap.find( name ) );

		SAFE_RELEASE( pModel );
	};

public:

	inline void addRef()	{	++refCount;	};

	int setResource( IDirect3DTexture9* _texture );


};




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

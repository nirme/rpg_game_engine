#pragma once

#ifndef _TEMPLATE_BASIC_RESOURCE
#define _TEMPLATE_BASIC_RESOURCE

#include "utility.h"
#include "Core_Defines.h"




//template for resource handling

template <typename _type> class Resource
{
public:
//map contains all loaded resources together with their id
	#ifndef RESOURCE_MAP_WITH_STRINGS
		typedef map <UINT*, Resource*> ResourceMap;
	#else //RESOURCE_MAP_WITH_STRINGS
		typedef map <string*, Resource*, scompare> ResourceMap;
	#endif //RESOURCE_MAP_WITH_STRINGS

	#ifdef(MULTI_THREADED_LOADING)
		static mutable boost::mutex resourceMapMutex;
	#endif //MULTI_THREADED_LOADING


protected:

	#ifndef RESOURCE_MAP_WITH_STRINGS
		UINT id;
	#else //RESOURCE_MAP_WITH_STRINGS
		string id;
	#endif //RESOURCE_MAP_WITH_STRINGS

	const _type resource;
	UINT refCount;

	//indicates if resource is in memory or is waiting for load by external loader
	bool loaded;

	#ifdef(MULTI_THREADED_LOADING)

	//needed for multithreaded apps
		mutable boost::mutex mutex;
		boost::condition_variable conditionVar;

	//full load function going around multithreaded loading
		static int (*singleThreadRequestFunction)(void*);

	#endif //MULTI_THREADED_LOADING


//resource have names in ANSI only
	static ResourceMap loadedResMap;

	// static pointer to external function, it must be initilize before main function
	// its purpose is for adding request to loader queue whithout knowing its class
	// initialization should look like this:
	// int (*(yourType)::requestFunction)(void*) = &InsertYourFunctionName;
	static int (*requestFunction)(void*);


public:

	#ifndef RESOURCE_MAP_WITH_STRINGS
		Resource(UINT _id, _type _resource = NULL)
	#else //RESOURCE_MAP_WITH_STRINGS
		Resource(string _id, _type _resource = NULL)
	#endif //RESOURCE_MAP_WITH_STRINGS
			: id(_id), resource(_resource)
	{
		loaded = (resource != NULL) ? true : false;

		refCount = 1;
		loadedResMap.insert( ResourceMap::value_type( _id, this ) );
	};

	virtual	~Resource()
	{
		ResourceMap::iterator it = loadedResMap.find(id);
		if( it != loadedResMap.end() )
			loadedResMap.erase( it );
	};


	#ifndef RESOURCE_MAP_WITH_STRINGS
		static Resource* getBaseResource(UINT _id)
	#else //RESOURCE_MAP_WITH_STRINGS
		static Resource* getBaseResource(string _id)
	#endif //RESOURCE_MAP_WITH_STRINGS
	{
		#ifdef(MULTI_THREADED_LOADING)
			boost::mutex::scoped_lock lock( resourceMapMutex );
		#endif //MULTI_THREADED_LOADING

		ResourceMap::iterator it = loadedResMap.find(_id);

		if( it == loadedResMap.end() )
		{
			Resource* res = new Resource(_id);

			#ifdef(MULTI_THREADED_LOADING)
				(*singleThreadRequestFunction)((void*) res);
			#else
				(*requestFunction)((void*) res);
			#endif

			return res;
		}

		(*it).second->addRef();
		return (*it).second;
	};


	#ifndef RESOURCE_MAP_WITH_STRINGS
		inline UINT getId()
	#else //RESOURCE_MAP_WITH_STRINGS
		string getId()
	#endif //RESOURCE_MAP_WITH_STRINGS
	{
		return id;
	};


	inline bool isLoaded()
	{
		return loaded;
	};


	#ifdef(MULTI_THREADED_LOADING)

		#ifndef RESOURCE_MAP_WITH_STRINGS
			static Resource* getBaseResourceMultiThreaded(UINT _id)
		#else //RESOURCE_MAP_WITH_STRINGS
			static Resource* getBaseResourceMultiThreaded(string _id)
		#endif //RESOURCE_MAP_WITH_STRINGS
		{
			boost::mutex::scoped_lock lock( resourceMapMutex );

			ResourceMap::iterator it = loadedResMap.find(_id);

			if( it == loadedResMap.end() )
			{
				Resource* res = new Resource(_id);
				(*requestFunction)((void*) res);
				return res;
			}

			(*it).second->addRef();
			return (*it).second;
		};


		int waitToLoad()
		{
			boost::mutex::scoped_lock lock(mutex);

			if (loaded)
				return 0;

			while (!loaded)
			{
				conditionVar.wait(lock);
			}

			return 0;
		};

	#endif //MULTI_THREADED_LOADING


//name mustn't be set in ctor because of the possibility of string copy errors

	inline UINT addRef()		{	return ++refCount;	};

	inline UINT getRefCount()	{	return refCount;	};

	UINT release()
	{
		#ifdef(MULTI_THREADED_LOADING)
			boost::mutex::scoped_lock lock(mutex);
		#endif //MULTI_THREADED_LOADING

		UINT rfc = --refCount;
		if(refCount == 0 && loaded)
		{
			delete this;
		}

		return rfc;
	};

	UINT checkAndRelease()
	{
		#ifdef(MULTI_THREADED_LOADING)
			boost::mutex::scoped_lock lock(mutex);
		#endif //MULTI_THREADED_LOADING

		if(refCount == 0)
		{
			delete this;
		}

		return refCount;
	};


	inline _type getResourcePointer()
	{
		return resource;
	};


	virtual static int releaseAll()
	{
		#ifdef(MULTI_THREADED_LOADING)
			boost::mutex::scoped_lock lock( resourceMapMutex );
		#endif //MULTI_THREADED_LOADING

		
		for(ResourceMap::iterator it = loadedResMap.begin(); it != loadedResMap.end(); it = loadedResMap.begin())
			delete (*it).second;
		loadedResMap.clear();

		return 0;
	};


//leave it as is, don't have time for lost/reset device
	virtual int onLostDevice() = 0;
	virtual int onResetDevice() = 0;

//loops running OnLost/ResetDevice func for every res
	static int lostDevice()
	{
		bool bResult = true;

		// Iterate through the list of loaded resources
		for( ResourceMap::iterator it = loadedResMap.begin(); it != loadedResMap.end(); it++ )
			// Notify the instance of this resource that the device is lost
			// by calling it's onLostDevice entry point
			bResult &= SUCCEEDED( ( *it ).second->onLostDevice() );

		return bResult ? 0 : -1;
	};

	static int resetDevice()
	{
		bool bResult = true;

		// Iterate through the list of loaded resources
		for( ResourceMap::iterator it = loadedResMap.begin(); it != loadedResMap.end(); it++ )
			// Notify the instance of this resource that the device is lost
			// by calling it's onLostDevice entry point
			bResult &= SUCCEEDED( ( *it ).second->onResetDevice() );

		return bResult ? 0 : -1;
	};
};


#endif //_TEMPLATE_BASIC_RESOURCE
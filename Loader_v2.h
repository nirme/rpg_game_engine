#pragma once

#ifndef _CORE_LOADER_V2
#define _CORE_LOADER_V2

#include "utility.h"
#include "thread_safe_queue.h"

#include "Core_Logger.h"
#include "Core_Variable_Library.h"
#include "Core_Defines.h"

#include "Load_Helper_Functions.h"


#include "Core_Graphic.h"
#include "Core_Graphic_Resource.h"
#include "Core_Light.h"
#include "Core_Bitmap_Font_v2.h"
#include "Core_SoundSystem.h"

#include "Core_Graphic_Object.h"

#include "Base_Node.h"

#include "Effect_Include_Menager.h"

#include "GUI_Loading_Func.h"



#define		RESOURCE_GRAPHIC	0x00
#define		RESOURCE_SOUND		0x01
#define		RESOURCE_OTHER		0x02



typedef int (*ResourceLoadFunction) (sqlite3*, void* _ptr);

//typedef int (*NodeLoadFunction) (BaseNode* /*parent*/, const string& /*node name*/, const void* /*data pointer*/, UINT /*data size*/, BaseNode** /*loaded node*/);



enum REQUEST_TYPE
{
	RESOURCE_TEXTURE		= 0x00,
	RESOURCE_MESH			= 0x01,
	RESOURCE_EFFECT			= 0x02,
	RESOURCE_SOUNDDATA		= 0x03,
	RESOURCE_BITMAP_FONT	= 0x04,
	RESOURCE_GDI_FONT		= 0x05,

//	RESOURCE_NODE			= 0x05,
//	RESOURCE_GRAPHIC_OBJECT	= 0x06,
//	RESOURCE_OTHERS			= 0x07,
};
//

struct NodeRequestItem
{
	BaseNode*	parent;
	string		nodeName;
	bool		loadRecursive;

	NodeRequestItem(BaseNode* _parent, const string& _nodeName, bool _loadRecursive) : parent(_parent), nodeName(_nodeName), loadRecursive(_loadRecursive) {};
};
//

struct UserRequestItem
{
	UINT			userLoadFunctionIndex;
	vector<BYTE>	userDefinedData;

	UserRequestItem()
	{
		userLoadFunctionIndex = -1;
	};

	UserRequestItem(UINT _userLoadFunctionIndex, void* _userDefinedData, UINT _userDefinedDataSize) : userLoadFunctionIndex(_userLoadFunctionIndex)
	{
		userDefinedData.resize(_userDefinedDataSize);
		memcpy(&(userDefinedData[0]), _userDefinedData, _userDefinedDataSize);
	};

	void setValues(UINT _userLoadFunctionIndex, void* _userDefinedData, UINT _userDefinedDataSize)
	{
		userLoadFunctionIndex = _userLoadFunctionIndex;
		userDefinedData.resize(_userDefinedDataSize);
		memcpy(&(userDefinedData[0]), _userDefinedData, _userDefinedDataSize);
	};
};
//

struct LoaderRequestQueueItem
{
	REQUEST_TYPE	iRequestType;
	void*			pResourcePointer;

	LoaderRequestQueueItem() : iRequestType(RESOURCE_OTHERS), pResourcePointer(NULL) {};
	LoaderRequestQueueItem(REQUEST_TYPE _iRequestType, void* _pResourcePointer) : iRequestType(_iRequestType), pResourcePointer(_pResourcePointer) {};
	
	LoaderRequestQueueItem(BaseNode* _parent, string _nodeName, bool _loadRecursive) : iRequestType(RESOURCE_NODE)
	{
		pResourcePointer = new NodeRequestItem(_parent, _nodeName, _loadRecursive);
	};

	LoaderRequestQueueItem(NodeRequestItem* _userDefinedData) : iRequestType(RESOURCE_NODE), pResourcePointer(_userDefinedData) {};
	LoaderRequestQueueItem(UserRequestItem* _userDefinedData) : iRequestType(RESOURCE_OTHERS), pResourcePointer(_userDefinedData) {};
};
//


class Loader
{
private:

	static bool initialized;

	//thread for worker function to work on
	static boost::thread thread;
	static int workerFunction();
	//controlling state of loader thread
	static boost::mutex threadControlMutex;
	static boost::condition_variable threadPausedCondVar;
	static boost::condition_variable threadPausedSignalCondVar;
	static bool setPaused;
	static bool threadPaused;

	//queue with request of load
	static threadSafeQueue<LoaderRequestQueueItem*> requestQueue;

	//pointers to db files
	static vector<sqlite3*> ppSqlDatabase;


	static EffectIncludeMenager* effectIncludeMenager;
	static ID3DXEffectPool* effectPool;
	static vector <D3DXMACRO> effectMacros;
	static UserDefinedCallbackSetup* callbackSetup;


	static vector<ResourceLoadFunction> basicResourceLoader;

	//node loading functions
	static map<UINT, NodeLoadFunction> nodeLoader;

	static map<UINT,ResourceLoadFunction> userResourceLoader;


	Loader(){};



protected:

	static int resourceLoadNode				(sqlite3* _db, void* _ptr);
	static int resourceLoadOthers			(sqlite3* _db, void* _ptr);


	// basic resources load functions for loading in normal and multithreaded mode
	static int resourceLoadTexture(sqlite3* _db, void* _ptr);
	static int resourceLoadMesh(sqlite3* _db, void* _ptr);
	static int resourceLoadEffect (sqlite3* _db, void* _ptr);
	static int resourceLoadSound(sqlite3* _db, void* _ptr);
	static int resourceLoadBitmapFont(sqlite3* _db, void* _ptr);
	static int resourceLoadGDIFont(sqlite3* _db, void* _ptr);


	//CHANGE TO COMPOUND RESOURCES

	static int resourceLoadLight			(sqlite3* _db, void* _ptr);
	static int resourceLoadParticleSystem	(sqlite3* _db, void* _ptr);


public:

	static int initialize();
	static int shutdown();

	static void pauseLoading(bool _pause);
	static void pauseLoadingAndWait(bool _pause);
	static inline bool isLoaderPaused()
	{
		return threadPaused;
	};

	static inline void request(LoaderRequestQueueItem* item)
	{
		requestQueue.pushBack(item);
	};
	static inline UINT getRequestQueueSize()
	{
		return requestQueue.size();
	};
	static inline void clearRequestQueue()
	{
		requestQueue.clear();
	};



	static int loadGUISystem(GUISystem* _guiSystem, UINT _systemId = 0)
	{
		return loadGUISystem(ppSqlDatabase[RESOURCE_OTHER], _guiSystem, _systemId);
	};



	// singlethreaded load functions
	static int resourceLoad(REQUEST_TYPE _requestType, void* _ptr)
	{
		return basicResourceLoader[int(_requestType)](ppSqlDatabase[db], _ptr);
	};


//	static int registerNodeLoadingFunction(UINT _id, NodeLoadFunction _function);
//	static int registerUserResourceLoadingFunction(UINT _id, ResourceLoadFunction _function);
//

};



#endif //_CORE_LOADER_V2
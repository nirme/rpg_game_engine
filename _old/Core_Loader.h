#pragma once

#ifndef _CORE_LOADER
#define _CORE_LOADER

#include "utility.h"
#include "Core_Logger.h"

#include "Core_Variable_Library.h"
#include "Core_Defines.h"

#include "Core_Resource.h"
#include "Core_Graphic.h"
#include "Core_Sound.h"

#include "Core_Light.h"



#define SELECT_BY_ID_QUERY "SELECT * FROM resource WHERE id = %d"

#define RESOURCE_NUMBER	5

typedef enum REQUEST_TYPE
{
	RESOURCE_TEXTURE	= 0x00,
	RESOURCE_EFFECT		= 0x01,
	RESOURCE_MESH		= 0x02,
	RESOURCE_SOUNDDATA	= 0x03,
	RESOURCE_LIGHT		= 0x04,
};
//

typedef HRESULT (*loadFunc) (void*);


D3DXFRAME* createFrameHierarchy(const void* dataPtr, UINT memSize, ID3DXSkinInfo* pSkinInfo, D3DXMATRIX* boneOffsets = NULL);

HRESULT parseAnimationData(const void* data, UINT size, ID3DXAnimationController* pAnimController);

HRESULT parseAnimationsBlock(const void* data, UINT size, UINT numAnimation, ID3DXKeyframedAnimationSet* pAnimSet);



struct QueueItem
{
	void*	pResourcePointer;
	UINT	eRequestType;

	QueueItem(void* _pResourcePointer, UINT _eRequestType) : pResourcePointer(_pResourcePointer), eRequestType(_eRequestType){};
};
//

class LoaderQueue
{
private:
	queue<QueueItem*> q_RequestList;
	mutable boost::mutex mutex;
	boost::condition_variable conditionVar;


public:

	inline void AddRequest(QueueItem* item)
	{
		boost::mutex::scoped_lock lock(mutex);
		q_RequestList.push(item);
		mutex.unlock();
		conditionVar.notify_one();
	};

	inline QueueItem* WaitAndGetRequest()
	{
		boost::mutex::scoped_lock lock(mutex);
		while (q_RequestList.empty())
		{
			conditionVar.wait(lock);
		}

		QueueItem* item = q_RequestList.front();
		q_RequestList.pop();
		return item;
	};

	inline bool Empty() const
	{
		boost::mutex::scoped_lock lock(mutex);
		return q_RequestList.empty();
	};

	inline UINT ElementsInQueue()
	{
		boost::mutex::scoped_lock lock(mutex);
		return q_RequestList.size();
	};

	inline void Clear()
	{
		boost::mutex::scoped_lock lock(mutex);

		while (!q_RequestList.empty())
		{
			delete q_RequestList.front();
			q_RequestList.pop();
		}
	};

};
//

class Loader
{
protected:
	static boost::thread thread;

	static LoaderQueue requestQueue;

	static sqlite3** ppSqlDatabase;

	static loadFunc* loaderFunction;


	Loader(){};

	static HRESULT workerFunction();
//

	static HRESULT createStmt(sqlite3* _db, UINT _id, sqlite3_stmt** _stmt, const char* _sql = NULL);

	static UINT additinalLoadTypes;

// loader function
	static HRESULT loadTexture	(void* pObject);
	static HRESULT loadEffect	(void* pObject);
	static HRESULT loadMesh		(void* pObject);
	static HRESULT loadSoundData(void* pObject);
	static HRESULT loadLight	(void* pObject);


public:

	static HRESULT Initialise(UINT _additinalLoadTypes = 0);
	static HRESULT Shutdown();

	inline static void AddRequest(QueueItem *item)
	{
		requestQueue.AddRequest(item);
	};
//

	inline static void AddRequest(QueueItem item)
	{
		QueueItem* it = new QueueItem(item.pResourcePointer, item.eRequestType);
		requestQueue.AddRequest(it);
	};
//

};




// RESOURCE LOADERS FUNCTION TO CONNECT TO CLASSES

//	Texture loader function
int resourceLoaderTexture(void* pItem);
//	Effect loader function
int resourceLoaderEffect(void* pItem);
//	Mesh loader function
int resourceLoaderMesh(void* pItem);
//	SoundData loader function
int resourceLoaderSoundData(void* pItem);
//	Light loader function
int resourceLoaderLight(void* pItem);


#endif _CORE_LOADER

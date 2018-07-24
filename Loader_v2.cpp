#include "Loader_v2.h"



//initializing static vars in Loader
bool										Loader::initialized;
boost::thread								Loader::thread;
boost::mutex								Loader::threadControlMutex;
boost::condition_variable					Loader::threadPausedCondVar;
boost::condition_variable					Loader::threadPausedSignalCondVar;
bool										Loader::setPaused		=	false;
bool										Loader::threadPaused	=	false;
threadSafeQueue<LoaderRequestQueueItem*>	Loader::requestQueue;
vector<sqlite3*>							Loader::ppSqlDatabase;
vector<ResourceLoadFunction>				Loader::basicResourceLoader;
map<UINT,NodeLoadFunction>					Loader::nodeLoader;
map<UINT,ResourceLoadFunction>				Loader::userResourceLoader;






//////////////////////////////////////////////////////////////////////////////
////////                        LOADER METHODS                        ////////
//////////////////////////////////////////////////////////////////////////////

int Loader::workerFunction()
{
	LoaderRequestQueueItem* item;

	while (true)
	{
		{//scope to set mutex
			boost::mutex::scoped_lock lock(threadControlMutex);

			if (setPaused)
			{
				threadPaused = true;
				threadPausedSignalCondVar.notify_all();
				threadControlMutex.unlock();

				while (setPaused)
				{
					threadPausedCondVar.wait(lock);
				}

				{//scope to set mutex
					boost::mutex::scoped_lock lock2(threadControlMutex);
					threadPaused = false;
				}
			}
		}

//NEED TO CHECK IF interruption work
		try
		{
			item = requestQueue.popFront();
		}
		catch (boost::thread_interrupted e)
		{
			return 0;
		}

		int result = 0;
		int dbIndex = 0;
		if ((int)(item->iRequestType) < (int)RESOURCE_SOUNDDATA)
			dbIndex = RESOURCE_GRAPHIC;
		else if (item->iRequestType == RESOURCE_SOUNDDATA)
			dbIndex = RESOURCE_SOUND;
		else
			dbIndex = RESOURCE_OTHER;


		result = basicResourceLoader[int(item->iRequestType)](ppSqlDatabase[dbIndex], item->pResourcePointer);

		if (item->iRequestType == RESOURCE_OTHERS)
		{
			UserRequestItem* uItem = static_cast<UserRequestItem*>(item->pResourcePointer);
			delete uItem;
		}

		SAFE_DELETE(item);

		if (result)
			return result;
	}
};

int Loader::resourceLoadNode(sqlite3* _db, void* _ptr)
{
	NodeRequestItem* nItem = static_cast <NodeRequestItem*> (_ptr);

/*
+----------------+
|    sg_node     |
+---+------------+
|col|  name      |
+---+------------+
| 0 | id         |
| 1 | class_id   |
| 2 | class_data |
+---+------------+
*/
	sqlite3_stmt* stmt = NULL;
	ostringstream sql;
	sql << "SELECT `id` `class_id`, `class_data` FROM `sg_node` WHERE `name` = \"" << nItem->nodeName << "\"";

	int result = sqlite3_prepare_v2(_db, sql.str().c_str(), -1, &stmt, NULL);

	if (result != SQLITE_OK)
	{
		sqlite3_finalize(stmt);
		delete nItem;
		showError(true, "sqlite3_prepare_v2 failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
		return 1;
	}

	result = sqlite3_step(stmt);
	if (result != SQLITE_ROW && result != SQLITE_DONE)
	{
		sqlite3_finalize(stmt);
		delete nItem;
		showError(true, "sqlite3_step failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
		return 2;
	}
	if (result == SQLITE_DONE)
	{
		sqlite3_finalize(stmt);
		string str = nItem->nodeName;
		delete nItem;
		showError(false, "Node '%s' not found in database.\n", str.c_str());
		return 0;
	}

	UINT id = (UINT)sqlite3_column_int(stmt, 0);
	UINT nodeClassId = (UINT)sqlite3_column_int(stmt, 1);
	map<UINT,NodeLoadFunction>::iterator it = nodeLoader.find(nodeClassId);
	if (it == nodeLoader.end())
	{
		sqlite3_finalize(stmt);
		delete nItem;
		showError(false, "Node loading function for class id %d not registered.\n", nodeClassId);
		return 0;
	}

	BaseNode* child = NULL;
	result = (*it).second(nItem->parent, nItem->nodeName, sqlite3_column_blob(stmt, 2), (UINT)sqlite3_column_bytes(stmt, 2), &child);
	sqlite3_finalize(stmt);
	sql.clear();

	if (result)
	{
		delete nItem;
		return result;
	}

	nItem->parent->addChild(child);
	child->setParent(nItem->parent);

	sql << "SELECT `name` FROM `sg_node` WHERE `parent_id` = " << id ;

	result = sqlite3_prepare_v2(_db, sql.str().c_str(), -1, &stmt, NULL);

	if (result != SQLITE_OK)
	{
		sqlite3_finalize(stmt);
		delete nItem;
		showError(true, "sqlite3_prepare_v2 failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
		return 3;
	}

	while ((result = sqlite3_step(stmt)) == SQLITE_ROW)
	{
		const char* cn = reinterpret_cast<const char*>(sqlite3_column_text(stmt,0));
		child->addChild(string(cn));
	}
	if (result != SQLITE_DONE)
	{
		sqlite3_finalize(stmt);
		delete nItem;
		showError(true, "sqlite3_step failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
		return 4;
	}

	if (nItem->loadRecursive)
	{
		for (int i=0;i<child->getChildrenNumber();i++)
		{
			NodeRequestItem* nReq = new NodeRequestItem(child, child->getChildName(i), true);
			result = resourceLoadNode(_db, nReq);
			if (result)
				return result;
		}
	}

	delete nItem;
	return 0;
};

int Loader::resourceLoadOthers(sqlite3* _db, void* _ptr)
{
	UserRequestItem* uItem = static_cast<UserRequestItem*>(_ptr);
	map<UINT,ResourceLoadFunction>::iterator it = userResourceLoader.find(uItem->userLoadFunctionIndex);
	if (it != userResourceLoader.end())
	{
		int result = (*it).second(_db, &(uItem->userDefinedData[0]));
		delete uItem;
		return result;
	}
	else
	{
		delete uItem;
		showError(false, "User resource loading function for id %d not registered.\n", uItem->userLoadFunctionIndex);
		return 0;
	}
};




//PUBLIC METHODS

int Loader::initialize()
{
	ppSqlDatabase.clear();
	sqlite3* dbLink = NULL;
	int flags = SQLITE_OPEN_READONLY;
	string filepath;
	int result = 0;

	// Set graphic database
	filepath = string(PATH_DATA) + VariableLibrary::getValueStr("filenames", "RESOURCE_GRAPHIC");
	result = sqlite3_open_v2(filepath.c_str(), &dbLink, flags, NULL);
	if (result != SQLITE_OK)
	{
		showError(true, "SQLite failed loading file \"%s\" with error code %d:\n\"%s\"", filepath.c_str(), result, sqlite3_errmsg(dbLink));
		return 1;
	}
	ppSqlDatabase.push_back(dbLink);

	result = 0;
	dbLink = NULL;


	// Set sound database
	filepath = string(PATH_DATA) + VariableLibrary::getValueStr("filenames", "RESOURCE_SOUND");
	result = sqlite3_open_v2(filepath.c_str(), &dbLink, flags, NULL);
	if (result != SQLITE_OK)
	{
		showError(true, "SQLite failed loading file \"%s\" with error code %d:\n\"%s\"", filepath.c_str(), result, sqlite3_errmsg(dbLink));
		return 2;
	}
	ppSqlDatabase.push_back(dbLink);

	result = 0;
	dbLink = NULL;


	// Set others database
	filepath = string(PATH_DATA) + VariableLibrary::getValueStr("filenames", "RESOURCE_OTHER");
	result = sqlite3_open_v2(filepath.c_str(), &dbLink, flags, NULL);
	if (result != SQLITE_OK)
	{
		showError(true, "SQLite failed loading file \"%s\" with error code %d:\n\"%s\"", filepath.c_str(), result, sqlite3_errmsg(dbLink));
		return 3;
	}
	ppSqlDatabase.push_back(dbLink);

	result = 0;
	dbLink = NULL;

	EffectIncludeMenager* effectIncludeMenager = new EffectIncludeMenager;
	effectInclude->initialize(ppSqlDatabase[RESOURCE_GRAPHIC]);


	//set loader functions
	basicResourceLoader.resize(8);
	basicResourceLoader[(UINT)RESOURCE_TEXTURE] = Loader::resourceLoadTexture;

	basicResourceLoader[(UINT)RESOURCE_MESH] = Loader::resourceLoadMesh;

	basicResourceLoader[(UINT)RESOURCE_EFFECT] = Loader::resourceLoadEffect;

	basicResourceLoader[(UINT)RESOURCE_LIGHT] = Loader::resourceLoadLight;

	basicResourceLoader[(UINT)RESOURCE_SOUNDDATA] = Loader::resourceLoadSound;

	basicResourceLoader[(UINT)RESOURCE_NODE] = Loader::resourceLoadNode;

	basicResourceLoader[(UINT)RESOURCE_GRAPHIC_OBJECT] = Loader::resourceLoadGraphicObject;

	basicResourceLoader[(UINT)RESOURCE_OTHERS] = Loader::resourceLoadOthers;


	// start loading thread
	thread = boost::thread(&(Loader::workerFunction));

	initialized = true;

	return 0;
};

int Loader::shutdown()
{
	// Send interrupt signal which will throw thread_interrupted exception
	//and wait for the thread to finish work
	thread.interrupt();
	thread.join();

	requestQueue.clear();

	if (!basicResourceLoader[(UINT)RESOURCE_EFFECT].second)
		SAFE_DELETE(reinterpret_cast <EffectIncludeMenager*> (basicResourceLoader[(UINT)RESOURCE_EFFECT].second));


	// Close all opened DB links
	int result = SQLITE_OK;
	for (UINT i=0;i<ppSqlDatabase.size();++i)
	{
		result += sqlite3_close(ppSqlDatabase[i]);
	}

	if (result != SQLITE_OK)
		return 1;
	return 0;
};

void Loader::pauseLoading(bool _pause)
{
	boost::mutex::scoped_lock lock(threadControlMutex);
	setPaused = _pause;
	if (!setPaused)
	{
		threadPausedCondVar.notify_all();
	}
};

void Loader::pauseLoadingAndWait(bool _pause)
{
	boost::mutex::scoped_lock lock(threadControlMutex);
	setPaused = _pause;

	if (setPaused)
	{
		while (!threadPaused)
		{
			threadPausedSignalCondVar.wait(lock);
		}
	}
	else
	{
		threadPausedCondVar.notify_all();
	}
};


#pragma once

#ifndef _BASE_NODE
#define _BASE_NODE


#include "utility.h"
#include "Core_Defines.h"



class RenderCuller;


class BaseNode
{
private:

	static NodeLoadFunction;

	string name;
	BaseNode* pParentNode;

	typedef map<string, BaseNode*> ChildMap;

	ChildMap m_pChildNode;


	#if defined(MULTI_THREADED_LOADING)
		bool loaded;
		bool loadingFlag;
		bool deletionFlag;
		mutable boost::mutex mutex;

	public:

		bool isLoading()
		{
			boost::mutex::scoped_lock lock(mutex);
		};

		void lock()
		{
			mutex.lock();
		};

		void unlock()
		{
			mutex.unlock();
		};


	#endif //MULTI_THREADED_LOADING


public:
	BaseNode(const string _name = string(""), BaseNode* _parentNode = NULL);
	virtual ~BaseNode() {};

	virtual void release();

	void deleteChildren();
	BaseNode* setName(string _name = string(""));
	string getName();
	bool compareName(string cmpName);
	virtual BaseNode* setParent(BaseNode* _parentNode = NULL);
	BaseNode* getParent();
	BaseNode* addChild(string _childName);
	virtual BaseNode* addChild(BaseNode* _childNode);
	BaseNode* setChild(BaseNode* _childNode);
	void removeChild(string _childName);
	virtual void removeChild(BaseNode* _childNode);
	BaseNode* dropChild(string _childName);
	virtual BaseNode* dropChild(BaseNode* _childNode);
	int getChildrenNumber();
	int getChild(UINT _childIndex, BaseNode** _child);
	int getChild(string _childName, BaseNode** _child);
	string getChildName(UINT _childIndex);
	bool isChild(string _childName);

	virtual void update() = 0;


	virtual int cullToRender( RenderCuller* _culler )
	{
		ChildMap::iterator it = m_pChildNode.begin();
		ChildMap::iterator it_end = m_pChildNode.end();
		for ( it; it != it_end; it++ )
		{
			if ( (*it).second )
				(*it).second->cullToRender( RenderCuller* _culler );
		};
	};

};


#endif //_BASE_NODE

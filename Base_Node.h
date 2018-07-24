#pragma once

#ifndef _BASE_NODE
#define _BASE_NODE


#include "utility.h"
#include "Core_Defines.h"



#define		0x00		NODE_FLAG_EMPTY
#define		0x01		NODE_FLAG_LOADED
#define		0x02		NODE_FLAG_NOT_LOADED
#define		0x04		NODE_FLAG_LOADING

#define		0x07		NODE_FLAG_SETUP

#define		0x08		NODE_FLAG_IS_LEAF


#define		0x10		NODE_FLAG_RESERVED1
#define		0x20		NODE_FLAG_RESERVED2
#define		0x40		NODE_FLAG_RESERVED3
#define		0x80		NODE_FLAG_DELETE




typedef int (*NodeLoadFunc) ( BaseNode* /*node*/ );


class BaseNode
{
public:

	typedef		map<string, BaseNode*>		ChildMap;
	typedef		vector<BaseNode*>			ChildVector;

protected:

	string name;

	BaseNode* pParent;
	ChildMap m_pChildren;
	ChildVector v_pChildren;

	USHORT childrenNumber;
	USHORT childrenLoaded;
	USHORT childrenIteratorIndex;

	BYTE stateFlag;


	static NodeLoadFunc nodeLoadFunc;

	#ifdef(MULTI_THREADED_LOADING)

		mutable boost::mutex mutex;

		static NodeLoadFunc nodeLoadFuncMultiThreaded;

	#endif


public:


	inline void setName( const char* _name )	{	name = _name;		};
	inline void setName( const string &_name )	{	name = _name;		};
	inline virtual void setParent( BaseNode* _pParent )	{	pParent = _pParent;	};

	inline string    getName()		{	return name;		};
	inline BaseNode* getParent()	{	return pParent;		};
	inline BYTE      getState()		{	return stateFlag;	};

	inline bool isEmpty()	{	return !( stateFlag & NODE_FLAG_SETUP );	};
	inline bool isLoaded()	{	return ( stateFlag & NODE_FLAG_LOADED );	};
	inline bool isLeaf()	{	return ( stateFlag & NODE_FLAG_IS_LEAF );	};

#ifdef(MULTI_THREADED_LOADING)
	inline bool isLoading()	{	return ( stateFlag & NODE_FLAG_LOADING );	};
#endif

	BaseNode* getChild( const char* _name );
	BaseNode* getChild( const string &_name );
	inline BaseNode* getChild( USHORT _index )	{	return _index < childrenNumber ? v_pChildren[_index] : NULL;	};

	inline void resetIteratorIndex()			{	childrenIteratorIndex = 0;	};
	inline BaseNode* iterateNextChild()			{	return childrenIteratorIndex < childrenNumber ? v_pChildren[ childrenIteratorIndex++ ] : NULL;	};

	inline USHORT getChildrenNumber()			{	return childrenNumber;	};
	inline USHORT getChildrenLoaded()			{	return childrenLoaded;	};


	void setup( const char* _name, BaseNode* _pParent, USHORT _childrenNumber = 0, BaseNode* const* _v_children = NULL );
	void setup( const string &_name, BaseNode* _pParent, USHORT _childrenNumber = 0, BaseNode* const* _v_children = NULL );

	void setup( const char* _name, BaseNode* _pParent, USHORT _childrenNumber, char* const* _v_childrenNames );
	void setup( const string &_name, BaseNode* _pParent, USHORT _childrenNumber, string* const _v_childrenNames );


	void setupChildren( USHORT _childrenNumber, BaseNode* const* _v_children );
	void setupChildren( USHORT _childrenNumber, char* const* _v_childrenNames = NULL );
	void setupChildren( USHORT _childrenNumber, string* const _v_childrenNames );


	int loadChild( BaseNode* _child );
	int loadChildren( USHORT _childrenNumber, BaseNode* const* _v_children );


	BaseNode();

	BaseNode( const char* _name, BaseNode* _pParent, USHORT _childrenNumber = 0, BaseNode* const* _v_children = NULL );
	BaseNode( const string &_name, BaseNode* _pParent, USHORT _childrenNumber = 0, BaseNode* const* _v_children = NULL );

	BaseNode( const char* _name, BaseNode* _pParent, USHORT _childrenNumber, char* const* _v_childrenNames );
	BaseNode( const string &_name, BaseNode* _pParent, USHORT _childrenNumber, string* const _v_childrenNames );

	virtual ~BaseNode();

	void release();


	int update( TIMETYPE _timeDelta );
	virtual int updateNode( TIMETYPE _timeDelta );

	int restruc();
	virtual int restrucNode();


	int loadNode();

#ifdef(MULTI_THREADED_LOADING)
	int loadNodeMultithreaded();
#endif

};



#endif //_BASE_NODE

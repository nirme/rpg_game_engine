#include "Base_Node.h"




BaseNode* BaseNode::getChild( const char* _name )
{
	ChildMap::iterator it = m_pChildren.find( _name );
	if ( it != m_pChildren.end() )
		return it.second;
	return NULL;
};


BaseNode* BaseNode::getChild( const string &_name )
{
	ChildMap::iterator it = m_pChildren.find( _name );
	if ( it != m_pChildren.end() )
		return it.second;
	return NULL;
};


void BaseNode::setup( const char* _name, BaseNode* _pParent, USHORT _childrenNumber = 0, BaseNode* const* _v_children = NULL )
{
	name = _name;
	pParent = _pParent;
	setupChildren( _childrenNumber, _v_children );
};


void BaseNode::setup( const string &_name, BaseNode* _pParent, USHORT _childrenNumber = 0, BaseNode* const* _v_children = NULL )
{
	name = _name;
	pParent = _pParent;
	setupChildren( _childrenNumber, _v_children );
};


void BaseNode::setup( const char* _name, BaseNode* _pParent, USHORT _childrenNumber, char* const* _v_childrenNames )
{
	name = _name;
	pParent = _pParent;
	setupChildren( _childrenNumber, _v_childrenNames );
};


void BaseNode::setup( const string &_name, BaseNode* _pParent, USHORT _childrenNumber, string* const _v_childrenNames )
{
	name = _name;
	pParent = _pParent;
	setupChildren( _childrenNumber, _v_childrenNames );
};



void BaseNode::setupChildren( USHORT _childrenNumber, BaseNode* const* _v_children )
{
	//clear containers counters and indices
	m_pChildren.clear();
	v_pChildren.clear();

	childrenNumber = 0;
	childrenLoaded = 0;
	childrenIteratorIndex = 0;


	if ( !_childrenNumber || !_v_children )
	{
		stateFlag = NODE_FLAG_IS_LEAF;
		return;
	}

	for ( USHORT i = 0; i < _childrenNumber; ++i )
	{
		if ( _v_children[i] )
		{
			if ( m_pChildren.insert( ChildMap::value_type( _v_children[i]->getName(), _v_children[i] ) ).second )
			{
				v_pChildren.push_back( _v_children[i] );
			}
		}
	}

	childrenNumber = m_pChildren.size();
	childrenLoaded = childrenNumber;

	stateFlag = NODE_FLAG_LOADED;
};


void BaseNode::setupChildren( USHORT _childrenNumber, char* const* _v_childrenNames )
{
	m_pChildren.clear();
	v_pChildren.clear();

	childrenNumber = 0;
	childrenLoaded = 0;
	childrenIteratorIndex = 0;

	if ( !_childrenNumber || !_v_childNames )
	{
		stateFlag = NODE_FLAG_IS_LEAF;
		return;
	}


	for ( USHORT i = 0; i < _childrenNumber; ++i )
	{
		if ( _v_children[i] )
		{
			m_pChildren.insert( ChildMap::value_type( _v_childrenNames[i], NULL ) );
		}
	}

	childrenNumber = m_pChildren.size();
	v_pChildren.resize( childrenNumber );

	stateFlag = NODE_FLAG_NOT_LOADED;
};


void BaseNode::setupChildren( USHORT _childrenNumber, string* const _v_childrenNames )
{
	m_pChildren.clear();
	v_pChildren.clear();

	childrenNumber = 0;
	childrenLoaded = 0;
	childrenIteratorIndex = 0;

	if ( !_childrenNumber || !_v_childNames )
	{
		stateFlag = NODE_FLAG_IS_LEAF;
		return;
	}


	for ( USHORT i = 0; i < _childrenNumber; ++i )
	{
		if ( _v_children[i] )
		{
			m_pChildren.insert( ChildMap::value_type( _v_childrenNames[i], NULL ) );
		}
	}

	childrenNumber = m_pChildren.size();
	v_pChildren.resize( childrenNumber );

	stateFlag = NODE_FLAG_NOT_LOADED;
};


int BaseNode::loadChild( BaseNode* _child )
{
	if ( childrenLoaded == childrenNumber )
		return 0x00010000;

	ChildMap::iterator it = m_pChildren.find( _child->getName() );
	if ( it == m_pChildren.end() )
		return 0x00000001;

	if ( it->second )
		return 0x00000100;

	v_pChildren[childrenLoaded++] = _child;
	it->second = _child;


	return 0;
};


int BaseNode::loadChildren( USHORT _childrenNumber, BaseNode* const* _v_children )
{
	if ( childrenLoaded == childrenNumber )
		return 0x00010000;

	if ( childrenNumber - childrenLoaded < _childrenNumber )
		return 0x00020000;

	if ( !_v_children )
		return 0x00040000;

	ChildMap::iterator it;
	BYTE err1 = 0;
	BYTE err2 = 0;


	for ( USHORT i = 0; i < _childrenNumber; ++i)
	{
		it = m_pChildren.find( _v_children[i]->getName() );
		if ( it == m_pChildren.end() )
		{
			err1++;
			continue;
		}

		if ( it->second )
		{
			err2++;
			continue;
		}

		it->second = _child;
		v_pChildren[childrenLoaded++] = _child;
	}


	return ( 0x00000100 * err2 + err1 );
};



BaseNode::BaseNode() : pParent(NULL), childrenNumber(0), childrenLoaded(0), childrenIteratorIndex(0), stateFlag(0)
{};


BaseNode::BaseNode( const char* _name, BaseNode* _pParent, USHORT _childrenNumber, BaseNode* const* _v_children )
{
	name = _name;
	pParent = _pParent;
	setupChildren( _childrenNumber, _v_children );
};


BaseNode::BaseNode( const string &_name, BaseNode* _pParent, USHORT _childrenNumber, BaseNode* const* _v_children )
{
	name = _name;
	pParent = _pParent;
	setupChildren( _childrenNumber, _v_children );
};


BaseNode::BaseNode( const char* _name, BaseNode* _pParent, USHORT _childrenNumber, char* const* _v_childrenNames )
{
	name = _name;
	pParent = _pParent;
	setupChildren( _childrenNumber, _v_childrenNames );
};


BaseNode::BaseNode( const string &_name, BaseNode* _pParent, USHORT _childrenNumber, string* const _v_childrenNames )
{
	name = _name;
	pParent = _pParent;
	setupChildren( _childrenNumber, _v_childrenNames );
};


virtual BaseNode::~BaseNode()
{};


void BaseNode::release()
{
	#ifdef(MULTI_THREADED_LOADING)
		boost::mutex::scoped_lock lock(mutex);
	#endif

	for ( childrenIteratorIndex = 0; childrenIteratorIndex < childrenNumber; ++childrenIteratorIndex )
	{
		if ( v_pChildren[childrenIteratorIndex] )
		{
			SAFE_RELEASE( v_pChildren[childrenIteratorIndex] );
		}
	}


	#ifdef(MULTI_THREADED_LOADING)

		if ( stateFlag & NODE_FLAG_LOADING )
		{
			stateFlag &= NODE_FLAG_DELETE;
		}
		else
		{
			delete this;
		}

	#else

		delete this;

	#endif
};


int BaseNode::update( TIMETYPE _timeDelta )
{
	int tmp = updateNode( _timeDelta );
	if ( tmp < 0 )
		return tmp;
	int res = tmp;

	for ( USHORT i = 0; i < childrenLoaded; ++i )
	{
		if ( ( tmp = v_pChildren[i]->update( _timeDelta ) ) < 0 )
			return tmp;
		res += tmp;
	}

	return res;
};


int BaseNode::updateNode( TIMETYPE _timeDelta )
{
	return 0;
};


int BaseNode::restruc()
{
	int tmp = restrucNode();
	if ( tmp < 0 )
		return tmp;
	int res = tmp;

	for ( USHORT i = 0; i < childrenLoaded; ++i )
	{
		if ( ( tmp = v_pChildren[i]->restruc() ) < 0 )
			return tmp;
		res += tmp;
	}

	return res;
};


int BaseNode::restrucNode()
{
	return 0;
};



int BaseNode::loadNode()
{
	#ifdef(MULTI_THREADED_LOADING)
		boost::mutex::scoped_lock lock(mutex);
	#endif

	if ( stateFlag & NODE_FLAG_LOADED )
		return 1;

	return nodeLoadFunc(this);
};


#ifdef(MULTI_THREADED_LOADING)

	int BaseNode::loadNodeMultithreaded()
	{
		boost::mutex::scoped_lock lock(mutex);

		if ( stateFlag & NODE_FLAG_LOADED )
			return 1;

		if ( stateFlag & NODE_FLAG_LOADING )
			return 0;

		return nodeLoadFuncMultiThreaded(this);
	};

#endif

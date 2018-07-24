#include "Base_Node.h"



BaseNode::BaseNode(string _name, BaseNode* _parentNode)
{
	name = _name;
	pParentNode = _parentNode;
};


BaseNode::~BaseNode()
{
	deleteChildren();
};


void BaseNode::release()
{
	#if defined(MULTI_THREADED_LOADING)

		boost::mutex::scoped_lock lock(mutex);

		if ( loadingFlag )
		{
			deletionFlag true;
			return ;
		}

	#endif //MULTI_THREADED_LOADING

	delete this;
};


void BaseNode::deleteChildren()
{
	map<string, BaseNode*>::iterator it = m_pChildNode.begin();
	map<string, BaseNode*>::iterator it_end = m_pChildNode.end();
	for (it; it != it_end; it++)
	{
		(*it).second->deleteChildren();
		delete (*it).second;
		(*it).second = NULL;
	}

	m_pChildNode.clear();
};

BaseNode* BaseNode::setName(string _name)
{
	name = _name;
	return this;
};

string BaseNode::getName()
{
	return name;
};

bool BaseNode::compareName(string cmpName)
{
	if (name.size() != cmpName.size() || tolower(name) != tolower(cmpName))
		return false;
	return true;
};

BaseNode* BaseNode::setParent(BaseNode* _parentNode)
{
	pParentNode = _parentNode;
	return this;
};

BaseNode* BaseNode::getParent()
{
	return pParentNode;
};

BaseNode* BaseNode::addChild(string _childName)
{
	map<string, BaseNode*>::iterator it = m_pChildNode.begin();
	map<string, BaseNode*>::iterator it_end = m_pChildNode.end();
	for (it; it != it_end; it++)
	{
		if (tolower((*it).first) == tolower(_childName))
			return (*it).second;
	}

	m_pChildNode.insert(map<string, BaseNode*>::value_type(_childName, NULL));
	return NULL;
};

BaseNode* BaseNode::addChild(BaseNode* _childNode)
{
	string childName = _childNode->getName();
	map<string, BaseNode*>::iterator it = m_pChildNode.begin();
	map<string, BaseNode*>::iterator it_end = m_pChildNode.end();
	for (it; it != it_end; it++)
	{
		if (tolower((*it).first) == tolower(childName))
		{
			(*it).second = _childNode;
			_childNode->setParent(this);
			return (*it).second;
		}
	}

	m_pChildNode.insert(map<string, BaseNode*>::value_type(childName, _childNode));
	return (*(m_pChildNode.rbegin())).second;
};


BaseNode* BaseNode::setChild(BaseNode* _childNode)
{
	map<string, BaseNode*>::iterator it = m_pChildNode.find(_childNode->getName());
	if ( it == m_pChildNode.end() )
		return NULL;
	(*it).second = _childNode;
	(*it).second->setParent(this);
	return (*it).second;
};


void BaseNode::removeChild(string _childName)
{
	map<string, BaseNode*>::iterator it = m_pChildNode.begin();
	map<string, BaseNode*>::iterator it_end = m_pChildNode.end();
	for (it; it != it_end; it++)
	{
		if (tolower((*it).first) == tolower(_childName))
		{
			if ((*it).second)
			{
				(*it).second->deleteChildren();
				delete (*it).second;
			}
			m_pChildNode.erase(it);
			return;
		}
	}
};

void BaseNode::removeChild(BaseNode* _childNode)
{
	if (!_childNode)
		return;

	string childName = _childNode->getName();
	map<string, BaseNode*>::iterator it = m_pChildNode.begin();
	map<string, BaseNode*>::iterator it_end = m_pChildNode.end();
	for (it; it != it_end; it++)
	{
		if (tolower((*it).first) == tolower(childName))
		{
			if ((*it).second)
			{
				(*it).second->deleteChildren();
				delete (*it).second;
			}
			m_pChildNode.erase(it);
			return;
		}
	}
};

BaseNode* BaseNode::dropChild(string _childName)
{
	map<string, BaseNode*>::iterator it = m_pChildNode.begin();
	map<string, BaseNode*>::iterator it_end = m_pChildNode.end();
	for (it; it != it_end; it++)
	{
		if (tolower((*it).first) == tolower(_childName))
		{
			BaseNode* child = (*it).second;
			m_pChildNode.erase(it);
			return child;
		}
	}
	return NULL;
};

BaseNode* BaseNode::dropChild(BaseNode* _childNode)
{
	if (!_childNode)
		return NULL;

	string childName = _childNode->getName();
	map<string, BaseNode*>::iterator it = m_pChildNode.begin();
	map<string, BaseNode*>::iterator it_end = m_pChildNode.end();
	for (it; it != it_end; it++)
	{
		if (tolower((*it).first) == tolower(childName))
		{
			BaseNode* child = (*it).second;
			m_pChildNode.erase(it);
			return child;
		}
	}
	return NULL;
};

int BaseNode::getChildrenNumber()
{
	return int(m_pChildNode.size());
};

int BaseNode::getChild(UINT _childIndex, BaseNode** _child)
{
	if (m_pChildNode.size() <= _childIndex)
		return 2;

	map<string, BaseNode*>::iterator it = m_pChildNode.begin();
	for (UINT i=0;i<_childIndex;i++, it++){}
	*_child = (*it).second;
	if (!(*it).second)
		return 1;
	return 0;
};

int BaseNode::getChild(string _childName, BaseNode** _child)
{
	map<string, BaseNode*>::iterator it = m_pChildNode.begin();
	map<string, BaseNode*>::iterator it_end = m_pChildNode.end();
	for (it; it != it_end; it++)
	{
		if (tolower((*it).first) == _childName)
		{
			*_child = (*it).second;
			if (!(*it).second)
				return 1;
			return 0;
		}
	}
	return 2;
};

string BaseNode::getChildName(UINT _childIndex)
{
	if (_childIndex >= m_pChildNode.size())
		return string();

	map<string, BaseNode*>::iterator it = m_pChildNode.begin();
	for (UINT i=0;i<_childIndex;i++, it++){}
	
	return (*it).first;
};

bool BaseNode::isChild(string _childName)
{
	map<string, BaseNode*>::iterator it = m_pChildNode.begin();
	map<string, BaseNode*>::iterator it_end = m_pChildNode.end();
	for (it; it != it_end; it++)
		if (tolower((*it).first) == tolower(_childName))
			return true;

	return false;
};

#pragma once

#ifndef _BASE_NODES_LOADING_FUNCTIONS
#define _BASE_NODES_LOADING_FUNCTIONS


#include "utility.h"
#include "Core_Defines.h"
#include "Base_Node.h"
#include "Geo_Node.h"



class __declspec(novtable) NodeLoadClass
{
public:
	virtual int loadNode(BaseNode* _parent, const string& _name, void* _data, UINT _dataLength, BaseNode** _nodeOut) = 0;
	virtual ~NodeLoadClass() = 0;
};


class BaseNodeLoadClass : public NodeLoadClass
{
public:
	BaseNodeLoadClass() {};
	~BaseNodeLoadClass() {};

	int loadNode(BaseNode* _parent, const string& _name, void* _data, UINT _dataLength, BaseNode** _nodeOut = NULL)
	{
		BaseNode* node = new BaseNode(_name, NULL);
		_parent->setChild(node);

		if (_nodeOut)
			*_nodeOut = node;
		return 0;
	};
};




class GeoNodeLoadClass : public NodeLoadClass
{
public:
	GeoNodeLoadClass() {};
	~GeoNodeLoadClass() {};

	int loadNode(BaseNode* _parent, const string& _name, void* _data, UINT _dataLength, BaseNode** _nodeOut = NULL)
	{
		GeoNode* node = new GeoNode(_name, NULL);
		_parent->setChild(node);

		if (_nodeOut)
			*_nodeOut = node;
		return 0;
	};
};




#endif //_BASE_NODES_LOADING_FUNCTIONS
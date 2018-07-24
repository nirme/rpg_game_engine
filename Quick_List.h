#pragma once

#ifndef _QUICK_LIST
#define _QUICK_LIST

#include "utility.h"



template<class _Ty> class quickListNode
{
private:
	quickListNode* prev;
	quickListNode* next;

	inline quickListNode(_Ty _val, quickListNode* _prev, quickListNode* _next) : val(_val), prev(_prev), next(_next) {};

public:

	_Ty val;

	inline quickListNode* getPrev()
	{
		return prev;
	};

	inline quickListNode* getNext()
	{
		return next;
	};


template <class _Tx> friend class quickList;
};



template<class _Tx> class quickList
{
public:

private:
	UINT elementsNumber;

	quickListNode<_Tx>* first;
	quickListNode<_Tx>* last;

public:

	quickList()
	{
		elementsNumber = 0;
		first = NULL;
		last = NULL;
	};

	~quickList()
	{
		if (elementsNumber)
		{
			quickListNode<_Tx>* n = first;
			quickListNode<_Tx>* o = NULL;
			while(n)
			{
				o = n->next;
				delete n;
				n = o;
			}
		}
	};

	inline _Tx front()
	{
		return first->val;
	};

	inline _Tx back()
	{
		return last->val;
	};


	inline UINT size()
	{
		return elementsNumber;
	};


	inline quickListNode<_Tx>* begin()
	{
		return first;
	};

	inline quickListNode<_Tx>* end()
	{
		return last;
	};


	inline void pushFront(_Tx _val)
	{
		quickListNode<_Tx>* n = new quickListNode<_Tx>(_val, NULL, first);
		if (first)
			first->prev = n;
		else
			last = n;
		first = n;

		elementsNumber++;
	};

	inline void pushBack(_Tx _val)
	{
		quickListNode<_Tx>* n = new quickListNode<_Tx>(_val, last, NULL);
		if (last)
			last->next = n;
		else
			first = n;
		last = n;

		elementsNumber++;
	};

	inline void popFront()
	{
		if (first)
		{
			quickListNode<_Tx>* n = first;
			first = first->next;
			if (first)
				first->prev = NULL;
			else
				first = last = NULL;

			delete n;
			elementsNumber--;
		}
	};

	inline void popBack()
	{
		if (last)
		{
			quickListNode<_Tx>* n = last;
			last = last->prev;
			if (last)
				last->next = NULL;
			else
				first = last = NULL;
			delete n;
			elementsNumber--;
		}
	};

	inline quickListNode<_Tx>* remove(quickListNode<_Tx>* _node)
	{
		if (_node->prev)
			_node->prev->next = _node->next;
		else
			first = _node->next;

		if (_node->next)
			_node->next->prev = _node->prev;
		else
			last = _node->prev;

		quickListNode<_Tx>* n = _node->next;
		delete _node;
		elementsNumber--;
		return n;
	};

	inline quickListNode<_Tx>* insertAfter(quickListNode<_Tx>* _node, _Tx _val)
	{
		quickListNode<_Tx>* n = NULL;

		if (_node)
		{
			n = new quickListNode<_Tx>(_val, _node, _node->next);

			if (_node->next)
				_node->next->prev = n;
			else
				last = n;

			_node->next = n;
		}
		else
		{
			n = new quickListNode<_Tx>(_val, NULL, first);

			if (first)
				first->prev = n;
			else
				last = n;

			first = n;
		}

		elementsNumber++;

		return n;
	};

	inline quickListNode<_Tx>* insertBefore(quickListNode<_Tx>* _node, _Tx _val)
	{
		quickListNode<_Tx>* n = NULL;

		if (_node)
		{
			n = new quickListNode<_Tx>(_val, _node->prev, _node);

			if (_node->prev)
				_node->prev->next = n;
			else
				first = n;

			_node->prev = n;
		}
		else
		{
			n = new quickListNode<_Tx>(_val, last, NULL);

			if (last)
				last->next = n;
			else
				first = n;

			last = n;
		}

		elementsNumber++;

		return n;
	};

};



#endif //_QUICK_LIST
#pragma once

#ifndef _KEYFRAME_LOOKUP_MAP
#define _KEYFRAME_LOOKUP_MAP

#include "utility.h"



template< class _Key, class _Value, class _Compare = std::less<_Key> > class KeyframeLookupMap
{
private:

	struct TreeNode;
	struct TreeNode : public pair < _Key, _Value >
	{
		TreeNode* pListLeft;
		TreeNode* pListRight;

		//TreeNode* pTreeParent;
		TreeNode* pTreeLeft;
		TreeNode* pTreeRight;

		TreeNode() : pair < _Key, _Value >::pair(), pListLeft(NULL), pListRight(NULL), pTreeLeft(NULL), pTreeRight(NULL) {};
		TreeNode( _Key &_key, _Value &_value ) : pair < _Key, _Value >::pair( _key, _value ), pListLeft(NULL), pListRight(NULL), pTreeLeft(NULL), pTreeRight(NULL) {};
	};

public:

	class Iterator;
	class Iterator
	{
		friend class KeyframeLookupMap;
		friend class Iterator;
	private:
		TreeNode* pVal;

		Iterator( TreeNode* _pValue ) : pVal( _pValue ) {};

	public:

		Iterator( const Iterator &it ) : pVal( it.pVal ) {};

		Iterator& operator++ ()		{	if ( pVal->pListRight )	pVal = pVal->pListRight;	return *this;	};
		Iterator operator++ (int)	{	Iterator tmp(*this);	operator++();	return tmp;	};
		Iterator& operator-- ()		{	if ( pVal->pListLeft )	pVal = pVal->pListLeft;	return *this;	};
		Iterator operator-- (int)	{	Iterator tmp(*this);	operator--();	return tmp;	};

		bool operator== ( const Iterator& it )		{	return pVal == it.pVal;	};
		bool operator!= ( const Iterator& it )		{	return pVal != it.pVal;	};

		pair< _Key, _Value >& operator*() { return pVal; };
	};

	struct StorageContainer
	{
		_Key key;
		_Value value;
	};


private:

	TreeNode* generateTree( TreeNode** _ppList, UINT _first, UINT _last)
	{
		UINT piv = _first + ( ( _last - _first ) / 2 );

		_ppList[piv]->pTreeLeft = _first < piv ? generateTree( _ppList, _first, piv-1 ) : NULL;
		_ppList[piv]->pTreeRight = piv < _last ? generateTree( _ppList, piv+1, _last ) : NULL;

		return _ppList[piv];
	};


	_Compare compare;

	TreeNode* pRoot;

	UINT elementsNumber;

	TreeNode* pNodeBeforeList;
	TreeNode* pNodeAfterList;


public:

	KeyframeMap() : pRoot(NULL), elementsNumber(0), pNodeBeforeList(NULL), pNodeAfterList(NULL)
	{
		compare = _Compare();

		pNodeBeforeList = new TreeNode;
		pNodeAfterList = new TreeNode;
	};

	KeyframeMap( const _Compare &_compare ) : pRoot(NULL), elementsNumber(0), pNodeBeforeList(NULL), pNodeAfterList(NULL)
	{
		compare = _compare;

		pNodeBeforeList = new TreeNode;
		pNodeAfterList = new TreeNode;
	};


	~KeyframeMap()
	{
		TreeNode* pTmp1 = NULL;
		TreeNode* pTmp2 = pNodeBeforeList;

		while ( pTmp2 )
		{
			pTmp1 = pTmp2;
			pTmp2 = pTmp2->pListRight;
			SAFE_DELETE( pTmp1 );
		}

		pRoot = NULL;
		elementsNumber = 0;
		pNodeBeforeList = NULL;
		pNodeAfterList = NULL;

	};


	_Compare getComparator()
	{
		return compare;
	};


	void insert( StorageContainer* _pContainer, UINT _elements )
	{
		if ( !_elements )
			return;


		TreeNode** ppTmp = new TreeNode* [_elements];

		for ( UINT i = 0; i < _elements; ++i )
		{
			ppTmp[i] = new TreeNode( _pContainer[i].key, _pContainer[i].value );
		}


		bool flag = true;

		while ( flag )
		{
			flag = false;

			for ( UINT i = 1; i < _elements; ++i )
			{
				if ( !compare( ppTmp[i-1]->first, ppTmp[i]->first ) )
				{
					TreeNode* tmp = ppTmp[i-1];
					ppTmp[i-1] = ppTmp[i];
					ppTmp[i] = tmp;

					flag = true;
				}
			}
		}


		ZeroMemory( pNodeBeforeList, sizeof(TreeNode) );
		ZeroMemory( pNodeAfterList, sizeof(TreeNode) );

		pNodeBeforeList->pListRight = ppTmp[0];
		ppTmp[0]->pListLeft = pNodeBeforeList;

		for ( UINT i = 1; i < _elements; ++i )
		{
			ppTmp[i-1]->pListRight = ppTmp[i];
			ppTmp[i]->pListLeft = ppTmp[i-1];
		}

		ppTmp[_elements-1]->pListRight = pNodeAfterList;
		pNodeAfterList->pListLeft = ppTmp[_elements-1];


		pRoot = generateTree( ppTmp, 0, _elements-1 );

		delete [] ppTmp;

		elementsNumber = _elements;

		return;
	};


	void insert( _Key* _pKey, _Value* _pValue, UINT _elements )
	{
		if ( !_elements )
			return;


		TreeNode** ppTmp = new TreeNode* [_elements];

		for ( UINT i = 0; i < _elements; ++i )
		{
			ppTmp[i] = new TreeNode( _pKey[i], _pValue[i] );
		}


		bool flag = true;

		while ( flag )
		{
			flag = false;

			for ( UINT i = 1; i < _elements; ++i )
			{
				if ( !compare( ppTmp[i-1]->first, ppTmp[i]->first ) )
				{
					TreeNode* tmp = ppTmp[i-1];
					ppTmp[i-1] = ppTmp[i];
					ppTmp[i] = tmp;

					flag = true;
				}
			}
		}


		ZeroMemory( pNodeBeforeList, sizeof(TreeNode) );
		ZeroMemory( pNodeAfterList, sizeof(TreeNode) );

		pNodeBeforeList->pListRight = ppTmp[0];
		ppTmp[0]->pListLeft = pNodeBeforeList;

		for ( UINT i = 1; i < _elements; ++i )
		{
			ppTmp[i-1]->pListRight = ppTmp[i];
			ppTmp[i]->pListLeft = ppTmp[i-1];
		}

		ppTmp[_elements-1]->pListRight = pNodeAfterList;
		pNodeAfterList->pListLeft = ppTmp[_elements-1];


		pRoot = generateTree( ppTmp, 0, _elements-1 );

		delete [] ppTmp;

		elementsNumber = _elements;

		return;
	};


	void insert( pair< _Key, _Value >* _pPairs, UINT _elements )
	{
		if ( !_elements )
			return;


		TreeNode** ppTmp = new TreeNode* [_elements];

		for ( UINT i = 0; i < _elements; ++i )
		{
			ppTmp[i] = new TreeNode( _pPairs[i].first, _pPairs[i].second );
		}


		bool flag = true;

		while ( flag )
		{
			flag = false;

			for ( UINT i = 1; i < _elements; ++i )
			{
				if ( !compare( ppTmp[i-1]->first, ppTmp[i]->first ) )
				{
					TreeNode* tmp = ppTmp[i-1];
					ppTmp[i-1] = ppTmp[i];
					ppTmp[i] = tmp;

					flag = true;
				}
			}
		}


		ZeroMemory( pNodeBeforeList, sizeof(TreeNode) );
		ZeroMemory( pNodeAfterList, sizeof(TreeNode) );

		pNodeBeforeList->pListRight = ppTmp[0];
		ppTmp[0]->pListLeft = pNodeBeforeList;

		for ( UINT i = 1; i < _elements; ++i )
		{
			ppTmp[i-1]->pListRight = ppTmp[i];
			ppTmp[i]->pListLeft = ppTmp[i-1];
		}

		ppTmp[_elements-1]->pListRight = pNodeAfterList;
		pNodeAfterList->pListLeft = ppTmp[_elements-1];


		pRoot = generateTree( ppTmp, 0, _elements-1 );

		delete [] ppTmp;

		elementsNumber = _elements;

		return;
	};


	UINT size() const
	{
		return elementsNumber;
	};


	bool empty() const
	{
		return elementsNumber == 0;
	};


	void clear()
	{
		TreeNode* pTmp1 = NULL;
		TreeNode* pTmp2 = pNodeBeforeList;

		while ( pTmp2 )
		{
			pTmp1 = pTmp2;
			pTmp2 = pTmp2->pListRight;
			SAFE_DELETE( pTmp1 );
		}

		pRoot = NULL;
		elementsNumber = 0;
		pNodeBeforeList = NULL;
		pNodeAfterList = NULL;
	};


	Iterator find( _Key keyValue )
	{
		TreeNode* pTmp = pRoot;

		while ( pTmp )
		{
			if ( compare( keyValue, pTmp->first ) )
				pTmp = pTmp->pTreeLeft;
			else if ( compare( pTmp->first, keyValue ) )
				pTmp = pTmp->pTreeRight;
			else
				return Iterator( pTmp );
		}

		return Iterator( pNodeAfterList );
	};


	Iterator findBefore( _Key keyValue )
	{
		TreeNode* pTmpParent = NULL;
		TreeNode* pTmp = pRoot;

		while ( pTmp )
		{
			if ( compare( keyValue, pTmp->first ) )
			{
				pTmpParent = pTmp;
				pTmp = pTmp->pTreeLeft;
			}
			else if ( compare( pTmp->first, keyValue ) )
			{
				pTmpParent = pTmp;
				pTmp = pTmp->pTreeRight;
			}
			else
				return Iterator( pTmp->pListLeft );
		}

		return compare( keyValue, pTmpParent->first ) ? Iterator( pTmpParent->pListLeft ) : Iterator( pTmpParent );
	};


	Iterator findAfter( _Key keyValue )
	{
		TreeNode* pTmpParent = NULL;
		TreeNode* pTmp = pRoot;

		while ( pTmp )
		{
			if ( compare( keyValue, pTmp->first ) )
			{
				pTmpParent = pTmp;
				pTmp = pTmp->pTreeLeft;
			}
			else if ( compare( pTmp->first, keyValue ) )
			{
				pTmpParent = pTmp;
				pTmp = pTmp->pTreeRight;
			}
			else
				return Iterator( pTmp->pListRight );
		}

		return compare( keyValue, pTmpParent->first ) ? Iterator( pTmpParent ) : Iterator( pTmpParent->pListRight );
	};


	Iterator begin()
	{
		return Iterator( pNodeBeforeList->pTreeRight );
	};

	Iterator end()
	{
		return Iterator( pNodeAfterList );
	};


	Iterator rbegin()
	{
		return Iterator( pNodeAfterList->pTreeLeft );
	};

	Iterator rend()
	{
		return Iterator( pNodeBeforeList );
	};


	KeyframeLookupMap& operator = ( const KeyframeLookupMap &_keyframeLookupMap )
	{
		compare = _keyframeLookupMap->getComparator();

		TreeNode* firstNode = _keyframeLookupMap.begin().pVal;
		UINT elements = _keyframeLookupMap.size();

		if ( !elements )
			return;

		TreeNode* pTmpNode = firstNode;
		TreeNode** ppTmp = new TreeNode* [elements];

		for ( UINT i = 0; i < elements; ++i )
		{
			ppTmp[i] = new TreeNode( pTmpNode->first, pTmpNode->second );
			pTmpNode = pTmpNode->pListRight;
		}


		ZeroMemory( pNodeBeforeList, sizeof(TreeNode) );
		ZeroMemory( pNodeAfterList, sizeof(TreeNode) );

		pNodeBeforeList->pListRight = ppTmp[0];
		ppTmp[0]->pListLeft = pNodeBeforeList;

		for ( UINT i = 1; i < elements; ++i )
		{
			ppTmp[i-1]->pListRight = ppTmp[i];
			ppTmp[i]->pListLeft = ppTmp[i-1];
		}

		ppTmp[elements-1]->pListRight = pNodeAfterList;
		pNodeAfterList->pListLeft = ppTmp[elements-1];

		pRoot = generateTree( ppTmp, 0, elements-1 );
		delete [] ppTmp;
		elementsNumber = elements;

		return (*this);
	};
};


#endif //_KEYFRAME_LOOKUP_MAP

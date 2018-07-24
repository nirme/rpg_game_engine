#include "Hierarchical_Animation_Set.h"




Resource <HierarchicalAnimationSet*>::ResourceMap Resource<HierarchicalAnimationSet*>::loadedResMap;



HierarchicalAnimationSet::HierarchicalAnimationSet() :	pRootFrame(NULL), 
														pFramesArray(NULL), 
{};


HierarchicalAnimationSet::~HierarchicalAnimationSet()
{
	SAFE_DELETE_ARRAY( pFramesArray );
};


void HierarchicalAnimationSet::addRef()
{
	refCount++;
};


void HierarchicalAnimationSet::release()
{
	refCount--;
	if ( !refCount )
	{
		delete this;
	}
};


void HierarchicalAnimationSet::registerBones( char** _bonesNames, USHORT _bonesNumber )
{
	if ( !_bonesNumber )
		return 0;

	vStrBonesNames.resize( _bonesNumber );
	mIndStrBonesNames.clear();

	for ( USHORT i = 0; i < _bonesNumber; ++i )
	{
		pair< map< string, USHORT >::iterator, bool> ret = mIndStrBonesNames.insert( map< string, USHORT >::value_type( _bonesNames[ i ], i ) );
		vStrBonesNames[ i ] = &(ret.first->first);
	}
};


int HierarchicalAnimationSet::registerFrame( USHORT _boneIndex, const USHORT* _boneFirstSiblingIndex, const USHORT* _boneFirstChildIndex, bool _isRoot )
{
	if ( _boneIndex >= vStrBonesNames.size() )
		return -20;

	if ( !pFramesArray )
	{
		pFramesArray = new BONE_FRAME [ vStrBonesNames.size() ];
		if ( !pFramesArray )
			return -10;

		for ( USHORT i = 0; i < vStrBonesNames.size(); ++i )
			pFramesArray[i].boneIndex = i;
	}

	if ( _isRoot )
		pRootFrame = &(pFramesArray[_boneIndex]);

	pFramesArray[_boneIndex].pFirstSibling = _boneFirstSiblingIndex ? &(pFramesArray[*_boneFirstSiblingIndex]) : NULL;
	pFramesArray[_boneIndex].pFirstChild = _boneFirstChildIndex ? &(pFramesArray[*_boneFirstChildIndex]) : NULL;

	return 0;
};


void HierarchicalAnimationSet::registerKeyframedAnimation( KeyframedAnimation* _pAnimation )
{
	vpAnimations.push_back( _pAnimation );
};


USHORT HierarchicalAnimationSet::getBoneIndex( const string& _boneName )
{
	map< string, USHORT >::iterator it = mIndStrBonesNames.find(_boneName);

	if ( it == mIndStrBonesNames.end() )
		throw NotFoundException("bone name not found");

	return it->second;
};


int HierarchicalAnimationSet::getAnimationIndexByName( const char* _pAnimationName, USHORT* _pIndex )
{
	AnimationMap::iterator it = m_AnimationsIndices.find( _pAnimationName );

	if ( it == m_AnimationsIndices.end() )
		return 1;

	*_pIndex = (*it).second;
	return 0;
};


int HierarchicalAnimationSet::getAnimationIndexByName( const string* _pAnimationName, USHORT* _pIndex )
{
	AnimationMap::iterator it = m_AnimationsIndices.find( _pAnimationName->c_str() );

	if ( it == m_AnimationsIndices.end() )
		return 1;

	*_pIndex = (*it).second;
	return 0;
};


int HierarchicalAnimationSet::cloneFrameHierarchy( BONE_FRAME** _ppFramesArray, BONE_FRAME** _ppRootFrame )
{
	BONE_FRAME* pTmpFramesArray = new BONE_FRAME [ vStrBonesNames.size() ];

	if ( !pTmpFramesArray )
		return -10;

	for ( USHORT i = 0; i < vStrBonesNames.size(); ++i )
	{
		pTmpFramesArray[i].boneIndex = i;
		pTmpFramesArray[i].pFirstChild = &( pTmpFramesArray[ pFramesArray[i].pFirstChild->boneIndex ] );
		pTmpFramesArray[i].pFirstSibling = &( pTmpFramesArray[ pFramesArray[i].pFirstSibling->boneIndex ] );
		pTmpFramesArray[i].pTransform = NULL;
	}

	*_ppFramesArray = pTmpFramesArray;
	*_ppRootFrame = pTmpFramesArray[ pRootFrame->boneIndex ];

	return 0;
};




HAnimationSet::~HAnimationSet()
{
	SAFE_DELETE( resource );
};


int HAnimationSet::setResource( HierarchicalAnimationSet* _pHierarchicalAnimationSet )
{
	#ifdef(MULTI_THREADED_LOADING)
		boost::mutex::scoped_lock lock(mutex);
	#endif //MULTI_THREADED_LOADING

	if (loaded)
		return 1;

	resource = _pHierarchicalAnimationSet;
	loaded = (resource != NULL) ? true : false;

	#ifdef(MULTI_THREADED_LOADING)
		conditionVar.notify_all();
	#endif //MULTI_THREADED_LOADING

	return 0;
};


#ifndef RESOURCE_MAP_WITH_STRINGS
	HAnimationSet* HAnimationSet::getResource( UINT _id )
#else //RESOURCE_MAP_WITH_STRINGS
	HAnimationSet* HAnimationSet::getResource( string _id )
#endif //RESOURCE_MAP_WITH_STRINGS
{
	return reinterpret_cast <HAnimationSet*> ( getBaseResource( _id ) );
};


#ifdef(MULTI_THREADED_LOADING)
	#ifndef RESOURCE_MAP_WITH_STRINGS
		HAnimationSet* HAnimationSet::getResourceMultiThreaded( UINT _id )
	#else //RESOURCE_MAP_WITH_STRINGS
		HAnimationSet* HAnimationSet::getResourceMultiThreaded( string _id )
	#endif //RESOURCE_MAP_WITH_STRINGS
	{
		return reinterpret_cast <HAnimationSet*> ( getBaseResourceMultiThreaded( _id ) );
	};
#endif //MULTI_THREADED_LOADING

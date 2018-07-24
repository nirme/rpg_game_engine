#pragma once

#ifndef _HIERARCHICAL_ANIMATION_SET
#define _HIERARCHICAL_ANIMATION_SET

#include "utility.h"
#include "Keyframe_Map.h"
#include "Keyframe_Lookup_Map.h"
#include "Template_Basic_Resource.h"



struct BONE_FRAME;
struct BONE_FRAME
{
	USHORT boneIndex;
	D3DXMATRIX* pTransform;

	BONE_FRAME* pFirstSibling;
	BONE_FRAME* pFirstChild;


	BONE_FRAME() : boneIndex(-1), pTransform(NULL), pFirstSibling(NULL), pFirstChild(NULL) {};

	BONE_FRAME( USHORT _boneIndex, BONE_FRAME* _pFirstSibling, BONE_FRAME* _pFirstChild ) : boneIndex(_boneIndex), pTransform(NULL), pFirstSibling(_pFirstSibling), pFirstChild(_pFirstChild)
	{};
};
////

class QuaternionNlerp
{
public:
	inline D3DXQUATERNION* operator() ( D3DXQUATERNION* _output, const D3DXQUATERNION* _value1, const D3DXQUATERNION* _value2, float s )
	{
		return D3DXQuaternionNormalize( _output, (D3DXQUATERNION*) D3DXVec4Lerp( (D3DXVECTOR4*) _output, (D3DXVECTOR4*) _value1, (D3DXVECTOR4*) _value2, s) );
	};
};
////

typedef		D3DXVECTOR3* (*Vector3Interpolator) ( D3DXVECTOR3*, const D3DXVECTOR3*, const D3DXVECTOR3*, float );
typedef		KeyframeMap<float,D3DXVECTOR3,Vector3Interpolator>		Vector3KeyframeMap;
typedef		KeyframeMap<float,D3DXQUATERNION,QuaternionNlerp>		QuaternionKeyframeMap;


struct BoneKeyframedAnimation
{
	USHORT boneIndex;

	Vector3KeyframeMap km_scaleKeyframes;
	QuaternionKeyframeMap km_rotationKeyframes;
	Vector3KeyframeMap km_translationKeyframes;


	BoneKeyframedAnimation() :	boneIndex(0), 
								km_scaleKeyframes( &D3DXVec3Lerp ), 
								km_rotationKeyframes( QuaternionNlerp() ), 
								km_translationKeyframes( &D3DXVec3Lerp )
	{};
};
////

class AnimatedNode;

class UserDefinedCallbackKey
{
public:
	virtual ~UserDefinedCallbackKey() = 0;
	virtual UserDefinedCallbackKey& clone( UserDefinedCallbackKey** _userDefinedCallbackKey ) = 0;
	virtual setupLocal( AnimatedNode* _pAnimatedNode ) = 0;
	virtual int trigger() = 0;
};
////

typedef		KeyframeLookupMap<float, UserDefinedCallbackKey*>		CallbackKeyframeMap;


struct KeyframedAnimation
{
	string animationName;

	BoneKeyframedAnimation* pBonesAnimations;
	USHORT bonesAnimationsNumber;

	CallbackKeyframeMap klm_callbackKeys;

	float totalLength;


	KeyframedAnimation() :	pBonesAnimations(NULL), bonesAnimationsNumber(0), 
							length(0.0f), 
	{};

	~KeyframedAnimation()
	{
		SAFE_DELETE_ARRAY( pBonesAnimations );
	};
};
////



class HierarchicalAnimationSet
{
private:

	// bones info
	vector<string*> vStrBonesNames;
	map< string, USHORT > mIndStrBonesNames;

	// hierarchy info
	BONE_FRAME* pRootFrame;
	BONE_FRAME* pFramesArray;

	// animations
	vector< KeyframedAnimation* > vpAnimations;
	typedef map< const char*, USHORT > AnimationMap;
	AnimationMap m_AnimationsIndices;


public:

	HierarchicalAnimationSet();
	~HierarchicalAnimationSet();

	void registerBones( char** _bonesNames, USHORT _bonesNumber );
	int registerFrame( USHORT _boneIndex, const USHORT* _boneFirstSiblingIndex, const USHORT* _boneFirstChildIndex, bool _isRoot = false );
	int registerKeyframedAnimation( KeyframedAnimation* _pAnimation );

	inline USHORT getBonesNumber()	{	return vStrBonesNames.size();	};
	inline const string* getBoneName( USHORT _boneIndex )	{	return _boneIndex >= bonesNumber ? NULL : vStrBonesNames[_boneIndex];	};
	inline const char* getBoneNameChr( USHORT _boneIndex )	{	return _boneIndex >= bonesNumber ? NULL : vStrBonesNames[_boneIndex]->c_str();	};

	USHORT getBoneIndex( const string& _boneName );

	inline USHORT getAnimationsNumber()	{	return vpAnimations.size();	};

	int getAnimationIndexByName( const char* _pAnimationName, USHORT* _pIndex );
	int getAnimationIndexByName( const string* _pAnimationName, USHORT* _pIndex );

	inline const KeyframedAnimation* getAnimation( USHORT _animationIndex )	{	return _animationIndex >= vpAnimations.size() ? NULL : vpAnimations[ _animationIndex ];	};

	int cloneFrameHierarchy( BONE_FRAME** _ppFramesArray, BONE_FRAME** _ppRootFrame );
};
////




class HAnimationSet : public Resource <HierarchicalAnimationSet*>
{
private:
	~HAnimationSet();

public:

	#ifndef RESOURCE_MAP_WITH_STRINGS
		HAnimationSet( UINT _id ) : Resource( _id, NULL )	{};
	#else //RESOURCE_MAP_WITH_STRINGS
		HAnimationSet( string _id ) : Resource( _id, NULL )	{};
	#endif //RESOURCE_MAP_WITH_STRINGS


	int setResource( HierarchicalAnimationSet* _pHierarchicalAnimationSet );


	#ifndef RESOURCE_MAP_WITH_STRINGS
		static HAnimationSet* getResource( UINT _id );
	#else //RESOURCE_MAP_WITH_STRINGS
		static HAnimationSet* getResource( string _id );
	#endif //RESOURCE_MAP_WITH_STRINGS


	#ifdef(MULTI_THREADED_LOADING)
		#ifndef RESOURCE_MAP_WITH_STRINGS
			static HAnimationSet* getResourceMultiThreaded( UINT _id );
		#else //RESOURCE_MAP_WITH_STRINGS
			static HAnimationSet* getResourceMultiThreaded( string _id );
		#endif //RESOURCE_MAP_WITH_STRINGS
	#endif //MULTI_THREADED_LOADING


	int onLostDevice(){ return 0; };
	int onResetDevice(){ return 0; };
};


#endif //_HIERARCHICAL_ANIMATION_SET

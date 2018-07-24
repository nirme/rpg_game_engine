#pragma once

#ifndef _ANIMATED_NODE
#define _ANIMATED_NODE


#include "utility.h"
#include "Core_Defines.h"

#include "Geo_Node.h"
#include "Renderable_Node.h"

#include "Core_Graphic_Object.h"

#include "Graphic_Node.h"




class AnimatedNode
{
protected:


	struct AnimationSetKerning
	{
		double transitionTime;
		bool speedTransition;
		bool weigtTransition;
		bool resetPosition;
		D3DXTRANSITION_TYPE transitionType;
	};

	struct AnimationSetHolder
	{
		const char* name;
		USHORT index;
		ID3DXAnimationSet* animationSet;
		USHORT defaultTrack;

		vector <AnimationSetKerning*> v_asKerning;
	};


	typedef		map < const char*, AnimationSetHolder* >		AnimationSetMap;
	typedef		vector < AnimationSetHolder* >					AnimationSetVector;


	AnimationInfo* pAnimationInstance;

	ID3DXAnimationController* pAnimController;
	AnimationSetMap m_animationSets;
	AnimationSetVector v_animationSets;

	BYTE tracksNumber;
	vector < USHORT > v_trackAnimationSets;
	short actualTrack;
	USHORT actualTrackLoopCount;

	bool updateOnRenderOnly;


public:

	int playAnimation( USHORT _animationSetIndex, USHORT _trackLoopCount = 0 )
	{
		if ( _animationSetIndex >= v_animationSets.size() )
			return 1;

		BYTE nextTrack = ( actualTrack + 1 ) % tracksNumber;

		v_trackAnimationSets[nextTrack] = _animationSetIndex;

		AnimationSetKerning* kern;

		v_trackAnimationSets[ actualTrack ]->

		if ( )
			kern
		= v_animationSets[ _animationSetIndex ]->v_asKerning[ v_trackAnimationSets[ actualTrack ] ];

		pAnimController->SetTrackAnimationSet( nextTrack, v_animationSets[ animationSetIndex ]->animationSet );

		pAnimController->UnkeyAllTrackEvents( nextTrack );

		if ( kern )
		{
			double time = pAnimController->GetTime();
			pAnimController->KeyTrackEnable( actualTrack, FALSE, time + kern->transitionTime );

			if ( kern->speedTransition )
			{
				pAnimController->KeyTrackSpeed( actualTrack, 0.0f, time, kern->transitionTime, kern->transitionType );
				pAnimController->KeyTrackSpeed( nextTrack, 1.0f, time, kern->transitionTime, kern->transitionType );
			}

			if ( kern->weigtTransition )
			{
				pAnimController->KeyTrackWeight( actualTrack, 0.0f, time, kern->transitionTime, kern->transitionType );
				pAnimController->KeyTrackWeight( nextTrack, 1.0f, time, kern->transitionTime, kern->transitionType );
			}

			if ( kern->resetPosition )
				pAnimController->SetTrackPosition( nextTrack, 0.0 );

			pAnimController->SetTrackEnable( nextTrack, TRUE );
		}
		else
		{
			double time = pAnimController->GetTime();
			pAnimController->KeyTrackEnable( actualTrack, FALSE, time + kern->transitionTime );

			if ( kern->speedTransition )
			{
				pAnimController->KeyTrackSpeed( actualTrack, 0.0f, time, kern->transitionTime, kern->transitionType );
				pAnimController->KeyTrackSpeed( nextTrack, 1.0f, time, kern->transitionTime, kern->transitionType );
			}

			if ( kern->weigtTransition )
			{
				pAnimController->KeyTrackWeight( actualTrack, 0.0f, time, kern->transitionTime, kern->transitionType );
				pAnimController->KeyTrackWeight( nextTrack, 1.0f, time, kern->transitionTime, kern->transitionType );
			}

			if ( kern->resetPosition )
				pAnimController->SetTrackPosition( nextTrack, 0.0 );

			pAnimController->SetTrackEnable( nextTrack, TRUE );
		}


pAC->GetAnimationSet( m_dwAnimIdxLoiter, &pAS );
pAC->SetTrackAnimationSet( dwNewTrack, pAS );
pAS->Release();

pAC->UnkeyAllTrackEvents( m_dwCurrentTrack );
pAC->UnkeyAllTrackEvents( dwNewTrack );

pAC->KeyTrackEnable( m_dwCurrentTrack, FALSE, m_dTimeCurrent + IDLE_TRANSITION_TIME );
pAC->KeyTrackSpeed( m_dwCurrentTrack, 0.0f, m_dTimeCurrent, IDLE_TRANSITION_TIME, D3DXTRANSITION_LINEAR );
pAC->KeyTrackWeight( m_dwCurrentTrack, 0.0f, m_dTimeCurrent, IDLE_TRANSITION_TIME, D3DXTRANSITION_LINEAR );
pAC->SetTrackEnable( dwNewTrack, TRUE );
pAC->KeyTrackSpeed( dwNewTrack, 1.0f, m_dTimeCurrent, IDLE_TRANSITION_TIME, D3DXTRANSITION_LINEAR );
pAC->KeyTrackWeight( dwNewTrack, 1.0f, m_dTimeCurrent, IDLE_TRANSITION_TIME, D3DXTRANSITION_LINEAR );
if( bResetPosition )
    pAC->SetTrackPosition( dwNewTrack, 0.0 );

m_dwCurrentTrack = dwNewTrack;

pAC->Release();












ID3DXAnimationController* pAnimController;
AnimationSetMap m_animationSets;
AnimationSetVector v_animationSets;

BYTE tracksNumber;
vector < USHORT > v_trackAnimationSets;
BYTE actualTrack;
USHORT actualTrackLoopCount;
	};


/*

ID3DXMesh* pBlendedMesh;
ID3DXSkinInfo* pSkinInfo;

SubsetsBuffer* subsetsData;

DWORD dwMaxNumFaceInfls;

DWORD dwPaletteSize;


// if equipable
D3DXMATRIX** bonesPalettePointers;

//if controlable
ID3DXAnimationController* pAnimController;
D3DXFRAME* pRootFrame;

DWORD workingPaletteTimestamp;

D3DXMATRIX** bonesOffsetsArray;
D3DXMATRIX** bonesTransformsArray;
D3DXMATRIX* bonesPalette;

	*/

public:

	virtual void setGraphicObject( GraphicObject* _graphicObject )
	{
		SAFE_RELEASE( graphicObject );
		graphicObject = _graphicObject;
		pAnimationInstance = graphicObject->getAnimationInfo();
	};

	inline AnimationInfo* getAnimationInstance()	{	return pAnimationInstance;	};


	virtual void setup(	const char* _name, 
						BaseNode* _pParent, 
						WorldPosition* _pWorldPosition, 
						GraphicObject* _pGraphicObject, 
						BoundingVolume* _pBoundingVolume, 
						USHORT _childrenNumber = 0, 
						BaseNode* const* _v_children = NULL );

	virtual void setup(	const string &_name, 
						BaseNode* _pParent, 
						WorldPosition* _pWorldPosition, 
						GraphicObject* _pGraphicObject, 
						BoundingVolume* _pBoundingVolume, 
						USHORT _childrenNumber = 0, 
						BaseNode* const* _v_children = NULL );

	virtual void setup(	const char* _name, 
						BaseNode* _pParent, 
						WorldPosition* _pWorldPosition, 
						GraphicObject* _pGraphicObject, 
						BoundingVolume* _pBoundingVolume, 
						USHORT _childrenNumber, 
						char* const* _v_childrenNames );

	virtual void setup(	const string &_name, 
						BaseNode* _pParent, 
						WorldPosition* _pWorldPosition, 
						GraphicObject* _pGraphicObject, 
						BoundingVolume* _pBoundingVolume, 
						USHORT _childrenNumber, 
						string* const _v_childrenNames );











	AnimatedNode() : GraphicNode(), pAnimationInstance(NULL)
	{};

};

#endif //_ANIMATED_NODE

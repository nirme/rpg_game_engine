#pragma once

#ifndef _ANIMATED_NODE
#define _ANIMATED_NODE


#include "Graphic_Node.h"



class AnimatedNode : public GraphicNode
{
protected:
	AnimationInfo* clonedAnimationInfo;

public:
	AnimatedNode(string const & _name, BaseNode* _parentNode = NULL) : GraphicNode(_name, _parentNode), clonedAnimationInfo(NULL)
	{};

	AnimatedNode(	string const & _name,
					GraphicObject* _graphicObject,
					WorldPosition* _pWorldPosition = NULL,
					BoundingSphere* _volume = NULL,
					BaseNode* _parentNode = NULL) : GraphicNode(_name, _graphicObject, _pWorldPosition, _volume, _parentNode), clonedAnimationInfo(NULL)
	{
		clonedAnimationInfo = graphicObject->getAnimationInfo();
	};

	virtual ~AnimatedNode()
	{};


	virtual int updateNode()
	{
		if (clonedAnimationInfo)
			return 0;

		if (graphicObject->isLoaded())
		{
			clonedAnimationInfo = graphicObject->getAnimationInfo();
			return 0;
		}
		return 1;
	};

	virtual int playAnimationByName(const char* _name)
	{
		if (updateNode())
			return 1;

		//clonedAnimationInfo->pAnimController->GetAnimationSetByName
	};

	virtual int playAnimationByIndex(const UINT _index)
	{
/*		if (updateNode())
			return 1;

		if (_index >= clonedAnimationInfo->pAnimController->GetNumAnimationSets())
			return 2;

		ID3DXAnimationSet* tmpAnimSet;
		clonedAnimationInfo->pAnimController->GetAnimationSet(_index, &tmpAnimSet);

		D3DXVec3TransformNormal

*/





/*

    DWORD dwNewTrack = ( m_dwCurrentTrack == 0 ? 1 : 0 );
    LPD3DXANIMATIONCONTROLLER pAC;
    LPD3DXANIMATIONSET pAS;
    m_pAI->GetAnimController( &pAC );

    if( m_fSpeed == m_fSpeedWalk )
        pAC->GetAnimationSet( m_dwAnimIdxWalk, &pAS );
    else
        pAC->GetAnimationSet( m_dwAnimIdxJog, &pAS );

    pAC->SetTrackAnimationSet( dwNewTrack, pAS );
    pAS->Release();

    pAC->UnkeyAllTrackEvents( m_dwCurrentTrack );
    pAC->UnkeyAllTrackEvents( dwNewTrack );

    pAC->KeyTrackEnable( m_dwCurrentTrack, false, m_dTimeCurrent + MOVE_TRANSITION_TIME );
    pAC->KeyTrackSpeed( m_dwCurrentTrack, 0.0f, m_dTimeCurrent, MOVE_TRANSITION_TIME, D3DXTRANSITION_LINEAR );
    pAC->KeyTrackWeight( m_dwCurrentTrack, 0.0f, m_dTimeCurrent, MOVE_TRANSITION_TIME, D3DXTRANSITION_LINEAR );
    pAC->SetTrackEnable( dwNewTrack, true );
    pAC->KeyTrackSpeed( dwNewTrack, 1.0f, m_dTimeCurrent, MOVE_TRANSITION_TIME, D3DXTRANSITION_LINEAR );
    pAC->KeyTrackWeight( dwNewTrack, 1.0f, m_dTimeCurrent, MOVE_TRANSITION_TIME, D3DXTRANSITION_LINEAR );

    m_dwCurrentTrack = dwNewTrack;

    pAC->Release();

*/

		return 0;

	};

};





#endif //_ANIMATED_NODE

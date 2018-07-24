#include "HierarchicalAnimationController.h"



void UpdateFrames( BONE_FRAME* _pFrame, const D3DXMATRIX* _pRootTransform )
{
	D3DXMatrixMultiply( _pFrame->pTransform, _pRootTransform, _pFrame->pTransform );

	if ( _pFrame->pFirstSibling )
		updateFrames( _pFrame->pFirstSibling, _pRootTransform );

	if ( _pFrame->pFirstChild )
		updateFrames( _pFrame->pFirstChild, _pFrame->pTransform );
};



HierarchicalAnimationController::EventEnableTrack::EventEnableTrack( Track* _track, float _time, bool _enable ) : 
	Event( _track, _time ), 
	enable(_enable)
{};


bool HierarchicalAnimationController::EventEnableTrack::updateTrack( float _totalTime )
{
	if ( _totalTime <= time )
	{
		_track->enabled = enable;
		delete this;
		return true;
	}
	return false;
};


HierarchicalAnimationController::EventChangeSpeed::EventChangeSpeed( Track* _track, float _timeBegin, float _timeEnd, float _speedBegin, float _speedEnd ) : 
	Event( _track, _timeEnd ), 
	timeBegin( _timeBegin ), 
	oneDivByTimeLength( 1.0f / ( _timeEnd - _timeBegin ) ), 
	speedBegin( _speedBegin ), 
	speedEnd( _speedEnd ), 
	speedDiff( _speedEnd - _speedBegin )
{};


bool HierarchicalAnimationController::EventChangeSpeed::updateTrack( float _totalTime )
{
	if ( _totalTime >= time )
	{
		_track->speed = speedEnd;
		delete this;
		return true;
	}

	if ( _totalTime > timeBegin )
		_track->speed = speedBegin + ( speedDiff * ( ( _totalTime - timeBegin ) * oneDivByTimeLength ) );

	return false;
};


HierarchicalAnimationController::EventChangeWeight::EventChangeWeight( Track* _track, float _timeBegin, float _timeEnd, float _weightBegin, float _weightEnd ) : 
	Event( _track, _timeEnd ), 
	timeBegin( _timeBegin ), 
	oneDivByTimeLength( 1.0f / ( _timeEnd - _timeBegin ) ), 
	weightBegin( _weightBegin ), 
	weightEnd( _weightEnd ), 
	weightDiff( _weightEnd - _weightBegin )
{};


bool HierarchicalAnimationController::EventChangeWeight::updateTrack( float _totalTime )
{
	if ( _totalTime >= time )
	{
		_track->weight = weightEnd;
		delete this;
		return true;
	}

	if ( _totalTime > timeBegin )
		_track->weight = weightBegin + ( weightDiff * ( ( _totalTime - timeBegin ) * oneDivByTimeLength ) );

	return false;
};


HierarchicalAnimationController::EventSpeedFadeIn::EventSpeedFadeIn( Track* _track, float _timeNow, float _timeLength ) : 
	Event( _track, _timeNow + _timeLength ), 
	timeBegin( _timeNow ), 
	oneDivByTimeLength( 1.0f / _timeLength ), 
	speedBegin( _track->speed ), 
	speedDiff( 1.0f - _track->speed )
{};


bool HierarchicalAnimationController::EventSpeedFadeIn::updateTrack( float _totalTime )
{
	if ( _totalTime >= time )
	{
		_track->speed = 1.0f;
		delete this;
		return true;
	}

	if ( _totalTime > timeBegin )
	{
		_track->enabled = true;
		_track->speed = speedBegin + ( speedDiff * ( ( _totalTime - timeBegin ) * oneDivByTimeLength ) );
	}

	return false;
};


HierarchicalAnimationController::EventSpeedFadeOut::EventSpeedFadeOut( Track* _track, float _timeNow, float _timeLength ) : 
	Event( _track, _timeNow + _timeLength ), 
	timeBegin( _timeNow ), 
	oneDivByTimeLength( 1.0f / _timeLength ), 
	speedBegin( _track->speed )
{};


bool HierarchicalAnimationController::EventSpeedFadeOut::updateTrack( float _totalTime )
{
	if ( _totalTime >= time )
	{
		_track->speed = 0.0f;
		_track->enabled = false;
		delete this;
		return true;
	}

	if ( _totalTime > timeBegin )
		_track->speed = speedBegin - ( speedBegin * ( ( _totalTime - timeBegin ) * oneDivByTimeLength ) );

	return false;
};


HierarchicalAnimationController::EventWeightFadeIn::EventWeightFadeIn( Track* _track, float _timeNow, float _timeLength ) : 
	Event( _track, _timeNow + _timeLength ), 
	timeBegin( _timeNow ), 
	oneDivByTimeLength( 1.0f / _timeLength ), 
	weightBegin( _track->weight ), 
	weightDiff( 1.0f - _track->weight )
{};


bool HierarchicalAnimationController::EventWeightFadeIn::updateTrack( float _totalTime )
{
	if ( _totalTime >= time )
	{
		_track->weight = 1.0f;
		delete this;
		return true;
	}

	if ( _totalTime > timeBegin )
	{
		_track->enabled = true;
		_track->weight = weightBegin + ( weightDiff * ( ( _totalTime - timeBegin ) * oneDivByTimeLength ) );
	}

	return false;
};


HierarchicalAnimationController::EventWeightFadeOut::EventWeightFadeOut( Track* _track, float _timeNow, float _timeLength ) : 
	Event( _track, _timeNow + _timeLength ), 
	timeBegin( _timeNow ), 
	oneDivByTimeLength( 1.0f / _timeLength ), 
	weightBegin( _track->weight )
{};


bool HierarchicalAnimationController::EventWeightFadeOut::updateTrack( float _totalTime )
{
	if ( _totalTime >= time )
	{
		_track->weight = 0.0f;
		_track->enabled = false;
		delete this;
		return true;
	}

	if ( _totalTime > timeBegin )
		_track->weight = weightBegin - ( weightBegin * ( ( _totalTime - timeBegin ) * oneDivByTimeLength ) );

	return false;
};


HierarchicalAnimationController::HierarchicalAnimationController( Model* _pModel, HAnimationSet* _pResourceAnimationSet, ID3DXSkinInfo* _pSkinInfo, BYTE _maxTracksNumber, BYTE _maxEventsNumber ) : pModel(_pModel), pResourceAnimationSet(_pResourceAnimationSet), pAnimationSet(pResourceAnimationSet->getResourcePointer()), useCallbackKeys(false), useLocalCallbackKeys(false), pLocalCallbackKeys(NULL), maxTracksNumber(_maxTracksNumber), maxEventsNumber(_maxEventsNumber), paletteActual(false), timeTotal(0.0f)
{
	pModel->addRef();
	pResourceAnimationSet->addRef();

	pAnimationSet->cloneFrameHierarchy( &pRootFrame, &pFramesArray );

	ppBonesOffsets = new D3DXMATRIX* [ pAnimationSet->getBonesNumber() ];
	pPalette = new D3DXMATRIX [ pAnimationSet->getBonesNumber() ];

	for ( UINT i = 0; i < pAnimationSet->getBonesNumber(); ++i )
		ppBonesOffsets = _pSkinInfo->GetBoneOffsetMatrix(i);

	pTracks = new Track [ maxTracksNumber ];
	for ( BYTE i = 0; i < maxTracksNumber; ++i )
		pTracks[i].index = i;
};


HierarchicalAnimationController::~HierarchicalAnimationController()
{
	SAFE_RELEASE( pModel );
	SAFE_RELEASE( pResourceAnimationSet );
	SAFE_DELETE_ARRAY( pLocalCallbackKeys );
	SAFE_DELETE_ARRAY( pFramesArray );
	SAFE_DELETE_ARRAY( ppBonesOffsets);
	SAFE_DELETE_ARRAY( pPalette);
	SAFE_DELETE_ARRAY( pTracks );
	unkeyAllEvents();
};


int HierarchicalAnimationController::setupLocalCallbackKeys( bool _useLocalCallbackKeys, AnimatedNode* _pAnimatedNode )
{
	if ( !_useLocalCallbackKeys )
	{
		SAFE_DELETE_ARRAY( pLocalCallbackKeys );
		useLocalCallbackKeys = false;
		return 0;
	}

	if ( !_pAnimatedNode )
		return 16;

	USHORT animationsNumber = pAnimationSet->getAnimationsNumber();
	SAFE_DELETE_ARRAY( pLocalCallbackKeys );
	pLocalCallbackKeys = new LocalCallbackKeys [ animationsNumber ];

	if ( !pLocalCallbackKeys )
	{
		useLocalCallbackKeys = false;
		return 4;
	}

	for ( USHORT i = 0; i < animationsNumber; ++i )
	{
		pLocalCallbackKeys[i].pKeyframedAnimation = pAnimationSet->getAnimation(i);
		pLocalCallbackKeys[i].klm_callbackKeys = pLocalCallbackKeys[i].pKeyframedAnimation->klm_callbackKeys;

		CallbackKeyframeMap::Iterator it = pLocalCallbackKeys[i].klm_callbackKeys.begin();
		for ( CallbackKeyframeMap::Iterator itBase = pLocalCallbackKeys[i].pKeyframedAnimation->klm_callbackKeys.begin(); itBase != pLocalCallbackKeys[i].pKeyframedAnimation->klm_callbackKeys.begin(); itBase++ )
		{
			(*itBase).second->clone( &( (*it).second ) );
			(*it).second->setupLocal( _pAnimatedNode );
			it++;
		}
	}

	useLocalCallbackKeys = true;

	return 0;
};


int HierarchicalAnimationController::advanceTime( float _timeDelta )
{
	timeTotal += _timeDelta;

	list<Event*>::iterator it = l_pEvents.begin();
	list<Event*>::iterator it_end = l_pEvents.end();

	while ( it != it_end )
	{
		if ( (*it)->updateTrack( timeTotal ) )
		{
			it = l_pEvents.erase( it );
			maxEventsNumber--;
		}
	}

	USHORT paletteLength = pAnimationSet->getBonesNumber();

	for ( BYTE i = 0; i < paletteLength; ++i )
		D3DXMatrixIdentity( &( pPalette[i] ) );


	float tmpTime;
	D3DXMATRIX tmpMx;
	D3DXVECTOR3 tmpScaleVec3;
	D3DXVECTOR3 tmpTranslateVec3;
	D3DXQUATERNION tmpRotationQuat;
	BoneKeyframedAnimation* tmpKfAnim = NULL;


	for ( BYTE i = 0; i < maxTracksNumber; ++i )
	{
		if ( pTracks[i].enabled )
		{
			float lastTime = pTracks[i].time;
			pTracks[i].time = _timeDelta * pTracks[i].speed;

			float tt = 1.0f / pTracks[i].pAnimation->totalLength;
			UINT loopsLast = lastTime * tt;
			UINT loopsNow = pTracks[i].time * tt;

			bool loopFlag = loopsLast != loopsNow;

			if ( loopFlag )
			{
				pTracks[i].time = fmod( pTracks[i].time, pTracks[i].pAnimation->totalLength );
				pTracks[i].loopCount--;
			}

			CallbackKeyframeMap* ckm = useLocalCallbackKeys ? &pLocalCallbackKeys : &(pTracks[i].pAnimation->klm_callbackKeys);
			if ( useCallbackKeys && ckm->size() )
			{
				CallbackKeyframeMap::Iterator it_end = loopFlag ? ckm->end() : ckm->findAfter( pTracks[i].time );
				UserDefinedCallbackKey* uDefCallback;
				while ( pTracks[i].lastCallbackIt != it_end )
				{
					if ( !( uDefCallback = (*pTracks[i].lastCallbackIt).second ) )
						uDefCallback->trigger();
					pTracks[i].lastCallbackIt++;
				}
			}

			if ( loopFlag )
				pTracks[i].lastCallbackIt = ckm->begin();

			if ( !( pTracks[i].loopCount ) )
			{
				pTracks[i].time = 0.0f;
				pTracks[i].enabled = false;
			}
			else
			{
				if ( useCallbackKeys && ckm->size() )
				{
					CallbackKeyframeMap::Iterator it_end = ckm->findAfter( pTracks[i].time );
					UserDefinedCallbackKey* uDefCallback;
					while ( pTracks[i].lastCallbackIt != it_end )
					{
						if ( !( uDefCallback = (*pTracks[i].lastCallbackIt).second ) )
							uDefCallback->trigger();
						pTracks[i].lastCallbackIt++;
					}
				}


				for ( USHORT j = 0; j < pTracks[i].pAnimation->bonesAnimationsNumber; ++j )
				{
					tmpKfAnim = &( pTracks[i].pAnimation->pBonesAnimations[j] );

					D3DXMatrixTransformation(	&tmpMx, 
												tmpKfAnim->km_scaleKeyframes.interpolatedValue( tmpTime, &tmpScaleVec3 ), 
												tmpKfAnim->km_rotationKeyframes.interpolatedValue( tmpTime, &tmpRotationQuat ), 
												tmpKfAnim->km_translationKeyframes.interpolatedValue( tmpTime, &tmpTranslateVec3 ) );

					tmpMx *= pTracks[i].weight;

					D3DXMatrixMultiply( &( pPalette[ tmpKfAnim->boneIndex ] ), &( pPalette[ tmpKfAnim->boneIndex ] ), &tmpMx );
				}
			}
		}
	}


	D3DXMatrixIdentity( &tmpMx );
	UpdateFrames( pRootFrame, &tmpMx );

	for ( BYTE i = 0; i < paletteLength; ++i )
		D3DXMatrixMultiply( &( pPalette[i] ), ppBonesOffsets[i], &( pPalette[i] ) );

	return 0;
};


int HierarchicalAnimationController::setMaxNumTracks( BYTE _maxTracksNumber )
{
	if ( !_maxTracksNumber )
		return 4;

	if ( _maxTracksNumber == maxTracksNumber )
		return 0;

	Track* pTmpTracks = new Track [ _maxNumTracks ];
	if ( !pTmpTracks )
		return -10;

	BYTE mtn = min( maxTracksNumber, _maxTracksNumber );
	memcpy( pTmpTracks, pTracks, mtn * sizeof( Track ) );

	list<Event*>::iterator it = l_pEvents.begin();
	list<Event*>::iterator it_end = l_pEvents.end();

	while ( it != it_end )
	{
		BYTE trackIndex = (*it)->getTrackIndex();
		if ( trackIndex >= mtn )
		{
			(*it)->changeTrack( &( pTmpTracks[ trackIndex ] ) );
			it++;
		}
		else
		{
			SAFE_DELETE((*it));
			l_pEvents.erase( it );
		}
	}

	for ( BYTE i = maxTracksNumber; i < _maxTracksNumber; ++i )
		pTmpTracks[i].index = i;

	SAFE_DELETE_ARRAY( pTracks );
	pTracks = pTmpTracks;
	maxTracksNumber = _maxTracksNumber;

	return 0;
};


int HierarchicalAnimationController::setTrackAnimationByIndex( BYTE _trackIndex, USHORT _animationIndex )
{
	if ( _trackIndex >= maxTracksNumber )
		return 4;

	KeyframedAnimation* pKA = pAnimationSet->getAnimation( _trackIndex );
	if ( !pKA )
		return 8;

	pTracks[ _trackIndex ].pAnimation = pKA;
	pTracks[ _trackIndex ].animationIndex = _animationIndex;
	pTracks[ _trackIndex ].time = 0.0f;

	return 0;
};


int HierarchicalAnimationController::setTrackAnimationByName( BYTE _trackIndex, const char* _pAnimationName )
{
	if ( _trackIndex >= maxTracksNumber )
		return 4;

	USHORT trackIndex = 0;
	if ( pAnimationSet->getAnimationIndexByName( _pAnimationName, &trackIndex ) )
		return 8;

	pTracks[ _trackIndex ].pAnimation = pAnimationSet->getAnimation( trackIndex );
	pTracks[ _trackIndex ].animationIndex = trackIndex;
	pTracks[ _trackIndex ].time = 0.0f;

	return 0;
};


int HierarchicalAnimationController::getTrackAnimationIndex( BYTE _trackIndex, USHORT* _pAnimationIndex )
{
	if ( _trackIndex >= maxTracksNumber )
		return 4;

	*_pAnimationIndex = pTracks[ _trackIndex ].animationIndex;
	return 0;
};


int HierarchicalAnimationController::setTrackDesc( BYTE _trackIndex, TRACK_DESC* _pTrackDesc )
{
	if ( !_pTrackDesc )
		return 2;

	if ( _trackIndex >= maxTracksNumber )
		return 4;

	pTracks[ _trackIndex ].enabled = _pTrackDesc->enabled;
	pTracks[ _trackIndex ].loopCount = _pTrackDesc->loopCount;
	pTracks[ _trackIndex ].speed = _pTrackDesc->speed;
	pTracks[ _trackIndex ].weight = _pTrackDesc->weight;
	return 0;
};


int HierarchicalAnimationController::getTrackDesc( BYTE _trackIndex, TRACK_DESC* _pTrackDesc )
{
	if ( !_pTrackDesc )
		return 2;

	if ( _trackIndex >= maxTracksNumber )
		return 4;

	_pTrackDesc->enabled = pTracks[ _trackIndex ].enabled;
	_pTrackDesc->loopCount = pTracks[ _trackIndex ].loopCount;
	_pTrackDesc->speed = pTracks[ _trackIndex ].speed;
	_pTrackDesc->weight = pTracks[ _trackIndex ].weight;

	return 0;
};


int HierarchicalAnimationController::setTrackEnabled( BYTE _trackIndex, bool _enabled )
{
	if ( _trackIndex >= maxTracksNumber )
		return 4;

	pTracks[ _trackIndex ].enabled = _enabled;
	return 0;
};


int HierarchicalAnimationController::getTrackEnabled( BYTE _trackIndex, bool* _pEnabled )
{
	if ( _trackIndex >= maxTracksNumber )
		return 4;

	*_pEnabled = pTracks[ _trackIndex ].enabled;
	return 0;
};


int HierarchicalAnimationController::setTrackLoopCount( BYTE _trackIndex, UINT _loopCount )
{
	if ( _trackIndex >= maxTracksNumber )
		return 4;

	pTracks[ _trackIndex ].loopCount = _loopCount;
	return 0;
};


int HierarchicalAnimationController::getTrackLoopCount( BYTE _trackIndex, UINT* _loopCount )
{
	if ( _trackIndex >= maxTracksNumber )
		return 4;

	*_loopCount = pTracks[ _trackIndex ].loopCount;
	return 0;
};


int HierarchicalAnimationController::setTrackPosition( BYTE _trackIndex, float _position )
{
	if ( _trackIndex >= maxTracksNumber )
		return 4;

	pTracks[ _trackIndex ].time = _position;
	return 0;
};


int HierarchicalAnimationController::getTrackPosition( BYTE _trackIndex, float* _pPosition )
{
	if ( _trackIndex >= maxTracksNumber )
		return 4;

	*_pPosition = pTracks[ _trackIndex ].time;
	return 0;
};


int HierarchicalAnimationController::setTrackSpeed( BYTE _trackIndex, float _speed )
{
	if ( _trackIndex >= maxTracksNumber )
		return 4;

	pTracks[ _trackIndex ].speed = _speed;
	return 0;
};


int HierarchicalAnimationController::getTrackSpeed( BYTE _trackIndex, float* _pSpeed )
{
	if ( _trackIndex >= maxTracksNumber )
		return 4;

	*_pSpeed = pTracks[ _trackIndex ].speed;
	return 0;
};


int HierarchicalAnimationController::setTrackWeight( BYTE _trackIndex, float _weight )
{
	if ( _trackIndex >= maxTracksNumber )
		return 4;

	pTracks[ _trackIndex ].weight = _weight;
	return 0;
};


int HierarchicalAnimationController::getTrackWeight( BYTE _trackIndex, float* _pWeight )
{
	if ( _trackIndex >= maxTracksNumber )
		return 4;

	*_pWeight = pTracks[ _trackIndex ].weight;
	return 0;
};


int HierarchicalAnimationController::unkeyAllTrackEvents( BYTE _trackIndex )
{
	if ( _trackIndex >= maxTracksNumber )
		return 4;

	list<Event*>::iterator it = l_pEvents.begin();
	list<Event*>::iterator it_end = l_pEvents.end();

	while ( it != it_end )
	{
		if ( (*it)->getTrackIndex() == _trackIndex )
		{
			delete (*it);
			it = l_pEvents.erase( it );
		}
		else
			it++;
	}

	return 0;
};


int HierarchicalAnimationController::unkeyAllEvents()
{
	list<Event*>::iterator it = l_pEvents.begin();
	list<Event*>::iterator it_end = l_pEvents.end();

	while ( it != it_end )
		delete (*it);

	l_pEvents.clear();

	return 0;
};


int HierarchicalAnimationController::resetTime()
{
	unkeyAllEvents();
	timeTotal = 0.0f;
	paletteActual = false;
	for ( BYTE i = 0; i < maxTracksNumber; ++i )
	{
		pTracks[i].enabled = false;
		pTracks[i].loopCount = 0;
		pTracks[i].time = 0.0f;
		pTracks[i].speed = 1.0f;
		pTracks[i].weight = 1.0f;
	}

	return 0;
};


int HierarchicalAnimationController::pushEventTrackEnable( BYTE _trackIndex, float _timeBegin, bool _enable )
{
	if ( _trackIndex >= maxTracksNumber )
		return 4;

	if ( l_pEvents.size() >= maxEventsNumber )
		return 8;

	l_pEvents.push_back( new EventTrackEnable ( &(pTracks[_trackIndex]), _timeBegin, _enable ) );

	return 0;
};


int HierarchicalAnimationController::pushEventSpeedChange( BYTE _trackIndex, float* _timeBegin, float _timeEnd, float* _speedBegin, float _speedEnd )
{
	if ( _trackIndex >= maxTracksNumber )
		return 4;

	if ( l_pEvents.size() >= maxEventsNumber )
		return 8;

	l_pEvents.push_back( new EventSpeedChange(	&(pTracks[_trackIndex]), 
												_timeBegin ? *_timeBegin : timeTotal, 
												_timeBegin ? _timeEnd : timeTotal + _timeEnd, 
												_speedBegin ? *_speedBegin : pTracks[_trackIndex].speed, 
												_speedEnd ) );

	return 0;
};


int HierarchicalAnimationController::pushEventSpeedFadeIn( BYTE _trackIndex, float _timeLength )
{
	if ( _trackIndex >= maxTracksNumber )
		return 4;

	if ( l_pEvents.size() >= maxEventsNumber )
		return 8;

	l_pEvents.push_back( new EventSpeedFadeIn( &(pTracks[_trackIndex]), timeTotal, _timeLength ) );

	return 0;
};


int HierarchicalAnimationController::pushEventSpeedFadeOut( BYTE _trackIndex, float _timeLength )
{
	if ( _trackIndex >= maxTracksNumber )
		return 4;

	if ( l_pEvents.size() >= maxEventsNumber )
		return 8;

	l_pEvents.push_back( new EventSpeedFadeOut( &(pTracks[_trackIndex]), timeTotal, _timeLength ) );

	return 0;
};


int HierarchicalAnimationController::pushEventWeightChange( BYTE _trackIndex, float* _timeBegin, float _timeEnd, float* _weightBegin, float _weightEnd )
{
	if ( _trackIndex >= maxTracksNumber )
		return 4;

	if ( l_pEvents.size() >= maxEventsNumber )
		return 8;

	l_pEvents.push_back( new EventWeightChange(	&(pTracks[_trackIndex]), 
												_timeBegin ? *_timeBegin : timeTotal, 
												_timeBegin ? _timeEnd : timeTotal + _timeEnd, 
												_weightBegin ? *_weightBegin : pTracks[_trackIndex].weight, 
												_weightEnd ) );

	return 0;
};


int HierarchicalAnimationController::pushEventWeightFadeIn( BYTE _trackIndex, float _timeLength )
{
	if ( _trackIndex >= maxTracksNumber )
		return 4;

	if ( l_pEvents.size() >= maxEventsNumber )
		return 8;

	l_pEvents.push_back( new EventWeightFadeIn( &(pTracks[_trackIndex]), timeTotal, _timeLength ) );

	return 0;
};


int HierarchicalAnimationController::pushEventWeightFadeOut( BYTE _trackIndex, float _timeLength )
{
	if ( _trackIndex >= maxTracksNumber )
		return 4;

	if ( l_pEvents.size() >= maxEventsNumber )
		return 8;

	l_pEvents.push_back( new EventWeightFadeOut( &(pTracks[_trackIndex]), timeTotal, _timeLength ) );

	return 0;
};

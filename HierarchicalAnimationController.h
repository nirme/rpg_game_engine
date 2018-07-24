#pragma once

#ifndef _CORE_HIERARCHICAL_ANIMATION_CONTROLLER
#define _CORE_HIERARCHICAL_ANIMATION_CONTROLLER

#include "utility.h"

#include "Keyframe_Map.h"
#include "Keyframe_Lookup_Map.h"

#include "Hierarchical_Animation_Set.h"

#include "Subset_Data.h"

#include "Skin_Info.h"


class Model;
class AnimatedNode;

void UpdateFrames( BONE_FRAME* _pFrame, const D3DXMATRIX* _pRootTransform );


class HierarchicalAnimationController
{
public:

	struct TRACK_DESC
	{
		bool enabled;
		UINT loopCount;
		float speed;
		float weight;
	};
	////


private:

	struct Track
	{
		BYTE index;
		bool enabled;
		USHORT animationIndex;
		KeyframedAnimation* pAnimation;
		UINT loopCount;
		float time;
		float speed;
		float weight;
		CallbackKeyframeMap::Iterator lastCallbackIt;

		Track() : index(0), enabled(false), animationIndex(0), pAnimation(NULL), loopCount(0), time(0.0f), speed(0.0f), weight(0.0f), lastCallbackIndex(0) {};
	};
	////

	class Event
	{
	protected:
		Track* track;
		float time;

	public:
		Event( Track* _track, float _time ) : track(_track), time(_time) {};
		virtual bool updateTrack( float _totalTime ) = 0;
		inline Track* getTrack()	{	return track;	};
		inline BYTE getTrackIndex()	{	return track->index;	};
		inline changeTrack( Track* _newTrack )	{	track = _newTrack;	};
	};

	class EventTrackEnable : public Event
	{
	protected:
		bool enable;
	public:
		EventTrackEnable( Track* _track, float _time, bool _enable ) : Event( _track, _time ), enable(_enable) {};
		bool updateTrack( float _totalTime );
	};

	class EventSpeedChange : public Event
	{
	protected:
		float timeBegin;
		float oneDivByTimeLength;
		float speedBegin;
		float speedEnd;
		float speedDiff;
	public:
		EventSpeedChange( Track* _track, float _timeBegin, float _timeEnd, float _speedBegin, float _speedEnd );
		bool updateTrack( float _totalTime );
	};

	class EventWeightChange : public Event
	{
	protected:
		float timeBegin;
		float oneDivByTimeLength;
		float weightBegin;
		float weightEnd;
		float weightDiff;
	public:
		EventWeightChange( Track* _track, float _timeBegin, float _timeEnd, float _weightBegin, float _weightEnd );
		bool updateTrack( float _totalTime );
	};

	class EventSpeedFadeIn : public Event
	{
	protected:
		float timeBegin;
		float oneDivByTimeLength;
		float speedBegin;
		float speedDiff;
	public:
		EventSpeedFadeIn( Track* _track, float _timeNow, float _timeLength );
		bool updateTrack( float _totalTime );
	};

	class EventSpeedFadeOut : public Event
	{
	protected:
		float timeBegin;
		float oneDivByTimeLength;
		float speedBegin;
	public:
		EventSpeedFadeOut( Track* _track, float _timeNow, float _timeLength );
		bool updateTrack( float _totalTime );
	};

	class EventWeightFadeIn : public Event
	{
	protected:
		float timeBegin;
		float oneDivByTimeLength;
		float weightBegin;
		float weightDiff;
	public:
		EventWeightFadeIn( Track* _track, float _timeNow, float _timeLength );
		bool updateTrack( float _totalTime );
	};

	class EventWeightFadeOut : public Event
	{
	protected:
		float timeBegin;
		float oneDivByTimeLength;
		float weightBegin;
	public:
		EventWeightFadeOut( Track* _track, float _timeNow, float _timeLength );
		bool updateTrack( float _totalTime );
	};
	////

	struct LocalCallbackKeys
	{
		KeyframedAnimation* pKeyframedAnimation;
		CallbackKeyframeMap klm_callbackKeys;

		LocalCallbackKeys() : pKeyframedAnimation(NULL)	{};
	};
	////

protected:

	Model* pModel;

	HAnimationSet* pResourceAnimationSet;
	HierarchicalAnimationSet* pAnimationSet;

	bool useCallbackKeys;
	bool useLocalCallbackKeys;
	LocalCallbackKeys* pLocalCallbackKeys;

	BONE_FRAME* pRootFrame;
	BONE_FRAME* pFramesArray;

	D3DXMATRIX** ppBonesOffsets;
	D3DXMATRIX* pPalette;

	bool paletteActual;


	Track* pTracks;
	BYTE maxTracksNumber;

	list<Event*> l_pEvents;
	BYTE maxEventsNumber;


	float timeTotal;


public:

	HierarchicalAnimationController( Model* _pModel, HAnimationSet* _pResourceAnimationSet, ID3DXSkinInfo* _pSkinInfo, BYTE _maxTracksNumber, BYTE _maxEventsNumber );
	~HierarchicalAnimationController();

	inline const HierarchicalAnimationSet* getAnimationSet()	{	return pAnimationSet;	};

	inline bool getUseCallbackKeys()							{	return useCallbackKeys;	};
	inline void setUseCallbackKeys( bool _useCallbackKeys )		{	useCallbackKeys = _useCallbackKeys;	};

	int setMaxNumTracks( BYTE _maxTracksNumber );
	inline BYTE getMaxNumTracks()								{	return maxTracksNumber;	};

	inline void setMaxNumEvents( BYTE _maxEventsNumber )		{	maxEventsNumber = _maxEventsNumber;	};
	inline BYTE getMaxNumEvents()								{	return maxEventsNumber;	};

	inline USHORT getNumAnimations()							{	return pAnimationSet->getAnimationsNumber();	};

	inline float getTime()										{	return timeTotal;	};

	int setupLocalCallbackKeys( bool _useLocalCallbackKeys, AnimatedNode* _pAnimatedNode );

	int setTrackAnimationByIndex( BYTE _trackIndex, USHORT _animationIndex );
	int setTrackAnimationByName( BYTE _trackIndex, const char* _pAnimationName );
	int getTrackAnimationIndex( BYTE _trackIndex, USHORT* _pAnimationIndex );

	int setTrackDesc( BYTE _trackIndex, TRACK_DESC* _pTrackDesc );
	int getTrackDesc( BYTE _trackIndex, TRACK_DESC* _pTrackDesc );

	int setTrackEnabled( BYTE _trackIndex, bool _enabled );
	int getTrackEnabled( BYTE _trackIndex, bool* _pEnabled );

	int setTrackLoopCount( BYTE _trackIndex, UINT _loopCount );
	int getTrackLoopCount( BYTE _trackIndex, UINT* _loopCount );

	int setTrackPosition( BYTE _trackIndex, float _position );
	int getTrackPosition( BYTE _trackIndex, float* _pPosition );

	int setTrackSpeed( BYTE _trackIndex, float _speed );
	int getTrackSpeed( BYTE _trackIndex, float* _pSpeed );

	int setTrackWeight( BYTE _trackIndex, float _weight );
	int getTrackWeight( BYTE _trackIndex, float* _pWeight );

	int unkeyAllTrackEvents( BYTE _trackIndex );
	int unkeyAllEvents();

	int advanceTime( float _timeDelta );

	int resetTime();


	int pushEventTrackEnable( BYTE _trackIndex, float _timeBegin, bool _enable );

	int pushEventSpeedChange( BYTE _trackIndex, float* _timeBegin, float _timeEnd, float* _speedBegin, float _speedEnd );
	int pushEventSpeedFadeIn( BYTE _trackIndex, float _timeLength );
	int pushEventSpeedFadeOut( BYTE _trackIndex, float _timeLength );

	int pushEventWeightChange( BYTE _trackIndex, float* _timeBegin, float _timeEnd, float* _weightBegin, float _weightEnd );
	int pushEventWeightFadeIn( BYTE _trackIndex, float _timeLength );
	int pushEventWeightFadeOut( BYTE _trackIndex, float _timeLength );

};



#endif //_CORE_HIERARCHICAL_ANIMATION_CONTROLLER
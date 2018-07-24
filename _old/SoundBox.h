#pragma once

#ifndef _SOUND_BOX
#define _SOUND_BOX

//#include "Core_Sound.h"
#include "Core_Timer_v2.h"
/*


///////////////////////////////////////////////////////////////
////////                   SoundBox                    ////////
///////////////////////////////////////////////////////////////

class SoundBox
{
protected:
	Channel* channel;
	bool playbackLooped;

	typedef struct SoundTrack
	{
		UINT id;
		SoundData* sound;
		SoundTrack(UINT _id = 0, SoundData* _sound = NULL) : id(_id), sound(_sound) {};
	};
	typedef vector <SoundTrack> SoundTracksVector;
	SoundTracksVector v_sounds;

	int playingSoundVectorIndex;

	D3DXVECTOR3* channelPosition;
	bool positionVectorOwner;

//priv_methods:
	int getIndexFromID(UINT _id);
	int checkAndLoad(UINT _index);
	void freeTrack(UINT _index);
	int playTrack(UINT _index);

public:
	SoundBox(D3DXVECTOR3* _channelPosition = NULL, bool _isDialogue = false);
	~SoundBox();
	int addSound(UINT _id, bool _autoload = true);
	UINT getSoundsNumber();
	int loadAllSounds();
	void freeAllSounds();
	int isSoundLoadedByID(UINT _id);
	int isSoundLoadedByIndex(UINT _index);
	int freeSoundByID(UINT _id);
	int freeSoundByIndex(UINT _index);
	bool areSoundsLoaded();
	D3DXVECTOR3* setPositionVector(D3DXVECTOR3* _channelPosition = NULL);
	D3DXVECTOR3* getPositionVector();
	int playSoundByID(UINT _id);
	int playSoundByIndex(UINT _index);
	void stopPlayback();
	int playedSoundIndex();
	bool isPlaying();
	void loopPlayback(bool _loop);
	void pausePlayback(bool _pause);
	bool isPlaybackPaused();
	wchar_t* getDialougueText();
};




///////////////////////////////////////////////////////////////
////////                   MusicBox                    ////////
///////////////////////////////////////////////////////////////

typedef enum PLAYBACK_REPEAT_TYPE
{
	NO_REPEAT = 0x00,
	REPEAT_ONE = 0x01,
	REPEAT_ALL = 0x02
};


class MusicBox
{
protected:
	Channel* channel;
	bool channelPaused;

	typedef struct MusicTrack
	{
		UINT id;
		bool preload;
		SoundData* sound;
		MusicTrack(UINT _id = 0, bool _preload = true, SoundData* _sound = NULL) : id(_id), sound(_sound), preload(_preload) {};
	};

	typedef vector <MusicTrack> MusicTracksVector;
	MusicTracksVector v_sounds;
	short usedTrackIndex;
	short loadedTrackIndex;
	bool loadingActiveTrack;


	float musicTransitionTime;
	float musicTransitionExponential;
	Timer* transitionTimer;
	bool timerCounting;

	vector <short> vi_staticMusicQueue;
	short staticMusicActiveTrackQueueIndex;
	short staticMusicNextTrackQueueIndex;
	vector <short> vi_dynamicMusicQueue;
	short dynamicMusicActiveTrackQueueIndex;

	typedef enum PLAYBACK_STATE
	{
		NO_PLAYBACK = 0x00,
		STATIC = 0x01,
		DYNAMIC = 0x02,
		STATIC_TO_DYNAMIC = 0x03,
		DYNAMIC_TO_STATIC = 0x04
	};

	PLAYBACK_STATE playbackState;
	bool isActualTrackDynamic;

	PLAYBACK_REPEAT_TYPE staticPlaybackType;

	list<MusicTrack*> removalList;


//priv_methods:
	void timerStart();
	void timerStop();
	float timerGetProgress();
	UINT loadAndCheckTrack(MusicTrack* tr);
	USHORT getLocalIndexFromId(UINT _id);
	void checkAndReleaseTrack(MusicTrack* tr);

	short playTrack(USHORT index);
	void stopChannel();
	void setChannelVolume(float vol);


public:
	MusicBox(	float _musicTransitionTime = 4000.0f,
				float _musicTransitionExponential = 1.0f,
				PLAYBACK_REPEAT_TYPE _type = REPEAT_ALL
	);

	~MusicBox();

	UINT update();

	USHORT addMusic(UINT _id, bool _preload);
	USHORT getSoundsNumber();

	void setTrackPreloadByID(UINT _id, bool _preload);
	void setTrackPreloadByIndex(USHORT _index, bool _preload);
	void setPreloadAllSounds(bool _preload);

	void setMusicTransitionTime(float _musicTransitionTime = 4000.0f);
	float getMusicTransitionTime();
	void setMusicTransitionExponential(float _musicTransitionExponential = 1.0f);
	float getMusicTransitionExponential();


	bool isPlaybackFromDynamicStack();
	short getStaticQueueSize();
	short getDynamicStackSize();

	short addToDynamicStackByID(UINT _id, bool withTransition = true);
	short addToDynamicStackByIndex(USHORT _index, bool withTransition = true);
	short popFromDynamicStack(bool withTransition = true);
	short popAllFromDynamicStack(bool withTransition = true);

	void setStaticPlaybackType(PLAYBACK_REPEAT_TYPE _type);
	PLAYBACK_REPEAT_TYPE getStaticPlaybackType();

	short addToStaticQueueByID(UINT _id);
	short addToStaticQueueByIndex(USHORT _index);
	short removeFromStaticQueueByQueueIndex(USHORT _index);
	short removeFromStaticQueueByTrackIndex(USHORT _index);
	short removeFromStaticQueueByID(UINT _id);
	short moveToStaticQueueByQueueIndex(USHORT _index, bool withTransition = true, bool freeDynamicQueue = false);


	bool isMusicPlaying();
	void stopPlayback(bool withTransition = true);
	bool isPlaybackPaused();
	void pausePlayback(bool _pause);
};
*/

#endif //_SOUND_BOX

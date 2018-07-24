#pragma once

#ifndef _MUSIC_MIXER
#define _MUSIC_MIXER


#include "utility.h"
#include "Core_Logger.h"
#include "Core_Variable_Library.h"
#include "Core_Defines.h"

#include "Core_Timer_v2.h"
#include "Core_Channel.h"
#include "Core_SoundSystem.h"


#define	ONLOAD_SLEEP_TIME		20


///////////////////////////////////////////////////////////////
////////                  MusicMixer                   ////////
///////////////////////////////////////////////////////////////

typedef		unsigned short		INSTRUCTION_INDEX;
#define		INDEX_DIRECT		1
#define		INDEX_NEXT_PREV		2


typedef		unsigned short		REPEAT_TYPE;
#define		STOP_AFTER			1
#define		REPEAT_ONE			2
#define		REPEAT_ALL			3



FMOD_RESULT F_CALLBACK MusicMixerCallback(FMOD_CHANNEL* _channel, FMOD_CHANNEL_CALLBACKTYPE _type, void* _commanddata1, void* _commanddata2);



class MusicMixer
{
public:

	enum MUSIC_BOX_STATE
	{
		MUSIC_BOX_STOPPED	= 0x00,
		MUSIC_BOX_PAUSED	= 0x01,
		MUSIC_BOX_PLAYING	= 0x02
	};

	struct Soundtrack
	{
		UINT soundId;
		bool preloadSound;
		bool stayLoadedAfterUse;
		Sound* sound;
		bool release;
		Soundtrack(UINT _soundId = 0, bool _preloadSound = false, bool _stayLoadedAfterUse = false, Sound* _sound = NULL) : 
			soundId(_soundId), preloadSound(_preloadSound), stayLoadedAfterUse(_stayLoadedAfterUse), sound(_sound), release(false)
			{};
	};

	struct Playlist
	{
		string name;
		vector<USHORT> indexList;
		bool shuffle;
		REPEAT_TYPE repeatType;
		int nextShuffledIndex;

		Playlist(const string &_name, bool _shuffle, REPEAT_TYPE _repeatType) : shuffle(_shuffle), repeatType(_repeatType)
		{
			name = _name;
			nextShuffledIndex = -1;
		};
	};

	struct MusicMixerIntruction
	{
		bool isset;
		int index;
		int playlist;
		INSTRUCTION_INDEX indexType;
		bool waitForCallback;
		MusicMixerIntruction() : isset(false), index(-1), indexType(0), waitForCallback(false)	{};
	};


private:

	bool initialized;
	bool setToShutdown;


	mutable boost::mutex channelMutex;

	Channel* musicChannel;

	bool setOnCallback;
	int onCallbackIndex;
	int onCallbackPlaylist;

	MUSIC_BOX_STATE state;

	vector<Soundtrack> soundtracks;
	int playingIndex;
	int playingPlaylistIndex;
	int playingPlaylist;

	vector<Playlist> playlists;


	boost::thread thread;
	boost::condition_variable instructionConditionVar;
	mutable boost::mutex instructionMutex;
	MusicMixerIntruction mixerIntruction;


//functions:

	int playSound(USHORT _index, USHORT _playlist);

	int removeSound(UINT _index);
	int removeSound(UINT _index, UINT _playlist);

	int workerFunction();

public:

	int getPlayingIndex()
	{
		return playingIndex;
	};

	MusicMixer();

	int initialize(Channel* _channel, bool _shuffleLowPlaylist = true, USHORT _playlistTransitionTime = 0, USHORT _userSetIndexTransitionTime = 0);

	int addSoundtrack(UINT _soundId, bool _preloadSound = false, bool _stayLoadedAfterUse = false);

	int addPlaylist(const string &_name, bool _shuffle = true, REPEAT_TYPE _repeatType = REPEAT_ALL);
	int addToPlaylistByIndex(USHORT _playlistIndex, int _index);
	int addToPlaylistByIndex(string _playlistName, int _index);

	int addPlaylist(const string &_name, USHORT* _soundtracksIndexArray, USHORT _soundtracksIndexCount, bool _shuffle = true, REPEAT_TYPE _repeatType = REPEAT_ALL);

	int playNextTrack(bool _waitTillSyncPoint = false);
	int playPrevTrack(bool _waitTillSyncPoint = false);

	int getPlaylistIndexByName(const string &_name);

	int playFromPlaylist(UINT _playlistIndex, bool _waitTillSyncPoint = false);
	int playIndexFromPlaylist(int _playlistIndex, int _index, bool _waitTillSyncPoint = false);

	int pauseMusic(bool _pause);

	int stopMusic();

	int shutdown();



	inline bool isSetOnCallback()
	{
		return setOnCallback;
	};
};


#endif //_MUSIC_MIXER

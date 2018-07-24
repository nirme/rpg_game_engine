#include "MusicMixer.h"



FMOD_RESULT F_CALLBACK MusicMixerCallback(FMOD_CHANNEL* _channel, FMOD_CHANNEL_CALLBACKTYPE _type, void* _commanddata1, void* _commanddata2)
{
	MusicMixer* musicBox = NULL;
	reinterpret_cast <FMOD::Channel*> (_channel)->getUserData(reinterpret_cast <void**> (&musicBox));


	if (_type == FMOD_CHANNEL_CALLBACKTYPE_END)
	{
		int i = musicBox->getPlayingIndex();
	}
	else if (_type == FMOD_CHANNEL_CALLBACKTYPE_SYNCPOINT)
	{
		if (!musicBox->isSetOnCallback())
			return FMOD_OK;
	}


//channelCallbackSet

	return FMOD_OK;
};



int MusicMixer::playSound(USHORT _index, USHORT _playlist)
{
	if (_playlist >= playlists.size())
		return 1;

	if (_index >= playlists[_playlist].indexList.size())
		return 2;

	if (!soundtracks[playlists[_playlist].indexList[_index]].sound)
		return 3;

	if (!soundtracks[playlists[_playlist].indexList[_index]].sound->isLoaded())
		return 4;

	{
		boost::mutex::scoped_lock lock(channelMutex);

		if (int ret = musicChannel->playSound(soundtracks[playlists[_playlist].indexList[_index]].sound))
			return ret;

		musicChannel->setUserData(this);

		musicChannel->setCallback(MusicMixerCallback);

		playingPlaylistIndex = _index;
		playingPlaylist = _playlist;
		playingIndex = playlists[_playlist].indexList[_index];
	}

	return 0;
};


int MusicMixer::removeSound(UINT _index)
{
	if (state != MUSIC_BOX_STOPPED)
		return 1;

	if (soundtracks[_index].sound && !soundtracks[_index].stayLoadedAfterUse)
		SAFE_RELEASE(soundtracks[_index].sound);

	return 0;
};


int MusicMixer::removeSound(UINT _index, UINT _playlist)
{
	if (_playlist >= playlists.size())
		return 11;

	if (_index >= playlists[_playlist].indexList.size())
		return 12;

	return removeSound(playlists[_playlist].indexList[_index]);
};


int MusicMixer::workerFunction()
{
	bool waitForInstruction = true;
	bool loading = false;
	MusicMixerIntruction loadingRequest = MusicMixerIntruction();

	MusicMixerIntruction instruction;

	while (true)
	{
		if (setToShutdown)
			break;

		instruction = MusicMixerIntruction();
		{//mutex scope
			boost::mutex::scoped_lock lock(instructionMutex);

			if (mixerIntruction.isset)
			{
				instruction = mixerIntruction;
				mixerIntruction = MusicMixerIntruction();
			}
			else if (waitForInstruction)
			{
				try
				{
					while (!mixerIntruction.isset)
					{
						instructionConditionVar.wait(lock);
					}

					instruction = mixerIntruction;
					mixerIntruction = MusicMixerIntruction();
				}
				catch (boost::thread_interrupted e)
				{
					break;
				}
			}
		}


		if (instruction.isset)
		{
			int plIndi = -1;
			int pllst = -1;

			if (instruction.indexType == INDEX_DIRECT)
			{
				plIndi = instruction.index;
				pllst = instruction.playlist;
			}
			else if (instruction.indexType == INDEX_NEXT_PREV)
			{
				pllst = playingPlaylist;
				int t = (playingPlaylistIndex + instruction.index) % playlists[pllst].indexList.size();
				plIndi = (t >= 0) ? t : (playlists[pllst].indexList.size() - t);
			}
			int i = playlists[pllst].indexList[plIndi];

			if (loading)
			{
				int loadedIndex = -1;
				int newIndex = i;

				if (loadingRequest.indexType == INDEX_DIRECT)
					loadedIndex = playlists[loadingRequest.playlist].indexList[loadingRequest.index];
				else //loadingRequest.indexType == INDEX_NEXT_PREV
				{
					int t = (playingPlaylistIndex + loadingRequest.index) % playlists[pllst].indexList.size();
					loadedIndex = playlists[playingPlaylist].indexList[(t >= 0) ? t : (playlists[pllst].indexList.size() - t)];
				}

				if (loadedIndex != newIndex)
					removeSound(loadedIndex);

				waitForInstruction = true;
				loading = false;
				loadingRequest = MusicMixerIntruction();
			}

			if (!soundtracks[i].sound)
				soundtracks[i].sound = Sound::getResource(soundtracks[i].soundId);

			if (soundtracks[i].sound->isLoaded())
			{
				if (state == MUSIC_BOX_STOPPED)
				{
					boost::mutex::scoped_lock lock(channelMutex);

					playSound(plIndi, pllst);
				}
				else if (state == MUSIC_BOX_PLAYING)
				{
					boost::mutex::scoped_lock lock(channelMutex);

					if (instruction.waitForCallback)
					{
						bool callback = setOnCallback;
						int callbackIndex = onCallbackIndex;
						int callbackPlaylist = onCallbackPlaylist;

						setOnCallback = true;
						onCallbackIndex = plIndi;
						onCallbackPlaylist = pllst;

						if (callback && (i != playlists[callbackPlaylist].indexList[callbackIndex]))
							removeSound(callbackIndex, callbackPlaylist);
					}
					else //!instruction.waitForCallback
					{
						int pi = playingIndex, ppi = playingPlaylistIndex, pp = playingPlaylist;
						playSound(plIndi, pllst);

						if (playlists[pllst].indexList[plIndi] != pi)
							removeSound(pi);
					}
				}

				waitForInstruction = true;
				loading = false;
				instruction = MusicMixerIntruction();
			}
			else //!soundtracks[i].sound->isLoaded()
			{
				waitForInstruction = false;
				loading = true;
				loadingRequest = instruction;
				Sleep(ONLOAD_SLEEP_TIME);
				continue;
			}
		}
		else if (loading)
		{
			int plIndi = -1;
			int pllst = -1;

			if (loadingRequest.indexType == INDEX_DIRECT)
			{
				plIndi = loadingRequest.index;
				pllst = loadingRequest.playlist;
			}
			else if (loadingRequest.indexType == INDEX_NEXT_PREV)
			{
				pllst = playingPlaylist;
				int t = (playingPlaylistIndex + loadingRequest.index) % playlists[pllst].indexList.size();
				plIndi = (t >= 0) ? t : (playlists[pllst].indexList.size() - t);
			}

			int i = playlists[pllst].indexList[plIndi];

			if (!soundtracks[i].sound)
				soundtracks[i].sound = Sound::getResource(soundtracks[i].soundId);

			if (soundtracks[i].sound->isLoaded())
			{
				if (state == MUSIC_BOX_STOPPED)
				{
					boost::mutex::scoped_lock lock(channelMutex);

					playSound(plIndi, pllst);
				}
				else if (state == MUSIC_BOX_PLAYING)
				{
					boost::mutex::scoped_lock lock(channelMutex);

					if (loadingRequest.waitForCallback)
					{
						bool callback = setOnCallback;
						int callbackIndex = onCallbackIndex;
						int callbackPlaylist = onCallbackPlaylist;

						setOnCallback = true;
						onCallbackIndex = plIndi;
						onCallbackPlaylist = pllst;

						if (callback && (i != playlists[callbackPlaylist].indexList[callbackIndex]))
							removeSound(callbackIndex, callbackPlaylist);
					}
					else //!loadingRequest.waitForCallback
					{
						int pi = playingIndex, ppi = playingPlaylistIndex, pp = playingPlaylist;
						playSound(plIndi, pllst);

						if (playlists[pllst].indexList[plIndi] != pi)
							removeSound(pi);
					}
				}

				waitForInstruction = true;
				loading = false;
				loadingRequest = MusicMixerIntruction();
			}
			else //!soundtracks[i].sound->isLoaded()
			{
				Sleep(ONLOAD_SLEEP_TIME);
				continue;
			}
		}
	}//while(true)

	return 0;
};


MusicMixer::MusicMixer()
{
	initialized = false;
	setToShutdown = false;

	musicChannel = 0;

	setOnCallback = false;
	onCallbackIndex = -1;
	onCallbackPlaylist = -1;

	state = MUSIC_BOX_STOPPED;

	playingIndex = -1;
	playingPlaylistIndex = -1;
	playingPlaylist = -1;

	mixerIntruction = MusicMixerIntruction();
};


int MusicMixer::initialize(Channel* _channel, bool _shuffleLowPlaylist, USHORT _playlistTransitionTime, USHORT _userSetIndexTransitionTime)
{
	if (!_channel)
		return 1;

	setToShutdown = false;

	musicChannel = _channel;

	setOnCallback = false;
	onCallbackIndex = -1;
	onCallbackPlaylist = -1;

	state = MUSIC_BOX_STOPPED;

	playingIndex =-1;
	playingPlaylistIndex =-1;
	playingPlaylist =-1;

	mixerIntruction = MusicMixerIntruction();

	int ret = 0;
	for (UINT i=0;i<soundtracks.size();++i)
	{
		if (!soundtracks[i].sound)
		{
			soundtracks[i].sound = Sound::getResource(soundtracks[i].soundId);

			if (!soundtracks[i].sound)
			{
				soundtracks.erase(soundtracks.begin()+i);
				ret++;
				i--;
				continue;
			}

			if (!soundtracks[i].preloadSound)
			{
				soundtracks[i].sound->release();
			}
		}
	}


	thread = boost::thread(boost::bind(&MusicMixer::workerFunction, this));

	initialized = true;
	if (ret)
		return 10000 + ret;
	return 0;
};


int MusicMixer::addSoundtrack(UINT _soundId, bool _preloadSound, bool _stayLoadedAfterUse)
{
	Sound* sound = NULL;
	sound = Sound::getResource(_soundId);

	if (!sound)
	{
		showError(false, "Soundtrack by id %d don't exist.\n", _soundId);
		return -1;
	}

	_soundId = sound->getId();

	if (!initialized || !_preloadSound)
	{
		SAFE_RELEASE(sound);
	}

	soundtracks.push_back(Soundtrack(_soundId, _preloadSound, _stayLoadedAfterUse, sound));

	return soundtracks.size() - 1;
};


int MusicMixer::addPlaylist(const string &_name, bool _shuffle, REPEAT_TYPE _repeatType)
{
	playlists.push_back(Playlist(_name, _shuffle, _repeatType));
	return playlists.size() - 1;
};


int MusicMixer::addToPlaylistByIndex(USHORT _playlistIndex, int _index)
{
	if (_playlistIndex >=playlists.size())
		return 1;

	if (_index >= soundtracks.size())
		return 2;

	playlists[_playlistIndex].indexList.push_back(_index);

	return 0;
};


int MusicMixer::addToPlaylistByIndex(string _playlistName, int _index)
{
	for (UINT i=0;i<playlists.size();++i)
	{
		if (playlists[i].name == _playlistName)
			return addToPlaylistByIndex(i, _index);
	}
	return 10;
};


int MusicMixer::addPlaylist(const string &_name, USHORT* _soundtracksIndexArray, USHORT _soundtracksIndexCount, bool _shuffle, REPEAT_TYPE _repeatType)
{
	int pli = addPlaylist(_name, _shuffle, _repeatType);

	int res = 0;
	for (UINT i=0;i<_soundtracksIndexCount;++i)
	{
		if (addToPlaylistByIndex(pli, _soundtracksIndexArray[i]))
			res++;
	}

	return res;
};


int MusicMixer::playNextTrack(bool _waitTillSyncPoint)
{
	boost::mutex::scoped_lock lock(instructionMutex);

	if (mixerIntruction.isset && mixerIntruction.indexType == INDEX_NEXT_PREV)
	{
		mixerIntruction.index++;
		if (!mixerIntruction.index)
		{
			mixerIntruction = MusicMixerIntruction();
			return 0;
		}
		mixerIntruction.playlist = -1;
		mixerIntruction.waitForCallback = _waitTillSyncPoint;
	}
	else
	{
		mixerIntruction.isset = true;
		mixerIntruction.index = 1;
		mixerIntruction.playlist = -1;
		mixerIntruction.indexType = INDEX_NEXT_PREV;
		mixerIntruction.waitForCallback = _waitTillSyncPoint;
	}

	instructionConditionVar.notify_one();
	return 0;
};


int MusicMixer::playPrevTrack(bool _waitTillSyncPoint)
{
	boost::mutex::scoped_lock lock(instructionMutex);

	if (mixerIntruction.isset && mixerIntruction.indexType == INDEX_NEXT_PREV)
	{
		mixerIntruction.index--;
		if (!mixerIntruction.index)
		{
			mixerIntruction = MusicMixerIntruction();
			return 0;
		}
		mixerIntruction.playlist = -1;
		mixerIntruction.waitForCallback = _waitTillSyncPoint;
	}
	else
	{
		mixerIntruction.isset = true;
		mixerIntruction.index = -1;
		mixerIntruction.playlist = -1;
		mixerIntruction.indexType = INDEX_NEXT_PREV;
		mixerIntruction.waitForCallback = _waitTillSyncPoint;
	}

	instructionConditionVar.notify_one();
	return 0;
};


int MusicMixer::getPlaylistIndexByName(const string &_name)
{
	for (UINT i=0;i<playlists.size();++i)
	{
		if (playlists[i].name == _name)
			return i;
	}

	return -1;
};


int MusicMixer::playFromPlaylist(UINT _playlistIndex, bool _waitTillSyncPoint)
{
	if (_playlistIndex >= playlists.size())
		return 1;

	boost::mutex::scoped_lock lock(instructionMutex);

	mixerIntruction.isset = true;
	mixerIntruction.index = rand() % playlists.size();
	mixerIntruction.playlist = _playlistIndex;
	mixerIntruction.indexType = INDEX_DIRECT;
	mixerIntruction.waitForCallback = _waitTillSyncPoint;

	instructionConditionVar.notify_one();
	return 0;
};


int MusicMixer::playIndexFromPlaylist(int _playlistIndex, int _index, bool _waitTillSyncPoint)
{
	if (_playlistIndex >= playlists.size())
		return 1;

	if (_index >= playlists[_playlistIndex].indexList.size())
		return 2;

	boost::mutex::scoped_lock lock(instructionMutex);

	mixerIntruction.isset = true;
	mixerIntruction.index = _index;
	mixerIntruction.playlist = _playlistIndex;
	mixerIntruction.indexType = INDEX_DIRECT;
	mixerIntruction.waitForCallback = _waitTillSyncPoint;

	instructionConditionVar.notify_one();
	return 0;
};


int MusicMixer::pauseMusic(bool _pause)
{
	if (!initialized)
		return 1;

	if (_pause)
	{
		if (state == MUSIC_BOX_PAUSED)
			return 0;

		if (state == MUSIC_BOX_PLAYING)
		{
			boost::mutex::scoped_lock lock(channelMutex);
			musicChannel->setPaused(true);
			state = MUSIC_BOX_PAUSED;
			return 0;
		}
	}
	else
	{
		if (state == MUSIC_BOX_PLAYING)
			return 0;

		if (state == MUSIC_BOX_PAUSED)
		{
			boost::mutex::scoped_lock lock(channelMutex);
			musicChannel->setPaused(false);
			state = MUSIC_BOX_PLAYING;
			return 0;
		}
	}

	return 10;
};


int MusicMixer::stopMusic()
{
	if (!initialized)
		return 1;

	if (state == MUSIC_BOX_PLAYING || state == MUSIC_BOX_PAUSED)
	{
		boost::mutex::scoped_lock lock(channelMutex);
		musicChannel->stop();
		state = MUSIC_BOX_STOPPED;
		return 0;
	}

	return 10;
};


int MusicMixer::shutdown()
{
	initialized = false;
	setToShutdown = true;
	thread.interrupt();
//		instructionConditionVar.notify_one();
	thread.join();

	stopMusic();

	channelMutex;
	SAFE_RELEASE(musicChannel);

	setOnCallback = false;
	onCallbackIndex = -1;
	onCallbackPlaylist = -1;

	state = MUSIC_BOX_STOPPED;

	soundtracks.clear();

	playingIndex = -1;
	playingPlaylistIndex = -1;
	playingPlaylist = -1;

	playlists.clear();

	mixerIntruction = MusicMixerIntruction();

	return 0;
};

#include "SoundBox.h"



///////////////////////////////////////////////////////////////
////////                   SoundBox                    ////////
///////////////////////////////////////////////////////////////
/*
int SoundBox::getIndexFromID(UINT _id)
{
	for (UINT i=0;i<v_sounds.size();i++)
	{
		if (v_sounds[i].id == _id)
			return i;
	}

	return -1;
};

int SoundBox::checkAndLoad(UINT _index)
{
	if (v_sounds[_index].sound == NULL)
		v_sounds[_index].sound = SoundData::getResource(v_sounds[_index].id);

	if (!v_sounds[_index].sound->isLoaded())
		return 1;

	return 0;
};

void SoundBox::freeTrack(UINT _index)
{
	if (v_sounds[_index].sound != NULL)
	{
		v_sounds[_index].sound->release();
		v_sounds[_index].sound = NULL;
	}
};

int SoundBox::playTrack(UINT _index)
{
	if (_index >= v_sounds.size())
		return -1;

	channel->stop();

	if (v_sounds[_index].sound == NULL)
		v_sounds[_index].sound = SoundData::getResource(v_sounds[_index].id);

	if (!v_sounds[_index].sound->isLoaded())
		return 1;

	playingSoundVectorIndex = _index;
	channel->playSound(v_sounds[_index].sound);
	return 0;
};


SoundBox::SoundBox(D3DXVECTOR3* _channelPosition, bool _isDialogue)
{
	if (_channelPosition != NULL)
	{
		channelPosition = _channelPosition;
		positionVectorOwner = false;
	}
	else
	{
		channelPosition = new D3DXVECTOR3;
		positionVectorOwner = true;
	}

	Sound::getChanel(&channel, _isDialogue ? DIALOGUE : BG_SOUNDS, channelPosition);

	playbackLooped = false;
	playingSoundVectorIndex = -1;
};

SoundBox::~SoundBox()
{
	channel->release();
	v_sounds.clear();

	if (positionVectorOwner)
	{
		delete channelPosition;
	}
};

int SoundBox::addSound(UINT _id, bool _autoload)
{
	int i = getIndexFromID(_id);
	if (i == -1)
	{
		v_sounds.push_back(SoundTrack(_id, NULL));
		i = v_sounds.size() - 1;
	}

	if (v_sounds[i].sound == NULL && _autoload)
	{
		v_sounds[i].sound = SoundData::getResource(v_sounds[i].id);
	}

	return i;
};

UINT SoundBox::getSoundsNumber()
{
	return v_sounds.size();
};

int SoundBox::loadAllSounds()
{
	int lo = 0;
	for (UINT i=0;i<v_sounds.size();i++)
	{
		lo += checkAndLoad(i);
	}

	return lo;
};

void SoundBox::freeAllSounds()
{
	for (UINT i=0;i<v_sounds.size();i++)
	{
		freeTrack(i);
	}
};

int SoundBox::isSoundLoadedByID(UINT _id)
{
	int i = getIndexFromID(_id);

	if (i == -1)
		return -1;

	if (v_sounds[i].sound == NULL)
		return 0;

	if (!v_sounds[i].sound->isLoaded())
		return 1;

	return 2;
};

int SoundBox::isSoundLoadedByIndex(UINT _index)
{
	if (_index >= v_sounds.size())
		return -1;

	if (v_sounds[_index].sound == NULL)
		return 0;

	if (!v_sounds[_index].sound->isLoaded())
		return 1;

	return 2;
};

int SoundBox::freeSoundByID(UINT _id)
{
	int i = getIndexFromID(_id);

	if (i == -1)
		return -1;

	freeTrack(i);

	return 0;
};


int SoundBox::freeSoundByIndex(UINT _index)
{
	if (_index >= v_sounds.size())
		return -1;

	freeTrack(_index);

	return 0;
};

bool SoundBox::areSoundsLoaded()
{
	for (UINT i=0;i<v_sounds.size();i++)
	{
		if (v_sounds[i].sound == NULL)
			return false;
		if (!v_sounds[i].sound->isLoaded())
			return false;
	}

	return true;
};

D3DXVECTOR3* SoundBox::setPositionVector(D3DXVECTOR3* _channelPosition)
{
	if (positionVectorOwner && _channelPosition != NULL)
	{
		delete channelPosition;
		channelPosition = _channelPosition;
		positionVectorOwner = false;
	}
	else if (!positionVectorOwner && _channelPosition == NULL)
	{
		channelPosition = new D3DXVECTOR3;
		positionVectorOwner = true;
	}
	else if (!positionVectorOwner && _channelPosition != NULL)
	{
		channelPosition = _channelPosition;
		positionVectorOwner = false;
	}

	return channelPosition;
};

D3DXVECTOR3* SoundBox::getPositionVector()
{
	return channelPosition;
};

int SoundBox::playSoundByID(UINT _id)
{
	int i = getIndexFromID(_id);

	if (i == -1)
		return -1;

	return playTrack(i);
};

int SoundBox::playSoundByIndex(UINT _index)
{
	if (_index >= v_sounds.size())
		return -1;

	return playTrack(_index);
};

void SoundBox::stopPlayback()
{
	playingSoundVectorIndex = -1;
	channel->stop();
};

int SoundBox::playedSoundIndex()
{
	if (!channel->isPlaying())
		playingSoundVectorIndex = -1;
	return playingSoundVectorIndex;
};

bool SoundBox::isPlaying()
{
	bool r = channel->isPlaying();
	if (!r)
		playingSoundVectorIndex = -1;
	return r;
};

void SoundBox::loopPlayback(bool _loop)
{
	channel->setLoop(_loop ? -1 : 0);
};

void SoundBox::pausePlayback(bool _pause)
{
	channel->setPaused(_pause);
};

bool SoundBox::isPlaybackPaused()
{
	return channel->isPaused();
};

wchar_t* SoundBox::getDialougueText()
{
	if (!channel->isPlaying())
		playingSoundVectorIndex = -1;

	if (playingSoundVectorIndex < 0)
		return NULL;

	return v_sounds[playingSoundVectorIndex].sound->getText();
};



///////////////////////////////////////////////////////////////
////////                   MusicBox                    ////////
///////////////////////////////////////////////////////////////

void MusicBox::timerStart()
{
	transitionTimer->setTimeProceed(true);
	transitionTimer->setTimerRaceFromNow(musicTransitionTime);
	timerCounting = true;
};

void MusicBox::timerStop()
{
	transitionTimer->setTimeProceed(false);
	timerCounting = false;
};

float MusicBox::timerGetProgress()
{
	if (timerCounting)
		return 1.0f - transitionTimer->getTimerPowProgress(1 / musicTransitionExponential);
	return 0.0f;
};

UINT MusicBox::loadAndCheckTrack(MusicTrack* tr)
{
	if (tr == NULL)
		return -1;

	if (tr->sound == NULL)
	{
		tr->sound = SoundData::getResource(tr->id);
		return 2;
	}

	if (!tr->sound->isLoaded())
	{
		return 1;
	}

	return 0;
};

USHORT MusicBox::getLocalIndexFromId(UINT _id)
{
	for (USHORT i=0;i<v_sounds.size();i++)
		if (v_sounds[i].id == _id)
			return i;
	return -1;
};

void MusicBox::checkAndReleaseTrack(MusicTrack* tr)
{
	short i = getLocalIndexFromId(tr->id);
	if (usedTrackIndex != i && loadedTrackIndex != i && !tr->preload && tr->sound != NULL)
	{
		tr->sound->release();
		tr->sound = NULL;
	}
};

short MusicBox::playTrack(USHORT index)
{
	if (channel->isPlaying())
		return -2;

	if (index < 0 || index >= v_sounds.size())
		return -1;

	if (v_sounds[index].sound == NULL)
	{
		v_sounds[index].sound = SoundData::getResource(v_sounds[index].id);
	}

	if (!v_sounds[index].sound->isLoaded())
	{
		loadedTrackIndex = index;
		loadingActiveTrack = true;
		return 200;
	}

	loadedTrackIndex = -1;
	loadingActiveTrack = false;

	usedTrackIndex = index;

	channel->changeVolume(Sound::getVolume(MUSIC));
	channel->playSound(v_sounds[index].sound);
	return 0;
};

void MusicBox::stopChannel()
{
	if (!channel->isPlaying())
	{
		if (loadingActiveTrack)
		{
			loadedTrackIndex = -1;
			loadingActiveTrack = false;
		}
		else
		{
			usedTrackIndex = -1;
		}
	}

	if (channel->isPlaying())
	{
		channel->stop();
		usedTrackIndex = -1;
	}
};

void MusicBox::setChannelVolume(float vol)
{
	channel->changeVolume(Sound::getVolume(MUSIC) * vol);
};



//public methods

MusicBox::MusicBox(	float _musicTransitionTime, float _musicTransitionExponential, PLAYBACK_REPEAT_TYPE _type)
{
	Sound::getChanel(&channel, MUSIC);
	channelPaused = false;

	usedTrackIndex = -1;
	loadedTrackIndex = -1;
	loadingActiveTrack = false;

	musicTransitionTime = _musicTransitionTime;
	musicTransitionExponential = _musicTransitionExponential;
	transitionTimer = Timer::createTimer();
	transitionTimer->setTimeProceed(false);
	timerCounting = false;

	staticMusicActiveTrackQueueIndex = -1;
	staticMusicNextTrackQueueIndex = -1;
	dynamicMusicActiveTrackQueueIndex = -1;

	playbackState = NO_PLAYBACK;
	isActualTrackDynamic = false;

	staticPlaybackType = _type;
};

MusicBox::~MusicBox()
{
	channel->stop();
	channel->release();

	for (UINT i=0;i<v_sounds.size();i++)
	{
		if (v_sounds[i].sound != NULL)
		{
			v_sounds[i].sound->release();
			v_sounds[i].sound = NULL;
		}
	}
	v_sounds.clear();

	transitionTimer->dropTimer();

	vi_staticMusicQueue.clear();
	vi_dynamicMusicQueue.clear();

	removalList.clear();
};

UINT MusicBox::update()
{
	if (channelPaused)
		return 1;

	if (!removalList.empty() && !channel->isPlaying())
	{
		if (vi_dynamicMusicQueue.empty())
		{
			list<MusicTrack*>::iterator it;
			for (it=removalList.begin();it!=removalList.end();it++)
			{
				checkAndReleaseTrack((*it));
			}
			removalList.clear();
		}
		else //!vi_dynamicMusicQueue.empty()
		{
			UINT id = v_sounds[vi_dynamicMusicQueue[vi_dynamicMusicQueue.size() - 1]].id;
			list<MusicTrack*>::iterator it;
			for (it=removalList.begin();it!=removalList.end();it++)
			{
				if ((*it)->id != id)
					checkAndReleaseTrack((*it));
			}
			removalList.clear();
		}
	}


	bool switchFlag = false;
	do
	{
		switchFlag = false;

		switch(playbackState)
		{
			case (NO_PLAYBACK):
			{
				if (channel->isPlaying())
				{
					if (!vi_dynamicMusicQueue.empty())
					{
						playbackState = isActualTrackDynamic ? DYNAMIC : STATIC_TO_DYNAMIC;
						switchFlag = true;
						break;
					}
					else if (staticMusicNextTrackQueueIndex != -1)
					{
						playbackState = isActualTrackDynamic ? DYNAMIC_TO_STATIC : STATIC;
						switchFlag = true;
						break;
					}

					float pr = timerGetProgress();
					if (pr > 0.0f)
					{
						setChannelVolume(pr);
						return 0;
					}
					else //pr == 0.0f
					{
						if (!isActualTrackDynamic)
						{
							if (staticMusicActiveTrackQueueIndex != -1)
							{
								checkAndReleaseTrack(&(v_sounds[vi_staticMusicQueue[staticMusicActiveTrackQueueIndex]]));
								staticMusicActiveTrackQueueIndex = -1;
							}
						}
						else
						{
							dynamicMusicActiveTrackQueueIndex = -1;
						}

						timerStop();
						stopChannel();
						return 0;
					}
				}
				else //!channel->isPlaying()
				{
					if (!vi_dynamicMusicQueue.empty())
					{
						playbackState = DYNAMIC;
						switchFlag = true;
						break;
					}
					else if (staticMusicNextTrackQueueIndex != -1) 
					{
						playbackState = STATIC;
						switchFlag = true;
						break;
					}

					return 0;
				}
			} //case (NO_PLAYBACK)

			case (STATIC):
			{
				isActualTrackDynamic = false;

				if (channel->isPlaying())
				{
					if (vi_dynamicMusicQueue.size() > 0)
					{
						playbackState = STATIC_TO_DYNAMIC;
						switchFlag = true;
						break;
					}
					else //vi_dynamicMusicQueue.size() == 0)
					{
						if (vi_staticMusicQueue.size() == 0)
						{
							timerStart();
							playbackState = NO_PLAYBACK;
							switchFlag = true;
							break;
						}
						else //vi_staticMusicQueue.size() > 0
						{
							if (staticMusicNextTrackQueueIndex == -1 && !timerCounting)
							{
								return 0;
							}
							else //staticMusicNextTrackQueueIndex != -1
							{
								float pr = timerGetProgress();
								if (pr > 0.0f)
								{
									setChannelVolume(pr);
									return 0;
								}
								else //pr <= 0
								{
									timerStop();
									stopChannel();
									return 0;
								}
							}
						}
					}
				}
				else //!channel->isPlaying()
				{
					if (timerCounting)
						timerStop();

					if (vi_dynamicMusicQueue.size() > 0)
					{
						if (staticMusicActiveTrackQueueIndex != -1)
						{
							checkAndReleaseTrack(&(v_sounds[vi_staticMusicQueue[staticMusicActiveTrackQueueIndex]]));
							staticMusicActiveTrackQueueIndex = -1;
						}

						playbackState = DYNAMIC;
						switchFlag = true;
						break;
					}
					else //vi_dynamicMusicQueue.size() == 0)
					{
						if (vi_staticMusicQueue.size() == 0)
						{
							if (staticMusicActiveTrackQueueIndex != -1)
							{
								checkAndReleaseTrack(&(v_sounds[vi_staticMusicQueue[staticMusicActiveTrackQueueIndex]]));
								staticMusicActiveTrackQueueIndex = -1;
							}

							playbackState = NO_PLAYBACK;
							switchFlag = true;
							break;
						}
						else //vi_staticMusicQueue.size() > 0
						{
							if (!loadingActiveTrack)
							{
								if (staticMusicNextTrackQueueIndex != -1)
								{
									if (vi_staticMusicQueue[staticMusicActiveTrackQueueIndex] != vi_staticMusicQueue[staticMusicNextTrackQueueIndex])
									{
										checkAndReleaseTrack(&(v_sounds[vi_staticMusicQueue[staticMusicActiveTrackQueueIndex]]));
										staticMusicActiveTrackQueueIndex = staticMusicNextTrackQueueIndex;
									}
									staticMusicNextTrackQueueIndex = -1;

									return playTrack(vi_staticMusicQueue[staticMusicActiveTrackQueueIndex]);
								}
								else
								{
									if (staticPlaybackType == NO_REPEAT)
									{
										if (staticMusicActiveTrackQueueIndex != -1)
										{
											checkAndReleaseTrack(&(v_sounds[vi_staticMusicQueue[staticMusicActiveTrackQueueIndex]]));
											staticMusicActiveTrackQueueIndex = -1;
										}

										playbackState = NO_PLAYBACK;
										switchFlag = true;
										break;
									}
									else if (staticPlaybackType == REPEAT_ONE)
									{
										return playTrack(vi_staticMusicQueue[staticMusicActiveTrackQueueIndex]);
									}
									else //staticPlaybackType == REPEAT_ALL
									{
										short nextTrqi = (staticMusicActiveTrackQueueIndex + 1) % vi_staticMusicQueue.size();
										if (vi_staticMusicQueue[nextTrqi] != vi_staticMusicQueue[staticMusicActiveTrackQueueIndex])
										{
											checkAndReleaseTrack(&(v_sounds[vi_staticMusicQueue[staticMusicActiveTrackQueueIndex]]));
										}
										staticMusicActiveTrackQueueIndex = nextTrqi;
										return playTrack(vi_staticMusicQueue[staticMusicActiveTrackQueueIndex]);
									}
								}
							}
							else //loadingActiveTrack
							{
								if (staticMusicNextTrackQueueIndex != -1)
								{
									if (staticMusicActiveTrackQueueIndex != -1 && vi_staticMusicQueue[staticMusicActiveTrackQueueIndex] != vi_staticMusicQueue[staticMusicNextTrackQueueIndex])
									{
										checkAndReleaseTrack(&(v_sounds[vi_staticMusicQueue[staticMusicActiveTrackQueueIndex]]));
									}
									staticMusicActiveTrackQueueIndex = staticMusicNextTrackQueueIndex;
									staticMusicNextTrackQueueIndex = -1;

									return playTrack(vi_staticMusicQueue[staticMusicActiveTrackQueueIndex]);
								}
								else //staticMusicNextTrackQueueIndex == -1
								{
									return playTrack(vi_staticMusicQueue[staticMusicActiveTrackQueueIndex]);
								}
							}
						}
					}
				}
			} //case (STATIC)

			case (DYNAMIC):
			{
				isActualTrackDynamic = true;

				if (channel->isPlaying())
				{
					if (vi_dynamicMusicQueue.size() == 0)
					{
						playbackState = DYNAMIC_TO_STATIC;
						switchFlag = true;
						break;
					}
					else //vi_dynamicMusicQueue.size() > 0
					{
						if (dynamicMusicActiveTrackQueueIndex != (vi_dynamicMusicQueue.size() - 1) || timerCounting)
						{
							float pr = timerGetProgress();
							if (pr > 0.0f)
							{
								setChannelVolume(pr);
								return 0;
							}
							else //pr == 0.0f
							{
								timerStop();
								stopChannel();
								return 0;
							}
						}
						else //dynamicMusicActiveTrackQueueIndex == (vi_dynamicMusicQueue.size() - 1) && !timerCounting
						{
							return 0;
						}
					}
				}
				else //!channel->isPlaying()
				{
					if (timerCounting)
						timerStop();

					if (vi_dynamicMusicQueue.size() == 0)
					{
						dynamicMusicActiveTrackQueueIndex = -1;
						playbackState = STATIC;
						switchFlag = true;
						break;
					}
					else //vi_dynamicMusicQueue.size() != 0
					{
						if (dynamicMusicActiveTrackQueueIndex != (vi_dynamicMusicQueue.size() - 1))
						{
							dynamicMusicActiveTrackQueueIndex = vi_dynamicMusicQueue.size() - 1;
							return playTrack(vi_dynamicMusicQueue[dynamicMusicActiveTrackQueueIndex]);
						}
						else //dynamicMusicActiveTrackQueueIndex == (vi_dynamicMusicQueue.size() - 1)
						{
							return playTrack(vi_dynamicMusicQueue[dynamicMusicActiveTrackQueueIndex]);
						}
					}
				}
			} //case (DYNAMIC)

			case (STATIC_TO_DYNAMIC):
			{
				if (vi_dynamicMusicQueue.empty())
				{
					playbackState = STATIC;
					switchFlag = true;
					break;
				}
				else //!vi_dynamicMusicQueue.empty()
				{
					if (channel->isPlaying())
					{
						float pr = timerGetProgress();
						if (pr > 0.0f)
						{
							setChannelVolume(pr);
							return 0;
						}
						else //pr == 0.0f
						{
							timerStop();
							stopChannel();
							return 0;
						}
					}
					else //!channel->isPlaying()
					{
						checkAndReleaseTrack(&(v_sounds[vi_staticMusicQueue[staticMusicActiveTrackQueueIndex]]));
						playbackState = DYNAMIC;
						switchFlag = true;
						break;
					}
				}
			} //case (STATIC_TO_DYNAMIC)

			case (DYNAMIC_TO_STATIC):
			{
				if (vi_dynamicMusicQueue.empty())
				{
					playbackState = DYNAMIC;
					switchFlag = true;
					break;
				}
				else //!vi_dynamicMusicQueue.empty()
				{
					if (channel->isPlaying())
					{
						float pr = timerGetProgress();
						if (pr > 0.0f)
						{
							setChannelVolume(pr);
							return 0;
						}
						else //pr == 0.0f
						{
							timerStop();
							stopChannel();
							return 0;
						}
					}
					else //!channel->isPlaying()
					{
						dynamicMusicActiveTrackQueueIndex = -1;
						playbackState = STATIC;
						switchFlag = true;
						break;
					}
				}
			} //case (DYNAMIC_TO_STATIC)

		}
	}
	while (switchFlag);

	return 0;
};

USHORT MusicBox::addMusic(UINT _id, bool _preload)
{
	short index = getLocalIndexFromId(_id);

	if (index == -1)
	{
		SoundData* s = NULL;
		if (_preload)
			s = SoundData::getResource(_id);
		v_sounds.push_back(MusicTrack(_id, _preload, s));
		index = v_sounds.size() - 1;
	}
	else
	{
		if (_preload && v_sounds[index].sound == NULL)
		{
			v_sounds[index].sound = SoundData::getResource(v_sounds[index].id);
		}
		else if (!_preload && v_sounds[index].sound != NULL)
		{
			removalList.push_back(&(v_sounds[index]));
		}
		v_sounds[index].preload = _preload;
	}

	return index;
};

USHORT MusicBox::getSoundsNumber()
{
	return v_sounds.size();
};

void MusicBox::setTrackPreloadByID(UINT _id, bool _preload)
{
	short index = getLocalIndexFromId(_id);

	if (index != -1 && v_sounds[index].preload != _preload)
	{
		if (_preload && v_sounds[index].sound == NULL)
		{
			v_sounds[index].sound = SoundData::getResource(v_sounds[index].id);
		}
		else if (!_preload && v_sounds[index].sound != NULL)
		{
			removalList.push_back(&(v_sounds[index]));
		}
		v_sounds[index].preload = _preload;
	}
};

void MusicBox::setTrackPreloadByIndex(USHORT _index, bool _preload)
{
	if (_index >= 0 && _index < v_sounds.size() && v_sounds[_index].preload != _preload)
	{
		if (_preload && v_sounds[_index].sound == NULL)
		{
			v_sounds[_index].sound = SoundData::getResource(v_sounds[_index].id);
		}
		else if (!_preload && v_sounds[_index].sound != NULL)
		{
			removalList.push_back(&(v_sounds[_index]));
		}
		v_sounds[_index].preload = _preload;
	}
};

void MusicBox::setPreloadAllSounds(bool _preload)
{
	bool ip = channel->isPlaying();
	for (USHORT i=0;i<v_sounds.size();i++)
	{
		if (v_sounds[i].preload != _preload)
		{
			if (_preload)
			{
				if (v_sounds[i].sound == NULL)
					v_sounds[i].sound = SoundData::getResource(v_sounds[i].id);
			}
			else
			{
				if (v_sounds[i].sound != NULL)
				{
					if (ip)
						removalList.push_back(&(v_sounds[i]));
					else
					{
						v_sounds[i].sound->release();
						v_sounds[i].sound = NULL;
					}
				}
			}
			v_sounds[i].preload = _preload;
		}
	}
};

void MusicBox::setMusicTransitionTime(float _musicTransitionTime)
{
	if (_musicTransitionTime < 0.0f)
		musicTransitionTime = 0.0f;
	else if (_musicTransitionTime > 30000.0f)
		musicTransitionTime = 30000.0f;
	else
		musicTransitionTime = _musicTransitionTime;
};

float MusicBox::getMusicTransitionTime()
{
	return musicTransitionTime;
};

void MusicBox::setMusicTransitionExponential(float _musicTransitionExponential)
{
	if (_musicTransitionExponential < 0.2f)
		musicTransitionExponential = 0.2f;
	else if (_musicTransitionExponential > 10.0f)
		musicTransitionExponential = 10.0f;
	else
		musicTransitionExponential = _musicTransitionExponential;
};

float MusicBox::getMusicTransitionExponential()
{
	return musicTransitionExponential;
};

bool MusicBox::isPlaybackFromDynamicStack()
{
	return isActualTrackDynamic;
};

short MusicBox::getStaticQueueSize()
{
	return vi_staticMusicQueue.size();
};

short MusicBox::getDynamicStackSize()
{
	return vi_dynamicMusicQueue.size();
};

short MusicBox::addToDynamicStackByID(UINT _id, bool withTransition)
{
	short i = getLocalIndexFromId(_id);
	if (i == -1)
		return -1;

	if (vi_dynamicMusicQueue.size() == 0)
		return -1;
	
	if (dynamicMusicActiveTrackQueueIndex != -1)
	{
		short vd = vi_dynamicMusicQueue[dynamicMusicActiveTrackQueueIndex];

		if (v_sounds[vd].id != v_sounds[i].id)
		{
			removalList.push_back(&(v_sounds[vd]));
		}
	}

	vi_dynamicMusicQueue.push_back(i);

	if (!withTransition)
		stopChannel();
	else
		timerStart();

	return 0;
};

short MusicBox::addToDynamicStackByIndex(USHORT _index, bool withTransition)
{
	if (_index <0 || _index >= v_sounds.size())
		return -1;

	if (vi_dynamicMusicQueue.size() == 0)
		return -1;

	if (dynamicMusicActiveTrackQueueIndex != -1)
	{
		short vd = vi_dynamicMusicQueue[dynamicMusicActiveTrackQueueIndex];

		if (v_sounds[vd].id != v_sounds[_index].id)
		{
			removalList.push_back(&(v_sounds[vd]));
		}
	}

	vi_dynamicMusicQueue.push_back(_index);

	if (!withTransition)
		stopChannel();
	else
		timerStart();

	return 0;
};

short MusicBox::popFromDynamicStack(bool withTransition)
{
	if (vi_dynamicMusicQueue.size() == 0)
		return -1;

	short i = vi_dynamicMusicQueue.size() - 1;
	short tr = vi_dynamicMusicQueue[i-1];

	if (dynamicMusicActiveTrackQueueIndex != -1)
	{
		short vd = vi_dynamicMusicQueue[dynamicMusicActiveTrackQueueIndex];

		if (v_sounds[vd].id != v_sounds[tr].id)
		{
			removalList.push_back(&(v_sounds[vd]));
		}
	}

	vi_dynamicMusicQueue.pop_back();

	if (!withTransition)
		stopChannel();
	else
		timerStart();

	return 0;
};

short MusicBox::popAllFromDynamicStack(bool withTransition)
{
	if (vi_dynamicMusicQueue.size() == 0)
		return -1;

	if (dynamicMusicActiveTrackQueueIndex != -1)
	{
		USHORT vd = vi_dynamicMusicQueue[dynamicMusicActiveTrackQueueIndex];
		removalList.push_back(&(v_sounds[vd]));
	}

	vi_dynamicMusicQueue.clear();
	
	if (!withTransition)
		stopChannel();
	else
		timerStart();

	return 0;
};

void MusicBox::setStaticPlaybackType(PLAYBACK_REPEAT_TYPE _type)
{
	staticPlaybackType = _type;
};

PLAYBACK_REPEAT_TYPE MusicBox::getStaticPlaybackType()
{
	return staticPlaybackType;
};

short MusicBox::addToStaticQueueByID(UINT _id)
{
	return addToStaticQueueByIndex(getLocalIndexFromId(_id));
};

short MusicBox::addToStaticQueueByIndex(USHORT _index)
{
	if (_index < 0 || _index >= v_sounds.size())
		return -1;

	vi_staticMusicQueue.push_back(_index);
	return 0;
};

short MusicBox::removeFromStaticQueueByQueueIndex(USHORT _index)
{
	if (_index < 0 || _index >= vi_staticMusicQueue.size())
		return -1;

	if (isActualTrackDynamic)
	{
		checkAndReleaseTrack(&(v_sounds[vi_staticMusicQueue[_index]]));
		vi_staticMusicQueue.erase(vi_staticMusicQueue.begin() += _index);

		if (staticMusicActiveTrackQueueIndex > _index)
			staticMusicActiveTrackQueueIndex -= 1;
		if (staticMusicNextTrackQueueIndex > _index)
			staticMusicNextTrackQueueIndex -= 1;

		return 0;
	}

	if (staticMusicActiveTrackQueueIndex != _index)
	{
		checkAndReleaseTrack(&(v_sounds[vi_staticMusicQueue[_index]]));
		vi_staticMusicQueue.erase(vi_staticMusicQueue.begin() += _index);

		if (staticMusicActiveTrackQueueIndex > _index)
			staticMusicActiveTrackQueueIndex -= 1;
		if (staticMusicNextTrackQueueIndex > _index)
			staticMusicNextTrackQueueIndex -= 1;

		return 0;
	}

	stopChannel();

	if (staticMusicNextTrackQueueIndex == -1)
		staticMusicNextTrackQueueIndex = staticMusicActiveTrackQueueIndex;
	if (staticMusicNextTrackQueueIndex > _index)
		staticMusicNextTrackQueueIndex -= 1;
	staticMusicActiveTrackQueueIndex = -1;

	checkAndReleaseTrack(&(v_sounds[vi_staticMusicQueue[_index]]));
	vi_staticMusicQueue.erase(vi_staticMusicQueue.begin() += _index);

	timerStop();

	return 0;
};

short MusicBox::removeFromStaticQueueByTrackIndex(USHORT _index)
{
	if (_index < 0 || _index >= vi_staticMusicQueue.size())
		return -1;

	for (USHORT i=0;i<vi_staticMusicQueue.size();i++)
	{
		if (vi_staticMusicQueue[i] == _index)
			removeFromStaticQueueByQueueIndex(vi_staticMusicQueue[i]);
	}

	return 0;
};

short MusicBox::removeFromStaticQueueByID(UINT _id)
{
	return removeFromStaticQueueByTrackIndex(getLocalIndexFromId(_id));
};

short MusicBox::moveToStaticQueueByQueueIndex(USHORT _index, bool withTransition, bool freeDynamicQueue)
{
	if (_index < 0 || _index >= vi_staticMusicQueue.size())
		return -1;

	staticMusicNextTrackQueueIndex = _index;

	if (vi_dynamicMusicQueue.size() == 0 || freeDynamicQueue)
	{
		if (freeDynamicQueue)
		{
			if (dynamicMusicActiveTrackQueueIndex != -1)
			{
				USHORT vd = vi_dynamicMusicQueue[dynamicMusicActiveTrackQueueIndex];

				removalList.push_back(&(v_sounds[vd]));
			}

			vi_dynamicMusicQueue.clear();
		}

		if (!withTransition)
			stopChannel();
		else if (channel->isPlaying() && playbackState != NO_PLAYBACK)
			timerStart();
	}

	return 0;
};

bool MusicBox::isMusicPlaying()
{
	if (playbackState == NO_PLAYBACK)
		return false;
	return true;
};

void MusicBox::stopPlayback(bool withTransition)
{
	if (!withTransition)
		stopChannel();
	else if (channel->isPlaying() && playbackState != NO_PLAYBACK)
		timerStart();

	playbackState = NO_PLAYBACK;
};

bool MusicBox::isPlaybackPaused()
{
	return channelPaused;
};

void MusicBox::pausePlayback(bool _pause)
{
	channelPaused = _pause;
	channel->setPaused(_pause);
};
*/

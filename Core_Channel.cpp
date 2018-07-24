#include "Core_Channel.h"



vector<pair<bool, Channel*>>	Channel::channelsVector;
FMOD::System*					Channel::soundSystem = NULL;



int Channel::setupChannels(FMOD::System* _soundSystem, USHORT _maxChannelNumber)
{
	if (!_soundSystem)
		return 1;
	if (!_maxChannelNumber)
		return 2;

	soundSystem = _soundSystem;

	channelsVector.resize(_maxChannelNumber);
	for (int i=0;i<channelsVector.size();++i)
	{
		channelsVector[i].first = false;
		channelsVector[i].second = NULL;
	}

	return 0;
};


int Channel::getFreeChannelIndex()
{
	for (int i=0;i<channelsVector.size();++i)
		if (!channelsVector[i].first)
			return i;
	return -1;
};


int Channel::releaseAll()
{
	soundSystem = NULL;

	for (int i=0;i<channelsVector.size();++i)
	{
		SAFE_RELEASE(channelsVector[i].second);
		channelsVector[i].first = false;
	}
	return 0;
};


Channel::Channel()
{
	index = 0xFFFF;
	channel = NULL;

	channelReady = false;

	channelGroup = NULL;
	channelGroupIndex = short(0xFFFF);

	volume = 1.0f;

	mode = FMOD_DEFAULT;
	is3D = false;


	positionLinked = false;
	position = NULL;
	velocityLinked = false;
	velocity = NULL;

	priority = 128;

	soundConeOrientationSet = false;
	soundConeOrientation = NULL;

	soundConeSettingsSet = false;
	soundConeInsideAngle = 360.0f;
	soundConeOutsideAngle = 360.0f;
	soundConeOutsideVolume = 1.0f;

	customSettingsSet = false;
	minAudibleDistance = SOUNDSYSTEM_DEFAULT_MIN_AUDIBLE_DISTANCE;
	maxAudibleDistance = SOUNDSYSTEM_DEFAULT_MAX_AUDIBLE_DISTANCE;
};


int Channel::setupChannel(FMOD::Channel* _channel, FMOD::ChannelGroup* _channelGroup, short _groupIndex, FMOD_MODE _mode, FMOD_VECTOR* _position, FMOD_VECTOR* _velocity)
{
	int i;
	_channel->getIndex(&i);
	index = i;
	channel = _channel;

	channelGroup = _channelGroup;
	channelGroupIndex = _groupIndex;

	volume = 1.0f;

	mode = _mode;
	is3D = (_mode & FMOD_3D) ? true : false;

	if (is3D)
	{
		if (_position)
		{
			positionLinked = true;
			position = _position;
		}
		else
		{
			positionLinked = false;
			position = new FMOD_VECTOR;
			ZeroMemory(position, sizeof(FMOD_VECTOR));
		}

		if (_velocity)
		{
			velocityLinked = true;
			velocity = _velocity;
		}
		else
		{
			velocityLinked = false;
			velocity = new FMOD_VECTOR;
			ZeroMemory(velocity, sizeof(FMOD_VECTOR));
		}


		soundConeOrientationSet = false;
		soundConeOrientation = NULL;

		soundConeSettingsSet = false;
		soundConeInsideAngle = 360.0f;
		soundConeOutsideAngle = 360.0f;
		soundConeOutsideVolume = 1.0f;

		customSettingsSet = false;
		minAudibleDistance = SOUNDSYSTEM_DEFAULT_MIN_AUDIBLE_DISTANCE;
		maxAudibleDistance = SOUNDSYSTEM_DEFAULT_MAX_AUDIBLE_DISTANCE;
	}

	priority = 128;

	channelReady = false;

	channelsVector[index].first = true;
	channelsVector[index].second = this;

	return 0;
};


int Channel::release()
{
	channelsVector[index].first = false;
	channelsVector[index].second = NULL;

	if (!positionLinked && position)
		delete position;
	if (!velocityLinked && velocity)
		delete velocity;

	delete this;
	return 0;
};


int Channel::setVolume(float _volume)
{
	_volume = _volume < 0.0f ? 0.0f : ( _volume > 1.0f ? 1.0f : _volume );

	if (volume == _volume)
		return 0;

	volume = _volume;

	channel->setVolume(volume);

	return 0;
};


float Channel::getVolume()
{
	return volume;
};


int Channel::set3DPosition(FMOD_VECTOR const & _position)
{
	*position = _position;
	return 0;
};


int Channel::set3DPosition(D3DVECTOR const & _position)
{
	memcpy(position, &_position, sizeof(D3DVECTOR));
	return 0;
};


int Channel::set3DPosition(FMOD_VECTOR* _position)
{
	if (!positionLinked && !_position)
		return 0;

	if (positionLinked && !_position)
	{
		positionLinked = false;
		position = new FMOD_VECTOR;
		FMOD_FAILED(channel->set3DAttributes(position, NULL));
	}
	else if (!positionLinked && _position)
	{
		FMOD_FAILED(channel->set3DAttributes(_position, NULL));
		positionLinked = true;
		delete position;
		position = _position;
	}
	else //positionLinked && _position
	{
		position = _position;
		FMOD_FAILED(channel->set3DAttributes(position, NULL));
	}

	return 0;
};


int Channel::set3DPosition(D3DVECTOR* _position)
{
	return set3DPosition(reinterpret_cast <FMOD_VECTOR*> (_position));
};


int Channel::set3DVelocity(FMOD_VECTOR const & _velocity)
{
	*velocity = _velocity;
	return 0;
};


int Channel::set3DVelocity(D3DVECTOR const & _velocity)
{
	memcpy(velocity, &_velocity, sizeof(D3DVECTOR));
	return 0;
};


int Channel::set3DVelocity(FMOD_VECTOR* _velocity)
{
	if (!velocityLinked && !_velocity)
		return 0;

	if (velocityLinked && !_velocity)
	{
		velocityLinked = false;
		velocity = new FMOD_VECTOR;
		FMOD_FAILED(channel->set3DAttributes(velocity, NULL));
	}
	else if (!velocityLinked && _velocity)
	{
		FMOD_FAILED(channel->set3DAttributes(_velocity, NULL));
		velocityLinked = true;
		delete velocity;
		velocity = _velocity;
	}
	else //velocityLinked && _velocity
	{
		velocity = _velocity;
		FMOD_FAILED(channel->set3DAttributes(velocity, NULL));
	}

	return 0;
};


int Channel::set3DVelocity(D3DVECTOR* _velocity)
{
	return set3DPosition(reinterpret_cast <FMOD_VECTOR*> (_velocity));
};


FMOD_VECTOR* Channel::getPositionPointer()
{
	return position;
};


FMOD_VECTOR Channel::getPosition()
{
	return *position;
};


FMOD_VECTOR* Channel::getVelocityPointer()
{
	return velocity;
};


FMOD_VECTOR Channel::getVelocity()
{
	return *velocity;
};


bool Channel::isPositionLinked()
{
	return positionLinked;
};


bool Channel::isVelocityLinked()
{
	return velocityLinked;
};


int Channel::setCustomMinMaxAudibleDistance(float _minAudibleDistance, float _maxAudibleDistance)
{
	if (minAudibleDistance == _minAudibleDistance && maxAudibleDistance == _maxAudibleDistance)
		return 0;

	if (_minAudibleDistance < 0.0f)
		return 1;
	if (_minAudibleDistance > _maxAudibleDistance)
		return 2;

	minAudibleDistance = _minAudibleDistance;
	maxAudibleDistance = _maxAudibleDistance;

	channel->set3DMinMaxDistance(minAudibleDistance, maxAudibleDistance);

	customSettingsSet = true;

	return 0;
};


int Channel::getMinMaxAudibleDistance(float* _minAudibleDistance, float* _maxAudibleDistance)
{
	if (_minAudibleDistance)
		*_minAudibleDistance = minAudibleDistance;

	if (_maxAudibleDistance)
		*_maxAudibleDistance = maxAudibleDistance;

	return 0;
};


int Channel::setupSoundCone(FMOD_VECTOR* _soundConeOrientation, float _soundConeInsideAngle, float _soundConeOutsideAngle, float _soundConeOutsideVolume)
{
	if (!_soundConeOrientation)
		return 1;

	if (_soundConeInsideAngle < 0.0f || _soundConeInsideAngle > 360.0f)
		return 2;

	if (_soundConeOutsideAngle < _soundConeInsideAngle || _soundConeOutsideAngle > 360.0f)
		return 3;

	if (_soundConeOutsideVolume > 1.0f || _soundConeOutsideVolume < 0.0f)
		return 4;

	soundConeOrientation = _soundConeOrientation;
	soundConeInsideAngle = _soundConeInsideAngle;
	soundConeOutsideAngle = _soundConeOutsideAngle;
	soundConeOutsideVolume = _soundConeOutsideVolume;


	channel->set3DConeOrientation(soundConeOrientation);
	channel->set3DConeSettings(soundConeInsideAngle, soundConeOutsideAngle, soundConeOutsideVolume);

	soundConeOrientationSet = true;
	soundConeSettingsSet = true;

	return 0;
};


int Channel::setupSoundCone(D3DXVECTOR3* _soundConeOrientation, float _soundConeInsideAngle, float _soundConeOutsideAngle, float _soundConeOutsideVolume)
{
	return setupSoundCone(reinterpret_cast <FMOD_VECTOR*> (_soundConeOrientation), _soundConeInsideAngle, _soundConeOutsideAngle, _soundConeOutsideVolume);
};


int Channel::resetSoundCone()
{
	soundConeOrientation = NULL;
	soundConeInsideAngle = 360.0f;
	soundConeOutsideAngle = 360.0f;
	soundConeOutsideVolume = 1.0f;

	channel->set3DConeSettings(soundConeInsideAngle, soundConeOutsideAngle, soundConeOutsideVolume);

	soundConeSettingsSet = false;

	return 0;
};


FMOD_VECTOR* Channel::getSoundConeOrientationVec()
{
	return soundConeOrientation;
};


int Channel::getSoundConeSettings(float* _soundConeInsideAngle, float* _soundConeOutsideAngle, float* _soundConeOutsideVolume)
{
	if (_soundConeInsideAngle)
		*_soundConeInsideAngle = soundConeInsideAngle;
	if (_soundConeOutsideAngle)
		*_soundConeOutsideAngle = soundConeOutsideAngle;
	if (_soundConeOutsideVolume)
		*_soundConeOutsideVolume = soundConeOutsideVolume;
	return 0;
};


int Channel::setPriority(USHORT _priority)
{
	if (_priority > 256)
		_priority = 256;
	if (priority == _priority)
		return 0;

	priority = _priority;

	channel->setPriority(priority);


	return 0;
};


USHORT Channel::getPriority()
{
	return priority;
};


int Channel::playSound(Sound* _sound, bool _paused)
{
	if (!_sound)
		return 1;

	FMOD_RESULT fresult = FMOD_OK;


	if (FMOD_FAILED(fresult = soundSystem->playSound(FMOD_CHANNELINDEX(index), _sound->getResourcePointer(), true, NULL)))
	{
		showError(false, "FMOD::System::playSound error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
		return -1;
	}

	if (FMOD_FAILED(fresult = channel->setChannelGroup(channelGroup)))
	{
		showError(false, "FMOD::Channel::setChannelGroup error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
		return -2;
	}


	if (FMOD_FAILED(fresult = channel->setVolume(volume)))
	{
		showError(false, "FMOD::Channel::setVolume error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
		return -3;
	}


	if (FMOD_FAILED(fresult = channel->setMode(mode)))
	{
		showError(false, "FMOD::Channel::setMode error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
		return -4;
	}


	if (is3D)
	{
		if (FMOD_FAILED(fresult = channel->set3DAttributes(position, velocity)))
		{
			showError(false, "FMOD::Channel::set3DAttributes error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
			return -5;
		}


		if (soundConeOrientationSet)
		{
			if (FMOD_FAILED(fresult = channel->set3DConeOrientation(soundConeOrientation)))
			{
				showError(false, "FMOD::Channel::set3DConeOrientation error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
				return -6;
			}
		}

		if (soundConeSettingsSet)
		{
			if (FMOD_FAILED(fresult = channel->set3DConeSettings(soundConeInsideAngle, soundConeOutsideAngle, soundConeOutsideVolume)))
			{
				showError(false, "FMOD::Channel::set3DConeSettings error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
				return -7;
			}
		}

		if (customSettingsSet)
		{
			if (FMOD_FAILED(fresult = channel->set3DMinMaxDistance(minAudibleDistance, maxAudibleDistance)))
			{
				showError(false, "FMOD::Channel::set3DMinMaxDistance error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
				return -8;
			}
		}
	}

	if (FMOD_FAILED(fresult = channel->setPriority(priority)))
	{
		showError(false, "FMOD::Channel::setPriority error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
		return -9;
	}



	if (!_paused)
	{
		if (FMOD_FAILED(fresult = channel->setPaused(false)))
		{
			showError(false, "FMOD::Channel::setPaused error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
			return -10;
		}
	}

	channelReady = true;

	return 0;
};


int Channel::setPaused(bool _paused)
{
	if (!channelReady)
		return 1;

	FMOD_RESULT fresult = FMOD_OK;
	if (FMOD_FAILED(fresult = channel->setPaused(_paused)))
	{
		showError(false, "FMOD::Channel::setPaused error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
		return -1;
	}

	return 0;
};


int Channel::getPaused(bool* _paused)
{
	if (!channelReady)
	{
		*_paused = false;
		return 2;
	}
	if (!_paused)
		return 1;

	FMOD_RESULT fresult = FMOD_OK;
	if (FMOD_FAILED(fresult = channel->getPaused(_paused)))
	{
		showError(false, "FMOD::Channel::getPaused error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
		return -1;
	}

	return 0;
};


int Channel::stop()
{
	if (!channelReady)
		return 1;

	channelReady = false;

	FMOD_RESULT fresult = FMOD_OK;
	if (FMOD_FAILED(fresult = channel->stop()))
	{
		showError(false, "FMOD::Channel::stop error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
		return -1;
	}

	return 0;
};


int Channel::isPlaying(bool* _isPlaying)
{
	if (!_isPlaying)
		return 1;
	if (!channelReady)
	{
		*_isPlaying = false;
		return 2;
	}

	FMOD_RESULT fresult = FMOD_OK;
	if (FMOD_FAILED(fresult = channel->isPlaying(_isPlaying)))
	{
		showError(false, "FMOD::Channel::isPlaying error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
		return -1;
	}

	return 0;
};


int Channel::isVirtual(bool* _isVirtual)
{
	if (!channelReady)
		return 1;
	if (!_isVirtual)
		return 2;

	FMOD_RESULT fresult = FMOD_OK;
	if (FMOD_FAILED(fresult = channel->isVirtual(_isVirtual)))
	{
		showError(false, "FMOD::Channel::isVirtual error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
		return -1;
	}

	return 0;
};


int Channel::getUserData(void** _userData)
{
	return int(channel->getUserData(_userData));
};


int Channel::setUserData(void* _userData)
{
	return int(channel->setUserData(_userData));
};


int Channel::setCallback(FMOD_CHANNEL_CALLBACK _callback)
{
	if (!channelReady)
		return 1;

	FMOD_RESULT fresult = FMOD_OK;
	if (FMOD_FAILED(fresult = channel->setCallback(_callback)))
	{
		showError(false, "FMOD::Channel::setCallback error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
		return -1;
	}

	return 0;
};


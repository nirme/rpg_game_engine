#include "Core_SoundSystem.h"



FMOD::System*						SoundSystem::soundSystem = NULL;
UINT								SoundSystem::driverId = 0;

SoundSystem::SoundSystemState		SoundSystem::systemState;
SoundSystem::SoundSystemVariables	SoundSystem::systemVariables;
SoundSystem::SoundSystem3DSettings	SoundSystem::system3DSettings;

FMOD::ChannelGroup*					SoundSystem::masterChannelGroup;
float								SoundSystem::masterChannelGroupVolume = SOUNDSYSTEM_DEFAULT_CHANNELGROUP_VOLUME;

vector<SoundSystem::ChannelGroup>	SoundSystem::channelGroups;

Channel*							SoundSystem::musicChannel = NULL;
Channel*							SoundSystem::sounds2DChannel = NULL;




int SoundSystem::setupFMODSystem()
{
	if (systemState.created)
		return 0;

	FMOD_RESULT fresult = FMOD_OK;
	if (FMOD_FAILED(fresult = FMOD::System_Create(&(soundSystem))))
	{
		showError(false, "FMOD::System_Create error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
		return -1;
	}

	if (FMOD_FAILED(fresult = soundSystem->getVersion(&systemState.version)))
	{
		SAFE_RELEASE(soundSystem);
		showError(false, "FMOD::System::getVersion error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
		return -2;
	}

	if (systemState.version < FMOD_VERSION)
	{
		SAFE_RELEASE(soundSystem);
		showError(false, "FMOD library error: You are using an old version (%08x) of FMOD library. This program requires at least version %08x\n", systemState.version, FMOD_VERSION);
		return -3;
	}

	systemState.created = true;
	return 0;
};


int SoundSystem::dropFMODSystem()
{
	if (!systemState.created)
		return 0;

	FMOD_RESULT fresult = FMOD_OK;
	if (FMOD_FAILED(fresult = soundSystem->release()))
	{
		showError(false, "FMOD::System::release error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
		return -1;
	}

	soundSystem = NULL;
	systemState.created = false;
	return 0;
};


int SoundSystem::clearSystemVariables()
{
	if (systemState.initialized)
		return 1;

	dropFMODSystem();

	systemVariables.speakerModeSetByUser = false;
	systemVariables.speakerMode = SOUNDSYSTEM_DEFAULT_SPEAKER_MODE;

	systemVariables.sampleRateSetByUser = false;
	systemVariables.sampleRate = SOUNDSYSTEM_DEFAULT_SAMPLE_RATE;

	systemVariables.formatSetByUser = false;
	systemVariables.format = SOUNDSYSTEM_DEFAULT_FORMAT;

	systemVariables.numOutputChannelsSetByUser = false;
	systemVariables.numOutputChannels = SOUNDSYSTEM_DEFAULT_NUM_OUTPUT_CHANNELS;

	systemVariables.maxInputChannelsSetByUser = false;
	systemVariables.maxInputChannels = SOUNDSYSTEM_DEFAULT_MAX_INPUT_CHANNELS;

	systemVariables.resampleMethodSetByUser = false;
	systemVariables.resampleMethod = SOUNDSYSTEM_DEFAULT_RESAMPLE_METHOD;

	systemVariables.maxSoftwareChannelsSetByUser = false;
	systemVariables.maxSoftwareChannels = SOUNDSYSTEM_DEFAULT_MAX_SOFTWARE_CHANNELS;

	systemVariables.maxVirtualChannelsSetByUser = false;
	systemVariables.maxVirtualChannels = SOUNDSYSTEM_DEFAULT_MIN_VIRTUAL_CHANNELS;

	systemVariables.HRTFMinAngleSetByUser = false;
	systemVariables.HRTFMinAngle = SOUNDSYSTEM_DEFAULT_HRTF_MINIMUM_ANGLE;

	systemVariables.HRTFMaxAngleSetByUser = false;
	systemVariables.HRTFMaxAngle = SOUNDSYSTEM_DEFAULT_HRTF_MAXIMUM_ANGLE;

	systemVariables.HRTFFrequencySetByUser = false;
	systemVariables.HRTFFrequency = SOUNDSYSTEM_DEFAULT_HRTF_FREQUENCY;

	systemVariables.vol0VirtualVolumeSetByUser = false;
	systemVariables.vol0VirtualVolume = SOUNDSYSTEM_DEFAULT_VIRTUAL_VOLUME_0_LEVEL;

	systemVariables.distanceFilterCenterFrequencySetByUser = false;
	systemVariables.distanceFilterCenterFrequency = SOUNDSYSTEM_DEFAULT_DF_CENTER_FREQUENCY;


	system3DSettings.minAudibleDistanceSetByUser = false;
	system3DSettings.minAudibleDistance = SOUNDSYSTEM_DEFAULT_MIN_AUDIBLE_DISTANCE;

	system3DSettings.maxAudibleDistanceSetByUser = false;
	system3DSettings.maxAudibleDistance = SOUNDSYSTEM_DEFAULT_MAX_AUDIBLE_DISTANCE;

	system3DSettings.dopplerScaleSetByUser = false;
	system3DSettings.dopplerScale = SOUNDSYSTEM_DEFAULT_DOPPLER_SCALE;

	system3DSettings.distanceFactorSetByUser = false;
	system3DSettings.distanceFactor = SOUNDSYSTEM_DEFAULT_DISTANCE_FACTOR;

	system3DSettings.rolloffScaleSetByUser = false;
	system3DSettings.rolloffScale = SOUNDSYSTEM_DEFAULT_INVERSE_ROLLOFF_SCALE;

	system3DSettings.defaultRolloffSetByUser = false;
	system3DSettings.defaultRolloff = SOUNDSYSTEM_DEFAULT_ROLLOFF;


	if (!system3DSettings.positionLinked)
	{
		SAFE_DELETE(system3DSettings.position);
		system3DSettings.positionLinked = false;
	}

	if (!system3DSettings.velocityLinked)
	{
		SAFE_DELETE(system3DSettings.velocity)
		system3DSettings.velocityLinked = false;
	}

	if (!system3DSettings.forwardLinked)
	{
		SAFE_DELETE(system3DSettings.forward);
		system3DSettings.forwardLinked = false;
	}

	if (!system3DSettings.upLinked)
	{
		SAFE_DELETE(system3DSettings.up);
		system3DSettings.upLinked = false;
	}


	return 0;
};


int SoundSystem::setupMasterChannelGroup()
{
	if (masterChannelGroup)
		return 0;

	if (!systemState.initFlags)
		return 1;

	FMOD_RESULT fresult = FMOD_OK;
	
	if (FMOD_FAILED(fresult = soundSystem->getMasterChannelGroup(&masterChannelGroup)))
	{
		showError(false, "FMOD::System::getMasterChannelGroup error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
		return -1;
	}

	masterChannelGroupVolume = SOUNDSYSTEM_DEFAULT_CHANNELGROUP_VOLUME;

	if (FMOD_FAILED(fresult = masterChannelGroup->setVolume(masterChannelGroupVolume)))
	{
		showError(false, "FMOD::ChannelGroup::setVolume error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
		return -2;
	}

	return 0;
};

int SoundSystem::setupChannelGroups()
{
	if (int res = setupMasterChannelGroup())
		return res;

	for (int i=0;i<channelGroups.size();++i)
	{
		if (!channelGroups[i].first)
		{
			FMOD_RESULT fresult = FMOD_OK;
			if (FMOD_FAILED(fresult = soundSystem->createChannelGroup(channelGroups[i].second.name.c_str(), &(channelGroups[i].first))))
			{
				showError(false, "FMOD::System::createChannelGroup error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
				return -11;
			}

			if (FMOD_FAILED(fresult = masterChannelGroup->setVolume(channelGroups[i].second.volume)))
			{
				showError(false, "FMOD::ChannelGroup::setVolume error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
				return -12;
			}
		}
	}

	return 0;
};



int SoundSystem::setupChannels()
{
	if (musicChannel && sounds2DChannel)
		return 0;

	if (!systemState.initFlags || !masterChannelGroup)
		return 10;

	if (int res = Channel::setupChannels(soundSystem, systemVariables.maxVirtualChannels))
		return res;

	FMOD::Channel* chn = NULL;
	FMOD_RESULT fresult = FMOD_OK;
	if (FMOD_FAILED(fresult = soundSystem->getChannel(systemVariables.maxVirtualChannels - 1, &chn)))
	{
		showError(false, "FMOD::System::getChannel error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
		return -1;
	}

	musicChannel = new Channel;
	musicChannel->setupChannel(chn, masterChannelGroup, SOUNDSYSTEM_MASTER_CHANNELGROUP_ID, SOUNDSYSTEM_DEFAULT_2D_CHANNEL_MODE, NULL, NULL);
	musicChannel->setPriority(0);
	musicChannel->setVolume(1.0f);


	if (FMOD_FAILED(fresult = soundSystem->getChannel(systemVariables.maxVirtualChannels - 2, &chn)))
	{
		showError(false, "FMOD::System::getChannel error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
		return -2;
	}

	sounds2DChannel = new Channel;
	sounds2DChannel->setupChannel(chn, masterChannelGroup, SOUNDSYSTEM_MASTER_CHANNELGROUP_ID, SOUNDSYSTEM_DEFAULT_2D_CHANNEL_MODE, NULL, NULL);
	sounds2DChannel->setPriority(1);
	sounds2DChannel->setVolume(1.0f);

	return 0;
};



int SoundSystem::getDeviceNumbers()
{
	bool flag = systemState.created;
	int res = setupFMODSystem();
	if (res)
		return res;

	FMOD_RESULT fresult = FMOD_OK;
	int deviceNum = 0;
	if (FMOD_FAILED(fresult = soundSystem->getNumDrivers(&deviceNum)))
	{
		showError(false, "FMOD::System::getNumDrivers error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
		return -10;
	}

	if (!flag)
		dropFMODSystem();

	return deviceNum;
};


int SoundSystem::getDeviceInfo(UINT _indexIn, string* _nameStringOut, wstring* _nameWstringOut, FMOD_CAPS* _deviceCapsOut, int* _outputRateOut, FMOD_SPEAKERMODE* _speakerModeOut)
{
	(*_nameStringOut) = "";
	(*_nameWstringOut) = L"";

	int num = getDeviceNumbers();
	if (num < 0)
		return num;
	if (_indexIn >= num)
	{
		showError(false, "SoundSystem::getDeviceInfo: given device index too big\n");
		return -10;
	}

	FMOD_RESULT fresult = FMOD_OK;

	if (_nameStringOut)
	{
		char name[1024];
		FMOD_GUID gid;
		if (FMOD_FAILED(fresult = soundSystem->getDriverInfo(_indexIn, name, 1024, &gid)))
		{
			showError(false, "FMOD::System::getDriverInfo error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
			return -11;
		}
		*_nameStringOut = name;
	}

	if (_nameWstringOut)
	{
		wchar_t name[1024];
		FMOD_GUID gid;
		if (FMOD_FAILED(fresult = soundSystem->getDriverInfoW(_indexIn, (short*)name, 1024, &gid)))
		{
			showError(false, "FMOD::System::getDriverInfoW error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
			return -12;
		}
		*_nameWstringOut = name;
	}

	if (_outputRateOut || _deviceCapsOut || _speakerModeOut)
	{
		int outputRate = 0;
		FMOD_CAPS deviceCaps;
		FMOD_SPEAKERMODE speakerMode;
		if (FMOD_FAILED(fresult = soundSystem->getDriverCaps(_indexIn, &deviceCaps, &outputRate, &speakerMode)))
		{
			showError(false, "FMOD::System::getDriverCaps error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
			return -13;
		}

		if (_outputRateOut)
			*_outputRateOut = outputRate;

		if (_deviceCapsOut)
			*_deviceCapsOut = deviceCaps;

		if (_speakerModeOut)
			*_speakerModeOut = speakerMode;
	}

	return 0;
};


int SoundSystem::initialize()
{
	//return if already initialized
	if (systemState.initialized)
		return 1;

	int res = 0;
	if (res = setupFMODSystem())
		return res	;

	//double check driver id is valid
	int num = getDeviceNumbers();
	if (num <= 0)
		return -100;
	if (driverId >= num)
		driverId = 0;

	//set device
	FMOD_RESULT fresult = FMOD_OK;
	if (FMOD_FAILED(fresult = soundSystem->setDriver(driverId)))
	{
		shutdown();
		showError(false, "FMOD::System::setDriver error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
		return -11;
	}


	int outputRate = 0;
	FMOD_CAPS deviceCaps = 0;
	FMOD_SPEAKERMODE speakerMode = FMOD_SPEAKERMODE_MONO;

	if (FMOD_FAILED(fresult = soundSystem->getDriverCaps(driverId, &systemState.caps, &outputRate, &speakerMode)))
	{
		showError(false, "FMOD::System::getDriverCaps error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
		return -12;
	}

	if (!systemState.caps)
	{
		showError(false, "Selected device is not supported.\n", fresult, FMOD_ErrorString(fresult));
		return -200;
	}

	if (systemState.caps == FMOD_CAPS_HARDWARE_EMULATED)
	{
		showError(false, "Hardware acceleration is turned off, it will cause higher sound latency.\n");
	}


	if (!systemVariables.speakerModeSetByUser)
		systemVariables.speakerMode = SOUNDSYSTEM_DEFAULT_SPEAKER_MODE;

	if (systemVariables.speakerModeSetByUser)
	{
		if (FMOD_FAILED(fresult = soundSystem->setSpeakerMode(systemVariables.speakerMode)))
		{
			if (systemVariables.speakerMode != SOUNDSYSTEM_DEFAULT_SPEAKER_MODE)
			{
				if (!FMOD_FAILED(fresult = soundSystem->setSpeakerMode(systemVariables.speakerMode)))
				{
					systemVariables.speakerModeSetByUser = false;
					systemVariables.speakerMode = SOUNDSYSTEM_DEFAULT_SPEAKER_MODE;
				}
			}

			if (FMOD_FAILED(fresult))
			{
				shutdown();
				showError(false, "FMOD::System::setSpeakerMode with value %d error: (%d) %s\n", systemVariables.speakerMode, fresult, FMOD_ErrorString(fresult));
				return -13;
			}
		}
	}


	//setup software format
	if (!systemVariables.sampleRateSetByUser)
		systemVariables.sampleRate = SOUNDSYSTEM_DEFAULT_SAMPLE_RATE;


	if (!systemVariables.formatSetByUser)
		systemVariables.format = SOUNDSYSTEM_DEFAULT_FORMAT;

	//check if device is not Sigmatel
	{
		char driverName[1024];
		if (FMOD_FAILED(fresult = soundSystem->getDriverInfo(driverId, driverName, 1024, NULL)))
		{
			shutdown();
			showError(false, "FMOD::System::getDriverInfo error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
			return -14;
		}

		//Sigmatel sound devices crackle for some reason if the format is PCM 16bit. PCM floating point output seems to solve it.
		if (strstr(driverName, "SigmaTel") && systemVariables.format == FMOD_SOUND_FORMAT_PCM16)
		{
			systemVariables.format = FMOD_SOUND_FORMAT_PCMFLOAT;
		}
	}


	if (!systemVariables.numOutputChannelsSetByUser)
	{
		systemVariables.numOutputChannels = SOUNDSYSTEM_DEFAULT_NUM_OUTPUT_CHANNELS;
	}
	else if (systemVariables.numOutputChannelsSetByUser && systemVariables.speakerModeSetByUser)
	{
		systemVariables.numOutputChannelsSetByUser = false;
		systemVariables.numOutputChannels = SOUNDSYSTEM_DEFAULT_NUM_OUTPUT_CHANNELS;
	}

	if (!systemVariables.maxInputChannelsSetByUser)
		systemVariables.maxInputChannels = SOUNDSYSTEM_DEFAULT_MAX_INPUT_CHANNELS;
	if (!systemVariables.resampleMethodSetByUser)
		systemVariables.resampleMethod = SOUNDSYSTEM_DEFAULT_RESAMPLE_METHOD;


	if (FMOD_FAILED(fresult = soundSystem->setSoftwareFormat(systemVariables.sampleRate,
														systemVariables.format,
														systemVariables.numOutputChannels,
														systemVariables.maxInputChannels,
														systemVariables.resampleMethod)))
	{
		if (systemVariables.sampleRateSetByUser ||
			systemVariables.formatSetByUser	||
			systemVariables.numOutputChannelsSetByUser ||
			systemVariables.maxInputChannelsSetByUser ||
			systemVariables.resampleMethodSetByUser ||
			systemVariables.maxSoftwareChannelsSetByUser)
		{
			if (!FMOD_FAILED(fresult = soundSystem->setSoftwareFormat(	SOUNDSYSTEM_DEFAULT_SAMPLE_RATE,
																		SOUNDSYSTEM_DEFAULT_FORMAT,
																		SOUNDSYSTEM_DEFAULT_NUM_OUTPUT_CHANNELS,
																		SOUNDSYSTEM_DEFAULT_MAX_INPUT_CHANNELS,
																		SOUNDSYSTEM_DEFAULT_RESAMPLE_METHOD)))
			{
				systemVariables.sampleRateSetByUser = false;
				systemVariables.formatSetByUser = false;
				systemVariables.numOutputChannelsSetByUser = false;
				systemVariables.maxInputChannelsSetByUser = false;
				systemVariables.resampleMethodSetByUser = false;
				systemVariables.maxSoftwareChannelsSetByUser = false;

				systemVariables.sampleRate = SOUNDSYSTEM_DEFAULT_SAMPLE_RATE;
				systemVariables.format = SOUNDSYSTEM_DEFAULT_FORMAT;
				systemVariables.numOutputChannels = SOUNDSYSTEM_DEFAULT_NUM_OUTPUT_CHANNELS;
				systemVariables.maxInputChannels = SOUNDSYSTEM_DEFAULT_MAX_INPUT_CHANNELS;
				systemVariables.resampleMethod = SOUNDSYSTEM_DEFAULT_RESAMPLE_METHOD;
			}
		}

		if (FMOD_FAILED(fresult))
		{
			shutdown();
			showError(false, "FMOD::System::setSoftwareFormat error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
			return -15;
		}
	}


	if (!systemVariables.maxSoftwareChannelsSetByUser)
		systemVariables.maxSoftwareChannels = SOUNDSYSTEM_DEFAULT_MAX_SOFTWARE_CHANNELS;

	if (FMOD_FAILED(fresult = soundSystem->setSoftwareChannels(systemVariables.maxSoftwareChannels)))
	{
		if (systemVariables.maxSoftwareChannelsSetByUser)
		{
			if (!FMOD_FAILED(fresult = soundSystem->setSoftwareChannels(SOUNDSYSTEM_DEFAULT_MAX_SOFTWARE_CHANNELS)))
			{
				systemVariables.maxSoftwareChannelsSetByUser = false;
				systemVariables.maxSoftwareChannels = SOUNDSYSTEM_DEFAULT_MAX_SOFTWARE_CHANNELS;
			}
		}

		if (FMOD_FAILED(fresult))
		{
			shutdown();
			showError(false, "FMOD::System::setSoftwareChannels error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
			return -16;
		}
	}


	if (!systemState.initFlagsSetByUser)
	{
		systemState.initFlags = SOUNDSYSTEM_DEFAULT_SYSTEM_INITFLAGS;
	}


	//setup advanced settings
	FMOD_ADVANCEDSETTINGS advancedSettings;
	ZeroMemory(&advancedSettings, sizeof(FMOD_ADVANCEDSETTINGS));
	advancedSettings.cbsize = sizeof(FMOD_ADVANCEDSETTINGS);

	if (systemState.initFlags & FMOD_INIT_HRTF_LOWPASS)
	{
		if (systemVariables.HRTFMinAngleSetByUser)
			advancedSettings.HRTFMinAngle = systemVariables.HRTFMinAngle;
		else
			advancedSettings.HRTFMinAngle = systemVariables.HRTFMinAngle = SOUNDSYSTEM_DEFAULT_HRTF_MINIMUM_ANGLE;

		if (systemVariables.HRTFMaxAngleSetByUser)
			advancedSettings.HRTFMaxAngle = systemVariables.HRTFMaxAngle;
		else
			advancedSettings.HRTFMaxAngle = systemVariables.HRTFMaxAngle = SOUNDSYSTEM_DEFAULT_HRTF_MAXIMUM_ANGLE;

		if (systemVariables.HRTFFrequencySetByUser)
			advancedSettings.HRTFFreq = systemVariables.HRTFFrequency;
		else
			advancedSettings.HRTFFreq = systemVariables.HRTFFrequency = SOUNDSYSTEM_DEFAULT_HRTF_FREQUENCY;
	}


	if (systemState.initFlags & FMOD_INIT_VOL0_BECOMES_VIRTUAL)
	{
		if (systemVariables.vol0VirtualVolumeSetByUser)
			advancedSettings.vol0virtualvol = systemVariables.vol0VirtualVolume;
		else
			advancedSettings.vol0virtualvol = systemVariables.vol0VirtualVolume = SOUNDSYSTEM_DEFAULT_VIRTUAL_VOLUME_0_LEVEL;
	}


	if (systemState.initFlags & FMOD_INIT_DISTANCE_FILTERING)
	{
		if (systemVariables.distanceFilterCenterFrequencySetByUser)
			advancedSettings.distanceFilterCenterFreq = systemVariables.distanceFilterCenterFrequency;
		else
			advancedSettings.distanceFilterCenterFreq = systemVariables.distanceFilterCenterFrequency = SOUNDSYSTEM_DEFAULT_DF_CENTER_FREQUENCY;
	}


	if (FMOD_FAILED(fresult = soundSystem->setAdvancedSettings(&advancedSettings)))
	{
		shutdown();
		showError(false, "FMOD::System::setAdvancedSettings error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
		return -17;
	}


	if (!systemVariables.maxVirtualChannelsSetByUser || systemVariables.maxVirtualChannels < SOUNDSYSTEM_DEFAULT_MIN_VIRTUAL_CHANNELS)
	{
		systemVariables.maxVirtualChannelsSetByUser = false;
		systemVariables.maxVirtualChannels = SOUNDSYSTEM_DEFAULT_MIN_VIRTUAL_CHANNELS;
	}

	if (FMOD_FAILED(fresult = soundSystem->init(systemVariables.maxVirtualChannels, systemState.initFlags, NULL)))
	{
		shutdown();
		showError(false, "FMOD::System::init error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
		return -18;
	}

	systemState.initialized = true;


	if (!system3DSettings.dopplerScaleSetByUser)
		system3DSettings.dopplerScale = SOUNDSYSTEM_DEFAULT_DOPPLER_SCALE;

	if (!system3DSettings.distanceFactorSetByUser)
		system3DSettings.distanceFactor = SOUNDSYSTEM_DEFAULT_DISTANCE_FACTOR;

	if (!system3DSettings.rolloffScaleSetByUser)
		system3DSettings.rolloffScale = SOUNDSYSTEM_DEFAULT_INVERSE_ROLLOFF_SCALE;


	if (FMOD_FAILED(fresult = soundSystem->set3DSettings(system3DSettings.dopplerScale, system3DSettings.distanceFactor, system3DSettings.rolloffScale)))
	{
		shutdown();
		showError(false, "FMOD::System::set3DSettings error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
		return -19;
	}


	if (FMOD_FAILED(fresult = soundSystem->set3DNumListeners(1)))
	{
		shutdown();
		showError(false, "FMOD::System::set3DNumListeners error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
		return -20;
	}


	if (!system3DSettings.positionLinked && system3DSettings.position == NULL)
	{
		system3DSettings.position = new FMOD_VECTOR;
		system3DSettings.position->x = 0.0f;
		system3DSettings.position->y = 0.0f;
		system3DSettings.position->z = 0.0f;
	}

	if (!system3DSettings.velocityLinked && system3DSettings.velocity == NULL)
	{
		system3DSettings.velocity = new FMOD_VECTOR;
		system3DSettings.velocity->x = 0.0f;
		system3DSettings.velocity->y = 0.0f;
		system3DSettings.velocity->z = 0.0f;
	}

	if (!system3DSettings.forwardLinked && system3DSettings.forward == NULL)
	{
		system3DSettings.forward = new FMOD_VECTOR;
		system3DSettings.forward->x = 0.0f;
		system3DSettings.forward->y = 0.0f;
		system3DSettings.forward->z = 1.0f;
	}

	if (!system3DSettings.upLinked && system3DSettings.up == NULL)
	{
		system3DSettings.up = new FMOD_VECTOR;
		system3DSettings.up->x = 0.0f;
		system3DSettings.up->y = 1.0f;
		system3DSettings.up->z = 0.0f;
	}


	if (FMOD_FAILED(fresult = soundSystem->set3DListenerAttributes(0, system3DSettings.position, system3DSettings.velocity, system3DSettings.forward, system3DSettings.up)))
	{
		shutdown();
		showError(false, "FMOD::System::set3DListenerAttributes error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
		return -21;
	}


	if (!system3DSettings.defaultRolloffSetByUser)
		system3DSettings.defaultRolloff = SOUNDSYSTEM_DEFAULT_ROLLOFF;

	if (!system3DSettings.minAudibleDistanceSetByUser)
		system3DSettings.minAudibleDistance = SOUNDSYSTEM_DEFAULT_MIN_AUDIBLE_DISTANCE;

	if (!system3DSettings.maxAudibleDistanceSetByUser)
		system3DSettings.maxAudibleDistance = SOUNDSYSTEM_DEFAULT_MAX_AUDIBLE_DISTANCE;


	if (setupChannelGroups() || setupChannels())
	{
		shutdown();
		return -22;
	}

	return 0;
};


int SoundSystem::shutdown()
{
	Channel::releaseAll();
	musicChannel = NULL;
	sounds2DChannel = NULL;
	Sound::releaseAll();

	for (int i=0;i<channelGroups.size();++i)
		channelGroups[i].first->release();
	channelGroups.resize(0);

	SAFE_RELEASE(masterChannelGroup);
	masterChannelGroupVolume = SOUNDSYSTEM_DEFAULT_CHANNELGROUP_VOLUME;

	if (systemState.initialized)
	{
		soundSystem->close();
		systemState.initialized = false;
	}

	clearSystemVariables();

	dropFMODSystem();

	driverId = 0;
	systemState.caps = 0;
	systemState.version = 0;
	systemState.initFlags = SOUNDSYSTEM_DEFAULT_SYSTEM_INITFLAGS;
	systemState.initFlagsSetByUser = false;

	return 0;
};


int SoundSystem::update()
{
	if (!systemState.initialized)
		return 1;

	FMOD_RESULT fresult = FMOD_OK;

	if (FMOD_FAILED(fresult = soundSystem->set3DListenerAttributes(0, system3DSettings.position, system3DSettings.velocity, system3DSettings.forward, system3DSettings.up)))
	{
		showError(false, "FMOD::System::set3DListenerAttributes error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
		return -1;
	}


	if (FMOD_FAILED(fresult = soundSystem->update()))
	{
		SAFE_RELEASE(soundSystem);
		showError(false, "FMOD::System::update error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
		return -1;
	}

	return 0;
};


int SoundSystem::setHRTFLowpass(bool _set)
{
	if (systemState.initialized)
		return 1;

	if (!systemState.initFlagsSetByUser)
	{
		systemState.initFlags = SOUNDSYSTEM_DEFAULT_SYSTEM_INITFLAGS;
		systemState.initFlagsSetByUser = true;
	}

	if (_set)
		systemState.initFlags = systemState.initFlags | FMOD_INIT_HRTF_LOWPASS;
	else
		systemState.initFlags = (systemState.initFlags | FMOD_INIT_HRTF_LOWPASS) ^ FMOD_INIT_HRTF_LOWPASS;

	return 0;
};

int SoundSystem::setDistanceFiltering(bool _set)
{
	if (systemState.initialized)
		return 1;

	if (!systemState.initFlagsSetByUser)
	{
		systemState.initFlags = SOUNDSYSTEM_DEFAULT_SYSTEM_INITFLAGS;
		systemState.initFlagsSetByUser = true;
	}

	if (_set)
		systemState.initFlags = systemState.initFlags | FMOD_INIT_DISTANCE_FILTERING;
	else
		systemState.initFlags = (systemState.initFlags | FMOD_INIT_DISTANCE_FILTERING) ^ FMOD_INIT_DISTANCE_FILTERING;

	return 0;
};

int SoundSystem::setVol0BecomesVirtual(bool _set)
{
	if (systemState.initialized)
		return 1;

	if (!systemState.initFlagsSetByUser)
	{
		systemState.initFlags = SOUNDSYSTEM_DEFAULT_SYSTEM_INITFLAGS;
		systemState.initFlagsSetByUser = true;
	}

	if (_set)
		systemState.initFlags = systemState.initFlags | FMOD_INIT_VOL0_BECOMES_VIRTUAL;
	else
		systemState.initFlags = (systemState.initFlags | FMOD_INIT_VOL0_BECOMES_VIRTUAL) ^ FMOD_INIT_VOL0_BECOMES_VIRTUAL;

	return 0;
};

int SoundSystem::setSynchroniseMixerWithUpdate(bool _set)
{
	if (systemState.initialized)
		return 1;

	if (!systemState.initFlagsSetByUser)
	{
		systemState.initFlags = SOUNDSYSTEM_DEFAULT_SYSTEM_INITFLAGS;
		systemState.initFlagsSetByUser = true;
	}

	if (_set)
		systemState.initFlags = systemState.initFlags | FMOD_INIT_SYNCMIXERWITHUPDATE;
	else
		systemState.initFlags = (systemState.initFlags | FMOD_INIT_SYNCMIXERWITHUPDATE) ^ FMOD_INIT_SYNCMIXERWITHUPDATE;

	return 0;
};

int SoundSystem::setGeometryUseClosest(bool _set)
{
	if (systemState.initialized)
		return 1;

	if (!systemState.initFlagsSetByUser)
	{
		systemState.initFlags = SOUNDSYSTEM_DEFAULT_SYSTEM_INITFLAGS;
		systemState.initFlagsSetByUser = true;
	}

	if (_set)
		systemState.initFlags = systemState.initFlags | FMOD_INIT_GEOMETRY_USECLOSEST;
	else
		systemState.initFlags = (systemState.initFlags | FMOD_INIT_GEOMETRY_USECLOSEST) ^ FMOD_INIT_GEOMETRY_USECLOSEST;

	return 0;
};


int SoundSystem::setSpeakerMode(FMOD_SPEAKERMODE _speakerMode)
{
	if (systemState.initialized)
		return 1;

	systemVariables.speakerModeSetByUser = true;
	systemVariables.speakerMode = _speakerMode;

	return 0;
};

int SoundSystem::resetSpeakerMode()
{
	if (systemState.initialized)
		return 1;

	systemVariables.speakerModeSetByUser = false;
	systemVariables.speakerMode = SOUNDSYSTEM_DEFAULT_SPEAKER_MODE;

	return 0;
};

FMOD_SPEAKERMODE SoundSystem::getSpeakerMode()
{
	return systemVariables.speakerMode;
};


int SoundSystem::setSampleRate(UINT _sampleRate)
{
	if (systemState.initialized)
		return 1;

	systemVariables.sampleRateSetByUser = true;
	systemVariables.sampleRate = _sampleRate;

	return 0;
};

int SoundSystem::resetSampleRate()
{
	if (systemState.initialized)
		return 1;

	systemVariables.sampleRateSetByUser = false;
	systemVariables.sampleRate = SOUNDSYSTEM_DEFAULT_SAMPLE_RATE;

	return 0;
};

UINT SoundSystem::getSampleRate()
{
	return systemVariables.sampleRate;
};


int SoundSystem::setSoundFormat(FMOD_SOUND_FORMAT _format)
{
	if (systemState.initialized)
		return 1;

	systemVariables.formatSetByUser = true;
	systemVariables.format = _format;

	return 0;
};

int SoundSystem::resetSoundFormat()
{
	if (systemState.initialized)
		return 1;

	systemVariables.formatSetByUser = false;
	systemVariables.format = SOUNDSYSTEM_DEFAULT_FORMAT;

	return 0;
};

FMOD_SOUND_FORMAT SoundSystem::getSoundFormat()
{
	return systemVariables.format;
};


int SoundSystem::setOutputChannelsNumber(UINT _numOutputChannels)
{
	if (systemState.initialized)
		return 1;

	systemVariables.numOutputChannelsSetByUser = true;
	systemVariables.numOutputChannels = _numOutputChannels;

	return 0;
};

int SoundSystem::resetOutputChannelsNumber()
{
	if (systemState.initialized)
		return 1;

	systemVariables.numOutputChannelsSetByUser = false;
	systemVariables.numOutputChannels = SOUNDSYSTEM_DEFAULT_NUM_OUTPUT_CHANNELS;

	return 0;
};

UINT SoundSystem::getOutputChannelsNumber()
{
	return systemVariables.numOutputChannels;
};


int SoundSystem::setMaxInputChannels(UINT _maxInputChannels)
{
	if (systemState.initialized)
		return 1;

	systemVariables.maxInputChannelsSetByUser = true;
	systemVariables.maxInputChannels = _maxInputChannels;

	return 0;
};

int SoundSystem::resetMaxInputChannels()
{
	if (systemState.initialized)
		return 1;

	systemVariables.maxInputChannelsSetByUser = false;
	systemVariables.maxInputChannels = SOUNDSYSTEM_DEFAULT_MAX_INPUT_CHANNELS;

	return 0;
};

UINT SoundSystem::getMaxInputChannels()
{
	return systemVariables.maxInputChannels;
};


int SoundSystem::setResampleMethod(FMOD_DSP_RESAMPLER _resampleMethod)
{
	if (systemState.initialized)
		return 1;

	systemVariables.resampleMethodSetByUser = true;
	systemVariables.resampleMethod = _resampleMethod;

	return 0;
};

int SoundSystem::resetResampleMethod()
{
	if (systemState.initialized)
		return 1;

	systemVariables.resampleMethodSetByUser = false;
	systemVariables.resampleMethod = SOUNDSYSTEM_DEFAULT_RESAMPLE_METHOD;

	return 0;
};

FMOD_DSP_RESAMPLER SoundSystem::getResampleMethod()
{
	return systemVariables.resampleMethod;
};


int SoundSystem::setMaxSoftwareChannels(UINT _maxSoftwareChannels)
{
	if (systemState.initialized)
		return 1;

	systemVariables.maxSoftwareChannelsSetByUser = true;
	systemVariables.maxSoftwareChannels = _maxSoftwareChannels;

	return 0;
};

int SoundSystem::resetMaxSoftwareChannels()
{
	if (systemState.initialized)
		return 1;

	systemVariables.maxSoftwareChannelsSetByUser = false;
	systemVariables.maxSoftwareChannels = SOUNDSYSTEM_DEFAULT_MAX_SOFTWARE_CHANNELS;

	return 0;
};

UINT SoundSystem::getMaxSoftwareChannels()
{
	return systemVariables.maxSoftwareChannels;
};


int SoundSystem::setMaxVirtualChannels(UINT _maxVirtualChannels)
{
	if (systemState.initialized)
		return 1;

	systemVariables.maxVirtualChannelsSetByUser = true;
	systemVariables.maxVirtualChannels = _maxVirtualChannels;

	return 0;
};

int SoundSystem::resetMaxVirtualChannels()
{
	if (systemState.initialized)
		return 1;

	systemVariables.maxVirtualChannelsSetByUser = false;
	systemVariables.maxVirtualChannels = SOUNDSYSTEM_DEFAULT_MIN_VIRTUAL_CHANNELS;

	return 0;
};

UINT SoundSystem::getMaxVirtualChannels()
{
	return systemVariables.maxVirtualChannels;
};


int SoundSystem::setHRTFMinAngle(float _HRTFMinAngle)
{
	if (systemState.initialized)
		return 1;

	systemVariables.HRTFMinAngleSetByUser = true;
	systemVariables.HRTFMinAngle = _HRTFMinAngle;

	return 0;
};

int SoundSystem::resetHRTFMinAngle()
{
	if (systemState.initialized)
		return 1;

	systemVariables.HRTFMinAngleSetByUser = false;
	systemVariables.HRTFMinAngle = SOUNDSYSTEM_DEFAULT_HRTF_MINIMUM_ANGLE;

	return 0;
};

float SoundSystem::getHRTFMinAngle()
{
	return systemVariables.HRTFMinAngle;
};


int SoundSystem::setHRTFMaxAngle(float _HRTFMaxAngle)
{
	if (systemState.initialized)
		return 1;

	systemVariables.HRTFMaxAngleSetByUser = true;
	systemVariables.HRTFMaxAngle = _HRTFMaxAngle;

	return 0;
};

int SoundSystem::resetHRTFMaxAngle()
{
	if (systemState.initialized)
		return 1;

	systemVariables.HRTFMaxAngleSetByUser = false;
	systemVariables.HRTFMaxAngle = SOUNDSYSTEM_DEFAULT_HRTF_MAXIMUM_ANGLE;

	return 0;
};

float SoundSystem::getHRTFMaxAngle()
{
	return systemVariables.HRTFMaxAngle;
};


int SoundSystem::setHRTFFrequency(float _HRTFFrequency)
{
	if (systemState.initialized)
		return 1;

	systemVariables.HRTFFrequencySetByUser = true;
	systemVariables.HRTFFrequency = _HRTFFrequency;

	return 0;
};

int SoundSystem::resetHRTFFrequency()
{
	if (systemState.initialized)
		return 1;

	systemVariables.HRTFFrequencySetByUser = false;
	systemVariables.HRTFFrequency = SOUNDSYSTEM_DEFAULT_HRTF_FREQUENCY;

	return 0;
};

float SoundSystem::getHRTFFrequency()
{
	return systemVariables.HRTFFrequency;
};


int SoundSystem::setVirtualVolumeLevel(float _vol0VirtualVolume)
{
	if (systemState.initialized)
		return 1;

	systemVariables.vol0VirtualVolumeSetByUser = true;
	systemVariables.vol0VirtualVolume = _vol0VirtualVolume;

	return 0;
};

int SoundSystem::resetVirtualVolumeLevel()
{
	if (systemState.initialized)
		return 1;

	systemVariables.vol0VirtualVolumeSetByUser = false;
	systemVariables.vol0VirtualVolume = SOUNDSYSTEM_DEFAULT_VIRTUAL_VOLUME_0_LEVEL;

	return 0;
};

float SoundSystem::getVirtualVolumeLevel()
{
	return systemVariables.vol0VirtualVolume;
};


int SoundSystem::setDistanceFilterCenterFrequency(float _distanceFilterCenterFrequency)
{
	if (systemState.initialized)
		return 1;

	systemVariables.distanceFilterCenterFrequencySetByUser = true;
	systemVariables.distanceFilterCenterFrequency = _distanceFilterCenterFrequency;

	return 0;
};

int SoundSystem::resetDistanceFilterCenterFrequency()
{
	if (systemState.initialized)
		return 1;

	systemVariables.distanceFilterCenterFrequencySetByUser = false;
	systemVariables.distanceFilterCenterFrequency = SOUNDSYSTEM_DEFAULT_DF_CENTER_FREQUENCY;

	return 0;
};

float SoundSystem::getDistanceFilterCenterFrequency()
{
	return systemVariables.distanceFilterCenterFrequency;
};


int SoundSystem::setMinimumAudibleDistance(float _minAudibleDistance)
{
	system3DSettings.minAudibleDistanceSetByUser = true;
	system3DSettings.minAudibleDistance = _minAudibleDistance;

	return 0;
};

int SoundSystem::resetMinimumAudibleDistance()
{
	system3DSettings.minAudibleDistanceSetByUser = false;
	system3DSettings.minAudibleDistance = SOUNDSYSTEM_DEFAULT_MIN_AUDIBLE_DISTANCE;

	return 0;
};

float SoundSystem::getMinimumAudibleDistance()
{
	return system3DSettings.minAudibleDistance;
};


int SoundSystem::setMaximumAudibleDistance(float _maxAudibleDistance)
{
	system3DSettings.maxAudibleDistanceSetByUser = true;
	system3DSettings.maxAudibleDistance = _maxAudibleDistance;

	return 0;
};

int SoundSystem::resetMaximumAudibleDistance()
{
	system3DSettings.maxAudibleDistanceSetByUser = false;
	system3DSettings.maxAudibleDistance = SOUNDSYSTEM_DEFAULT_MAX_AUDIBLE_DISTANCE;

	return 0;
};

float SoundSystem::getMaximumAudibleDistance()
{
	return system3DSettings.maxAudibleDistance;
};


int SoundSystem::setDopplerScale(float _dopplerScale)
{
	if (systemState.initialized)
	{
		FMOD_RESULT fresult = FMOD_OK;
		if (FMOD_FAILED(fresult = soundSystem->set3DSettings(_dopplerScale, system3DSettings.distanceFactor, system3DSettings.rolloffScale)))
		{
			showError(false, "FMOD::System::set3DSettings error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
			return -1;
		}
	}

	system3DSettings.dopplerScaleSetByUser = true;
	system3DSettings.dopplerScale = _dopplerScale;

	return 0;
};

int SoundSystem::resetDopplerScale()
{
	if (systemState.initialized)
	{
		FMOD_RESULT fresult = FMOD_OK;
		if (FMOD_FAILED(fresult = soundSystem->set3DSettings(SOUNDSYSTEM_DEFAULT_DOPPLER_SCALE, system3DSettings.distanceFactor, system3DSettings.rolloffScale)))
		{
			showError(false, "FMOD::System::set3DSettings error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
			return -1;
		}
	}

	system3DSettings.dopplerScaleSetByUser = false;
	system3DSettings.dopplerScale = SOUNDSYSTEM_DEFAULT_DOPPLER_SCALE;

	return 0;
};

float SoundSystem::getDopplerScale()
{
	return system3DSettings.dopplerScale;
};


int SoundSystem::setDistanceFactor(float _distanceFactor)
{
	if (systemState.initialized)
	{
		FMOD_RESULT fresult = FMOD_OK;
		if (FMOD_FAILED(fresult = soundSystem->set3DSettings(system3DSettings.dopplerScale, _distanceFactor, system3DSettings.rolloffScale)))
		{
			showError(false, "FMOD::System::set3DSettings error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
			return -1;
		}
	}

	system3DSettings.distanceFactorSetByUser = true;
	system3DSettings.distanceFactor = _distanceFactor;

	return 0;
};

int SoundSystem::resetDistanceFactor()
{
	if (systemState.initialized)
	{
		FMOD_RESULT fresult = FMOD_OK;
		if (FMOD_FAILED(fresult = soundSystem->set3DSettings(system3DSettings.dopplerScale, SOUNDSYSTEM_DEFAULT_DISTANCE_FACTOR, system3DSettings.rolloffScale)))
		{
			showError(false, "FMOD::System::set3DSettings error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
			return -1;
		}
	}

	system3DSettings.distanceFactorSetByUser = false;
	system3DSettings.distanceFactor = SOUNDSYSTEM_DEFAULT_DISTANCE_FACTOR;

	return 0;
};

float SoundSystem::getDistanceFactor()
{
	return system3DSettings.distanceFactor;
};


int SoundSystem::setRolloffScale(float _rolloffScale)
{
	if (systemState.initialized)
	{
		FMOD_RESULT fresult = FMOD_OK;
		if (FMOD_FAILED(fresult = soundSystem->set3DSettings(system3DSettings.dopplerScale, system3DSettings.distanceFactor, _rolloffScale)))
		{
			showError(false, "FMOD::System::set3DSettings error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
			return -1;
		}
	}

	system3DSettings.rolloffScaleSetByUser = true;
	system3DSettings.rolloffScale = _rolloffScale;

	return 0;
};

int SoundSystem::resetRolloffScale()
{
	if (systemState.initialized)
	{
		FMOD_RESULT fresult = FMOD_OK;
		if (FMOD_FAILED(fresult = soundSystem->set3DSettings(system3DSettings.dopplerScale, system3DSettings.distanceFactor, SOUNDSYSTEM_DEFAULT_INVERSE_ROLLOFF_SCALE)))
		{
			showError(false, "FMOD::System::set3DSettings error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
			return -1;
		}
	}

	system3DSettings.rolloffScaleSetByUser = false;
	system3DSettings.rolloffScale = SOUNDSYSTEM_DEFAULT_INVERSE_ROLLOFF_SCALE;

	return 0;
};

float SoundSystem::getRolloffScale()
{
	return system3DSettings.rolloffScale;
};



int SoundSystem::set3DPosition(FMOD_VECTOR* _position)
{
	FMOD_RESULT fresult = FMOD_OK;
	if (_position)
	{
		if (FMOD_FAILED(fresult = soundSystem->set3DListenerAttributes(0, _position, NULL, NULL, NULL)))
		{
			showError(false, "FMOD::System::set3DListenerAttributes error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
			return -1;
		}

		if (!system3DSettings.positionLinked)
			delete system3DSettings.position;

		system3DSettings.positionLinked = true;
		system3DSettings.position = _position;
	}
	else
	{
		FMOD_VECTOR* position = new FMOD_VECTOR;
		ZeroMemory(position, sizeof(FMOD_VECTOR));
		if (FMOD_FAILED(fresult = soundSystem->set3DListenerAttributes(0, position, NULL, NULL, NULL)))
		{
			delete position;
			showError(false, "FMOD::System::set3DListenerAttributes error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
			return -1;
		}

		if (!system3DSettings.positionLinked)
			delete system3DSettings.position;

		system3DSettings.positionLinked = false;
		system3DSettings.position = position;
	}

	return 0;
};

int SoundSystem::set3DVelocity(FMOD_VECTOR* _velocity)
{
	FMOD_RESULT fresult = FMOD_OK;
	if (_velocity)
	{
		if (FMOD_FAILED(fresult = soundSystem->set3DListenerAttributes(0, NULL, _velocity, NULL, NULL)))
		{
			showError(false, "FMOD::System::set3DListenerAttributes error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
			return -1;
		}

		if (!system3DSettings.velocityLinked)
			delete system3DSettings.velocity;

		system3DSettings.velocityLinked = true;
		system3DSettings.velocity = _velocity;
	}
	else
	{
		FMOD_VECTOR* velocity = new FMOD_VECTOR;
		ZeroMemory(velocity, sizeof(FMOD_VECTOR));
		if (FMOD_FAILED(fresult = soundSystem->set3DListenerAttributes(0, NULL, velocity, NULL, NULL)))
		{
			delete velocity;
			showError(false, "FMOD::System::set3DListenerAttributes error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
			return -1;
		}

		if (!system3DSettings.velocityLinked)
			delete system3DSettings.velocity;

		system3DSettings.velocityLinked = false;
		system3DSettings.velocity = velocity;
	}

	return 0;
};

int SoundSystem::set3DForward(FMOD_VECTOR* _forward)
{
	FMOD_RESULT fresult = FMOD_OK;
	if (_forward)
	{
		if (FMOD_FAILED(fresult = soundSystem->set3DListenerAttributes(0, NULL, NULL, _forward, NULL)))
		{
			showError(false, "FMOD::System::set3DListenerAttributes error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
			return -1;
		}

		if (!system3DSettings.forwardLinked)
			delete system3DSettings.forward;

		system3DSettings.forwardLinked = true;
		system3DSettings.forward = _forward;
	}
	else
	{
		FMOD_VECTOR* forward = new FMOD_VECTOR;
		ZeroMemory(forward, sizeof(FMOD_VECTOR));
		forward->z = 1.0f;
		if (FMOD_FAILED(fresult = soundSystem->set3DListenerAttributes(0, NULL, NULL, forward, NULL)))
		{
			delete forward;
			showError(false, "FMOD::System::set3DListenerAttributes error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
			return -1;
		}

		if (!system3DSettings.forwardLinked)
			delete system3DSettings.forward;

		system3DSettings.forwardLinked = false;
		system3DSettings.forward = forward;
	}

	return 0;
};

int SoundSystem::set3DUp(FMOD_VECTOR* _up)
{
	FMOD_RESULT fresult = FMOD_OK;
	if (_up)
	{
		if (FMOD_FAILED(fresult = soundSystem->set3DListenerAttributes(0, NULL, NULL, NULL, _up)))
		{
			showError(false, "FMOD::System::set3DListenerAttributes error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
			return -1;
		}

		if (!system3DSettings.upLinked)
			delete system3DSettings.up;

		system3DSettings.upLinked = true;
		system3DSettings.up = _up;
	}
	else
	{
		FMOD_VECTOR* up = new FMOD_VECTOR;
		ZeroMemory(up, sizeof(FMOD_VECTOR));
		up->y = 1.0f;
		if (FMOD_FAILED(fresult = soundSystem->set3DListenerAttributes(0, NULL, NULL, NULL, up)))
		{
			delete up;
			showError(false, "FMOD::System::set3DListenerAttributes error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
			return -1;
		}

		if (!system3DSettings.upLinked)
			delete system3DSettings.up;

		system3DSettings.upLinked = false;
		system3DSettings.up = up;
	}

	return 0;
};


int SoundSystem::set3DAttributes(FMOD_VECTOR* _position, FMOD_VECTOR* _velocity, FMOD_VECTOR* _forward, FMOD_VECTOR* _up)
{
	int res = 0;
	if (res = set3DPosition(_position))
		return res;
	if (res = set3DVelocity(_velocity))
		return res + 10;
	if (res = set3DForward(_forward))
		return res + 20;
	if (res = set3DUp(_up))
		return res + 30;

	return 0;
};



FMOD_VECTOR* SoundSystem::get3DPositionPointer()
{
	return system3DSettings.position;
};

bool SoundSystem::is3DPositionLinked()
{
	return system3DSettings.positionLinked;
};

FMOD_VECTOR* SoundSystem::get3DVelocityPointer()
{
	return system3DSettings.velocity;
};

bool SoundSystem::get3DVelocityLinked()
{
	return system3DSettings.velocityLinked;
};

FMOD_VECTOR* SoundSystem::get3DForwardPointer()
{
	return system3DSettings.forward;
};

bool SoundSystem::get3DForwardLinked()
{
	return system3DSettings.forwardLinked;
};

FMOD_VECTOR* SoundSystem::get3DUpPointer()
{
	return system3DSettings.up;
};

bool SoundSystem::get3DUpLinked()
{
	return system3DSettings.upLinked;
};


int SoundSystem::setDefaultRolloff(FMOD_MODE _defaultRolloff)
{
	system3DSettings.defaultRolloff = _defaultRolloff;
	system3DSettings.defaultRolloffSetByUser = true;
	return 0;
};

int SoundSystem::resetDefaultRolloff()
{
	system3DSettings.defaultRolloff = SOUNDSYSTEM_DEFAULT_ROLLOFF;
	system3DSettings.defaultRolloffSetByUser = false;
	return 0;
};

FMOD_MODE SoundSystem::getDefaultRolloff()
{
	return system3DSettings.defaultRolloff;
};



int SoundSystem::getChannelGroupsNumber()
{
	return channelGroups.size();
};


int SoundSystem::addChannelGroup(short* _groupIndex, string _groupName, float _volume, FMOD_MODE _rolloff, bool _worldRelative, bool _ignoreGeometry, float _minAudibleDistance, float _maxAudibleDistance)
{
	if (channelGroups.size() >= MAX_CHANNELGROUP_NUMBER)
		return 1;

	FMOD_MODE rolloff = (FMOD_3D_INVERSEROLLOFF | FMOD_3D_LINEARROLLOFF | FMOD_3D_LINEARSQUAREROLLOFF) & _rolloff;
	if (!rolloff)
		rolloff = FMOD_3D_INVERSEROLLOFF;
	FMOD_MODE relative = _worldRelative ? FMOD_3D_WORLDRELATIVE : FMOD_3D_HEADRELATIVE;
	FMOD_MODE igngeo = _ignoreGeometry ? FMOD_3D_IGNOREGEOMETRY : FMOD_DEFAULT;

	FMOD_MODE mode = FMOD_LOOP_OFF | FMOD_3D | rolloff | relative | igngeo;


	ChannelGroup chnGroup;
	chnGroup.first = NULL;
	chnGroup.second.name = _groupName;
	chnGroup.second.channelsMode = mode;
	chnGroup.second.volume = _volume;

	if (_minAudibleDistance < 0.0f)
		_minAudibleDistance = system3DSettings.minAudibleDistance;
	if (_maxAudibleDistance < 0.0f)
		_maxAudibleDistance = system3DSettings.maxAudibleDistance;

	if (_minAudibleDistance > _maxAudibleDistance)
		_minAudibleDistance = _maxAudibleDistance;

	chnGroup.second.minAudibleDistance = _minAudibleDistance;
	chnGroup.second.maxAudibleDistance = _maxAudibleDistance;


	if (systemState.initialized)
	{
		FMOD_RESULT fresult = FMOD_OK;
		if (FMOD_FAILED(fresult = soundSystem->createChannelGroup(chnGroup.second.name.c_str(), &(chnGroup.first))))
		{
			showError(false, "FMOD::System::createChannelGroup error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
			return -1;
		}

		if (FMOD_FAILED(fresult = chnGroup.first->setVolume(chnGroup.second.volume)))
		{
			showError(false, "FMOD::ChannelGroup::setVolume error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
			return -2;
		}

		if (FMOD_FAILED(fresult = masterChannelGroup->addGroup(chnGroup.first)))
		{
			showError(false, "FMOD::ChannelGroup::addGroup error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
			return -3;
		}
	}

	channelGroups.push_back(chnGroup);

	if (_groupIndex)
		*_groupIndex = channelGroups.size() - 1;

	return 0;
};


int SoundSystem::getChannelGroupIndexFromName(short* _index, const string &_name)
{
	if (!_index)
		return 1;

	for (UINT i=0;i<channelGroups.size();++i)
	{
		if (channelGroups[i].second.name == _name)
		{
			*_index = i;
			return 0;
		}
	}

	return 2;
};

int SoundSystem::getChannelGroupNameFromIndex(string &_name, short _index)
{
	int cgs = channelGroups.size();
	if (_index < cgs)
	{
		_name = channelGroups[_index].second.name;
		return 0;
	}

	return 2;
};

int SoundSystem::setChannelGroupVolume(short _index, float _volume)
{
	int cgs = channelGroups.size();
	if (_index < -1 || _index >= cgs)
		return 1;

	FMOD::ChannelGroup* group = NULL;
	if (_index == -1)
	{
		masterChannelGroupVolume = _volume;
		group = masterChannelGroup;
	}
	else
	{
		channelGroups[_index].second.volume = _volume;
		group = channelGroups[_index].first;
	}

	if (systemState.initialized)
	{
		FMOD_RESULT fresult = FMOD_OK;
		if (FMOD_FAILED(fresult = group->setVolume(_volume)))
		{
			showError(false, "FMOD::ChannelGroup::setVolume error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
			return -1;
		}
	}

	return 0;
};

int SoundSystem::getChannelGroupVolume(short _index, float* _volume)
{
	int cgs = channelGroups.size();
	if (_index < SOUNDSYSTEM_MASTER_CHANNELGROUP_ID || _index >= cgs)
		return 1;

	if (_index == SOUNDSYSTEM_MASTER_CHANNELGROUP_ID)
		*_volume = masterChannelGroupVolume;
	else
		*_volume = channelGroups[_index].second.volume;

	return 0;
};


int SoundSystem::get3DChannel(short _channelGroupIndex, FMOD_VECTOR* _position, FMOD_VECTOR* _velocity, Channel** _channel)
{
	if (!systemState.initialized)
		return 1;

	if (!_channel)
		return 2;

	int cgs = channelGroups.size();
	if (_channelGroupIndex < SOUNDSYSTEM_MASTER_CHANNELGROUP_ID || _channelGroupIndex >= cgs)
		return 3;

	int index = Channel::getFreeChannelIndex();
	if (index < 0)
		return 4;


	FMOD::Channel* chn = NULL;
	FMOD_RESULT fresult = FMOD_OK;
	if (FMOD_FAILED(fresult = soundSystem->getChannel(index, &chn)))
	{
		showError(false, "FMOD::System::getChannel error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
		return -1;
	}

	FMOD::ChannelGroup* group = NULL;
	FMOD_MODE mode = 0;
	float minAD = 0;
	float maxAD = 0;

	if (_channelGroupIndex == SOUNDSYSTEM_MASTER_CHANNELGROUP_ID)
	{
		group = masterChannelGroup;
		mode = SOUNDSYSTEM_DEFAULT_3D_CHANNEL_MODE | system3DSettings.defaultRolloff;
		minAD = system3DSettings.minAudibleDistance;
		maxAD = system3DSettings.maxAudibleDistance;
	}
	else
	{
		group = channelGroups[_channelGroupIndex].first;
		mode = channelGroups[_channelGroupIndex].second.channelsMode;
		minAD = channelGroups[_channelGroupIndex].second.minAudibleDistance;
		maxAD = channelGroups[_channelGroupIndex].second.maxAudibleDistance;
	}

	*_channel = new Channel;
	(*_channel)->setupChannel(chn, group, _channelGroupIndex, mode, _position, _velocity);
	(*_channel)->setCustomMinMaxAudibleDistance(minAD, maxAD);

	return 0;
};


int SoundSystem::get2DChannel(Channel** _channel)
{
	if (!systemState.initialized)
		return 1;

	if (!_channel)
		return 2;

	int index = Channel::getFreeChannelIndex();
	if (index < 0)
		return 4;


	FMOD::Channel* chn = NULL;
	FMOD_RESULT fresult = FMOD_OK;
	if (FMOD_FAILED(fresult = soundSystem->getChannel(index, &chn)))
	{
		showError(false, "FMOD::System::getChannel error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
		return -1;
	}


	FMOD_MODE mode = SOUNDSYSTEM_DEFAULT_2D_CHANNEL_MODE;
	FMOD::ChannelGroup* group = masterChannelGroup;

	*_channel = new Channel;
	(*_channel)->setupChannel(chn, masterChannelGroup, SOUNDSYSTEM_MASTER_CHANNELGROUP_ID, SOUNDSYSTEM_DEFAULT_2D_CHANNEL_MODE);

	return 0;
};


Channel* SoundSystem::getMusicChannel()
{
	return musicChannel;
};


Channel* SoundSystem::get2DEffectsChannel()
{
	return sounds2DChannel;
};


int SoundSystem::readInSoundFromMemory(void* _fileInMemory, UINT _fileSize, bool _streamed, bool _is3D, FMOD::Sound** _fmodSound)
{
	if (!systemState.initialized)
		return 1;

	FMOD::Sound* fmodSound = NULL;
	FMOD_MODE mode = (_is3D ? FMOD_3D : ( FMOD_SOFTWARE | FMOD_2D )) | FMOD_OPENMEMORY;

	FMOD_RESULT fresult = FMOD_OK;


	FMOD_CREATESOUNDEXINFO exinfo;
	ZeroMemory(&exinfo, sizeof(FMOD_CREATESOUNDEXINFO));
	exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
	exinfo.length = _fileSize;

	if (_streamed)
	{
		if (FMOD_FAILED(fresult = soundSystem->createStream(reinterpret_cast <char*> (_fileInMemory), mode, &exinfo, _fmodSound)))
		{
			showError(false, "FMOD::System::createStream error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
			return -1;
		}
	}
	else
	{
		if (FMOD_FAILED(fresult = soundSystem->createSound(reinterpret_cast <char*> (_fileInMemory), mode, &exinfo, _fmodSound)))
		{
			showError(false, "FMOD::System::createSound error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
			return -1;
		}
	}

	return 0;
};

int SoundSystem::readInSoundFromFile(string const & _filePath, bool _streamed, wstring _soundText, bool _is3D, Sound* _sound)
{
	if (!systemState.initialized)
		return 1;

	FMOD::Sound* fmodSound = NULL;
	FMOD_RESULT fresult = FMOD_OK;

	FMOD_MODE mode = FMOD_SOFTWARE | (_is3D ? FMOD_3D : FMOD_2D);

	FMOD_CREATESOUNDEXINFO exinfo;
	ZeroMemory(&exinfo, sizeof(FMOD_CREATESOUNDEXINFO));
	exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);

	if (_streamed)
	{
		if (FMOD_FAILED(fresult = soundSystem->createStream(_filePath.c_str(), mode, &exinfo, &fmodSound)))
		{
			showError(false, "FMOD::System::createStream error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
			return -1;
		}
	}
	else
	{
		if (FMOD_FAILED(fresult = soundSystem->createSound(_filePath.c_str(), mode, &exinfo, &fmodSound)))
		{
			showError(false, "FMOD::System::createSound error: (%d) %s\n", fresult, FMOD_ErrorString(fresult));
			return -1;
		}
	}

	return _sound->setResource(fmodSound, _streamed, _soundText);
};


int SoundSystem::playSound(Channel* _channel, Sound* _sound, bool _paused)
{
	if (!_channel || !_sound)
		return 1;

	return _channel->playSound(_sound, _paused);
};


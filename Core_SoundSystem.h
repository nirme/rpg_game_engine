#pragma once

#ifndef _CORE_SOUNDSYSTEM
#define _CORE_SOUNDSYSTEM



#include "utility.h"
#include "Core_Logger.h"
#include "Core_Variable_Library.h"
#include "Core_Defines.h"

#include "Core_Sound.h"
#include "Core_Channel.h"







class SoundSystem
{
public:
	struct SoundSystemState
	{
		bool created;
		bool initialized;
		UINT version;
		FMOD_CAPS caps;
		bool initFlagsSetByUser;
		FMOD_INITFLAGS initFlags;

		//Default ctor
		SoundSystemState() : created(false), initialized(false), version(0), caps(0), initFlagsSetByUser(false), initFlags(SOUNDSYSTEM_DEFAULT_SYSTEM_INITFLAGS)
		{};
	};

	struct SoundSystemVariables
	{
	//System::setSpeakerMode
		//default SOUNDSYSTEM_DEFAULT_SPEAKER_MODE =  FMOD_SPEAKERMODE_STEREO
		bool speakerModeSetByUser;
		FMOD_SPEAKERMODE speakerMode;

	//System::setSoftwareFormat
		//default SOUNDSYSTEM_DEFAULT_SAMPLE_RATE = 48000
		bool sampleRateSetByUser;
		UINT sampleRate;

		//default SOUNDSYSTEM_DEFAULT_FORMAT = FMOD_SOUND_FORMAT_PCM16
		bool formatSetByUser;
		FMOD_SOUND_FORMAT format;

		//not used when speaker mode is set
		//default SOUNDSYSTEM_DEFAULT_NUM_OUTPUT_CHANNELS = 0 (2)
		bool numOutputChannelsSetByUser;
		UINT numOutputChannels;

		//Set maximum channels of used sounds. It should be set to the highest number of channels loaded sounds have. 
		//More input channels takes more CPU time.
		//default SOUNDSYSTEM_DEFAULT_MAX_INPUT_CHANNELS = 2
		bool maxInputChannelsSetByUser;
		UINT maxInputChannels;

		//default SOUNDSYSTEM_DEFAULT_RESAMPLE_METHOD = FMOD_DSP_RESAMPLER_LINEAR
		bool resampleMethodSetByUser;
		FMOD_DSP_RESAMPLER resampleMethod;

	//System::setSoftwareChannels
		//default SOUNDSYSTEM_DEFAULT_MAX_SOFTWARE_CHANNELS = 32
		bool maxSoftwareChannelsSetByUser;
		UINT maxSoftwareChannels;

	//System::init
		//default SOUNDSYSTEM_DEFAULT_MIN_VIRTUAL_CHANNELS = 32
		bool maxVirtualChannelsSetByUser;
		UINT maxVirtualChannels;

	//Advanced settings
		//For use with FMOD_INIT_HRTF_LOWPASS. The angle range (0-360) of a 3D sound in relation to the listener, 
		//at which the HRTF function begins to have an effect. 0 = in front of the listener. 180 = from 90 degrees 
		//to the left of the listener to 90 degrees to the right. 360 = behind the listener.
		//default SOUNDSYSTEM_DEFAULT_HRTF_MINIMUM_ANGLE = 180.0f
		bool HRTFMinAngleSetByUser;
		float HRTFMinAngle;

		//For use with FMOD_INIT_HRTF_LOWPASS. The angle range (0-360) of a 3D sound in relation to the listener, 
		//at which the HRTF function has maximum effect. 0 = front of the listener. 180 = from 90 degrees 
		//to the left of the listener to 90 degrees to the right. 360 = behind the listener.
		//default SOUNDSYSTEM_DEFAULT_HRTF_MAXIMUM_ANGLE = 360.0f
		bool HRTFMaxAngleSetByUser;
		float HRTFMaxAngle;

		//For use with FMOD_INIT_HRTF_LOWPASS. The cutoff frequency of the HRTF's lowpass filter function 
		//when at maximum effect. (i.e. at HRTFMaxAngle).
		//default SOUNDSYSTEM_DEFAULT_HRTF_FREQUENCY = 4000.0f
		bool HRTFFrequencySetByUser;
		float HRTFFrequency;

		//For use with FMOD_INIT_VOL0_BECOMES_VIRTUAL. If this flag is used, and the volume is 0.0, then the sound 
		//will become virtual. Use this value to raise the threshold to a different point where a sound goes virtual.
		//default SOUNDSYSTEM_DEFAULT_VIRTUAL_VOLUME_0_LEVEL = 0.0f
		bool vol0VirtualVolumeSetByUser;
		float vol0VirtualVolume;

		//For use with FMOD_INIT_DISTANCE_FILTERING. The default center frequency in Hz for the distance filtering effect.
		//Default SOUNDSYSTEM_DEFAULT_DF_CENTER_FREQUENCY = 1500.0f
		bool distanceFilterCenterFrequencySetByUser;
		float distanceFilterCenterFrequency;

		//Default ctor
		SoundSystemVariables() :	speakerModeSetByUser(false),					speakerMode(SOUNDSYSTEM_DEFAULT_SPEAKER_MODE), 
									sampleRateSetByUser(false),						sampleRate(SOUNDSYSTEM_DEFAULT_SAMPLE_RATE), 
									formatSetByUser(false),							format(SOUNDSYSTEM_DEFAULT_FORMAT), 
									numOutputChannelsSetByUser(false),				numOutputChannels(SOUNDSYSTEM_DEFAULT_NUM_OUTPUT_CHANNELS), 
									maxInputChannelsSetByUser(false),				maxInputChannels(SOUNDSYSTEM_DEFAULT_MAX_INPUT_CHANNELS), 
									resampleMethodSetByUser(false),					resampleMethod(SOUNDSYSTEM_DEFAULT_RESAMPLE_METHOD), 
									maxSoftwareChannelsSetByUser(false),			maxSoftwareChannels(SOUNDSYSTEM_DEFAULT_MAX_SOFTWARE_CHANNELS), 
									maxVirtualChannelsSetByUser(false),				maxVirtualChannels(SOUNDSYSTEM_DEFAULT_MIN_VIRTUAL_CHANNELS), 
									HRTFMinAngleSetByUser(false),					HRTFMinAngle(SOUNDSYSTEM_DEFAULT_HRTF_MINIMUM_ANGLE), 
									HRTFMaxAngleSetByUser(false),					HRTFMaxAngle(SOUNDSYSTEM_DEFAULT_HRTF_MAXIMUM_ANGLE), 
									HRTFFrequencySetByUser(false),					HRTFFrequency(SOUNDSYSTEM_DEFAULT_HRTF_FREQUENCY), 
									vol0VirtualVolumeSetByUser(false),				vol0VirtualVolume(SOUNDSYSTEM_DEFAULT_VIRTUAL_VOLUME_0_LEVEL), 
									distanceFilterCenterFrequencySetByUser(false),	distanceFilterCenterFrequency(SOUNDSYSTEM_DEFAULT_DF_CENTER_FREQUENCY)
		{};
	};

	struct SoundSystem3DSettings
	{
		//default SOUNDSYSTEM_DEFAULT_MIN_AUDIBLE_DISTANCE = 1.0f
		bool minAudibleDistanceSetByUser;
		float minAudibleDistance;

		//default SOUNDSYSTEM_DEFAULT_MAX_AUDIBLE_DISTANCE = 10000.0f
		bool maxAudibleDistanceSetByUser;
		float maxAudibleDistance;

	//set3DSettings
		//scaling factor for doppler shift.
		//default SOUNDSYSTEM_DEFAULT_DOPPLER_SCALE = 1.0f
		bool dopplerScaleSetByUser;
		float dopplerScale;

		//relative distance factor to FMOD's units.
		//default SOUNDSYSTEM_DEFAULT_DISTANCE_FACTOR = 1.0f (1.0f = 1 metre). 
		bool distanceFactorSetByUser;
		float distanceFactor;

		//scaling factor for 3D sound rolloff or attenuation for FMOD_3D_INVERSEROLLOFF based sounds only (which is the default type).
		//default SOUNDSYSTEM_DEFAULT_INVERSE_ROLLOFF_SCALE = 1.0f
		bool rolloffScaleSetByUser;
		float rolloffScale;

		//set default 3d channels rolloff from linear to square
		//default SOUNDSYSTEM_DEFAULT_ROLLOFF = FMOD_3D_LINEARROLLOFF
		bool defaultRolloffSetByUser;
		FMOD_MODE defaultRolloff;

	//set3DListenerAttributes
		//the position of the listener in world space, measured in distance units.
		//default (0.0f, 0.0f, 0.0f)
		bool positionLinked;
		FMOD_VECTOR* position;

		//the velocity of the listener measured in distance units per second.
		//default (0.0f, 0.0f, 0.0f)
		bool velocityLinked;
		FMOD_VECTOR* velocity;

		//the forwards orientation of the listener. This vector must be of unit length and perpendicular to the up vector.
		//default (0.0f, 0.0f, 1.0f)
		bool forwardLinked;
		FMOD_VECTOR* forward;

		//the upwards orientation of the listener. This vector must be of unit length and perpendicular to the forwards vector.
		//default (0.0f, 1.0f, 0.0f)
		bool upLinked;
		FMOD_VECTOR* up;

		//Default ctor
		SoundSystem3DSettings() :	minAudibleDistanceSetByUser(false),	minAudibleDistance(SOUNDSYSTEM_DEFAULT_MIN_AUDIBLE_DISTANCE), 
									maxAudibleDistanceSetByUser(false),	maxAudibleDistance(SOUNDSYSTEM_DEFAULT_MAX_AUDIBLE_DISTANCE), 
									dopplerScaleSetByUser(false),		dopplerScale(SOUNDSYSTEM_DEFAULT_DOPPLER_SCALE), 
									distanceFactorSetByUser(false),		distanceFactor(SOUNDSYSTEM_DEFAULT_DISTANCE_FACTOR), 
									rolloffScaleSetByUser(false),		rolloffScale(SOUNDSYSTEM_DEFAULT_INVERSE_ROLLOFF_SCALE), 
									defaultRolloffSetByUser(false),		defaultRolloff(SOUNDSYSTEM_DEFAULT_ROLLOFF), 
									positionLinked(true),				position(NULL), 
									velocityLinked(true),				velocity(NULL), 
									forwardLinked(true),				forward(NULL), 
									upLinked(true),						up(NULL)
		{};
	};

	struct ChannelGroupSettings
	{
		string name;
		float volume;
		FMOD_MODE channelsMode;
		float minAudibleDistance;
		float maxAudibleDistance;

		ChannelGroupSettings() : volume(1.0f), channelsMode(0), minAudibleDistance(0.0f), maxAudibleDistance(0.0f)	{};
		ChannelGroupSettings(string &_name, float &_volume, FMOD_MODE &_channelsMode, float &_minAudibleDistance, float &_maxAudibleDistance) : 
			volume(_volume), channelsMode(_channelsMode), minAudibleDistance(_minAudibleDistance), maxAudibleDistance(_maxAudibleDistance)
		{
			name = _name;
		};
	};

	typedef pair<FMOD::ChannelGroup*, ChannelGroupSettings>		ChannelGroup;

protected:
	static FMOD::System* soundSystem;
	static UINT driverId;
	static SoundSystemState systemState;
	static SoundSystemVariables systemVariables;
	static SoundSystem3DSettings system3DSettings;

	static FMOD::ChannelGroup* masterChannelGroup;
	static float masterChannelGroupVolume;

	static vector<ChannelGroup> channelGroups;

	static Channel* musicChannel;
	static Channel* sounds2DChannel;

	static int setupFMODSystem();
	static int dropFMODSystem();

	static int clearSystemVariables();

	static int setupMasterChannelGroup();
	static int setupChannelGroups();

	static int setupChannels();


public:

	static int getDeviceNumbers();
	static int getDeviceInfo(	UINT _indexIn,
								string* _nameStringOut,
								wstring* _nameWstringOut = NULL,
								FMOD_CAPS* _deviceCapsOut = NULL,
								int* _outputRateOut = NULL,
								FMOD_SPEAKERMODE* _speakerModeOut = NULL);

	static int initialize();
	static int shutdown();
	static int update();

	static int setHRTFLowpass(bool _set);
	static int setDistanceFiltering(bool _set);
	static int setVol0BecomesVirtual(bool _set);
	static int setSynchroniseMixerWithUpdate(bool _set);
	static int setGeometryUseClosest(bool _set);


	static int setSpeakerMode(FMOD_SPEAKERMODE _speakerMode);
	static int resetSpeakerMode();
	static FMOD_SPEAKERMODE getSpeakerMode();

	static int setSampleRate(UINT _sampleRate);
	static int resetSampleRate();
	static UINT getSampleRate();

	static int setSoundFormat(FMOD_SOUND_FORMAT _format);
	static int resetSoundFormat();
	static FMOD_SOUND_FORMAT getSoundFormat();

	static int setOutputChannelsNumber(UINT _numOutputChannels);
	static int resetOutputChannelsNumber();
	static UINT getOutputChannelsNumber();

	static int setMaxInputChannels(UINT _maxInputChannels);
	static int resetMaxInputChannels();
	static UINT getMaxInputChannels();

	static int setResampleMethod(FMOD_DSP_RESAMPLER _resampleMethod);
	static int resetResampleMethod();
	static FMOD_DSP_RESAMPLER getResampleMethod();

	static int setMaxSoftwareChannels(UINT _maxSoftwareChannels);
	static int resetMaxSoftwareChannels();
	static UINT getMaxSoftwareChannels();

	static int setMaxVirtualChannels(UINT _maxVirtualChannels);
	static int resetMaxVirtualChannels();
	static UINT getMaxVirtualChannels();

	static int setHRTFMinAngle(float _HRTFMinAngle);
	static int resetHRTFMinAngle();
	static float getHRTFMinAngle();

	static int setHRTFMaxAngle(float _HRTFMaxAngle);
	static int resetHRTFMaxAngle();
	static float getHRTFMaxAngle();

	static int setHRTFFrequency(float _HRTFFrequency);
	static int resetHRTFFrequency();
	static float getHRTFFrequency();

	static int setVirtualVolumeLevel(float _vol0VirtualVolume);
	static int resetVirtualVolumeLevel();
	static float getVirtualVolumeLevel();

	static int setDistanceFilterCenterFrequency(float _distanceFilterCenterFrequency);
	static int resetDistanceFilterCenterFrequency();
	static float getDistanceFilterCenterFrequency();

	static int setMinimumAudibleDistance(float _minAudibleDistance);
	static int resetMinimumAudibleDistance();
	static float getMinimumAudibleDistance();

	static int setMaximumAudibleDistance(float _maxAudibleDistance);
	static int resetMaximumAudibleDistance();
	static float getMaximumAudibleDistance();

	static int setDopplerScale(float _dopplerScale);
	static int resetDopplerScale();
	static float getDopplerScale();

	static int setDistanceFactor(float _distanceFactor);
	static int resetDistanceFactor();
	static float getDistanceFactor();

	static int setRolloffScale(float _rolloffScale);
	static int resetRolloffScale();
	static float getRolloffScale();


	static int set3DPosition(FMOD_VECTOR* _position);
	static int set3DVelocity(FMOD_VECTOR* _velocity);
	static int set3DForward(FMOD_VECTOR* _forward);
	static int set3DUp(FMOD_VECTOR* _up);

	static int set3DAttributes(FMOD_VECTOR* _position, FMOD_VECTOR* _velocity, FMOD_VECTOR* _forward, FMOD_VECTOR* _up);

	static FMOD_VECTOR* get3DPositionPointer();
	static bool is3DPositionLinked();
	static FMOD_VECTOR* get3DVelocityPointer();
	static bool get3DVelocityLinked();
	static FMOD_VECTOR* get3DForwardPointer();
	static bool get3DForwardLinked();
	static FMOD_VECTOR* get3DUpPointer();
	static bool get3DUpLinked();

	static int setDefaultRolloff(FMOD_MODE _defaultRolloff);
	static int resetDefaultRolloff();
	static FMOD_MODE getDefaultRolloff();

	static int getChannelGroupsNumber();
	static int addChannelGroup(	short* _groupIndex,
								string _groupName,
								float _volume = SOUNDSYSTEM_DEFAULT_CHANNELGROUP_VOLUME,
								FMOD_MODE _rolloff = FMOD_3D_INVERSEROLLOFF,
								bool _worldRelative = true,
								bool _ignoreGeometry = false,
								float _minAudibleDistance = -1.0f,
								float _maxAudibleDistance = -1.0f);
	static int getChannelGroupIndexFromName(short* _index, const string &_name);
	static int getChannelGroupNameFromIndex(string &_name, short _index);
	static int setChannelGroupVolume(short _index, float _volume);
	static int getChannelGroupVolume(short _index, float* _volume);


	static int get3DChannel(short _channelGroupIndex, FMOD_VECTOR* _position, FMOD_VECTOR* _velocity, Channel** _channel);
	static int get2DChannel(Channel** _channel);


	static Channel* getMusicChannel();
	static Channel* get2DEffectsChannel();

	static int readInSoundFromMemory(void* _fileInMemory, UINT _fileSize, bool _streamed, bool _is3D, FMOD::Sound** _fmodSound);
	static int readInSoundFromFile(string const & _filePath, bool _streamed, wstring _soundText, bool _is3D, Sound* _sound);

	static int playSound(Channel* _channel, Sound* _sound, bool _paused = false);
};



#endif //_CORE_SOUNDSYSTEM


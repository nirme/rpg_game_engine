#pragma once

#ifndef _CORE_CHANNEL
#define _CORE_CHANNEL


#include "utility.h"
#include "Core_Logger.h"
#include "Core_Defines.h"
#include "Core_Sound.h"



class SoundSystem;


class Channel
{
	friend class SoundSystem;

private:
	static vector<pair<bool, Channel*>> channelsVector;
	static FMOD::System* soundSystem;

	static int setupChannels(FMOD::System* _soundSystem, USHORT _maxChannelNumber);
	static int getFreeChannelIndex();


protected:

	USHORT index;
	FMOD::Channel* channel;

	bool channelReady;


	FMOD::ChannelGroup* channelGroup;
	short channelGroupIndex;

	float volume;

	FMOD_MODE mode;
	bool is3D;


	bool positionLinked;
	FMOD_VECTOR* position;
	bool velocityLinked;
	FMOD_VECTOR* velocity;

	USHORT priority;

	//sound cone
	bool soundConeOrientationSet;
	FMOD_VECTOR* soundConeOrientation;

	bool soundConeSettingsSet;
	float soundConeInsideAngle;
	float soundConeOutsideAngle;
	float soundConeOutsideVolume;


	bool customSettingsSet;
	float minAudibleDistance;
	float maxAudibleDistance;



	Channel();
	int setupChannel(	FMOD::Channel* _channel,
						FMOD::ChannelGroup* _channelGroup,
						short _groupId,
						FMOD_MODE _mode,
						FMOD_VECTOR* _position = NULL,
						FMOD_VECTOR* _velocity = NULL);


public:

	static int releaseAll();

	int release();

	int setVolume(float _volume);
	float getVolume();

	int set3DPosition(FMOD_VECTOR const & _position);
	int set3DPosition(D3DVECTOR const & _position);
	int set3DPosition(FMOD_VECTOR* _position);
	int set3DPosition(D3DVECTOR* _position);

	int set3DVelocity(FMOD_VECTOR const & _velocity);
	int set3DVelocity(D3DVECTOR const & _velocity);
	int set3DVelocity(FMOD_VECTOR* _velocity);
	int set3DVelocity(D3DVECTOR* _velocity);

	FMOD_VECTOR* getPositionPointer();
	FMOD_VECTOR getPosition();
	FMOD_VECTOR* getVelocityPointer();
	FMOD_VECTOR getVelocity();
	bool isPositionLinked();
	bool isVelocityLinked();

	int setCustomMinMaxAudibleDistance(float _minAudibleDistance, float _maxAudibleDistance);
	int getMinMaxAudibleDistance(float* _minAudibleDistance, float* _maxAudibleDistance);

	int setupSoundCone(FMOD_VECTOR* _soundConeOrientation, float _soundConeInsideAngle, float _soundConeOutsideAngle, float _soundConeOutsideVolume);
	int setupSoundCone(D3DXVECTOR3* _soundConeOrientation, float _soundConeInsideAngle, float _soundConeOutsideAngle, float _soundConeOutsideVolume);
	int resetSoundCone();
	FMOD_VECTOR* getSoundConeOrientationVec();
	int getSoundConeSettings(float* _soundConeInsideAngle, float* _soundConeOutsideAngle, float* _soundConeOutsideVolume);

	int setPriority(USHORT _priority);
	USHORT getPriority();

	int playSound(Sound* _sound, bool _paused = false);

	int setPaused(bool _paused);
	int getPaused(bool* _paused);
	int stop();
	int isPlaying(bool* _isPlaying);
	int isVirtual(bool* _isVirtual);

	int getUserData(void** _userData);
	int setUserData(void* _userdata);

	int setCallback(FMOD_CHANNEL_CALLBACK _callback);

	inline int getChannelIndex()
	{
		return index;
	};
};



#endif //_CORE_CHANNEL

#pragma once

#ifndef _CORE_DEFINES
#define _CORE_DEFINES



// BASIC DEFINES
#define		UNICODE
#define		_UNICODE


#define		MULTI_THREADED_LOADING


#define		UVLONG							unsigned long long

#define		EPSILON							0.00001f
#define		EPSILON_ONE						0.99999f

#define		COLOR_UNIT						0.003921568f

#define		CONST_TAU						6.283185307f
#define		CONST_1PTAU						0.636619772f

#define		RANDOM_UNIT_FLOAT				(float((rand() % 1001) * ((rand() % 2) ? 0.001f : -0.001f)))

#define		RANDOM_POS_UNIT_FLOAT			(float(rand() % 1001) * 0.001f)



// APPLICATION PATHS
// base paths
#define		PATH_DATA						"data\\"
#define		PATH_BIN						"binaries\\"
#define		PATH_LOG						"logs\\"

// vars filename??
#define		VL_FILE							"vars.txt"

// default logs filenames
#define		LOG_FILE_APPLICATION			"application.log"
#define		LOG_FILE_SERVER					"server.log"
#define		LOG_FILE_CLIENT					"client.log"



// VARIABLE LIBRARY SETUP
#define		VARIABLE_LIBRARY_FILENAME		"var_lib.vl"
// varlib constraints
#define		MAX_DICTIONARY					10
#define		MAX_ENTRIES						256
#define		MAX_KEY_LENGTH					32
#define		MAX_VALUE_LENGTH				512









// BASE RESOURCE SETUP
// setup resource indexing with strings
#define		RESOURCE_MAP_WITH_STRINGS

// effect files variable names
#define		EFFECT_VAR_V3_MRT_MATERIAL		"mrtMaterial"
#define		EFFECT_VAR_FL_SPECULAR_INTENSITY	"mrtSpecularIntensity"
#define		EFFECT_VAR_FL_SPECULAR_POWER		"mrtSpecularPower"
#define		EFFECT_VAR_FL_EMISSIVE_FACTOR		"mrtEmissiveFactor"


#define		EFFECT_VAR_TEXTURE_0			"Texture0"
#define		EFFECT_VAR_TEXTURE_1			"Texture1"
#define		EFFECT_VAR_TEXTURE_2			"Texture2"
#define		EFFECT_VAR_TEXTURE_3			"Texture3"
#define		EFFECT_VAR_TEXTURE_4			"Texture4"
#define		EFFECT_VAR_TEXTURE_5			"Texture5"
#define		EFFECT_VAR_TEXTURE_6			"Texture6"
#define		EFFECT_VAR_TEXTURE_7			"Texture7"


//#define		
#define		EFFECT_VAR_TOTAL_PALETTE_SIZE	"TOTAL_PALETTE_SIZE"
#define		EFFECT_VAR_SKINNING_PALETTE		"skinningPalette"
#define		EFFECT_VAR_VERT_INFL_NUMBER		"boneInfluencesNumber"

#define		EFFECT_VAR_LIGHTS_VECTOR_SIZE	"LIGHTS_VECTOR_SIZE"
#define		EFFECT_VAR_V4_LIGHT_AMBIENT		"lightAmbient"
#define		EFFECT_VAR_V4_LIGHTS_VECTOR		"lightsVector"
#define		EFFECT_VAR_LIGHTS_NUMBER		"lightsNumber"










//Bones Hierarchy frame identifier
//#define		BONE_FRAME				0x0010
#define		BONE_NAME				0x0110
#define		BONE_OFFSET				0x0120
#define		BONE_INFLUENCE_NUMBER	0x0130
#define		BONE_INF_VERT_INDEXES	0x0140
#define		BONE_INF_VERT_WEIGHTS	0x0150
#define		BONE_CHILDREN_BLOCK		0x0210

//Animation set frame identifier
#define		ANIMATION_SET_FRAME		0x0010
#define		ANIMATION_SET_NAME		0x0110
#define		TICKS_PER_SECOND		0x0120
#define		PLAYBACK_TYPE			0x0130
#define		ANIMATIONS_NUMBER		0x0140
#define		CALLBACK_KEYS_NUMBER	0x0150
#define		CALLBACK_KEYS_ARRAY		0x0160
#define		ANIMATIONS_BLOCK		0x0210

//Animation frame identifier
#define		ANIMATION_FRAME			0x0010
#define		ANIMATION_NAME			0x0110
#define		SCALE_KEYS_ARRAY		0x0120
#define		ROTATION_KEYS_ARRAY		0x0130
#define		TRANSLATION_KEYS_ARRAY	0x0140



//Sound system default values
#define		FREE_CHANNELGROUP_NUMBER	0xFF
#define		MAX_CHANNELGROUP_NUMBER		0xFE


#define		SOUNDSYSTEM_DEFAULT_MIN_AUDIBLE_DISTANCE		1.0f
#define		SOUNDSYSTEM_DEFAULT_MAX_AUDIBLE_DISTANCE		10000.0f
#define		SOUNDSYSTEM_DEFAULT_DOPPLER_SCALE				1.0f
#define		SOUNDSYSTEM_DEFAULT_DISTANCE_FACTOR				1.0f
#define		SOUNDSYSTEM_DEFAULT_INVERSE_ROLLOFF_SCALE		1.0f
#define		SOUNDSYSTEM_DEFAULT_ROLLOFF						FMOD_3D_INVERSEROLLOFF

#define		SOUNDSYSTEM_DEFAULT_SPEAKER_MODE				FMOD_SPEAKERMODE_STEREO
#define		SOUNDSYSTEM_DEFAULT_SAMPLE_RATE					48000
#define		SOUNDSYSTEM_DEFAULT_FORMAT						FMOD_SOUND_FORMAT_PCM16
#define		SOUNDSYSTEM_DEFAULT_NUM_OUTPUT_CHANNELS			0
#define		SOUNDSYSTEM_DEFAULT_MAX_INPUT_CHANNELS			2
#define		SOUNDSYSTEM_DEFAULT_RESAMPLE_METHOD				FMOD_DSP_RESAMPLER_LINEAR
#define		SOUNDSYSTEM_DEFAULT_MAX_SOFTWARE_CHANNELS		32
#define		SOUNDSYSTEM_DEFAULT_MIN_VIRTUAL_CHANNELS		32
#define		SOUNDSYSTEM_DEFAULT_HRTF_MINIMUM_ANGLE			180.0f
#define		SOUNDSYSTEM_DEFAULT_HRTF_MAXIMUM_ANGLE			360.0f
#define		SOUNDSYSTEM_DEFAULT_HRTF_FREQUENCY				4000.0f
#define		SOUNDSYSTEM_DEFAULT_VIRTUAL_VOLUME_0_LEVEL		0.0f
#define		SOUNDSYSTEM_DEFAULT_DF_CENTER_FREQUENCY			1500.0f


#define		SOUNDSYSTEM_DEFAULT_CHANNELGROUP_VOLUME			1.0f

#define		SOUNDSYSTEM_DEFAULT_SYSTEM_INITFLAGS			FMOD_INIT_NORMAL | FMOD_INIT_VOL0_BECOMES_VIRTUAL | FMOD_INIT_SYNCMIXERWITHUPDATE | FMOD_INIT_GEOMETRY_USECLOSEST | FMOD_INIT_OCCLUSION_LOWPASS
#define		SOUNDSYSTEM_DEFAULT_3D_CHANNEL_MODE				FMOD_DEFAULT | FMOD_LOOP_OFF | FMOD_3D | FMOD_3D_WORLDRELATIVE
#define		SOUNDSYSTEM_DEFAULT_2D_CHANNEL_MODE				FMOD_DEFAULT | FMOD_LOOP_OFF | FMOD_2D

#define		SOUNDSYSTEM_MASTER_CHANNELGROUP_ID				short(-1)






// RENDERER CONSTS

#define		EFFECT_32B_DEPTH		"_32"
#define		EFFECT_64B_DEPTH		"_64"


#define		EFFECT_ID_MRT_CLEAR				"mrt_clear"
#define		EFFECT_ID_POST_MRT				"post_mrt"

#define		EFFECT_ID_TEXTURE_LT			"LightTransport"
#define		EFFECT_ID_TECHNIQUE_LTBB		"PassThrough"


#define		EFFECT_ID_VARIANCE_SHADOW_MAPPING		"vsm"
#define		EFFECT_ID_FORWARD_SHADOW_MAPPING		"fsm"


#define		EFFECT_SM_TECHNIQUE_LIGHT_DIRECTIONAL			"RenderToSMLightDir"
#define		EFFECT_SM_TECHNIQUE_LIGHT_OMNIDIRECTIONAL		"RenderToSMLightOmni"
#define		EFFECT_SM_TECHNIQUE_LIGHT_SPOT					"RenderToSMLightSpot"

#define		EFFECT_SM_SKINNED_TECHNIQUE_LIGHT_DIRECTIONAL			"RenderSkinnedToSMLightDir"
#define		EFFECT_SM_SKINNED_TECHNIQUE_LIGHT_OMNIDIRECTIONAL		"RenderSkinnedToSMLightOmni"
#define		EFFECT_SM_SKINNED_TECHNIQUE_LIGHT_SPOT					"RenderSkinnedToSMLightSpot"


#define		EFFECT_SM_LIGHT_OMNI_DIRECTION				"direction"	//float
#define		EFFECT_SM_LIGHT_NEAR_FAR_PLANE				"nearFarPlane"	//vec3
#define		EFFECT_SM_LIGHT_WORLD_VIEW_PROJ_MATRIX		"worldViewProjMx"	//mx 4x4

#define		EFFECT_SM_LIGHT_SKIN_BONES_INFLUENCE		"bonesInflNumber"
#define		EFFECT_SM_LIGHT_SKIN_PALETTE				"skinningMatrixPalette"


#define		EFFECT_SM_TECHNIQUE_BLUR_VARIANCE		"BlurVSM"

#define		EFFECT_SM_BLUR_SAMPLE_OFFSETS			"blurSampleOffsets"
#define		EFFECT_SM_BLUR_SAMPLE_WEIGHTS			"blurSampleWeights"
#define		EFFECT_SM_BLUR_SAMPLE_NUMBER			"blurSampleNumber"
#define		EFFECT_SM_BLUR_INPUT_TEXTURE			"blurInputTexture"



#define		EFFECT_ID_LIGHT_TRANSPORT				"lt_nan"
#define		EFFECT_ID_LIGHT_TRANSPORT_VSM			"lt_vsm"
#define		EFFECT_ID_LIGHT_TRANSPORT_FSM			"lt_fsm"


#define		EFFECT_LT_TECHNIQUE_AMBIENT						"RenderLTAmbient"

#define		EFFECT_LT_TECHNIQUE_DIRECTIONAL					"RenderLTDir"
#define		EFFECT_LT_TECHNIQUE_OMNIDIRECTIONAL				"RenderLTOmni"
#define		EFFECT_LT_TECHNIQUE_SPOT						"RenderLTSpot"

#define		EFFECT_LT_TECHNIQUE_DIRECTIONAL_NO_SM			"RenderLTDirNoSM"
#define		EFFECT_LT_TECHNIQUE_OMNIDIRECTIONAL_NO_SM		"RenderLTOmniNoSM"
#define		EFFECT_LT_TECHNIQUE_SPOT_NO_SM					"RenderLTSpotNoSM"


#define		EFFECT_LT_CAMERA_POSITION					"cameraPosition" //vec3

#define		EFFECT_LT_CAMERA_INV_VIEW					"cameraInvViewMx" //mx 4x4
#define		EFFECT_LT_CAMERA_INV_PROJECTION				"cameraInvProjectionMx" //mx 4x4
#define		EFFECT_LT_CAMERA_INV_VIEW_PROJECTION		"cameraInvViewProjectionMx" //mx 4x4

#define		EFFECT_LT_LIGHT_AMBIENT				"lightAmbient" //vec3

#define		EFFECT_LT_LIGHT_INFO_TABLE			"lightInfoTable" //vec4[6]
#define		EFFECT_LT_LIGHT_VIEW_PROJECTION		"lightViewProjectionMx" //mx 4x4
#define		EFFECT_LT_LIGHT_SHADOW_MAP_MX		"shadowMapMx" //mx 4x4
#define		EFFECT_LT_LIGHT_NEAR_FAR_PLANE		"nearFarPlane"	//vec3

#define		EFFECT_LT_COLOR_MAP				"MRT0" //tex
#define		EFFECT_LT_NORMAL_MAP			"MRT1" //tex
#define		EFFECT_LT_DEPTH_MAP				"MRT2" //tex

#define		EFFECT_LT_SHADOW_MAP			"shadowMap" //tex
#define		EFFECT_LT_SHADOW_FRONT_MAP		"shadowFrontMap" //tex
#define		EFFECT_LT_SHADOW_BACK_MAP		"shadowBackMap" //tex


#define		EFFECT_BMT_TEXT_EFFECT			"bitmap_text"

#define		EFFECT_BMT_TECHNIQUE			"RenderText"

#define		EFFECT_BMT_SCREEN_SIZE			"screenSize"
#define		EFFECT_BMT_TEXTURE				"p_bmTexture"
#define		EFFECT_BMT_BITMAP_SIZE			"textureDimensions"
#define		EFFECT_BMT_TEXT_POSITION		"textPosition"
#define		EFFECT_BMT_TEXT_SCALE			"scale"


#define		EFFECT_PANEL_TEXT_EFFECT		"panel"

#define		EFFECT_PANEL_TECHNIQUE			"RenderPanel"

#define		EFFECT_PANEL_SCREEN_SIZE		"screenSize"
#define		EFFECT_PANEL_PANEL_POSITION		"p_bmTexture"
#define		EFFECT_PANEL_TEX_POSITION		"textureDimensions"
#define		EFFECT_PANEL_TEXTURE			"textPosition"


#define		DIRECTIONAL_LIGHT_DISTANCE_FROM_CAMERA	100.0f

#define		LIGHT_MATRIX_H		100.0f
#define		LIGHT_MATRIX_W		100.0f

#define		MAX_LIGHT_ARRRAY		220


#define		MRT_COLOR_INDEX		0
#define		MRT_NORMAL_INDEX	1
#define		MRT_DEPTH_INDEX		2

// RENDERER CONSTS


// PARTICLE SYSTEM CONSTS

#define		BILLBOARDING_NONE			0
#define		BILLBOARDING_VIEWPLANE		1
#define		BILLBOARDING_VIEWPOINT		2

#define		PARTICLE_SORTING_NONE		0
#define		PARTICLE_SORTING_PARTIAL	1
#define		PARTICLE_SORTING_FULL		2



// GUI CONTROLS

#define		GUI_CONTROL_BUTTON_STATE_FREE			0
#define		GUI_CONTROL_BUTTON_STATE_ACTIVE			1
#define		GUI_CONTROL_BUTTON_STATE_DOWN			2
#define		GUI_CONTROL_BUTTON_STATE_DISABLED		3
#define		GUI_CONTROL_BUTTON_STATES_NUMBER		4


// sql types
#define		GUI_ELEMENT_TEXTURE			1
#define		GUI_ELEMENT_TEXT			2
#define		GUI_ELEMENT_BITMAPTEXT		3

#define		GUI_CONTROL_STATIC			1
#define		GUI_CONTROL_BUTTON			2


// GUI CONTROLS





#define		MATRIX_PALETTE_SIZE_DEFAULT		50



//application defs
#define		CLASS_NAME		L"defined class name"
#define		WINDOW_NAME		L"defined window name"
// cursor is used by directx
#define		_DX_MOUSE_USAGE





#endif //_CORE_DEFINES
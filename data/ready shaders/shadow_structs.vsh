#ifndef sm_VSH
#define sm_VSH



texture ShadowMap;
texture ShadowMapFront;
texture ShadowMapBack;

sampler ShadowMapSampler = sampler_state
{
	texture = <ShadowMap>;
	AddressU  = CLAMP;
	AddressV  = CLAMP;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
	MIPFILTER = NONE;
};

sampler ShadowMapFrontSampler = sampler_state
{
	texture = <ShadowMapFront>;
	AddressU  = CLAMP;
	AddressV  = CLAMP;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
	MIPFILTER = NONE;
};

sampler ShadowMapBackSampler = sampler_state
{
	texture = <ShadowMapBack>;
	AddressU  = CLAMP;
	AddressV  = CLAMP;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
	MIPFILTER = NONE;
};


#endif //sm_VSH

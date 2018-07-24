#ifndef post_VSH
#define post_VSH


texture MRT0;
texture MRT1;
texture MRT2;

sampler MRT0Sampler = sampler_state 
{
	texture = <MRT0>;
	AddressU  = CLAMP;
	AddressV  = CLAMP;
	MINFILTER = POINT;
	MAGFILTER = POINT;
	MIPFILTER = NONE;
};

sampler MRT1Sampler = sampler_state 
{
	texture = <MRT1>;
	AddressU  = CLAMP;
	AddressV  = CLAMP;
	MINFILTER = POINT;
	MAGFILTER = POINT;
	MIPFILTER = NONE;
};

sampler MRT2Sampler = sampler_state 
{
	texture = <MRT2>;
	AddressU  = CLAMP;
	AddressV  = CLAMP;
	MINFILTER = POINT;
	MAGFILTER = POINT;
	MIPFILTER = NONE;
};



// structures for quad rendering (post & light)
// input output structures are the same so there's no need to define both of them separately
struct VS_FS_QUAD_IN_OUT
{
	float4 Position : POSITION0;
	float2 TexCoord : TEXCOORD0;
	float2 Position2 : TEXCOORD1;
};




VS_FS_QUAD_IN_OUT vs_PassThrough( float4 Position : POSITION0 , float2 TexCoord : TEXCOORD0 )
{
	VS_FS_QUAD_IN_OUT Output;
	Output.Position = Position / Position.w;
	Output.TexCoord = TexCoord;
	Output.Position2 = Output.Position.xy;

	return Output;
}



#endif //post_VSH

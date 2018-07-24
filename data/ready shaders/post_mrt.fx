#include "post_structs.vsh"


texture LightTransport;

sampler LightTransportSampler = sampler_state 
{
	texture = <LightTransport>;
	AddressU  = CLAMP;
	AddressV  = CLAMP;
	MINFILTER = POINT;
	MAGFILTER = POINT;
	MIPFILTER = NONE;
};


float4 ps_PassThrough(VS_FS_QUAD_IN_OUT Input) : COLOR
{
	return tex2D(LightTransportSampler, Input.TexCoord);
}


//--------------------------------------------------------------------------------------
// Technique
//--------------------------------------------------------------------------------------

technique PassThrough
{
	pass p0
	{
		VertexShader = compile vs_3_0 vs_PassThrough();
		PixelShader = compile ps_3_0 ps_PassThrough();

		ZEnable				= FALSE;
		ZWriteEnable		= FALSE;
		CullMode			= NONE;
	}
}


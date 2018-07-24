#include "DR_structs.vsh"

float4x4 worldMx : WORLD;
float4x4 viewProjectionMx : VIEWPROJECTION;
float4x4 viewMx : VIEW;
float4x4 viewInverseMx : VIEWINVERSE;
float4x4 projectionMx : PROJECTION;

float specularIntensity = 0;
float specularPower = 0;
float emissiveFactor = 0;



texture baseTexture;
sampler TextureSampler = sampler_state
{
	Texture = <baseTexture>;
	MinFilter = ANISOTROPIC;
	MagFilter = ANISOTROPIC;
	MipFilter = LINEAR;
	MaxAnisotropy = 1;
	AddressU  = WRAP;
    AddressV  = WRAP;
};



VS_INTERPOLATED_NORMAL_OUTPUT vs_MRTInterpolatedNormals( VS_INTERPOLATED_NORMAL_INPUT Input )
{
	VS_INTERPOLATED_NORMAL_OUTPUT Output = (VS_INTERPOLATED_NORMAL_OUTPUT)0;

	Output.Position = mul(mul(Input.Position, worldMx), viewProjectionMx);
	Output.Normal = mul(mul(Input.Normal, worldMx), viewProjectionMx);
	Output.TexCoord = Input.TexCoord;
	Output.Depth = Output.Position.zw;

	return Output;
}

PS_MRT_OUTPUT ps_MRTInterpolatedNormals(VS_INTERPOLATED_NORMAL_OUTPUT Input)
{
	PS_MRT_OUTPUT Output = (PS_MRT_OUTPUT)0;

	Output.Color = float4(tex2D(TextureSampler, Input.TexCoord).xyz, specularIntensity);


	Output.Normal.xyz = Input.Normal;
	Output.Normal.w = specularPower;

	Output.Depth.rg = float2(Input.Depth.x / Input.Depth.y, emissiveFactor);

	return Output;
}


//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------

technique MRTInterpolatedNormals
{
	pass p0
	{
		VertexShader = compile vs_3_0 vs_MRTInterpolatedNormals();
		PixelShader = compile ps_3_0 ps_MRTInterpolatedNormals();

		ZEnable				= TRUE;
		ZWriteEnable		= TRUE;
		CullMode			= NONE;
	}
}

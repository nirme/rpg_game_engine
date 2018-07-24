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


texture normalMap;
sampler NormalSampler = sampler_state
{
	Texture = <normalMap>;
	MinFilter = ANISOTROPIC;
	MagFilter = ANISOTROPIC;
	MipFilter = LINEAR;
	MaxAnisotropy = 1;
	AddressU  = WRAP;
    AddressV  = WRAP;
};



VS_MAPPED_NORMAL_OUTPUT vs_MRTMappedNormals( VS_MAPPED_NORMAL_INPUT Input )
{
	VS_MAPPED_NORMAL_OUTPUT Output = (VS_MAPPED_NORMAL_OUTPUT)0;

	float4 worldPosition = mul(Input.Position, worldMx);
	float4 viewPosition = mul(worldPosition, viewMx);

	Output.Position = mul(viewPosition, projectionMx);
	Output.TexCoord = Input.TexCoord;
	Output.Depth = Output.Position.zw;

	Output.TangentToWorld[0] = mul(Input.Tangent, worldMx);
	Output.TangentToWorld[1] = mul(Input.Binormal, worldMx);
	Output.TangentToWorld[2] = mul(Input.Normal, worldMx);

	return Output;
}


PS_MRT_OUTPUT ps_MRTMappedNormals(VS_MAPPED_NORMAL_OUTPUT Input)
{
	PS_MRT_OUTPUT Output = (PS_MRT_OUTPUT)0;

	Output.Color = float4(tex2D(TextureSampler, Input.TexCoord).xyz, specularIntensity);

	float3 normal = tex2D(NormalSampler, Input.TexCoord);
	// transform to [-1,1]
	normal = 2 * normal - 1;
	// transform to world space and normalize
	normal = normalize( mul( normal, Input.TangentToWorld ) );
	// pack normal to values [0,1]
	Output.Normal.xyz = normal;
	Output.Normal.w = specularPower;

	Output.Depth.rg = float2(Input.Depth.x / Input.Depth.y, emissiveFactor);

	return Output;
}


//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------

technique MRTMappedNormals
{
	pass p0
	{
		VertexShader = compile vs_3_0 vs_MRTMappedNormals();
		PixelShader = compile ps_3_0 ps_MRTMappedNormals();

		ZEnable				= TRUE;
		ZWriteEnable		= TRUE;
		CullMode			= NONE;
	}
}

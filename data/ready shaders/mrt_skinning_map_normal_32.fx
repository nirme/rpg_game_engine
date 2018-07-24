#include "DR_structs.vsh"
#include "skinning_structs_v2.vsh"


float4x4 worldMx : WORLD;
float4x4 viewProjectionMx : VIEWPROJECTION;
float4x4 viewMx : VIEW;
float4x4 viewInverseMx : VIEWINVERSE;
float4x4 projectionMx : PROJECTION;

float4 nearFarPlane;

float specularIntensity = 0;
float specularPower = 0;
float emissiveFactor = 0;


int boneInfluencesNumber = 1;


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


VS_MAPPED_NORMAL_OUTPUT vs_MRTSkinnedMappedNormals( VS_SKINNED_MAPPED_NORMAL_INPUT Input )
{
	VS_MAPPED_NORMAL_OUTPUT Output = (VS_MAPPED_NORMAL_OUTPUT)0;

	VS_SKIN_MN_INPUT sMnInput = {Input.Position, Input.Normal, Input.Binormal, Input.Tangent, Input.BlendWeights, Input.BlendIndices};
	VS_SKIN_MN_OUTPUT sMnOutput = vs_Mapped_Normals_Skinning(sMnInput, boneInfluencesNumber);

	Output.Position = mul(mul(sMnOutput.Position, worldMx), viewProjectionMx);
	Output.TexCoord = Input.TexCoord;
	Output.Depth = Output.Position.zw;

	Output.TangentToWorld[0] = mul(sMnOutput.Tangent,  worldMx);
	Output.TangentToWorld[1] = mul(sMnOutput.Binormal, worldMx);
	Output.TangentToWorld[2] = mul(sMnOutput.Normal,   worldMx);

	return Output;
}


PS_MRT_OUTPUT ps_MRTSkinnedMappedNormals(VS_MAPPED_NORMAL_OUTPUT Input)
{
	PS_MRT_OUTPUT Output = (PS_MRT_OUTPUT)0;

	Output.Color = float4(tex2D(TextureSampler, Input.TexCoord).xyz, specularIntensity);

	float3 normal = tex2D(NormalSampler, Input.TexCoord);
	// transform to [-1,1]
	normal = 2 * normal - 1;
	// transform to world space and normalize
	normal = normalize( mul( normal, Input.TangentToWorld ) );
	// pack normal to values [0,1]
	Output.Normal.xyz = (normal + 1) * 0.5;
	Output.Normal.w = specularPower;

	Output.Depth.rg = float2(Input.Depth.x / Input.Depth.y, emissiveFactor);

	return Output;
}


//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------

technique MRTSkinnedMappedNormals
{
	pass p0
	{
		VertexShader = compile vs_3_0 vs_MRTSkinnedMappedNormals();
		PixelShader =  compile ps_3_0 ps_MRTSkinnedMappedNormals();

		ZEnable				= TRUE;
		ZWriteEnable		= TRUE;
		CullMode			= NONE;
	}
}

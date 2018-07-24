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

// inside skinning structs
//int boneInfluencesNumber = 1;


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


VS_INTERPOLATED_NORMAL_OUTPUT vs_MRTSkinnedInterpolatedNormals( VS_SKINNED_INTERPOLATED_NORMAL_INPUT Input )
{
	VS_INTERPOLATED_NORMAL_OUTPUT Output = (VS_INTERPOLATED_NORMAL_OUTPUT)0;

	VS_SKIN_IN_INPUT sInInput = { Input.Position, Input.Normal, Input.BlendWeights, Input.BlendIndices };
	VS_SKIN_IN_OUTPUT sInOutput = vs_Interpolated_Normals_Skinning( sInInput, boneInfluencesNumber );

	Output.Position = mul( mul( sInOutput.Position, worldMx ), viewProjectionMx );
	Output.Normal =   mul( mul( sInOutput.Normal,   worldMx ), viewProjectionMx );
	Output.TexCoord = Input.TexCoord;
	Output.Depth = Output.Position.zw;

	return Output;
}

PS_MRT_OUTPUT ps_MRTSkinnedInterpolatedNormals(VS_INTERPOLATED_NORMAL_OUTPUT Input)
{
	PS_MRT_OUTPUT Output = (PS_MRT_OUTPUT)0;

	Output.Color = float4(tex2D(TextureSampler, Input.TexCoord).xyz, specularIntensity);

	// pack normal to values [0,1]
	Output.Normal.xyz = (Input.Normal + 1) * 0.5;
	Output.Normal.w = specularPower;

	Output.Depth.rg = float2(Input.Depth.x / Input.Depth.y, emissiveFactor);

	return Output;
}


//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------

technique MRTSkinnedInterpolatedNormals
{
	pass p0
	{
		VertexShader = compile vs_3_0 vs_MRTSkinnedInterpolatedNormals();
		PixelShader =  compile ps_3_0 ps_MRTSkinnedInterpolatedNormals();

		ZEnable				= TRUE;
		ZWriteEnable		= TRUE;
		CullMode			= NONE;
	}
}

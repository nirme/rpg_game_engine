#include "shadow_structs.vsh"
#include "skinning_structs_v2.vsh"


float    direction;
float3   nearFarPlane;
float4x4 worldViewProjMx;



//--------------------------------------------------------------------------------------
// SHADOW MAPPING DIRECTIONAL
//--------------------------------------------------------------------------------------

struct VS_OUTPUT_SM_DIR
{
    float4 position : POSITION0 ;
	float2 depth : TEXCOORD1 ;
};

VS_OUTPUT_SM_DIR vs_RenderToSMLightDir( float4 Position : POSITION0 ) : POSITION0
{
	VS_OUTPUT_SM_DIR Output = (VS_OUTPUT_SM_DIR)0;
	Output.position = mul(Position, worldViewProjMx);
	Output.position /= Output.position.w;
	
	Output.depth.x = (Output.position.z - nearFarPlane.x) * nearFarPlane.z;
	Output.depth.y = 1;

	return Output;
}

float4 ps_RenderToSMLightDir( float2 Depth : TEXCOORD1 ) : COLOR0
{
	float z = Depth.x / Depth.y;
	return float4(z, z * z, 0, 0);
}


technique RenderToSMLightDir
{
    pass p0
    {
        VertexShader = compile vs_2_0 vs_RenderToSMLightDir();
        PixelShader = compile ps_2_0 ps_RenderToSMLightDir();

		CullMode = NONE;
   }
}



//--------------------------------------------------------------------------------------
// SHADOW MAPPING OMNIDIRECTIONAL
//--------------------------------------------------------------------------------------

struct VS_OUTPUT_SM_OMNI
{
    float4 position : POSITION0 ;
    float z : TEXCOORD1 ;
	float2 depth : TEXCOORD2 ;
};

struct PS_INPUT_SM_OMNI
{
    float z : TEXCOORD1 ;
	float2 depth : TEXCOORD2 ;
};


VS_OUTPUT_SM_OMNI vs_RenderToSMLightOmni( float4 Position : POSITION0 )
{
	VS_OUTPUT_SM_OMNI Output = (VS_OUTPUT_SM_OMNI)0;

	Output.position = mul(Position, worldViewProjMx);
	Output.position /= Output.position.w;
	Output.position.z *= direction;

	float L = length(Output.position.xyz);
	Output.position /= L;

	Output.z = Output.position.z;

	Output.position.z += 1;
	Output.position.x /= Output.position.z;
	Output.position.y /= Output.position.z;

	Output.position.z = (L - nearFarPlane.x) * nearFarPlane.z;
	Output.position.w = 1;
	Output.depth = Output.position.zw;

	return Output;
}

float4 ps_RenderToSMLightOmni( PS_INPUT_SM_OMNI Input ) : COLOR0
{
	clip(Input.z);
	float z = Input.depth.x / Input.depth.y;
	return float4(z, z * z, 0, 0);
}


technique RenderToSMLightOmni
{
    pass p0
    {
        VertexShader = compile vs_2_0 vs_RenderToSMLightOmni();
        PixelShader = compile ps_2_0 ps_RenderToSMLightOmni();

		CullMode = NONE;
   }
}



//--------------------------------------------------------------------------------------
// SHADOW MAPPING SPOT
//--------------------------------------------------------------------------------------

struct VS_OUTPUT_SM_SPOT
{
    float4 position : POSITION0 ;
	float2 depth : TEXCOORD1 ;
};


VS_OUTPUT_SM_SPOT vs_RenderToSMLightSpot( float4 Position : POSITION0 )
{
	VS_OUTPUT_SM_SPOT Output = (VS_OUTPUT_SM_SPOT)0;

	Output.position = mul(Position, worldViewProjMx);
	Output.position /= Output.position.w;

	float L = length(Output.position.xyz);
	Output.position /= L;

	Output.position.z += 1;
	Output.position.x /= Output.position.z;
	Output.position.y /= Output.position.z;

	Output.position.z = (L - nearFarPlane.x) * nearFarPlane.z;
	Output.position.w = 1;
	Output.depth = Output.position.zw;

	return Output;
}

float4 ps_RenderToSMLightSpot( float2 Depth : TEXCOORD1 ) : COLOR0
{
	float z = Depth.x / Depth.y;
	return float4(z, z * z, 0, 0);
}


technique RenderToSMLightSpot
{
    pass p0
    {
        VertexShader = compile vs_2_0 vs_RenderToSMLightSpot();
        PixelShader = compile ps_2_0 ps_RenderToSMLightSpot();

		CullMode = NONE;
   }
}




//--------------------------------------------------------------------------------------
// SKINNED SHADOW MAPPING
//--------------------------------------------------------------------------------------


int boneInfluencesNumber = 1;

//--------------------------------------------------------------------------------------
// SHADOW MAPPING SKINNED DIRECTIONAL
//--------------------------------------------------------------------------------------

VS_OUTPUT_SM_DIR vs_RenderSkinnedToSMLightDir( float4 Position : POSITION0 , float3 BlendWeights : BLENDWEIGHT , float4 BlendIndices : BLENDINDICES )
{
	VS_SKIN_V1_INPUT sInInput = { Position, BlendWeights, BlendIndices };
	VS_SKIN_V1_OUTPUT sInOutput = vs_SkinningV1( sInInput, boneInfluencesNumber );

	VS_OUTPUT_SM_DIR Output = (VS_OUTPUT_SM_DIR)0;

	Output.position = mul(sInOutput.Vector1, worldViewProjMx);
	Output.position /= Output.position.w;
	
	Output.depth.x = (Output.position.z - nearFarPlane.x) * nearFarPlane.z;
	Output.depth.y = 1;

	return Output;
}

float4 ps_RenderSkinnedToSMLightDir( float2 Depth : TEXCOORD1 ) : COLOR0
{
	float z = Depth.x / Depth.y;
	return float4(z, z * z, 0, 0);
}


technique RenderSkinnedToSMLightDir
{
    pass p0
    {
        VertexShader = compile vs_2_0 vs_RenderSkinnedToSMLightDir();
        PixelShader = compile ps_2_0 ps_RenderSkinnedToSMLightDir();

		CullMode = NONE;
   }
}



//--------------------------------------------------------------------------------------
// SHADOW MAPPING SKINNED OMNIDIRECTIONAL
//--------------------------------------------------------------------------------------

VS_OUTPUT_SM_OMNI vs_RenderSkinnedToSMLightOmni( float4 Position : POSITION0 , float3 BlendWeights : BLENDWEIGHT , float4 BlendIndices : BLENDINDICES )
{
	VS_SKIN_V1_INPUT sInInput = { Position, BlendWeights, BlendIndices };
	VS_SKIN_V1_OUTPUT sInOutput = vs_SkinningV1( sInInput, boneInfluencesNumber );

	VS_OUTPUT_SM_OMNI Output = (VS_OUTPUT_SM_OMNI)0;

	Output.position = mul(sInOutput.Vector1, worldViewProjMx);
	Output.position /= Output.position.w;
	Output.position.z *= direction;

	float L = length(Output.position.xyz);
	Output.position /= L;

	Output.z = Output.position.z;

	Output.position.z += 1;
	Output.position.x /= Output.position.z;
	Output.position.y /= Output.position.z;

	Output.position.z = (L - nearFarPlane.x) * nearFarPlane.z;
	Output.position.w = 1;
	Output.depth = Output.position.zw;

	return Output;
}

float4 ps_RenderSkinnedToSMLightOmni( PS_INPUT_SM_OMNI Input ) : COLOR0
{
	clip(Input.z);
	float z = Input.depth.x / Input.depth.y;
	return float4(z, z * z, 0, 0);
}


technique RenderSkinnedToSMLightOmni
{
    pass p0
    {
        VertexShader = compile vs_2_0 vs_RenderSkinnedToSMLightOmni();
        PixelShader = compile ps_2_0 ps_RenderSkinnedToSMLightOmni();

		CullMode = NONE;
   }
}



//--------------------------------------------------------------------------------------
// SHADOW MAPPING SKINNED SPOT
//--------------------------------------------------------------------------------------

VS_OUTPUT_SM_SPOT vs_RenderSkinnedToSMLightSpot( float4 Position : POSITION0 , float3 BlendWeights : BLENDWEIGHT , float4 BlendIndices : BLENDINDICES )
{
	VS_SKIN_V1_INPUT sInInput = { Position, BlendWeights, BlendIndices };
	VS_SKIN_V1_OUTPUT sInOutput = vs_SkinningV1( sInInput, boneInfluencesNumber );

	VS_OUTPUT_SM_SPOT Output = (VS_OUTPUT_SM_SPOT)0;

	Output.position = mul(sInOutput.Vector1, worldViewProjMx);
	Output.position /= Output.position.w;

	float L = length(Output.position.xyz);
	Output.position /= L;

	Output.position.z += 1;
	Output.position.x /= Output.position.z;
	Output.position.y /= Output.position.z;

	Output.position.z = (L - nearFarPlane.x) * nearFarPlane.z;
	Output.position.w = 1;
	Output.depth = Output.position.zw;

	return Output;
}

float4 ps_RenderSkinnedToSMLightSpot( float2 Depth : TEXCOORD1 ) : COLOR0
{
	float z = Depth.x / Depth.y;
	return float4(z, z * z, 0, 0);
}


technique RenderSkinnedToSMLightSpot
{
    pass p0
    {
        VertexShader = compile vs_2_0 vs_RenderSkinnedToSMLightSpot();
        PixelShader = compile ps_2_0 ps_RenderSkinnedToSMLightSpot();

		CullMode = NONE;
   }
}



//--------------------------------------------------------------------------------------
// SHADOW MAPPING BLUR
//--------------------------------------------------------------------------------------


#define    SAMPLE_COUNT    9

float2   blurSampleOffsets[SAMPLE_COUNT];
float    blurSampleWeights[SAMPLE_COUNT];
int    blurSampleNumber;


texture blurInputTexture;
sampler blurInputSampler = sampler_state
{
	texture = <blurInputTexture>;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
	MIPFILTER = LINEAR;
	AddressU  = CLAMP;
	AddressV  = CLAMP;
};


struct VS_IN_OUT_BLUR
{
	float4 Position : POSITION0;
	float2 TexCoord : TEXCOORD0;
};

VS_IN_OUT_BLUR vs_BlurPassThrough( VS_IN_OUT_BLUR Input )
{
	return Input;
}

float4 ps_BlurSample( float2 TexCoord : TEXCOORD0 ) : COLOR0
{
	float4 c = 0;

	// Combine a number of weighted image filter taps.
	for (int i = 0; i < blurSampleNumber; i++)
	{
		c += tex2D(blurInputSampler, TexCoord + blurSampleOffsets[i]) * blurSampleWeights[i];
	}

	return c;
}


technique BlurVSM
{
    pass p0
    {
        VertexShader = compile vs_2_0 vs_BlurPassThrough();
        PixelShader = compile ps_2_0 ps_BlurSample();

		CullMode = NONE;
   }
}

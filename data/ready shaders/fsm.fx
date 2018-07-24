
#include "DR_structs.vsh"
#include "shadow_structs.vsh"


float    direction;
float4   nearFarPlane;
float4x4 worldViewProjMx;


/////////////////////////////////////////////


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
	float4 z = Depth.x / Depth.y;
	return z;
}


technique RenderToSMLightDir
{
    pass p0
    {
        VertexShader = compile vs_3_0 vs_RenderToSMLightDir();
        PixelShader = compile ps_3_0 ps_RenderToSMLightDir();

		CullMode = NONE;
    }
}

/////////////////////////////////////////////


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
	float4 z = Input.depth.x / Input.depth.y;
	return z;
}


technique RenderToSMLightOmni
{
    pass p0
    {
        VertexShader = compile vs_3_0 vs_RenderToSMLightOmni();
        PixelShader = compile ps_3_0 ps_RenderToSMLightOmni();

		CullMode = NONE;
    }
}

/////////////////////////////////////////////////


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
	float4 z = Depth.x / Depth.y;
	return z;
}


technique RenderToSMLightSpot
{
    pass p0
    {
        VertexShader = compile vs_3_0 vs_RenderToSMLightSpot();
        PixelShader = compile ps_3_0 ps_RenderToSMLightSpot();

		CullMode = NONE;
    }
}


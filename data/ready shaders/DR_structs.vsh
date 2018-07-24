#ifndef MRT_VSH
#define MRT_VSH



// structures for models with normals in vertices
struct VS_INTERPOLATED_NORMAL_INPUT
{
	float4 Position : POSITION0;
	float3 Normal : NORMAL0;
	float2 TexCoord : TEXCOORD0;
};

struct VS_SKINNED_INTERPOLATED_NORMAL_INPUT
{
	float4 Position : POSITION0;
    float3 BlendWeights : BLENDWEIGHT;
    float4 BlendIndices : BLENDINDICES;
	float3 Normal : NORMAL0;
	float2 TexCoord : TEXCOORD0;
};


struct VS_INTERPOLATED_NORMAL_OUTPUT
{
	float4 Position : POSITION0;
	float2 TexCoord : TEXCOORD0;
	float3 Normal : TEXCOORD1;

// position.z, position.w
	float2 Depth : TEXCOORD2;
};




// structures for normal mapping, bump mapping, paralax occlusion...
struct VS_MAPPED_NORMAL_INPUT
{
	float4 Position : POSITION0;
	float3 Normal : NORMAL0;
	float2 TexCoord : TEXCOORD0;
	float3 Binormal : BINORMAL0;
	float3 Tangent : TANGENT0;
};

struct VS_SKINNED_MAPPED_NORMAL_INPUT
{
	float4 Position : POSITION0;
	float3 Normal : NORMAL0;
    float3 BlendWeights : BLENDWEIGHT ;
    float4 BlendIndices : BLENDINDICES;
	float2 TexCoord : TEXCOORD0;
	float3 Binormal : BINORMAL0;
	float3 Tangent : TANGENT0;
};

struct VS_MAPPED_NORMAL_OUTPUT
{
	float4 Position : POSITION0;
	float2 TexCoord : TEXCOORD0;

// position.z, position.w
	float2 Depth : TEXCOORD1;

// TangentToWorld[0] = mul(Tangent, World)
// TangentToWorld[1] = mul(Binormal, World)
// TangentToWorld[2] = mul(Normal, World)
	float3x3 TangentToWorld : TEXCOORD2;
};


struct PS_MRT_OUTPUT
{
	float4 Color : COLOR0;
	float4 Normal : COLOR1;
	float4 Depth : COLOR2;
};



#endif //MRT_VSH

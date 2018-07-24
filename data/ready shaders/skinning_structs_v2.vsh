#ifndef skinning_v2_VSH
#define skinning_v2_VSH


#ifndef SKINNING_PALETTE_SIZE_DEFAULT
#define SKINNING_PALETTE_SIZE_DEFAULT 50
#endif

const int TOTAL_PALETTE_SIZE = SKINNING_PALETTE_SIZE_DEFAULT;
float4x3 skinningPalette[ SKINNING_PALETTE_SIZE_DEFAULT ];

int boneInfluencesNumber = 1;



//-----------------------------------------------------------------------------
// Application default skinning structures
//-----------------------------------------------------------------------------

struct VS_SKIN_IN_INPUT
{
	float4 Position;
	float3 Normal;
	float3 BlendWeights;
	float4 BlendIndices;
};

struct VS_SKIN_IN_OUTPUT
{
	float4 Position;
	float3 Normal;
};


struct VS_SKIN_MN_INPUT
{
	float4 Position;
	float3 Normal;
	float3 Binormal;
	float3 Tangent;
	float3 BlendWeights;
	float4 BlendIndices;
};

struct VS_SKIN_MN_OUTPUT
{
	float4 Position;
	float3 Normal;
	float3 Binormal;
	float3 Tangent;
};



// call this function to skin VB position and normal
VS_SKIN_IN_OUTPUT vs_Interpolated_Normals_Skinning( const VS_SKIN_IN_INPUT Input, int bonesNumber )
{
	VS_SKIN_IN_OUTPUT Output = (VS_SKIN_IN_OUTPUT) 0;

	float lastWeight = 1.0;
	float weight;
	float blendWeights[ 3 ] = (float[ 3 ]) Input.BlendWeights;
	int indices[ 4 ] = (int[ 4 ]) D3DCOLORtoUBYTE4( Input.BlendIndices );

	for( int bone = 0; bone < bonesNumber - 1; ++ bone )
	{
		weight = blendWeights[ bone ];
		lastWeight -= weight;
		Output.Position.xyz += mul(Input.Position, skinningPalette[ indices[ bone ] ] ) * weight;
		Output.Normal       += mul(Input.Normal,   skinningPalette[ indices[ bone ] ] ) * weight;
	}

	Output.Position.xyz += mul(Input.Position, skinningPalette[ indices[ bonesNumber - 1 ] ] ) * lastWeight;
	Output.Normal       += mul(Input.Normal,   skinningPalette[ indices[ bonesNumber - 1 ] ] ) * lastWeight;
	Output.Position.w = 1.0f;

	return Output;
}


// call this function to skin VB position
VS_SKIN_MN_OUTPUT vs_Mapped_Normals_Skinning( const VS_SKIN_MN_INPUT Input, int bonesNumber )
{
	VS_SKIN_MN_OUTPUT Output = ( VS_SKIN_MN_OUTPUT ) 0;

	float lastWeight = 1.0;
	float weight;
	float blendWeights[ 3 ] = (float[ 3 ]) Input.BlendWeights;
	int indices[ 4 ] = (int[ 4 ]) D3DCOLORtoUBYTE4( Input.BlendIndices );

	for( int bone = 0; bone < bonesNumber - 1; ++ bone )
	{
		weight = blendWeights[ bone ];
		lastWeight -= weight;
		Output.Position.xyz += mul(Input.Position, skinningPalette[ indices[ bone ] ] ) * weight;
		Output.Normal       += mul(Input.Normal,   skinningPalette[ indices[ bone ] ] ) * weight;
		Output.Binormal     += mul(Input.Binormal, skinningPalette[ indices[ bone ] ] ) * weight;
		Output.Tangent      += mul(Input.Tangent,  skinningPalette[ indices[ bone ] ] ) * weight;
	}

	Output.Position.xyz += mul(Input.Position, skinningPalette[ indices[ bonesNumber - 1 ] ] ) * lastWeight;
	Output.Normal       += mul(Input.Normal,   skinningPalette[ indices[ bonesNumber - 1 ] ] ) * lastWeight;
	Output.Binormal     += mul(Input.Binormal, skinningPalette[ indices[ bonesNumber - 1 ] ] ) * lastWeight;
	Output.Tangent      += mul(Input.Tangent,  skinningPalette[ indices[ bonesNumber - 1 ] ] ) * lastWeight;

	Output.Position.w = 1.0f;

	return Output;
}




//-----------------------------------------------------------------------------
// Basic structures for user defined skinning
//-----------------------------------------------------------------------------

struct VS_SKIN_V1_INPUT
{
	float4 Vector1;
	float3 BlendWeights;
	float4 BlendIndices;
};

struct VS_SKIN_V1_OUTPUT
{
	float4 Vector1;
};


struct VS_SKIN_V2_INPUT
{
	float4 Vector1;
	float4 Vector2;
	float3 BlendWeights;
	float4 BlendIndices;
};

struct VS_SKIN_V2_OUTPUT
{
	float4 Vector1;
	float4 Vector2;
};


struct VS_SKIN_V3_INPUT
{
	float4 Vector1;
	float4 Vector2;
	float4 Vector3;
	float3 BlendWeights;
	float4 BlendIndices;
};

struct VS_SKIN_V3_OUTPUT
{
	float4 Vector1;
	float4 Vector2;
	float4 Vector3;
};


struct VS_SKIN_V4_INPUT
{
	float4 Vector1;
	float4 Vector2;
	float4 Vector3;
	float4 Vector4;
	float3 BlendWeights;
	float4 BlendIndices;
};

struct VS_SKIN_V4_OUTPUT
{
	float4 Vector1;
	float4 Vector2;
	float4 Vector3;
	float4 Vector4;
};



//-----------------------------------------------------------------------------
// User defined skinning function
//-----------------------------------------------------------------------------

VS_SKIN_V1_OUTPUT vs_SkinningV1( const VS_SKIN_V1_INPUT Input, int bonesNumber )
{
	VS_SKIN_V1_OUTPUT Output = (VS_SKIN_V1_OUTPUT)0;
	float lastWeight = 1.0;
	float weight;
	float blendWeights[ 3 ] = (float[ 3 ]) Input.BlendWeights;
	int indices[ 4 ] = (int[ 4 ]) D3DCOLORtoUBYTE4( Input.BlendIndices );

	for( int bone = 0; bone < bonesNumber - 1; ++ bone )
	{
		weight = blendWeights[ bone ];
		lastWeight -= weight;
		Output.Vector1.xyz += mul(Input.Vector1, skinningPalette[ indices[ bone ] ] ) * weight;
	}

	Output.Vector1.xyz += mul(Input.Vector1, skinningPalette[ indices[ bonesNumber - 1 ] ] ) * lastWeight;
	Output.Vector1.w = Input.Vector1.w;

	return Output;
}

VS_SKIN_V2_OUTPUT vs_SkinningV2( const VS_SKIN_V2_INPUT Input, int bonesNumber )
{
	VS_SKIN_V2_OUTPUT Output = (VS_SKIN_V2_OUTPUT)0;
	float lastWeight = 1.0;
	float weight;
	float blendWeights[ 3 ] = (float[ 3 ]) Input.BlendWeights;
	int indices[ 4 ] = (int[ 4 ]) D3DCOLORtoUBYTE4( Input.BlendIndices );

	for( int bone = 0; bone < bonesNumber - 1; ++ bone )
	{
		weight = blendWeights[ bone ];
		lastWeight -= weight;
		Output.Vector1.xyz += mul(Input.Vector1, skinningPalette[ indices[ bone ] ] ) * weight;
		Output.Vector2.xyz += mul(Input.Vector2, skinningPalette[ indices[ bone ] ] ) * weight;
	}

	Output.Vector1.xyz += mul(Input.Vector1, skinningPalette[ indices[ bonesNumber - 1 ] ] ) * lastWeight;
	Output.Vector2.xyz += mul(Input.Vector2, skinningPalette[ indices[ bonesNumber - 1 ] ] ) * lastWeight;

	Output.Vector1.w = Input.Vector1.w;
	Output.Vector2.w = Input.Vector2.w;

	return Output;
}

VS_SKIN_V3_OUTPUT vs_SkinningV3( const VS_SKIN_V3_INPUT Input, int bonesNumber )
{
	VS_SKIN_V3_OUTPUT Output = (VS_SKIN_V3_OUTPUT)0;
	float lastWeight = 1.0;
	float weight;
	float blendWeights[ 3 ] = (float[ 3 ]) Input.BlendWeights;
	int indices[ 4 ] = (int[ 4 ]) D3DCOLORtoUBYTE4( Input.BlendIndices );

	for( int bone = 0; bone < bonesNumber - 1; ++ bone )
	{
		weight = blendWeights[ bone ];
		lastWeight -= weight;
		Output.Vector1.xyz += mul(Input.Vector1, skinningPalette[ indices[ bone ] ] ) * weight;
		Output.Vector2.xyz += mul(Input.Vector2, skinningPalette[ indices[ bone ] ] ) * weight;
		Output.Vector3.xyz += mul(Input.Vector3, skinningPalette[ indices[ bone ] ] ) * weight;
	}

	Output.Vector1.xyz += mul(Input.Vector1, skinningPalette[ indices[ bonesNumber - 1 ] ] ) * lastWeight;
	Output.Vector2.xyz += mul(Input.Vector2, skinningPalette[ indices[ bonesNumber - 1 ] ] ) * lastWeight;
	Output.Vector3.xyz += mul(Input.Vector3, skinningPalette[ indices[ bonesNumber - 1 ] ] ) * lastWeight;

	Output.Vector1.w = Input.Vector1.w;
	Output.Vector2.w = Input.Vector2.w;
	Output.Vector3.w = Input.Vector3.w;

	return Output;
}

VS_SKIN_V4_OUTPUT vs_SkinningV4( const VS_SKIN_V4_INPUT Input, int bonesNumber )
{
	VS_SKIN_V4_OUTPUT Output = (VS_SKIN_V4_OUTPUT)0;
	float lastWeight = 1.0;
	float weight;
	float blendWeights[ 3 ] = (float[ 3 ]) Input.BlendWeights;
	int indices[ 4 ] = (int[ 4 ]) D3DCOLORtoUBYTE4( Input.BlendIndices );

	for( int bone = 0; bone < bonesNumber - 1; ++ bone )
	{
		weight = blendWeights[ bone ];
		lastWeight -= weight;
		Output.Vector1.xyz += mul(Input.Vector1, skinningPalette[ indices[ bone ] ] ) * weight;
		Output.Vector2.xyz += mul(Input.Vector2, skinningPalette[ indices[ bone ] ] ) * weight;
		Output.Vector3.xyz += mul(Input.Vector3, skinningPalette[ indices[ bone ] ] ) * weight;
		Output.Vector4.xyz += mul(Input.Vector4, skinningPalette[ indices[ bone ] ] ) * weight;
	}

	Output.Vector1.xyz += mul(Input.Vector1, skinningPalette[ indices[ bonesNumber - 1 ] ] ) * lastWeight;
	Output.Vector2.xyz += mul(Input.Vector2, skinningPalette[ indices[ bonesNumber - 1 ] ] ) * lastWeight;
	Output.Vector3.xyz += mul(Input.Vector3, skinningPalette[ indices[ bonesNumber - 1 ] ] ) * lastWeight;
	Output.Vector4.xyz += mul(Input.Vector4, skinningPalette[ indices[ bonesNumber - 1 ] ] ) * lastWeight;

	Output.Vector1.w = Input.Vector1.w;
	Output.Vector2.w = Input.Vector2.w;
	Output.Vector3.w = Input.Vector3.w;
	Output.Vector4.w = Input.Vector4.w;

	return Output;
}


#endif //skinning_v2_VSH

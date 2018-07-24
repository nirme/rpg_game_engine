float2 screenSize;

texture p_bmTexture;

float2 textureDimensions;

float2 textPosition;
float scale;
// 1.0f


//--------------------------------------------------------------------------------------
// Texture samplers
//--------------------------------------------------------------------------------------
sampler g_samFont =
sampler_state
{
	Texture		= <p_bmTexture>;
	MinFilter	= Anisotropic;
	MagFilter	= Anisotropic;
	MipFilter	= Linear;
};

//--------------------------------------------------------------------------------------


// Vertex shader output structure
struct VS_OUTPUT
{
	float4 Position : POSITION;
//	float4 Colour   : COLOR0;
	float2 Texture  : TEXCOORD0;
};


//--------------------------------------------------------------------------------------
// Vertex shader
//--------------------------------------------------------------------------------------

VS_OUTPUT VertFontNorm( float2 position : POSITION, 
                         float2 uvindex : TEXCOORD0, 
                         float2 letterOffset : TEXCOORD1, 
                         float2 textureOffset : TEXCOORD2, 
                         float2 letterSize : TEXCOORD3)
{
	VS_OUTPUT output;

	// Scale particle
	output.Position.x = textPosition.x + ((position.x + (uvindex.x * letterSize.x)) * scale);
	output.Position.y = textPosition.y + ((position.y + (uvindex.y * letterSize.y)) * scale);
	output.Position.z = 1.0f;
	output.Position.w = 1.0f;

	output.Texture.x = (textureOffset.x + (uvindex.x * letterSize.x)) / textureDimensions.x;
	output.Texture.y = (textureOffset.y + (uvindex.y * letterSize.y)) / textureDimensions.y;

	return output;
}

//--------------------------------------------------------------------------------------
// Pixel shader
//--------------------------------------------------------------------------------------

float4 PixScene(float2 Texture  : TEXCOORD0) : COLOR0
{
	return tex2D( g_samFont, Texture );
}


//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------


technique RenderText
{
	pass p0
	{
        VertexShader = ( vsArray30[ p_billboardind ] );
//		VertexShader = compile vs_3_0 VertScene();
		PixelShader = compile ps_3_0 PixScene();

		MultiSampleAntialias = TRUE;
		ZEnable              = FALSE;
		ZWriteEnable         = FALSE;
		CullMode             = NONE;
		AlphaBlendEnable     = TRUE;
		AlphaTestEnable      = TRUE;
		AlphaRef             = 0;
		AlphaFunc            = GREATER;
		SrcBlend             = SrcAlpha;
		DestBlend            = InvSrcAlpha;
	}
}


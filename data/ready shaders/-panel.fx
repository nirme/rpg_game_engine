// Xout =       (x / (0.5 * Wmax)) - 1
// Yout = -1 * ((y / (0.5 * Hmax)) - 1)
// Zout = 1.0f
// Wout = 1.0f

float2 screenSize;

float4 panelPosition;
float4 texturePosition;


texture p_pTexture;

sampler g_pTexture =
sampler_state
{
	Texture		= <p_pTexture>;
	MinFilter	= Anisotropic;
	MagFilter	= Anisotropic;
	MipFilter	= Linear;
};


// Vertex shader output structure
struct VS_OUTPUT
{
	float4 Position : POSITION;
	float2 Texture  : TEXCOORD0;
};



//--------------------------------------------------------------------------------------
// Vertex shader
//--------------------------------------------------------------------------------------

// Vertex shader without billboarding
VS_OUTPUT vs_Panel( float2 position : POSITION , 
                    float2 uvindex : TEXCOORD0 )
{
	VS_OUTPUT output;

	output.Position.x =  ((( 2.0f * panelPosition.x ) / screenSize.x ) - 1.0f );
	output.Position.y = -((( 2.0f * panelPosition.y ) / screenSize.y ) - 1.0f );

	output.Position.x += uvindex.x * ((( 2.0f * panelPosition.z ) / screenSize.x ) - 1.0f );
	output.Position.y -= uvindex.y * ((( 2.0f * panelPosition.w ) / screenSize.y ) - 1.0f );
	output.Position.z = 1.0f;
	output.Position.w = 1.0f;

	output.Texture.x = uvindex.x * texturePosition.z + texturePosition.x;
	output.Texture.y = uvindex.y * texturePosition.w + texturePosition.y;


	return output;
}

//--------------------------------------------------------------------------------------
// Pixel shader
//--------------------------------------------------------------------------------------

float4 ps_Panel( float2 Texture : TEXCOORD0 ) : COLOR0
{
	return tex2D( g_samFont, Texture );
}


//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------


technique RenderPanel
{
	pass p0
	{
		VertexShader = compile vs_3_0 vs_Panel();
		PixelShader =  compile ps_3_0 ps_Panel();

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


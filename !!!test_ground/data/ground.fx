
float4 lhtDir		= { 0.0f, 0.0f, -1.0f, 1.0f };  // Light Direction
float4 lightDiffuse	= { 0.6f, 0.6f, 0.6f, 1.0f };   // Light Diffuse
float4 MaterialAmbient : MATERIALAMBIENT = { 0.1f, 0.1f, 0.1f, 1.0f };
float4 MaterialDiffuse : MATERIALDIFFUSE = { 0.8f, 0.8f, 0.8f, 1.0f };

float4x4 g_mWorld : WORLD;
float4x4 g_mViewProj : VIEWPROJECTION;

texture g_txScene;


//--------------------------------------------------------------------------------------
// Texture samplers
//--------------------------------------------------------------------------------------
sampler g_samScene =
sampler_state
{
	Texture		= <g_txScene>;
	MinFilter	= Linear;
	MagFilter	= Linear;
	MipFilter	= Point;
};


//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	float4 Pos		: POSITION;
	float3 Normal	: NORMAL;
	float3 Tex0		: TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 Pos		: POSITION;
	float4 Diffuse	: COLOR0;
	float2 Tex0		: TEXCOORD0;
};


VS_OUTPUT VertScene(float4 Pos : POSITION,
					float3 Normal : NORMAL,
					float2 Tex0 : TEXCOORD0 )
{
	VS_OUTPUT o;

	o.Pos = mul( Pos, g_mWorld );
	o.Pos = mul( o.Pos, g_mViewProj );
	o.Tex0 = Tex0;
	float3 N = normalize( mul( Normal, (float3x3)g_mWorld ) );

	// Always fully lit the floor
	o.Diffuse = 1.0f;

	return o;
}

float4 PixScene(float4 Diffuse : COLOR0,
				float2 Tex0 : TEXCOORD0 ) : COLOR0
{
//float4 o = tex2D( g_samScene, Tex0 ) * Diffuse;
//o.w = 0.6f;
//return o;
	return tex2D( g_samScene, Tex0 ) * Diffuse;
}


//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------


technique RenderScene
{
	pass p0
	{
		VertexShader = compile vs_2_0 VertScene();
		PixelShader = compile ps_2_0 PixScene();

		ZEnable				= TRUE;
		ZWriteEnable		= TRUE;
		CullMode			= NONE;
		AlphaBlendEnable	= TRUE;
		AlphaTestEnable		= TRUE;
		AlphaRef			= 1;
		AlphaFunc			= GREATER;
		SrcBlend			= SrcAlpha;
		DestBlend			= InvSrcAlpha;
	}
}


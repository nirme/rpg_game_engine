
float4 lhtDir		= { 0.0f, 0.0f, -1.0f, 1.0f };  // Light Direction
float4 lightDiffuse	= { 0.6f, 0.6f, 0.6f, 1.0f };   // Light Diffuse
float4 MaterialAmbient : MATERIALAMBIENT = { 0.1f, 0.1f, 0.1f, 1.0f };
float4 MaterialDiffuse : MATERIALDIFFUSE = { 0.8f, 0.8f, 0.8f, 1.0f };

float4x4 p_mWorld : WORLD;
float4x4 p_mViewProj : VIEWPROJECTION;

texture p_ptTexture;
float2 p_sprites;

//--------------------------------------------------------------------------------------
// Texture samplers
//--------------------------------------------------------------------------------------
sampler g_samScene =
sampler_state
{
	Texture		= <p_ptTexture>;
	MinFilter	= Anisotropic;
	MagFilter	= Anisotropic;
	MipFilter	= Linear;
};


//--------------------------------------------------------------------------------------



// Vertex shader output structure
struct VS_OUTPUT
{
    float4 Position : POSITION;
    float4 Colour   : COLOR0;
    float2 Texture  : TEXCOORD0;
};


VS_OUTPUT VertScene(float4 position : POSITION,
					float2 uvindex : TEXCOORD0,
					float3 translation : TEXCOORD1, 
					float4 rotation : TEXCOORD2,
					float1 scale : TEXCOORD3,
					float1 sprite : TEXCOORD4,
					float4 color : COLOR0)
{
	// Build local matrix of a particle
	float1 sinx = sin(rotation.x);
	float1 siny = sin(rotation.y);
	float1 sinz = sin(rotation.z);
	float1 cosx = cos(rotation.x);
	float1 cosy = cos(rotation.y);
	float1 cosz = cos(rotation.z);

	// PARTICLE MATRIX
	//+---------------------------------------------+---------------------------------------------+------------------+-----+
	//| cos(y) * cos(z)                             | cos(y) * sin(z)                             | -sin(y)          |  0  |
	//+---------------------------------------------+---------------------------------------------+------------------+-----+
	//| sin(x) * sin(y) * cos(z) - cos(x) * sin(z)  | sin(x) * sin(y) * sin(z) + cos(x) * cos(z)  | sin(x) * cos(y)  |  0  |
	//+---------------------------------------------+---------------------------------------------+------------------+-----+
	//| cos(x) * sin(y) * cos(z) + sin(x) * sin(z)  | cos(x) * sin(y) * sin(z) - sin(x) * cos(z)  | cos(x) * cos(y)  |  0  |
	//+---------------------------------------------+---------------------------------------------+------------------+-----+
	//| translation.x                               | translation.y                               | translation.z    |  1  |
	//+---------------------------------------------+---------------------------------------------+------------------+-----+

	float4x4 localmx = { cosy * cosz,                         cosy * sinz,                         -siny,              0.0f,
						 sinx * siny * cosz - cosx * sinz,    sinx * siny * sinz + cosx * cosz,    sinx * cosy,      0.0f,
						 cosx * siny * cosz + sinx * sinz,    cosx * siny * sinz - sinx * cosz,    cosx * cosy,      0.0f, 
						 translation.x,                       translation.y,                       translation.z,    1.0f };



	VS_OUTPUT output;

	// Scale particle
	output.Position.x = position.x * scale;
	output.Position.y = position.y * scale;
	output.Position.z = position.z * scale;
	output.Position.w = 1.0f;

	// Transform particle to local position
	output.Position = mul(output.Position, localmx);

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.Position = mul(output.Position, p_mWorld);
	output.Position = mul(output.Position, p_mViewProj);



	// Store the input color for the pixel shader to use.
	output.Colour = color;


	// Calculate texture position based on sprite index and vertex uv index.
	sprite = floor(sprite);
	float2 uvlocal;
	uvlocal.x = sprite % p_sprites.x;
	uvlocal.y = floor(sprite / p_sprites.y) % p_sprites.y;

	output.Texture = (uvindex + uvlocal) / p_sprites;


	return output;
}


float4 PixScene(float4 Color : COLOR0, float2 Texture  : TEXCOORD0) : COLOR0
{
	return tex2D( g_samScene, Texture ) * Color;
}


//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------


technique RenderParticles
{
	pass p0
	{
		VertexShader = compile vs_3_0 VertScene();
		PixelShader = compile ps_3_0 PixScene();

		ZEnable				= TRUE;
		ZWriteEnable		= TRUE;
		CullMode			= NONE;
		AlphaBlendEnable	= TRUE;
		AlphaTestEnable		= TRUE;
		AlphaRef			= 0;
		AlphaFunc			= GREATER;
		SrcBlend			= SrcAlpha;
		DestBlend			= InvSrcAlpha;
	}
}


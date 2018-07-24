
float4 lhtDir		= { 0.0f, 0.0f, -1.0f, 1.0f };  // Light Direction
float4 lightDiffuse	= { 0.6f, 0.6f, 0.6f, 1.0f };   // Light Diffuse
float4 MaterialAmbient : MATERIALAMBIENT = { 0.1f, 0.1f, 0.1f, 1.0f };
float4 MaterialDiffuse : MATERIALDIFFUSE = { 0.8f, 0.8f, 0.8f, 1.0f };

float4x4 p_mWorld : WORLD;
float4x4 p_mViewProj : VIEWPROJECTION;
float4x4 p_mViewInv : VIEWINV;


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
					float4 particlePosition : TEXCOORD1, 
					float4 rotation : TEXCOORD2,
					float1 scale : TEXCOORD3,
					float1 sprite : TEXCOORD4,
					float4 color : COLOR0)
{

	// Calculate Z rotation of particle
	float1 sinz = sin(rotation.z);
	float1 cosz = cos(rotation.z);

	// PARTICLE Z ROTATED
	//+---------------------------------------------+---------------------------------------------+--------------+-----+
	//|  X                                          |  Y                                          |  Z           |  W  |
	//+---------------------------------------------+---------------------------------------------+--------------+-----+
	//|  particle.x * cos(z) - particle.y * sin(z)  |  particle.x * sin(z) + particle.y * cos(z)  |  particle.z  |  1  |
	//+---------------------------------------------+---------------------------------------------+--------------+-----+

	//remove translation from matrix
	p_mViewInv[3][0] = 0.0f;
	p_mViewInv[3][1] = 0.0f;
	p_mViewInv[3][2] = 0.0f;

	// Rotate particle with view inversion (billboarding type 1)
	position = mul( float4((position.x * cosz - position.y * sinz) * scale, (position.x * sinz + position.y * cosz) * scale, position.z * scale, 1.0f),
	                p_mViewInv);

	// Move particle position to world coords
	particlePosition = mul(particlePosition, p_mWorld);

	VS_OUTPUT output;

	// Add vertex position to particle world position
	output.Position.x = particlePosition.x + position.x;
	output.Position.y = particlePosition.y + position.y;
	output.Position.z = particlePosition.z + position.z;
	output.Position.w = 1.0f;

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


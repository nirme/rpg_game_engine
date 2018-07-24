
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
	// Move particle position to world coords
	particlePosition = mul(particlePosition, p_mWorld);

	// Calculate matrix to set particles to face view point	
	// Construct something like a view inversion matrix but instead of facing on view plane we want 
	// each particle to face view point to gain effect of "looking" at the viewer
	// Facing direction (lookAt) is pretty much normal of a plane on which we will render sprite
	//
	// right vector is normalized cross product of arbitrary up direction unit vector (0,1,0) and normalized facing direction vector
	// up vector is normalized cross product of normalized facing direction vector and calculated before right vector
	// right = staticUp x lookAt
	// up = lookAt x right
	//
	// +----------------------------+----------------------------+----------------------------+--------+
	// |  right.x                   |  right.y                   |  right.z                   |  0.0f  |
	// +----------------------------+----------------------------+----------------------------+--------+
	// |  up.x                      |  up.y                      |  up.z                      |  0.0f  |
	// +----------------------------+----------------------------+----------------------------+--------+
	// |  normalizedFacingVector.x  |  normalizedFacingVector.y  |  normalizedFacingVector.z  |  0.0f  |
	// +----------------------------+----------------------------+----------------------------+--------+
	// |  0.0f                      |  0.0f                      |  0.0f                      |  1.0f  |
	// +----------------------------+----------------------------+----------------------------+--------+

	float3 particleFacing = normalize(float3(p_mViewInv[3][0] - particlePosition.x, p_mViewInv[3][1] - particlePosition.y, p_mViewInv[3][2] - particlePosition.z));
	float3 xaxis;
	xaxis = normalize(cross(float3(0.0f, 1.0f, 0.0), particleFacing));
	float3 yaxis;
	yaxis = normalize(cross(particleFacing, xaxis));

	
	float4x4 localmx = { xaxis.x,            xaxis.y,            xaxis.z,   		 0.0f, 
						 yaxis.x,            yaxis.y,            yaxis.z,            0.0f, 
						 particleFacing.x,   particleFacing.y,   particleFacing.z,   0.0f, 
						 0.0f,               0.0f,               0.0f,               1.0f };

	//rotate particle by Z axis
	float1 sinz = sin(rotation.z);
	float1 cosz = cos(rotation.z);
	position = float4((position.x * cosz - position.y * sinz) * scale, (position.x * sinz + position.y * cosz) * scale, position.z * scale, 1.0f);

	//scale and transform vertex position
	position = mul(position * scale, localmx);

	

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
//float4 o = {0.0f, 0.0f, 0.0f, 1.0f};
//return o;

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


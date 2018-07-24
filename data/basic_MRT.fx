float4 MaterialAmbient : MATERIALAMBIENT = { 0.1f, 0.1f, 0.1f, 1.0f };
float4 MaterialDiffuse : MATERIALDIFFUSE = { 0.8f, 0.8f, 0.8f, 1.0f };


float4x4 p_mWorld : WORLD;
float4x4 p_mViewProj : VIEWPROJECTION;
float4x4 p_mViewInv : VIEWINV;


/*
struct VS_OUTPUT2 {
    float4 Position  : POSITION;
    float3 Normal    : TEXCOORD0;
    float2 TexCoord  : TEXCOORD1;
    float4 Position2 : TEXCOORD2;
};
*/

struct PS_MRT_OUTPUT
{
	float4 color : COLOR0;
	float4 normal : COLOR1;
	float1 depth : DEPTH0;
};


//////////////////////




struct VS_INPUT2 {
    float4 Position : POSITION; //in object space
    float3 Normal   : NORMAL;
    float2 TexCoord : TEXCOORD0;
};

struct VS_OUTPUT2 {
    float4 Position  : POSITION;
    float3 Normal    : TEXCOORD0;
    float2 TexCoord  : TEXCOORD1;
    float4 Position2 : TEXCOORD2;
};

struct PS_MRT_OUTPUT {
	float4 Color0 : COLOR0;
	float4 Color1 : COLOR1;
	float4 Color2 : COLOR2;
};


VS_OUTPUT2 SimpleVSInterpolatedNormal(VS_INPUT2 In)
{
    VS_OUTPUT2 Out;

    //normal in view space
    float3 NormalView = mul(In.Normal, WorldViewIT);
    
    float3 ViewPos = mul(In.Position, WorldView);
    
    Out.Normal = normalize(NormalView);
    Out.Position = mul(In.Position, WorldViewProj);
    Out.Position2 = mul(In.Position, WorldViewProj);
    Out.TexCoord = In.TexCoord;

	return Out;
}



PS_MRT_OUTPUT SimplePSInterpolatedNormal(VS_OUTPUT2 In)
{
	PS_MRT_OUTPUT Out;
	
	half4 diffuseTex = tex2D( DiffuseMapSampler, In.TexCoord);
	half3 normal = normalize(In.Normal);

	diffuseTex.w = diffuseTex.w - 0.01;
	clip(diffuseTex.www);
	
	//pack
	normal = normal * 0.5f + 0.5f;
	
	Out.Color0 = float4(diffuseTex.xyz, NotShadowed);
	Out.Color1 = float4(normal, 0.0);
	Out.Color2 = float4(In.Position2.z / In.Position2.w, 0.0f, 0.0f, 0.0f);

	return Out;
}


/*
texture p_ptTexture;
float2 p_sprites;
uint p_billboardind = 0;

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


//--------------------------------------------------------------------------------------
// Vertex shader
//--------------------------------------------------------------------------------------

// Vertex shader without billboarding
VS_OUTPUT VertSceneNorm( float4 position : POSITION, 
                         float2 uvindex : TEXCOORD0, 
                         float3 translation : TEXCOORD1, 
                         float4 rotation : TEXCOORD2, 
                         float1 scale : TEXCOORD3, 
                         float1 sprite : TEXCOORD4, 
                         float4 color : COLOR0 )
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

//--------------------------------------------------------------------------------------


// Vertex shader viewplane billboarding
VS_OUTPUT VertSceneBillPlane( float4 position : POSITION, 
                              float2 uvindex : TEXCOORD0, 
                              float4 particlePosition : TEXCOORD1, 
                              float4 rotation : TEXCOORD2, 
                              float1 scale : TEXCOORD3, 
                              float1 sprite : TEXCOORD4, 
                              float4 color : COLOR0 )
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

//--------------------------------------------------------------------------------------


// Vertex shader viewpoint billboarding
VS_OUTPUT VertSceneBillPoint( float4 position : POSITION, 
                              float2 uvindex : TEXCOORD0, 
                              float4 particlePosition : TEXCOORD1, 
                              float4 rotation : TEXCOORD2, 
                              float1 scale : TEXCOORD3, 
                              float1 sprite : TEXCOORD4, 
float4 color : COLOR0 )
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

	float3 particleFacing = normalize(float3(particlePosition.x - p_mViewInv[3][0], particlePosition.y - p_mViewInv[3][1], particlePosition.z - p_mViewInv[3][2]));
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
	position = float4(position.x * cosz - position.y * sinz, position.x * sinz + position.y * cosz, position.z, 1.0f);

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


VertexShader vsArray30[ 3 ] = 
{
	compile vs_3_0 VertSceneNorm(),
	compile vs_3_0 VertSceneBillPlane(),
	compile vs_3_0 VertSceneBillPoint()
};


//--------------------------------------------------------------------------------------
// Pixel shader
//--------------------------------------------------------------------------------------

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
        VertexShader = ( vsArray30[ p_billboardind ] );
//		VertexShader = compile vs_3_0 VertScene();
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


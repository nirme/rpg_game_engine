#include "post_structs.vsh"


float3    cameraPosition;
float4x4  cameraInvViewProjectionMx;

float3    lightAmbient = 0.0f;
#define   LIGHT_TABLE_SIZE    6
float4    lightInfoTable [LIGHT_TABLE_SIZE];


// light array setup:
//   lightInfoTable[0] = directional direction or point and spot position
//   lightInfoTable[1] = color diffuse.RGBx
//   lightInfoTable[2] = color specular.RGBx
//   lightInfoTable[3] = { range, attenuation0, attenuation1, attenuation2 }
// spotlight part:
//   lightInfoTable[4] = { is spot, cos hotspot, cos cutoff, falloff }
//   lightInfoTable[5] = spot light direction
//
// math: 
//       D = light direction normalized
//       L = light to point direction normalized
//       d = length of L
//       V = point to camera / eye direction normalized
//       N = point normal vector
//       R = reflected L
//       H = halfway vector - L + V normalized
//
//       lum = 1 / att0 + att1 * d + att2 * d^2
//
//       ambi = material color * light color
//       emis = material color * emissive factor
//       diff = material color * light color * (dot(N, L) : {0,1}) * lum
//
//    Phong model:
//       spec = light color * (spec intensity * ( (dot(R, V) : {0,1}) ^ specularPower )) * lum
//
//    Blinn-Phong model:
//       spec = light color * (spec intensity * ( (dot(N, H) : {0,1}) ^ specularPower )) * lum
//
//    spotlight:
//       lum2 = (-( dot(D, L) - cutoff) / (hotspot - cutoff)) ^ falloff
//       lum *= lum2
//
//
//       output = ambi + diff + spec + emis
//
//       may be better: output = max(emis , (ambi + diff + spec))
//
//       for distance from light (d) in luminance/attenuation might be better to use 
//       values from 1 to very big value to ease the usage of attenuation factors:
//       d = |L| / range
//       attenuation = att0 + att1 * d + att2 * d^2
//       luminance = 1 / (attenuation * 255 + 1)




// AMBIENT

float4 ps_RenderLTAmbient(VS_FS_QUAD_IN_OUT Input) : COLOR0
{
	float3 color = tex2D(MRT0Sampler, Input.TexCoord).rgb;
	float emissive = tex2D(MRT2Sampler, Input.TexCoord).g;
	float4 Output = float4(color * lightAmbient + color * emissive, 1);

	return Output;
}


//--------------------------------------------------------------------------------------
// Technique
//--------------------------------------------------------------------------------------

technique RenderLTAmbient
{
	pass p0
	{
		VertexShader = compile vs_3_0 vs_PassThrough();
		PixelShader = compile ps_3_0 ps_RenderLTAmbient();

		ZEnable				= FALSE;
		ZWriteEnable		= FALSE;
		CullMode			= NONE;
	}
}


// DIRECTIONAL

float4 ps_RenderLTDir(VS_FS_QUAD_IN_OUT Input) : COLOR0
{
	float4 mrt0 = tex2D(MRT0Sampler, Input.TexCoord);
	float4 mrt1 = tex2D(MRT1Sampler, Input.TexCoord);
	float depth = tex2D(MRT2Sampler, Input.TexCoord).r;

	float3 normal = 2.0f * mrt1.xyz - 1.0f;
	float4 position = mul(float4(Input.Position2.x, Input.Position2.y, depth, 1.0f), cameraInvViewProjectionMx);
	position /= position.w;

	float3 L = -(lightInfoTable[0].xyz);

	// cos between point normal and light direction
	float dotNL = dot(normal, L);
	clip(dotNL);

	float3 halfdir = normalize(L + normalize(cameraPosition - position.xyz));

	// diffuse
	float3 Output = mrt0.xyz * lightInfoTable[1].xyz * dotNL;
	// specular
	Output += lightInfoTable[2].xyz * mrt0.w * ( saturate(dot(normal, halfdir)), mrt1.w * 255 );

	return float4(Output, 1.0f);
}


//--------------------------------------------------------------------------------------
// Technique
//--------------------------------------------------------------------------------------

technique RenderLTDir
{
	pass p0
	{
		VertexShader = compile vs_3_0 vs_PassThrough();
		PixelShader = compile ps_3_0 ps_RenderLTDir();

		ZEnable				= FALSE;
		ZWriteEnable		= FALSE;
		CullMode			= NONE;

		BlendEnable[0]		= TRUE;
		SrcBlend			= ONE;
		DestBlend			= ONE;
		BlendOp				= ADD;
   }
}


// POINT

float4 ps_RenderLTOmni(VS_FS_QUAD_IN_OUT Input) : COLOR0
{
	float4 mrt0 = tex2D(MRT0Sampler, Input.TexCoord);
	float4 mrt1 = tex2D(MRT1Sampler, Input.TexCoord);
	float depth = tex2D(MRT2Sampler, Input.TexCoord).r;

	float3 normal = 2.0f * mrt1.xyz - 1.0f;
	float4 position = mul(float4(Input.Position2.x, Input.Position2.y, depth, 1.0f), cameraInvViewProjectionMx);
	position /= position.w;

	float3 D = lightInfoTable[0].xyz - position.xyz;
	float d = length(D);

	if (lightInfoTable[3].x < d)
		discard;

	d /= lightInfoTable[3].x;
	D = normalize(D);

	// cos between point normal and light direction
	float dotNL = dot(normal, D);
	clip(dotNL);

	// half vector between point-to-light and point-to-camera direction
	float3 halfdir = normalize(D + normalize(cameraPosition - position.xyz));

	// diffuse
	float3 Output = mrt0.xyz * lightInfoTable[1].xyz * dotNL;
	// specular
	Output += lightInfoTable[2].xyz * mrt0.w * pow(saturate(dot(normal, halfdir)), mrt1.w * 255);

	// divide by inverted lum1
	Output /= (lightInfoTable[3].y + lightInfoTable[3].z * d + lightInfoTable[3].w * d * d) * 255.0f + 1.0f;

	return float4(Output, 1.0f);
}


//--------------------------------------------------------------------------------------
// Technique
//--------------------------------------------------------------------------------------

technique RenderLTOmni
{
	pass p0
	{
		VertexShader = compile vs_3_0 vs_PassThrough();
		PixelShader = compile ps_3_0 ps_RenderLTOmni();

		ZEnable				= FALSE;
		ZWriteEnable		= FALSE;
		CullMode			= NONE;

		BlendEnable[0]		= TRUE;
		SrcBlend			= ONE;
		DestBlend			= ONE;
		BlendOp				= ADD;
	}
}


// SPOT

float4 ps_RenderLTSpot(VS_FS_QUAD_IN_OUT Input) : COLOR0
{
	float4 mrt0 = tex2D(MRT0Sampler, Input.TexCoord);
	float4 mrt1 = tex2D(MRT1Sampler, Input.TexCoord);
	float depth = tex2D(MRT2Sampler, Input.TexCoord).r;

	float3 normal = 2.0f * mrt1.xyz - 1.0f;
	float4 position = mul(float4(Input.Position2.x, Input.Position2.y, depth, 1.0f), cameraInvViewProjectionMx);
	position /= position.w;

	float3 D = lightInfoTable[0].xyz - position.xyz;
	float d = length(D);

	if (lightInfoTable[3].x < d)
		discard;

	d /= lightInfoTable[3].x;
	D = normalize(D);

	float dotLD = dot(lightInfoTable[5].xyz, -D);
	// clip if point is outside the cone
	if (dotLD < lightInfoTable[4].z)
		discard;

	// cos between point normal and light direction
	float dotNL = dot(normal, D);
	clip(dotNL);

	// half vector between point-to-light and point-to-camera direction
	float3 halfdir = normalize(D + normalize(cameraPosition - position.xyz));

	// diffuse
	float3 Output = mrt0.xyz * lightInfoTable[1].xyz * dotNL;
	// specular
	Output += lightInfoTable[2].xyz * mrt0.w * pow(saturate(dot(normal, halfdir)), mrt1.w * 255.0f);

	// divide by inverted lum1
	Output /= (lightInfoTable[3].y + lightInfoTable[3].z * d + lightInfoTable[3].w * d * d) * 255.0f + 1.0f;

	// return if point inside inner light cone
	if ((dotLD - lightInfoTable[4].y) >= 0.0f)
		return float4(Output, 1.0f);

	// else multiply by lum2
	Output *= pow( -(dotLD - lightInfoTable[4].z) / (lightInfoTable[4].y - lightInfoTable[4].z) , lightInfoTable[4].w);
	return float4(Output, 1.0f);
}

//--------------------------------------------------------------------------------------
// Technique
//--------------------------------------------------------------------------------------

technique RenderLTSpot
{
	pass p0
	{
		VertexShader = compile vs_3_0 vs_PassThrough();
		PixelShader = compile ps_3_0 ps_RenderLTSpot();

		ZEnable				= FALSE;
		ZWriteEnable		= FALSE;
		CullMode			= NONE;

		BlendEnable[0]		= TRUE;
		SrcBlend			= ONE;
		DestBlend			= ONE;
		BlendOp				= ADD;
	}
}

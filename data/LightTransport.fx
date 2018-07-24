
#include "post_structs.vsh"

//ShadowMapSampler
//ShadowMapFrontSampler
//ShadowMapBackSampler
//MRT0Sampler
//MRT1Sampler
//MRT2Sampler

///////////////////////////////////////////////////////////
////  Ambient + emissive                               ////
///////////////////////////////////////////////////////////

float4 ambient = {0.2f, 0.2f, 0.2f, 0.2f};


float4 ps_LightTransport_Ambient(VS_FS_QUAD_IN_OUT input)
{
	half3 color = tex2D(MRT0Sampler, input.TexCoord ).xyz;
	half3 output = ambient * color;
	return float4(output, 1.0f);
}

float4 ps_LightTransport_Ambient_Emissive(VS_FS_QUAD_IN_OUT input)
{
	half3 color = tex2D(MRT0Sampler, input.TexCoord ).xyz;
	half1 emissive = tex2D(MRT2Sampler, input.TexCoord).y;
	half3 output = ambient * color + emissive * color;
	return float4(output, 1.0f);
}


technique LightTransportAmbient
{
    pass p0
    {
        VertexShader = compile vs_2_0 vs_PassThrough();
        PixelShader = compile ps_2_0 ps_LightTransport_Ambient();
    }
}

technique LightTransportAmbientEmissive
{
    pass p0
    {
        VertexShader = compile vs_2_0 vs_PassThrough();
        PixelShader = compile ps_2_0 ps_LightTransport_Ambient_Emissive();
    }
}




///////////////////////////////////////////////////////////
////  Light transport post                             ////
///////////////////////////////////////////////////////////


float4x4 InvViewProj;
float4x4 InvProj;

float3 LightPosition;

// InvView * light view * light proj ( * ?texScaleBiasMat)
float4x4 ShadowMat;



//ShadowMapSampler
//ShadowMapFrontSampler
//ShadowMapBackSampler
//MRT0Sampler
//MRT1Sampler
//MRT2Sampler


float4 ps_LightTransport_Directional(VS_FS_QUAD_IN_OUT input)
{
	// color.RGB, specular intensity
	half4 diffuseTex  = tex2D(MRT0Sampler, input.TexCoord);
	// normal.XYZ, specular power
	half4 normalTex   = tex2D(MRT1Sampler, input.TexCoord);
	// depth
	float z           = tex2D(MRT2Sampler, input.TexCoord).x;


	half3 finalLighting = half3(0.0, 0.0, 0.0);



	//reconstruct original view-space position
	float4 positionViewSpace = mul(float4(input.Position2.x, input.Position2.y, z, 1.0), InvProj);
	positionViewSpace.xyz = positionViewSpace.xyz / positionViewSpace.www;
	positionViewSpace.w = 1.0f;


	//compute position in light space
	float4 positionLightSpace = mul(positionViewSpace, ShadowMat);
	half shadow = tex2Dproj(ShadowMapSampler, positionLightSpace);
	shadow += isNotShadowed;  //if not shadowed, push shadow to 1 (no shadow)
	shadow = saturate(shadow);
	//////////////////////////


//	tex2D(ShadowMapSampler);


	
	half3 normal = normalTex.xyz * 2 - 1;
	half3 albedo = diffuseTex.xyz;
	half3 emissive = albedo * normalTex.w * 10.0f;
	
	normal = normalize(normal);
	
	//diffuse lighting
	half NdotL = dot(normal, LightVector);//float3(0.0, 0.0, -1.0));
	half selfShadow = (NdotL > 0) ? 1 : 0;
	half3 diffuse = albedo * NdotL * selfShadow * LightColor;

	finalLighting = shadow * diffuse + Ambient + emissive;

	return float4(finalLighting, 1.0f);


}


float4 ps_LightTransport(VS_FS_QUAD_IN_OUT input)
{
	float4 output = {1.0f, 1.0f, 1.0f, 1.0f};
	return output;
}




float4 ps_LightTransport_Point(VS_FS_QUAD_IN_OUT input)
{
}

float4 ps_LightTransport_Spot(VS_FS_QUAD_IN_OUT input)
{
}





technique LightTransportDirectional
{
    pass p0
    {
        VertexShader = compile vs_2_0 vs_PassThrough();
        PixelShader = compile ps_2_0 ps_LightTransport_Directional();
    }
}

technique LightTransportPoint
{
    pass p0
    {
        VertexShader = compile vs_2_0 vs_PassThrough();
        PixelShader = compile ps_2_0 ps_LightTransport_Point();
    }
}

technique LightTransportSpot
{
    pass p0
    {
        VertexShader = compile vs_2_0 vs_PassThrough();
        PixelShader = compile ps_2_0 ps_LightTransport_Spot();
    }
}

#include "DR_structs.vsh"


float4 VSclearMRT( float4 position : POSITION0 )
{
	return position;
}

PS_MRT_OUTPUT PSclearMRT()
{
	PS_MRT_OUTPUT output;

	output.Color = 0.0f;
	output.Normal = 0.0f;
	output.Depth.x = 1.0f;
	output.Depth.yzw = 0.0f;

	return output;
}


//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------

technique MRTClearTechnique
{
	pass p0
	{
		VertexShader = compile vs_3_0 VSclearMRT();
		PixelShader = compile ps_3_0 PSclearMRT();

		ZEnable				= FALSE;
		ZWriteEnable		= FALSE;
		CullMode			= NONE;
	}
}

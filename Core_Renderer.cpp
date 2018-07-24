#include "Core_Renderer.h"




int Renderer::setupFullscreenQuad()
{
	// account for DirectX's texel center standard (NVIDIA's tricks)
	
	float u_adjust = 0.5f / (float) displayMode.Width;
	float v_adjust = 0.5f / (float) displayMode.Height;

	FullscreenQuadVertex quadVertices[4];

	// TRIANGLEFAN
	// 1-------2
	// | t1  / |
	// |   /   |
	// | /  t2 |
	// 0-------3

	quadVertices[0] = FullscreenQuadVertex( -1.0f, -1.0f, 0.0f, 0.0f + u_adjust, 1.0f + v_adjust);
	quadVertices[1] = FullscreenQuadVertex( -1.0f,  1.0f, 0.0f, 0.0f + u_adjust, 0.0f + v_adjust);
	quadVertices[2] = FullscreenQuadVertex(  1.0f,  1.0f, 0.0f, 1.0f + u_adjust, 0.0f + v_adjust);
	quadVertices[3] = FullscreenQuadVertex(  1.0f, -1.0f, 0.0f, 1.0f + u_adjust, 1.0f + v_adjust);


	HRESULT hr = S_OK;

	if (FAILED(hr = pDevice->CreateVertexBuffer(4 * sizeof(FullscreenQuadVertex), D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &p_fullscreenQuad, NULL)))
	{
		showError(false, "IDirect3DDevice9::CreateVertexBuffer error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -1;
	}

	void* pVoid = NULL;

	if (FAILED(hr = p_fullscreenQuad->Lock(0, 0, &pVoid, 0)))
	{
		showError(false, "IDirect3DVertexBuffer9::Lock error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -2;
	}

	memcpy(pVoid, quadVertices, 4 * sizeof(FullscreenQuadVertex));

	if (FAILED(hr = p_fullscreenQuad->Unlock()))
	{
		showError(false, "IDirect3DVertexBuffer9::Unlock error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -3;
	}


	D3DVERTEXELEMENT9 quadDeclaration[] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 }, 
		{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};

	if (FAILED(hr = pDevice->CreateVertexDeclaration(quadDeclaration, &p_fullscreenQuadVertexDecl)))
	{
		p_fullscreenQuad->Release();
		showError(false, "IDirect3DDevice9::CreateVertexDeclaration error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -4;
	}

	return 0;
};


int Renderer::releaseFullscreenQuad()
{
	IDX_SAFE_RELEASE(p_fullscreenQuad);
	IDX_SAFE_RELEASE(p_fullscreenQuadVertexDecl);
	return 0;
};



Renderer::Renderer() :	initialized(false), 
						pDevice(NULL), 
						p_backBuffer(NULL), 
						p_depthBuffer(NULL), 
						p_fullscreenQuad(NULL), 
						p_fullscreenQuadVertexDecl(NULL), 
						defaultBackgroundColor(0), 
						maxLightsUsed(-1), 
						supportsHWShadowMaps(false), 
						supports64bitRT(false), 
						pGUISprite(NULL), 
						guiSystem(NULL), 
						usedCamera(NULL)
{
	ZeroMemory(&displayMode, sizeof(D3DDISPLAYMODE));
	ambientLight = D3DXCOLOR(0);
};


Renderer::~Renderer()
{
	if (initialized)
		shutdown();
};



int Renderer::initialize(IDirect3DDevice9* _pDevice, D3DCAPS9* _deviceCaps, D3DDISPLAYMODE* _displayMode, bool _use64bitRenderTargets, bool _use64bitLightTransport, bool _useShadowMapping, bool _use64bitShadowMaps, UINT _shadowMapResolution, UINT _blurSamples)
{
	if (initialized)
		return 1;

	int result = 0;

	if ((result = checkDeviceCapabilities( _pDevice, _deviceCaps, _displayMode)) < 0)
		return -100 + result;


	pDevice = _pDevice;
	memcpy(&displayMode, _displayMode, sizeof(D3DDISPLAYMODE));

	pGUISprite = NULL;
	p_backBuffer = NULL;
	p_depthBuffer = NULL;


	if (FAILED(hr = D3DXCreateSprite(pDevice, &pGUISprite)))
	{
		showError(false, "D3DXCreateSprite error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -102;
	}


	if (FAILED(hr = pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &p_backBuffer)))
	{
		showError(false, "IDirect3DDevice9::GetBackBuffer error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -103;
	}

	if (FAILED(hr = pDevice->GetDepthStencilSurface(&p_depthBuffer)))
	{
		showError(false, "IDirect3DDevice9::GetDepthStencilSurface error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -104;
	}



	if ((result = setupFullscreenQuad()) < 0)
	{
		pDevice = NULL;
		return -200 + result;
	}

	int warnings = 0;

	if (result = setupRenderTargets(_use64bitRenderTargets && supports64bitRT))
	{
		if (result < 0)
		{
			releaseFullscreenQuad();
			pDevice = NULL;
			return -300 + result;
		}

		warnings += 10;
	}


	if (_useShadowMapping)
	{
		if (result = setupShadowMaps(true, _use64bitShadowMaps && supports64bitRT, _shadowMapResolution, true, _blurSamples))
		{
			if (result < 0)
			{
				releaseRenderTargets();
				releaseFullscreenQuad();
				pDevice = NULL;
				return -400 + result;
			}

			warnings += 100;
		}
	}


	if (result = setupLightTransport(_use64bitLightTransport && supports64bitRT, _useShadowMapping, true))
	{
		if (result < 0)
		{
			releaseShadowMaps();
			releaseRenderTargets();
			releaseFullscreenQuad();
			pDevice = NULL;
			return -500 + result;
		}

		warnings += 1000;
	}

	if (result = setupParticleSystemEffect())
	{
		if (result < 0)
		{
			releaseShadowMaps();
			releaseRenderTargets();
			releaseFullscreenQuad();
			releaseLightTransport();
			pDevice = NULL;
			return -600 + result;
		}

		warnings += 10000;
	}


	defaultBackgroundColor = D3DCOLOR_XRGB(0, 0, 0);
	maxLightsUsed = 0xFFFFFFFF;

	guiSystem = NULL;
	usedCamera = NULL;
	ambientLight = D3DXCOLOR(D3DCOLOR_XRGB(100, 100, 100);


	initialized = true;

	return warnings;
};



int Renderer::shutdown()
{
	if (!initialized)
		return 1;

	pDevice = NULL;
	ZeroMemory(&displayMode, sizeof(D3DDISPLAYMODE));

	IDX_SAFE_RELEASE(pGUISprite);
	IDX_SAFE_RELEASE(p_backBuffer);
	IDX_SAFE_RELEASE(p_depthBuffer);

	int warnings = 0;

	if (releaseFullscreenQuad())
		warnings += -1;

	if (releaseRenderTargets())
		warnings += -10;

	if (releaseShadowMaps())
		warnings += -100;

	if (releaseLightTransport())
		warnings += -1000;

	if (releaseParticleSystemEffect)
		warnings += -10000;


	defaultBackgroundColor = 0;
	maxLightsUsed = 0;
	guiSystem = NULL;
	usedCamera = NULL;
	ambientLight = D3DXCOLOR(0);


	initialized = false;

	return warnings;
};



int Renderer::render()
{
	if (!usedCamera)
		return -1;


	HRESULT hr = S_OK;


	if (FAILED(hr = pDevice->BeginScene()))
	{
		showError(false, "IDirect3DDevice9::BeginScene error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -101;
	}

	if (FAILED(hr = pGUISprite->Begin(0/*D3DXSPRITE_DONOTSAVESTATE | D3DXSPRITE_DONOTMODIFY_RENDERSTATE | D3DXSPRITE_ALPHABLEND*/)))
	{
		pDevice->EndScene();

		showError(false, "ID3DXSprite::Begin error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -102;
	}


	int warnings = 0;

	if (renderToMRT())
		warnings += 1;

	if (renderLightTransport())
		warnings += 10;

	if (passLightTransportToBackBuffer())
		warnings += 100;

	if (renderBackToFront())
		warnings += 1000;

	if (renderParticles())
		warnings += 10000;


	if (pGUISprite)
	{
		if (render2DElements())
			warnings += 100000;

		if (guiSystem)
		{
			if (renderGUI())
				warnings += 1000000;
		}
	}



	// clear lists after rendering
	l_renderChunks.clear();
	l_backToFrontRenderedChunks.clear();
	l_usedLightsWithShadows.clear();
	l_particles.clear();
	l_2dElements.clear();



	if (FAILED(hr = pDevice->EndScene()))
	{
		showError(false, "IDirect3DDevice9::EndScene error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
	}

	if (FAILED(hr = pGUISprite->End()))
	{
		showError(false, "ID3DXSprite::End error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -104;
	}


	return 0;
};




int Renderer::checkDeviceCapabilities(IDirect3DDevice9* _pDevice, D3DCAPS9* _deviceCaps, D3DDISPLAYMODE* _displayMode)
{
	if (!_pDevice || !_deviceCaps || !_displayMode)
		return 1;

	if (_deviceCaps->NumSimultaneousRTs < 4)
	{
		showError(false, "Device/driver does not support 4 render targets.");
		return -2;
	}


	{ //hw shadowmaps
		if (!(supportsHWShadowMaps = SUCCEEDED(_pDevice->CheckDeviceFormat(	D3DADAPTER_DEFAULT, 
																			_deviceCaps->DeviceType, 
																			_displayMode->Format, 
																			D3DUSAGE_DEPTHSTENCIL, 
																			D3DRTYPE_TEXTURE, 
																			D3DFMT_D32F_LOCKABLE))))
		{
			showError(false, "Device/driver does not support hardware shadow maps.");
		}
	}


	// support 64 bit targets
	{
		bool RT_4e16b = SUCCEEDED(_pDevice->CheckDeviceFormat(	D3DADAPTER_DEFAULT, 
																_deviceCaps->DeviceType, 
																_displayMode->Format, 
																D3DUSAGE_RENDERTARGET, 
																D3DRTYPE_TEXTURE, 
																D3DFMT_A16B16G16R16F));

		bool PPS_4e16b = SUCCEEDED(_pDevice->CheckDeviceFormat(	D3DADAPTER_DEFAULT, 
																_deviceCaps->DeviceType, 
																_displayMode->Format, 
																D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, 
																D3DRTYPE_TEXTURE, 
																D3DFMT_A16B16G16R16F));

		bool RT_2e32b = SUCCEEDED(_pDevice->CheckDeviceFormat(	D3DADAPTER_DEFAULT, 
																_deviceCaps->DeviceType, 
																_displayMode->Format, 
																D3DUSAGE_RENDERTARGET, 
																D3DRTYPE_TEXTURE, 
																D3DFMT_G32R32F));

		bool PPS_2e32b = SUCCEEDED(_pDevice->CheckDeviceFormat(	D3DADAPTER_DEFAULT, 
																_deviceCaps->DeviceType, 
																_displayMode->Format, 
																D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, 
																D3DRTYPE_TEXTURE, 
																D3DFMT_G32R32F));


		if (RT_4e16b && PPS_4e16b && RT_2e32b && PPS_2e32b)
			supports64bitRT = true;
		else
		{
			supports64bitRT = false;
			showError(false, "Device/driver does not support 64bit render targets.");
		}
	}


	if (!supports64bitRT)
	{
		bool RT_4e8b = SUCCEEDED(_pDevice->CheckDeviceFormat(	D3DADAPTER_DEFAULT, 
																_deviceCaps->DeviceType, 
																_displayMode->Format, 
																D3DUSAGE_RENDERTARGET, 
																D3DRTYPE_TEXTURE, 
																D3DFMT_A8R8G8B8));

		bool RT_2e16b = SUCCEEDED(_pDevice->CheckDeviceFormat(	D3DADAPTER_DEFAULT, 
																_deviceCaps->DeviceType, 
																_displayMode->Format, 
																D3DUSAGE_RENDERTARGET, 
																D3DRTYPE_TEXTURE, 
																D3DFMT_G16R16F));

		if (!RT_4e8b || !!RT_2e16b)
		{
			showError(false, "Device/driver does not support 32bit render targets.");
			return -3;
		}
	}

	return 0;
};


int Renderer::setupRenderTargets(bool _use64bitRenderTargets)
{
	D3DFORMAT RTvec = D3DFMT_A8R8G8B8;
	D3DFORMAT RTdepth = D3DFMT_G16R16F;
	MRTState.use64bitRT = false;

	if (use64bitRT = supports64bitRT && _use64bitRenderTargets)
	{
		RTvec = D3DFMT_A16B16G16R16F;
		RTdepth = D3DFMT_G32R32F;
		MRTState.use64bitRT = true;
	}

	if (!(MRTState.clearMRTeffect = Effect::getResource(string(EFFECT_ID_MRT_CLEAR) + (MRTState.use64bitRT ? EFFECT_64B_DEPTH : EFFECT_32B_DEPTH))))
	{
		releaseRenderTargets();
		return -1;
	}

	HRESULT hr = S_OK;

	MRTState.v_renderTargets.resize(3);

	//color
	if (FAILED(hr = pDevice->CreateTexture(	displayMode.Width, 
											displayMode.Height, 
											1, 
											D3DUSAGE_RENDERTARGET, 
											RTvec, 
											D3DPOOL_DEFAULT, 
											&(MRTState.v_renderTargets[0].renderTexture), 
											NULL)))
	{
		releaseRenderTargets();
		showError(false, "IDirect3DDevice9::CreateTexture error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -11;
	}

	if (FAILED(hr = MRTState.v_renderTargets[0].renderTexture->GetSurfaceLevel(0, &(MRTState.v_renderTargets[0].renderTarget))))
	{
		releaseRenderTargets();
		showError(false, "IDirect3DTexture9::GetSurfaceLevel error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -12;
	}


	//normal
	if (FAILED(hr = _pDevice->CreateTexture(displayMode.Width, 
											displayMode.Height, 
											1, 
											D3DUSAGE_RENDERTARGET, 
											RTvec, 
											D3DPOOL_DEFAULT, 
											&(MRTState.v_renderTargets[1].renderTexture), 
											NULL)))
	{
		releaseRenderTargets();
		showError(false, "IDirect3DDevice9::CreateTexture error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -11;
	}

	if (FAILED(hr = MRTState.v_renderTargets[1].renderTexture->GetSurfaceLevel(0, &(MRTState.v_renderTargets[1].renderTarget))))
	{
		releaseRenderTargets();
		showError(false, "IDirect3DTexture9::GetSurfaceLevel error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -12;
	}


	if (FAILED(hr = _pDevice->CreateTexture(displayMode.Width, 
											displayMode.Height, 
											1, 
											D3DUSAGE_RENDERTARGET, 
											RTdepth, 
											D3DPOOL_DEFAULT, 
											&(MRTState.v_renderTargets[2].renderTexture), 
											NULL)))
	{
		releaseRenderTargets();
		showError(false, "IDirect3DDevice9::CreateTexture error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -11;
	}

	if (FAILED(hr = MRTState.v_renderTargets[2].renderTexture->GetSurfaceLevel(0, &(MRTState.v_renderTargets[2].renderTarget))))
	{
		releaseRenderTargets();
		showError(false, "IDirect3DTexture9::GetSurfaceLevel error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -12;
	}


	if (!(MRTState.postMRTeffect = Effect::getResource(string(EFFECT_ID_POST_MRT))))
	{
		releaseRenderTargets();
		return -13;
	}

	ID3DXEffect* effect = MRTState.postMRTeffect->getResourcePointer();

	if (!(MRTState.h_techniquePassThroughLTtoBB = effect->GetTechniqueByName(EFFECT_ID_TECHNIQUE_LTBB)) || 
		!(MRTState.h_lightTransportTexture = effect->GetParameterByName(NULL, EFFECT_ID_TEXTURE_LT)))
	{
		releaseRenderTargets();
		showError(false, "Post MRT effect parameters error\n");
		return -14;
	}


	return 0;
};


int Renderer::releaseRenderTargets()
{
	for (UINT i = 0; i < v_renderTargets.size(); ++i)
		MRTState.v_renderTargets[i].release();

	SAFE_RELEASE(MRTState.clearMRTeffect);

	return 0;
};


int Renderer::setupShadowMaps(bool _useVarianceShadowMapping, bool _use64bitShadowMaps = false, UINT _shadowMapResolution = 0, bool _useBlur = false, UINT _blurSamples = 0)
{
	shadowMapsState.shadowMapResolution = _shadowMapResolution > 0 ? _shadowMapResolution : (min(displayMode.Width, displayMode.Height) / 2);

	if (shadowMapsState.useVarianceShadowMapping = _useVarianceShadowMapping)
	{
		shadowMapsState.use64bitShadowMaps = _useVarianceShadowMapping && supports64bitRT && _use64bitShadowMaps ? true : false;
		shadowMapsState.useBlur = _useVarianceShadowMapping && _useBlur && _blurSamples > 1;
	}
	else
	{
		shadowMapsState.use64bitShadowMaps = false;
		shadowMapsState.useBlur = false;
	}

	string effectId = shadowMapsState.useVarianceShadowMapping ? EFFECT_ID_VARIANCE_SHADOW_MAPPING : EFFECT_ID_FORWARD_SHADOW_MAPPING;

	if (!(shadowMapsState.pSMEffect = Effect::getResource(effectId)))
	{
		releaseShadowMaps();
		return -1;
	}


	{ //get techique params etc...

		ID3DXEffect* effect = shadowMapsState.pSMEffect->getResourcePointer();

		if (!(shadowMapsState.h_technique[LIGHT_DIRECTIONAL]			= effect->GetTechniqueByName(EFFECT_SM_TECHNIQUE_LIGHT_DIRECTIONAL)) || 
			!(shadowMapsState.h_technique[LIGHT_POINT]				= effect->GetTechniqueByName(EFFECT_SM_TECHNIQUE_LIGHT_OMNIDIRECTIONAL)) || 
			!(shadowMapsState.h_technique[LIGHT_SPOT]					= effect->GetTechniqueByName(EFFECT_SM_TECHNIQUE_LIGHT_SPOT)) || 

			!(shadowMapsState.h_techniqueSkinned[LIGHT_DIRECTIONAL]	= effect->GetTechniqueByName(EFFECT_SM_SKINNED_TECHNIQUE_LIGHT_DIRECTIONAL)) || 
			!(shadowMapsState.h_techniqueSkinned[LIGHT_POINT]			= effect->GetTechniqueByName(EFFECT_SM_SKINNED_TECHNIQUE_LIGHT_OMNIDIRECTIONAL)) || 
			!(shadowMapsState.h_techniqueSkinned[LIGHT_SPOT]			= effect->GetTechniqueByName(EFFECT_SM_SKINNED_TECHNIQUE_LIGHT_SPOT)) || 

			!(shadowMapsState.h_omniDirection							= effect->GetParameterByName(NULL, EFFECT_SM_LIGHT_OMNI_DIRECTION)) || 
			!(shadowMapsState.h_vec3NFPlane							= effect->GetParameterByName(NULL, EFFECT_SM_LIGHT_NEAR_FAR_PLANE)) || 
			!(shadowMapsState.h_worldLightViewProjectionMx			= effect->GetParameterByName(NULL, EFFECT_SM_LIGHT_WORLD_VIEW_PROJ_MATRIX)) || 
			!(shadowMapsState.h_bonesInfluenceNumber					= effect->GetParameterByName(NULL, EFFECT_SM_LIGHT_SKIN_BONES_INFLUENCE)) || 
			!(shadowMapsState.h_bonesSkinningPalette					= effect->GetParameterByName(NULL, EFFECT_SM_LIGHT_SKIN_PALETTE)))
		{
			releaseShadowMaps();
			showError(false, "Shadow mapping effect parameters error\n");
			return -2;
		}

		if (_useVarianceShadowMapping && _useBlur && _blurSamples > 1)
		{
			if (!(shadowMapsState.h_blurTechnique		= effect->GetTechniqueByName(EFFECT_SM_TECHNIQUE_BLUR_VARIANCE)) || 
				!(shadowMapsState.h_blurSampleOffsets	= effect->GetParameterByName(NULL, EFFECT_SM_BLUR_SAMPLE_OFFSETS)) || 
				!(shadowMapsState.h_blurSampleWeights	= effect->GetParameterByName(NULL, EFFECT_SM_BLUR_SAMPLE_WEIGHTS)) || 
				!(shadowMapsState.h_blurSampleNumber	= effect->GetParameterByName(NULL, EFFECT_SM_BLUR_SAMPLE_NUMBER)) || 
				!(shadowMapsState.h_blurInputTexture	= effect->GetParameterByName(NULL, EFFECT_SM_BLUR_INPUT_TEXTURE)))
			{
				releaseShadowMaps();
				showError(false, "Shadow mapping blur effect parameters error\n");
				return -2;
			}
		}
	}


	if (shadowMapsState.useVarianceShadowMapping)
	{
		if (shadowMapsState.useBlur && _blurSamples > 1)
		{
			if (!(_blurSamples % 2))
				_blurSamples += 1;

			UINT shadowMapsNumber = 3;
			shadowMapsState.v_shadowMaps.resize(shadowMapsNumber);
			D3DFORMAT format = shadowMapsState.use64bitShadowMaps ? D3DFMT_G32R32F : D3DFMT_G16R16F;


			for (UINT i = 0; i < shadowMapsNumber; ++i)
			{
				if (FAILED(hr = pDevice->CreateTexture(	shadowMapsState.shadowMapResolution, 
														shadowMapsState.shadowMapResolution, 
														1, 
														D3DUSAGE_RENDERTARGET, 
														format, 
														D3DPOOL_DEFAULT, 
														&(shadowMapsState.v_shadowMaps[i].renderTexture), 
														NULL)))
				{
					releaseShadowMaps();
					showError(false, "IDirect3DDevice9::CreateTexture error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
					return -11;
				}

				if (FAILED(hr = shadowMapsState.v_shadowMaps[i].renderTexture->GetSurfaceLevel(0, &(v_shadowMaps[i].renderTarget))))
				{
					releaseShadowMaps();
					showError(false, "IDirect3DTexture9::GetSurfaceLevel error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
					return -12;
				}
			}


			// setup blur kernel
			shadowMapsState.sampleOffsetsHorizontal.resize(_blurSamples);
			shadowMapsState.sampleOffsetsVertical.resize(_blurSamples);
			shadowMapsState.sampleWeights.resize(_blurSamples);

			shadowMapsState.sampleOffsetsHorizontal[0] = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
			shadowMapsState.sampleOffsetsVertical[0] = D3DXVECTOR2(0.0f, 0.0f, 0.0f, 0.0f);

			float theta = 4.0f;
			UINT pixelSize = 1.0f / float(shadowMapsState.shadowMapResolution);
			float weightTotal = 0.0f;
			shadowMapsState.sampleWeights[0] = weightTotal = ComputeGaussianFalloffCurve(0.0f, theta);

			for (UINT i = 0; i < (_blurSamples / 2); ++i)
			{
				float sampleOffset = i * 2 + 1.5f;
				shadowMapsState.sampleOffsetsHorizontal[i * 2 + 1] = D3DXVECTOR4(0.0f,  pixelSize * sampleOffset, 0.0f, 0.0f);
				shadowMapsState.sampleOffsetsHorizontal[i * 2 + 2] = D3DXVECTOR4(0.0f, -pixelSize * sampleOffset, 0.0f, 0.0f);
				shadowMapsState.sampleOffsetsVertical[i * 2 + 1] = D3DXVECTOR4( pixelSize * sampleOffset, 0.0f, 0.0f, 0.0f);
				shadowMapsState.sampleOffsetsVertical[i * 2 + 2] = D3DXVECTOR4(-pixelSize * sampleOffset, 0.0f, 0.0f, 0.0f);

				float weight = ComputeGaussianFalloffCurve(float(i + 1), theta);

				shadowMapsState.sampleWeights[i * 2 + 1] = weight;
				shadowMapsState.sampleWeights[i * 2 + 2] = weight;
				weightTotal += weight;
			}

			// normalize weights
			for (UINT i = 0; i < _blurSamples; ++i)
				shadowMapsState.sampleWeights[i] /= weightTotal;


			shadowMapsState.dualParaboloidFront = 0;
			shadowMapsState.dualParaboloidBack = 1;
			shadowMapsState.dualParaboloidFreeMap = 2;
			shadowMapsState.mapUsed = 0;
		}
		else // don't blur
		{
			UINT shadowMapsNumber = 2;
			shadowMapsState.v_shadowMaps.resize(shadowMapsNumber);
			D3DFORMAT format = shadowMapsState.use64bitShadowMaps ? D3DFMT_G32R32F : D3DFMT_G16R16F;


			for (UINT i = 0; i < shadowMapsNumber; ++i)
			{
				if (FAILED(hr = pDevice->CreateTexture(	shadowMapsState.shadowMapResolution, 
														shadowMapsState.shadowMapResolution, 
														1, 
														D3DUSAGE_RENDERTARGET, 
														format, 
														D3DPOOL_DEFAULT, 
														&(shadowMapsState.v_shadowMaps[i].renderTexture), 
														NULL)))
				{
					releaseShadowMaps();
					showError(false, "IDirect3DDevice9::CreateTexture error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
					return -11;
				}

				if (FAILED(hr = shadowMapsState.v_shadowMaps[i].renderTexture->GetSurfaceLevel(0, &(v_shadowMaps[i].renderTarget))))
				{
					releaseShadowMaps();
					showError(false, "IDirect3DTexture9::GetSurfaceLevel error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
					return -12;
				}
			}

			shadowMapsState.dualParaboloidFront = 0;
			shadowMapsState.dualParaboloidBack = 1;
			shadowMapsState.dualParaboloidFreeMap = -1;
			shadowMapsState.mapUsed = 0;
		}
	}
	else //FSM
	{
		UINT shadowMapsNumber = 2;
		shadowMapsState.v_shadowMaps.resize(shadowMapsNumber);
		D3DFORMAT format = D3DFMT_R32F;

		for (UINT i = 0; i < shadowMapsNumber; ++i)
		{
			if (FAILED(hr = pDevice->CreateTexture(	shadowMapsState.shadowMapResolution, 
													shadowMapsState.shadowMapResolution, 
													1, 
													D3DUSAGE_RENDERTARGET, 
													format, 
													D3DPOOL_DEFAULT, 
													&(shadowMapsState.v_shadowMaps[i].renderTexture), 
													NULL)))
			{
				releaseShadowMaps();
				showError(false, "IDirect3DDevice9::CreateTexture error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
				return -11;
			}

			if (FAILED(hr = shadowMapsState.v_shadowMaps[i].renderTexture->GetSurfaceLevel(0, &(v_shadowMaps[i].renderTarget))))
			{
				releaseShadowMaps();
				showError(false, "IDirect3DTexture9::GetSurfaceLevel error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
				return -12;
			}
		}


		shadowMapsState.dualParaboloidFront = 0;
		shadowMapsState.dualParaboloidBack = 1;
		shadowMapsState.dualParaboloidFreeMap = -1;
		shadowMapsState.mapUsed = 0;
	}


	if (shadowMapsState.useBlur)
	{ //setup vertex buffer for blur
		// account for DirectX's texel center standard
		float uv_adjust = 0.5f / (float) shadowMapsState.shadowMapResolution;

		FullscreenQuadVertex quadVertices[4];

		// TRIANGLEFAN
		// 1-------2
		// | t1  / |
		// |   /   |
		// | /  t2 |
		// 0-------3

		quadVertices[0] = FullscreenQuadVertex( -1.0f, -1.0f, 0.0f, 0.0f + uv_adjust, 1.0f + uv_adjust);
		quadVertices[1] = FullscreenQuadVertex( -1.0f,  1.0f, 0.0f, 0.0f + uv_adjust, 0.0f + uv_adjust);
		quadVertices[2] = FullscreenQuadVertex(  1.0f,  1.0f, 0.0f, 1.0f + uv_adjust, 0.0f + uv_adjust);
		quadVertices[3] = FullscreenQuadVertex(  1.0f, -1.0f, 0.0f, 1.0f + uv_adjust, 1.0f + uv_adjust);


		HRESULT hr = S_OK;

		if (FAILED(hr = pDevice->CreateVertexBuffer(4 * sizeof(FullscreenQuadVertex), D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &shadowMapsState.p_blurFullscreenQuad, NULL)))
		{
			releaseShadowMaps();
			showError(false, "IDirect3DDevice9::CreateVertexBuffer error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -21;
		}

		void* pVoid = NULL;

		if (FAILED(hr = shadowMapsState.p_blurFullscreenQuad->Lock(0, 0, &pVoid, 0)))
		{
			releaseShadowMaps();
			showError(false, "IDirect3DVertexBuffer9::Lock error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -22;
		}

		memcpy(pVoid, quadVertices, 4 * sizeof(FullscreenQuadVertex));

		if (FAILED(hr = shadowMapsState.p_blurFullscreenQuad->Unlock()))
		{
			releaseShadowMaps();
			showError(false, "IDirect3DVertexBuffer9::Unlock error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -23;
		}
	}


	{ //generate lights projection mx
		float aspect = (float)displayMode.Width / (float)displayMode.Height;
		float nearPlane = usedCamera->getNearViewPlaneDistance();
		float farPlane = usedCamera->getFarViewPlaneDistance();


		// light dir

		float volWidth = LIGHT_MATRIX_H;
		float volHeight = LIGHT_MATRIX_H;

		ZeroMemory(&(shadowMapsState.lightsProjectionMx[LIGHT_DIRECTIONAL]), sizeof(D3DXMATRIX));

		shadowMapsState.lightsProjectionMx[LIGHT_DIRECTIONAL]._11 = volWidth * 0.5f;
		shadowMapsState.lightsProjectionMx[LIGHT_DIRECTIONAL]._22 = volHeight * 0.5f;
		shadowMapsState.lightsProjectionMx[LIGHT_DIRECTIONAL]._33 = 1.0f / (farPlane - nearPlane);
		shadowMapsState.lightsProjectionMx[LIGHT_DIRECTIONAL]._43 = nearPlane / (nearPlane - farPlane);
		shadowMapsState.lightsProjectionMx[LIGHT_DIRECTIONAL]._44 = 1.0f;


		// light omni & spot

		float fov = D3DXToRadian(90);
		float yScale = 1.0f / tan(fov * 0.5f);
		float xScale = yScale / aspect;

		ZeroMemory(&(shadowMapsState.lightsProjectionMx[LIGHT_POINT]), sizeof(D3DXMATRIX));
		ZeroMemory(&(shadowMapsState.lightsProjectionMx[LIGHT_SPOT]), sizeof(D3DXMATRIX));

		shadowMapsState.lightsProjectionMx[LIGHT_POINT]._11 = shadowMapsState.lightsProjectionMx[LIGHT_SPOT]._11 = xScale;
		shadowMapsState.lightsProjectionMx[LIGHT_POINT]._22 = shadowMapsState.lightsProjectionMx[LIGHT_SPOT]._22 = yScale;
		shadowMapsState.lightsProjectionMx[LIGHT_POINT]._33 = shadowMapsState.lightsProjectionMx[LIGHT_SPOT]._33 = farPlane / (farPlane - nearPlane);
		shadowMapsState.lightsProjectionMx[LIGHT_POINT]._43 = shadowMapsState.lightsProjectionMx[LIGHT_SPOT]._43 = (-nearPlane * farPlane) / (farPlane - nearPlane);
		shadowMapsState.lightsProjectionMx[LIGHT_POINT]._34 = shadowMapsState.lightsProjectionMx[LIGHT_SPOT]._34 = 1.0f;
	}


	ZeroMemory(&shadowMapsState.lightShadowMapMx, sizeof(D3DXMATRIX));

	shadowMapsState.lightShadowMapMx._11 = 0.5f;
	shadowMapsState.lightShadowMapMx._22 = -0.5f;
	shadowMapsState.lightShadowMapMx._41 = shadowMapsState.lightShadowMapMx._42 = 0.5f + (0.5f / (float) shadowMapsState.shadowMapResolution);
	shadowMapsState.lightShadowMapMx._44 = 1.0f;

	shadowMapsState.useShadowMapping = true;

	return 0;
};


int Renderer::releaseShadowMaps()
{
	SAFE_RELEASE(shadowMapsState.pSMEffect);
	IDX_SAFE_RELEASE(shadowMapsState.p_blurFullscreenQuad);

	shadowMapsState.sampleOffsetsHorizontal.resize(0);
	shadowMapsState.sampleOffsetsVertical.resize(0);
	shadowMapsState.sampleWeights.resize(0);
	shadowMapsState.v_shadowMaps.resize(0);

	shadowMapsState.useShadowMapping = false;
	shadowMapsState.shadowMapResolution = 0;
	shadowMapsState.useVarianceShadowMapping = false;
	shadowMapsState.use64bitShadowMaps = false;
	shadowMapsState.useBlur = false;

	shadowMapsState.shadowMapIndex = -1;
	shadowMapsState.shadowMapDummyIndex = -1;
	shadowMapsState.dualParaboloidFront = -1;
	shadowMapsState.dualParaboloidBack = -1;
	shadowMapsState.dualParaboloidFreeMap = -1;
	shadowMapsState.mapUsed = -1;

	for (UINT i = 0; i < LIGHT_TYPES; ++i)
	{
		shadowMapsState.h_technique[i] = NULL;
		shadowMapsState.h_techniqueSkinned[i] = NULL;
	}

	shadowMapsState.h_omniDirection = NULL;
	shadowMapsState.h_vec3NFPlane = NULL;
	shadowMapsState.h_worldLightViewProjectionMx = NULL;
	shadowMapsState.h_bonesInfluenceNumber = NULL;
	shadowMapsState.h_bonesSkinningPalette = NULL;
	shadowMapsState.h_blurTechnique = NULL;
	shadowMapsState.h_blurSampleOffsets = NULL;
	shadowMapsState.h_blurSampleWeights = NULL;
	shadowMapsState.h_blurSampleNumber = NULL;
	shadowMapsState.h_blurInputTexture = NULL;

	return 0;
};


int Renderer::setupLightTransport(bool _use64bitLightTransport, bool _useShadowMaps, bool _useVSM)
{
	lightTransportState.use64bitLightTransport = (supports64bitRT && _use64bitLightTransport) ? true : false;
	lightTransportState.useShadowMaps = _useShadowMaps;
	lightTransportState.useVSM = _useVSM

	string effectId = string(lightTransportState.useShadowMaps ? (lightTransportState.useVSM ? EFFECT_ID_LIGHT_TRANSPORT_VSM : EFFECT_ID_LIGHT_TRANSPORT_FSM) : EFFECT_ID_LIGHT_TRANSPORT) + 
							(lightTransportState.use64bitLightTransport ? EFFECT_64B_DEPTH : EFFECT_32B_DEPTH);

	if (!(lightTransportState.pLTEffect = Effect::getResource(effectId)))
	{
		releaseLightTransport();
		return -1;
	}


	{ //get techique params etc...

		ID3DXEffect* effect = lightTransportState.pLTEffect->getResourcePointer();

		if (!(lightTransportState.h_techniqueAmbient					= effect->GetTechniqueByName(EFFECT_LT_TECHNIQUE_AMBIENT)) || 
			!(lightTransportState.h_technique[LIGHT_DIRECTIONAL]		= effect->GetTechniqueByName(EFFECT_LT_TECHNIQUE_DIRECTIONAL)) || 
			!(lightTransportState.h_technique[LIGHT_POINT]				= effect->GetTechniqueByName(EFFECT_LT_TECHNIQUE_OMNIDIRECTIONAL)) || 
			!(lightTransportState.h_technique[LIGHT_SPOT]				= effect->GetTechniqueByName(EFFECT_LT_TECHNIQUE_SPOT)) || 

			!(lightTransportState.h_cameraPosition						= effect->GetParameterByName(NULL, EFFECT_LT_CAMERA_POSITION)) || 
			!(lightTransportState.h_cameraInvViewProjectionMx			= effect->GetParameterByName(NULL, EFFECT_LT_CAMERA_INV_VIEW_PROJECTION)) || 

			!(lightTransportState.h_lightAmbient						= effect->GetParameterByName(NULL, EFFECT_LT_LIGHT_AMBIENT)) || 
			!(lightTransportState.h_lightTable							= effect->GetParameterByName(NULL, EFFECT_LT_LIGHT_INFO_TABLE)) || 

			!(lightTransportState.h_colorMap							= effect->GetParameterByName(NULL, EFFECT_LT_COLOR_MAP)) || 
			!(lightTransportState.h_normalMap							= effect->GetParameterByName(NULL, EFFECT_LT_NORMAL_MAP)) || 
			!(lightTransportState.h_depthMap							= effect->GetParameterByName(NULL, EFFECT_LT_DEPTH_MAP)))
		{
			releaseLightTransport();
			showError(false, "Light transport effect parameters error\n");
			return -2;
		}


		if (lightTransportState.useShadowMaps)
		{
			if (!(lightTransportState.shadowMap								= effect->GetParameterByName(NULL, EFFECT_LT_SHADOW_MAP)) || 
				!(lightTransportState.h_shadowFrontMap						= effect->GetParameterByName(NULL, EFFECT_LT_SHADOW_FRONT_MAP)) || 
				!(lightTransportState.h_shadowBackMap						= effect->GetParameterByName(NULL, EFFECT_LT_SHADOW_BACK_MAP)) || 

				!(lightTransportState.h_techniqueNoSM[LIGHT_DIRECTIONAL]	= effect->GetTechniqueByName(EFFECT_LT_TECHNIQUE_DIRECTIONAL_NO_SM)) || 
				!(lightTransportState.h_techniqueNoSM[LIGHT_POINT]			= effect->GetTechniqueByName(EFFECT_LT_TECHNIQUE_OMNIDIRECTIONAL_NO_SM)) || 
				!(lightTransportState.h_techniqueNoSM[LIGHT_SPOT]			= effect->GetTechniqueByName(EFFECT_LT_TECHNIQUE_SPOT_NO_SM)) || 

				!(lightTransportState.h_cameraInvViewMx						= effect->GetParameterByName(NULL, EFFECT_LT_CAMERA_INV_VIEW)) || 
				!(lightTransportState.h_cameraInvProjectionMx				= effect->GetParameterByName(NULL, EFFECT_LT_CAMERA_INV_PROJECTION)) || 
				!(lightTransportState.h_lightViewProjectionMx				= effect->GetParameterByName(NULL, EFFECT_LT_LIGHT_VIEW_PROJECTION)) || 

				!(lightTransportState.h_lightShadowMapMx					= effect->GetParameterByName(NULL, EFFECT_LT_LIGHT_SHADOW_MAP_MX)) || 
				!(lightTransportState.h_lightNearFarPlane					= effect->GetParameterByName(NULL, EFFECT_LT_LIGHT_NEAR_FAR_PLANE)))
			{
				releaseLightTransport();
				showError(false, "Light transport effect parameters error\n");
				return -2;
			}
		}
	}



	D3DFORMAT format = lightTransportState.use64bitLightTransport ? D3DFMT_A16B16G16R16F : D3DFMT_A8R8G8B8;

	HRESULT hr = S_OK;

	if (FAILED(hr = pDevice->CreateTexture(	displayMode.Width, 
											displayMode.Height, 
											1, 
											D3DUSAGE_RENDERTARGET, 
											format, 
											D3DPOOL_DEFAULT, 
											&(lightTransportState.lightTransport.renderTexture), 
											NULL)))
	{
		releaseLightTransport();
		showError(false, "IDirect3DDevice9::CreateTexture error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -11;
	}

	if (FAILED(hr = lightTransportState.lightTransport.renderTexture->GetSurfaceLevel(0, &(lightTransportState.lightTransport.renderTarget))))
	{
		releaseLightTransport();
		showError(false, "IDirect3DTexture9::GetSurfaceLevel error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -12;
	}

	return 0;
};


int Renderer::releaseLightTransport()
{
	lightTransportState.lightTransport.release();

	SAFE_RELEASE(lightTransportState.pLTEffect);

	lightTransportState.use64bitLightTransport = false;
	lightTransportState.useShadowMaps = false;
	lightTransportState.useVSM = false;

	lightTransportState.h_techniqueAmbient = NULL;
	lightTransportState.h_technique = NULL;
	lightTransportState.h_techniqueNoSM = NULL;
	lightTransportState.h_cameraPosition = NULL;
	lightTransportState.h_cameraInvViewProjectionMx = NULL;
	lightTransportState.h_cameraInvViewMx = NULL;
	lightTransportState.h_cameraInvProjectionMx = NULL;
	lightTransportState.h_lightTable = NULL;
	lightTransportState.h_lightViewProjectionMx = NULL;
	lightTransportState.h_colorMap = NULL;
	lightTransportState.h_normalMap = NULL;
	lightTransportState.h_depthMap = NULL;
	lightTransportState.shadowMap = NULL;
	lightTransportState.h_shadowFrontMap = NULL;
	lightTransportState.h_shadowBackMap = NULL;

	return 0;
};


int Renderer::setupParticleSystemEffect()
{
	if (!(particleSystemEffectState.pPSEffect = Effect::getResource(EFFECT_PS)))
	{
		releaseParticleSystemEffect();
		return 10;
	}

	ID3DXEffect* effect = particleSystemEffectState.pPSEffect->getResourcePointer();

	if (!(particleSystemEffectState.h_technique = effect->GetTechniqueByName(EFFECT_PS_TECHNIQUE)) || 
		!(particleSystemEffectState.h_ambient = effect->GetParameterByName(NULL, EFFECT_PS_AMBIENT)) || 
		!(particleSystemEffectState.h_emissiveFactor = effect->GetParameterByName(NULL, EFFECT_PS_EMISSIVE_FACTOR)) || 
		!(particleSystemEffectState.h_texture = effect->GetParameterByName(NULL, EFFECT_PS_TEXTURE)) || 
		!(particleSystemEffectState.h_sprites = effect->GetParameterByName(NULL, EFFECT_PS_SPRITES)) || 
		!(particleSystemEffectState.h_billboardind = effect->GetParameterByName(NULL, EFFECT_PS_BILLBOARDING)) || 
		!(particleSystemEffectState.h_matrixWorld = effect->GetParameterBySemantic(NULL, "WORLD")) || 
		!(particleSystemEffectState.h_matrixViewProjection = effect->GetParameterBySemantic(NULL, "VIEWPROJECTION")) || 
		!(particleSystemEffectState.h_matrixViewInversion = effect->GetParameterBySemantic(NULL, "VIEWINV")))
	{
		showError(false, "Particle system effect parameters error\n");
		releaseParticleSystemEffect();
		return 20;
	}

	return 0;
};


int Renderer::releaseParticleSystemEffect()
{
	SAFE_RELEASE(pPSEffect);

	h_technique = NULL;
	h_ambient = NULL;
	h_emissiveFactor = NULL;
	h_texture = NULL;
	h_sprites = NULL;
	h_billboardind = NULL;
	h_matrixWorld = NULL;
	h_matrixViewProjection = NULL;
	h_matrixViewInversion = NULL;

	return 0;
};



int Renderer::renderGUI()
{
	if (!guiSystem)
		return 0;

	HRESULT hr = S_OK;

	if (FAILED(hr = pDevice->SetRenderTarget(0, p_backBuffer)))
	{
		showError(false, "IDirect3DDevice9::SetRenderTarget error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -101;
	}

	int res = guiSystem->draw(pGUISprite);

	if (FAILED(hr = pDevice->SetRenderTarget(0, NULL)))
	{
		showError(false, "IDirect3DDevice9::SetRenderTarget error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -102;
	}

	return res;
};



int Renderer::render2DElements()
{
	if (!l_2dElements.size())
		return 0;

	HRESULT hr = S_OK;

	if (FAILED(hr = pDevice->SetRenderTarget(0, p_backBuffer)))
	{
		showError(false, "IDirect3DDevice9::SetRenderTarget error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -101;
	}


	int res = 0;
	list<GUIElement*>::iterator itend = l_2dElements.end();

	for (list<GUIElement*>::iterator it = l_2dElements.begin(); it != itend; it++)
	{
		res += (*it)->draw(pGUISprite);
	}


	if (FAILED(hr = pDevice->SetRenderTarget(0, NULL)))
	{
		showError(false, "IDirect3DDevice9::SetRenderTarget error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -102;
	}


	return res;
};



int Renderer::renderToMRT()
{
	l_renderChunks.sort(render_chunk_compare);

	HRESULT hr = S_OK;

	// set render targets
	for (int i = 0; i < MRTState.v_renderTargets.size(); ++i)
	{
		if (FAILED(hr = pDevice->SetRenderTarget(i, MRTState.v_renderTargets[i].renderTarget)))
		{
			showError(false, "IDirect3DDevice9::SetRenderTarget error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -301;
		}
	}

	// set default z buffer - it will be used in case we need to render some chunks in back to front rendering
	if (FAILED(hr = pDevice->SetDepthStencilSurface(p_depthBuffer)))
	{
		showError(false, "IDirect3DDevice9::SetDepthStencilSurface error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -302;
	}


	// clear all MRTs
	{
		ID3DXEffect* clearEffect = MRTState.clearMRTeffect->getResourcePointer();

		UINT clearPasses = 0;

		if (FAILED(hr = pDevice->SetFVF(NULL)))
		{
			showError(false, "IDirect3DDevice9::SetFVF error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -101;
		}

		if (FAILED(hr = pDevice->SetVertexDeclaration(p_fullscreenQuadVertexDecl)))
		{
			showError(false, "IDirect3DDevice9::SetVertexDeclaration error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -102;
		}

		if (FAILED(hr = pDevice->SetStreamSource(0, p_fullscreenQuad, 0, sizeof(FullscreenQuadVertex))))
		{
			showError(false, "IDirect3DDevice9::SetStreamSource error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -103;
		}


		if (FAILED(hr = clearEffect->Begin(&clearPasses, 0)))
		{
			showError(false, "ID3DXEffect::Begin error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -104;
		}

		for (UINT i = 0; i < clearPasses; ++i)
		{
			if (FAILED(hr = clearEffect->BeginPass(i)))
			{
				showError(false, "ID3DXEffect::BeginPass error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
				return -105;
			}

			if (FAILED(hr = pDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2)))
			{
				showError(false, "IDirect3DDevice9::DrawPrimitive error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
				return -106;
			}

			if (FAILED(hr = clearEffect->EndPass()))
			{
				showError(false, "ID3DXEffect::EndPass error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
				return -107;
			}

		}
		if (FAILED(hr = clearEffect->End()))
		{
			showError(false, "ID3DXEffect::End error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -108;
		}

		// clear default depth buffer
		if (FAILED(hr = pDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0)))
		{
			showError(false, "IDirect3DDevice9::Clear error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -109;
		}

		if (FAILED(hr = pDevice->SetStreamSource(0, NULL)))
		{
			showError(false, "IDirect3DDevice9::SetStreamSource error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -110;
		}
	}


	list<RenderChunk*>::iterator it = l_renderChunks.begin();
	list<RenderChunk*>::iterator it_end = l_renderChunks.end();
	Effect* actualEffect = NULL;
	ID3DXEffect* effect = NULL;

	DWORD timestamp = usedCamera->getTimestamp();
	D3DXMATRIX* viewMx = usedCamera->getViewMatrix();
	D3DXMATRIX* projectionMx = usedCamera->getProjectionMatrix();
	D3DXMATRIX* viewProjectionMx = usedCamera->getViewProjectionMatrix();
	D3DXMATRIX* viewInvMx = usedCamera->getViewInverseMatrix();

	int result = 0;

	while(it != it_end)
	{
		actualEffect = (*it)->getEffect();
		effect = actualEffect->getResourcePointer();

		// set camera matrices once per effect
		// this shouldn't realy be problem while using single camera
		// and any UI gizmos should be added after the whole scene is rendered
		if (result = actualEffect->setMatrices(timestamp, viewMx, projectionMx, viewInvMx, viewProjectionMx))
		{
			showError(false, "Effect::setMatrices error\n");
			return -201;
		}

		UINT uiPasses = 0;
		if (FAILED(hr = effect->Begin(&uiPasses, 0)))
		{
			showError(false, "ID3DXEffect::Begin error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -202;
		}

		list<RenderChunk*>::iterator it2;

		for (UINT i = 0; i < uiPasses; ++i)
		{
			it2 = it;
			if (FAILED(hr = effect->BeginPass(i)))
			{
				showError(false, "ID3DXEffect::BeginPass error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
				return -203;
			}

			while ((*it2) != it_end & (*it2) == actualEffect)
			{
				// apply chunk variables, textures, world matrix and skinning palette
				if ((*it2)->applyAllVariables())
				{
					showError(false, "RenderChunk::applyAllVariables error\n");
					return -201;
				}

				if (FAILED(hr = effect->CommitChanges()))
				{
					showError(false, "ID3DXEffect::CommitChanges error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
					return -205;
				}

				if ((*it2)->draw())
				{
					showError(false, "RenderChunk::draw error\n", result);
					return -201;
				}

				it2++;
			}

			if (FAILED(hr = effect->EndPass()))
			{
				showError(false, "ID3DXEffect::EndPass error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
				return -207;
			}

		}

		if (FAILED(hr = effect->End()))
		{
			showError(false, "ID3DXEffect::End error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -207;
		}


		it = it2;
	}

	for (int i = 0; i < v_renderTargets.size(); ++i)
	{
		if (FAILED(hr = pDevice->SetRenderTarget(i, NULL)))
		{
			showError(false, "IDirect3DDevice9::SetRenderTarget error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -303;
		}
	}

	if (FAILED(hr = pDevice->SetDepthStencilSurface(NULL)))
	{
		showError(false, "IDirect3DDevice9::SetDepthStencilSurface error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -304;
	}

	return 0;
};


int Renderer::renderVSMLightDirectional( pair< Light*, list< RenderChunk* > >* _lightWithShadow, const D3DXVECTOR3* _nearFarPlane, int _SMIndexUsed)
{
	Light* _light = _lightWithShadow->first;

	D3DXVECTOR3 lightPosition;
	D3DXVec3Normalize(&lightPosition, _light->getDirection());
	lightPosition *= DIRECTIONAL_LIGHT_DISTANCE_FROM_CAMERA;
	lightPosition = (*usedCamera->getPosition()) - lightPosition;

	D3DXMATRIX lightViewProjectionMx
	D3DXMatrixLookAtLH(&lightViewProjectionMx, lightPosition, _light->getDirection(), &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	D3DXMatrixMultiply(&lightViewProjectionMx, &lightViewProjectionMx, &(shadowMapsState.lightsProjectionMx[LIGHT_DIRECTIONAL]));

	D3DXMATRIX worldLightViewProjectionMx;


	ID3DXEffect* effect = shadowMapsState.pSMEffect->getResource();



	shadowMapsState.mapUsed = _SMIndexUsed;

	if (FAILED(hr = pDevice->SetRenderTarget(0, shadowMapsState.v_shadowMaps[_SMIndexUsed].renderTarget)))
	{
		showError(false, "IDirect3DDevice9::SetRenderTarget error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -101;
	}

	if (FAILED(hr = pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0)))
	{
		showError(false, "IDirect3DDevice9::SetRenderTarget error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -102;
	}


	if (!_lightWithShadow->second.size())
	{
		if (FAILED(hr = pDevice->SetRenderTarget(0, NULL)))
		{
			showError(false, "IDirect3DDevice9::SetRenderTarget error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -101;
		}

		return 0;
	}


	if (FAILED(hr = effect->SetTechnique(shadowMapsState.h_technique[LIGHT_DIRECTIONAL])))
	{
		showError(false, "ID3DXEffect::Begin error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -201;
	}

	if (FAILED(hr = effect->SetValue(shadowMapsState.h_vec3NFPlane, _nearFarPlane, sizeof(D3DXVECTOR3))))
	{
		showError(false, "ID3DXEffect::Begin error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -202;
	}


	list<RenderChunk*>::iterator it = _lightWithShadow->second.begin();
	list<RenderChunk*>::iterator it_end = _lightWithShadow->second.end();

	UINT uiPasses = 0;

	if (FAILED(hr = effect->Begin(&uiPasses, 0)))
	{
		showError(false, "ID3DXEffect::Begin error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -203;
	}

	for (UINT i = 0; i < uiPasses; ++i)
	{
		if (FAILED(hr = effect->BeginPass(uiPasses)))
		{
			showError(false, "ID3DXEffect::BeginPass error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -204;
		}

		while (it != it_end)
		{
			GraphicObject* instance = (*it)->getInstance();
			if (instance->getAnimationInfo())
				break;

			D3DXMatrixMultiply(&worldLightViewProjectionMx, instance->getWorldPosition(), lightViewProjectionMx);

			if (FAILED(hr = effect->SetMatrix(shadowMapsState.h_worldLightViewProjectionMx, &worldLightViewProjectionMx)))
			{
				showError(false, "ID3DXEffect::SetValue error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
				return -205;
			}

			if (FAILED(hr = effect->CommitChanges()))
			{
				showError(false, "ID3DXEffect::CommitChanges error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
				return -206;
			}


			for (it; (it != it_end) && ((*it)->getInstance() != instance); it++)
			{
				if (!(*it)->draw())
					return -301;
			}

		}

		if (FAILED(hr = effect->EndPass()))
		{
			showError(false, "ID3DXEffect::EndPass error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -207;
		}

	}

	if (FAILED(hr = effect->End()))
	{
		showError(false, "ID3DXEffect::End error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -208;
	}



	if (it == it_end)
	{
		if (FAILED(hr = pDevice->SetRenderTarget(0, NULL)))
		{
			showError(false, "IDirect3DDevice9::SetRenderTarget error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -101;
		}

		if (shadowMapsState.useBlur)
		{
			if (renderVSMBlur(shadowMapsState.mapUsed, 1 - shadowMapsState.mapUsed))
				return 401;
		}

		return 0;
	}



	if (FAILED(hr = effect->SetTechnique(shadowMapsState.h_techniqueSkinned[LIGHT_DIRECTIONAL])))
	{
		showError(false, "ID3DXEffect::Begin error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -201;
	}

	if (FAILED(hr = effect->SetValue(shadowMapsState.h_vec3NFPlane, &nearFarPlane, sizeof(D3DXVECTOR3))))
	{
		showError(false, "ID3DXEffect::Begin error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -202;
	}


	list<RenderChunk*>::iterator it_begin = it;

	UINT uiPasses = 0;
	if (FAILED(hr = effect->Begin(&uiPasses, 0)))
	{
		showError(false, "ID3DXEffect::Begin error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -203;
	}

	for (UINT i = 0; i < uiPasses; ++i)
	{
		if (FAILED(hr = effect->BeginPass(uiPasses)))
		{
			showError(false, "ID3DXEffect::BeginPass error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -204;
		}


		it = it_begin;

		while (it != it_end)
		{
			GraphicObject* instance = (*it)->getInstance();

			D3DXMatrixMultiply(&worldLightViewProjectionMx, instance->getWorldPosition(), &lightViewProjectionMx);

			if (FAILED(hr = effect->SetMatrix(shadowMapsState.h_worldLightViewProjectionMx, &worldLightViewProjectionMx)))
			{
				showError(false, "ID3DXEffect::SetValue error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
				return -205;
			}


			for (it; (it != it_end) && ((*it)->getInstance() != instance); it++)
			{
				if (!(*it)->applySkinningInfo(effect, shadowMapsState.h_bonesSkinningPalette, shadowMapsState.h_bonesInfluenceNumber))
					return -305;

				if (FAILED(hr = effect->CommitChanges()))
				{
					showError(false, "ID3DXEffect::CommitChanges error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
					return -206;
				}

				if (!(*it)->draw())
					return -301;
			}

		}

		if (FAILED(hr = effect->EndPass()))
		{
			showError(false, "ID3DXEffect::EndPass error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -207;
		}
	}

	if (FAILED(hr = effect->End()))
	{
		showError(false, "ID3DXEffect::End error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -208;
	}



	if (FAILED(hr = pDevice->SetRenderTarget(0, NULL)))
	{
		showError(false, "IDirect3DDevice9::SetRenderTarget error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -101;
	}

	if (shadowMapsState.useBlur)
	{
		if (renderVSMBlur(shadowMapsState.mapUsed, 1 - shadowMapsState.mapUsed))
			return 401;
	}

	return 0;
};



int Renderer::renderVSMLightSpot( pair< Light*, list< RenderChunk* > >* _lightWithShadow, const D3DXVECTOR3* _nearFarPlane, int _SMIndexUsed )
{
	Light* _light = _lightWithShadow->first;

	D3DXMATRIX* lightProjectionMx = &(shadowMapsState.lightsProjectionMx[LIGHT_SPOT]);
	D3DXMATRIX lightViewProjectionMx;
	D3DXMATRIX worldLightViewProjectionMx;

	D3DXMatrixLookAtLH(&lightViewProjectionMx, lightPosition, &((*lightPosition) + (*(_light->getDirection()))), &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	D3DXMatrixMultiply(&lightViewProjectionMx, &lightViewProjectionMx, lightProjectionMx);

	D3DXVECTOR3* lightPosition = _light->getPosition();
	D3DXVECTOR3* lightDirection = _light->getDirection();

	float lightConeCosSq = 0.0f;
	_light->getSpotlight(NULL, &lightConeCosSq, NULL);
	lightConeCosSq *= lightConeCosSq;
	float lightSineCosSq = 1.0f - lightConeCosSq;
	float oneByLightConeSin = 1.0f / pow(lightSineCosSq, 0.5f);


	GraphicObject* instance = NULL;
	D3DXVECTOR3 instancePosition;
	float instanceRadius = 0.0f;

	D3DXVECTOR3 lightV2Position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 lightV2InstanceLine = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	float lightV2InstanceDistSq = 0.0f;
	float lightV2DirInstanceDir = 0.0f;
	float instanceRadiusSq = 0.0f;
	D3DXVECTOR3 lightInstanceLine = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	float lightInstanceDistSq = 0.0f;
	float lightDirInstanceDir = 0.0f;


	ID3DXEffect* effect = shadowMapsState.pSMEffect->getResource();

	shadowMapsState.mapUsed = _SMIndexUsed;

	if (FAILED(hr = pDevice->SetRenderTarget(0, shadowMapsState.v_shadowMaps[_SMIndexUsed].renderTarget)))
	{
		showError(false, "IDirect3DDevice9::SetRenderTarget error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -101;
	}

	if (FAILED(hr = pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0)))
	{
		showError(false, "IDirect3DDevice9::SetRenderTarget error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -102;
	}


	if (!_lightWithShadow->second.size())
	{
		if (FAILED(hr = pDevice->SetRenderTarget(0, NULL)))
		{
			showError(false, "IDirect3DDevice9::SetRenderTarget error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -101;
		}

		return 0;
	}


	if (FAILED(hr = effect->SetTechnique(shadowMapsState.h_technique[LIGHT_SPOT])))
	{
		showError(false, "ID3DXEffect::Begin error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -201;
	}

	if (FAILED(hr = effect->SetValue(shadowMapsState.h_vec3NFPlane, _nearFarPlane, sizeof(D3DXVECTOR3))))
	{
		showError(false, "ID3DXEffect::Begin error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -202;
	}


	list<RenderChunk*>::iterator it = _lightWithShadow->second.begin();
	list<RenderChunk*>::iterator it_end = _lightWithShadow->second.end();

	UINT uiPasses = 0;

	if (FAILED(hr = effect->Begin(&uiPasses, 0)))
	{
		showError(false, "ID3DXEffect::Begin error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -203;
	}

	for (UINT i = 0; i < uiPasses; ++i)
	{
		if (FAILED(hr = effect->BeginPass(uiPasses)))
		{
			showError(false, "ID3DXEffect::BeginPass error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -204;
		}

		while (it != it_end)
		{
			GraphicObject* instance = (*it)->getInstance();
			if (instance->getAnimationInfo())
				break;

//CHANGE IT!!!!!!!!!!!!!!!!!!!!
			instance->getBoundingRange(&instancePosition, &instanceRadius);

			// test light cone and instance bounding sphere intersection

			lightV2InstanceLine = instancePosition - ( lightPosition - (instanceRadius * oneByLightConeSin) * lightDirection );
			lightV2DirInstanceDir = D3DXVec3Dot(&lightDirection, &lightV2InstanceLine);

			if (!( lightV2DirInstanceDir > 0 && 
				   lightV2DirInstanceDir * lightV2DirInstanceDir >= D3DXVec3Dot(&lightV2InstanceLine, &lightV2InstanceLine) * lightConeCosSq ))
			{
				// not in the V2 cone
				it++;
				while ((*it)->getInstance() == instance)
					it++;
				continue;
			}


			lightInstanceLine = instancePosition - lightPosition;
			lightInstanceDistSq = D3DXVec3Dot(&lightInstanceLine, &lightInstanceLine);
			lightDirInstanceDir = -D3DXVec3Dot(&lightDirection, &lightInstanceLine);

			if ( lightDirInstanceDir > 0 && 
				 lightDirInstanceDir * lightDirInstanceDir >= lightInstanceDistSq * lightSineCosSq && 
				 lightInstanceDistSq > instanceRadius * instanceRadius )
			{
				// in the V2 and V3 cone but behind light position
				it++;
				while (it != it_end && (*it)->getInstance() == instance)
					it++;
				continue;
			}

			// instance in the light cone

			D3DXMatrixMultiply(&worldLightViewProjectionMx, instance->getWorldPosition(), lightViewProjectionMx);

			if (FAILED(hr = effect->SetMatrix(shadowMapsState.h_worldLightViewProjectionMx, &worldLightViewProjectionMx)))
			{
				showError(false, "ID3DXEffect::SetValue error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
				return -205;
			}

			if (FAILED(hr = effect->CommitChanges()))
			{
				showError(false, "ID3DXEffect::CommitChanges error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
				return -206;
			}


			for (it; ( it != it_end ) && ( (*it)->getInstance() != instance ); it++)
			{
				if (!(*it)->draw())
					return -301;
			}

		}

		if (FAILED(hr = effect->EndPass()))
		{
			showError(false, "ID3DXEffect::EndPass error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -207;
		}

	}

	if (FAILED(hr = effect->End()))
	{
		showError(false, "ID3DXEffect::End error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -208;
	}


	if (it == it_end)
	{
		if (FAILED(hr = pDevice->SetRenderTarget(0, NULL)))
		{
			showError(false, "IDirect3DDevice9::SetRenderTarget error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -101;
		}

		if (shadowMapsState.useBlur)
		{
			if (renderVSMBlur(shadowMapsState.mapUsed, 1 - shadowMapsState.mapUsed))
				return 401;
		}

		return 0;
	}



	if (FAILED(hr = effect->SetTechnique(shadowMapsState.h_techniqueSkinned[LIGHT_SPOT])))
	{
		showError(false, "ID3DXEffect::Begin error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -201;
	}

	if (FAILED(hr = effect->SetValue(shadowMapsState.h_vec3NFPlane, &nearFarPlane, sizeof(D3DXVECTOR3))))
	{
		showError(false, "ID3DXEffect::Begin error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -202;
	}


	UINT uiPasses = 0;
	if (FAILED(hr = effect->Begin(&uiPasses, 0)))
	{
		showError(false, "ID3DXEffect::Begin error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -203;
	}


	list<RenderChunk*>::iterator it_begin = it;

	for (UINT i = 0; i < uiPasses; ++i)
	{
		if (FAILED(hr = effect->BeginPass(uiPasses)))
		{
			showError(false, "ID3DXEffect::BeginPass error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -204;
		}


		it = it_begin;

		while (it != it_end)
		{
			GraphicObject* instance = (*it)->getInstance();


//CHANGE IT!!!!!!!!!!!!!!!!!!!!
			instance->getBoundingRange(&instancePosition, &instanceRadius);

			// test light cone and instance bounding sphere intersection

			lightV2InstanceLine = instancePosition - ( lightPosition - (instanceRadius * oneByLightConeSin) * lightDirection );
			lightV2DirInstanceDir = D3DXVec3Dot(&lightDirection, &lightV2InstanceLine);

			if (!( lightV2DirInstanceDir > 0 && 
				   lightV2DirInstanceDir * lightV2DirInstanceDir >= D3DXVec3Dot(&lightV2InstanceLine, &lightV2InstanceLine) * lightConeCosSq ))
			{
				// not in the V2 cone
				it++;
				while ((*it)->getInstance() == instance)
					it++;
				continue;
			}


			lightInstanceLine = instancePosition - lightPosition;
			lightInstanceDistSq = D3DXVec3Dot(&lightInstanceLine, &lightInstanceLine);
			lightDirInstanceDir = -D3DXVec3Dot(&lightDirection, &lightInstanceLine);

			if ( lightDirInstanceDir > 0 && 
				 lightDirInstanceDir * lightDirInstanceDir >= lightInstanceDistSq * lightSineCosSq && 
				 lightInstanceDistSq > instanceRadius * instanceRadius )
			{
				// in the V2 and V3 cone but behind light position
				it++;
				while ((it != it_end) && ((*it)->getInstance() == instance))
					it++;
				continue;
			}

			// instance in the light cone

			D3DXMatrixMultiply(&worldLightViewProjectionMx, instance->getWorldPosition(), lightViewProjectionMx);

			if (FAILED(hr = effect->SetMatrix(shadowMapsState.h_worldLightViewProjectionMx, &worldLightViewProjectionMx)))
			{
				showError(false, "ID3DXEffect::SetValue error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
				return -205;
			}


			for (it; ( it != it_end ) && ( (*it)->getInstance() != instance ); it++)
			{
				if (!(*it)->applySkinningInfo(effect, shadowMapsState.h_bonesSkinningPalette, shadowMapsState.h_bonesInfluenceNumber))
					return -305;

				if (FAILED(hr = effect->CommitChanges()))
				{
					showError(false, "ID3DXEffect::CommitChanges error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
					return -206;
				}

				if (!(*it)->draw())
					return -301;
			}


		}

		if (FAILED(hr = effect->EndPass()))
		{
			showError(false, "ID3DXEffect::EndPass error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -207;
		}

	}

	if (FAILED(hr = effect->End()))
	{
		showError(false, "ID3DXEffect::End error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -208;
	}



	if (FAILED(hr = pDevice->SetRenderTarget(0, NULL)))
	{
		showError(false, "IDirect3DDevice9::SetRenderTarget error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -101;
	}

	if (shadowMapsState.useBlur)
	{
		if (renderVSMBlur(shadowMapsState.mapUsed, 1 - shadowMapsState.mapUsed))
			return 401;
	}

	return 0;
};



int Renderer::renderVSMLightOmni( Light*, list< RenderChunk* > >* _lightWithShadow, const D3DXVECTOR3* _nearFarPlane, int _SMIndexUsedFront, int _SMIndexUsedBack )
{
	Light* _light = _lightWithShadow->first;

	shadowMapsState.dualParaboloidFront = _SMIndexUsedFront;
	shadowMapsState.dualParaboloidBack = _SMIndexUsedBack;
	shadowMapsState.dualParaboloidFreeMap = 6 - ( _SMIndexUsedFront + _SMIndexUsedBack );

	D3DXVECTOR3* lightPosition = _light->getPosition();
	float lightRange = _light->getRange();
	float lightRangeInstanceRadiusSq = 0.0f;

	GraphicObject* goInstance = NULL;
	D3DXVECTOR3 instanceCenter;
	float instanceRadius = 0.0f;

	D3DXVECTOR3 lightInstanceVector;
	float lightInstanceDistSq = 0.0f;
/*
	bool instanceInfront = false;
	bool instanceBehind = false;
	list<RenderChunk*> smRenderChunks[2];

/*
	list<RenderChunk*>::iterator it = _lightWithShadow->second.begin();
	list<RenderChunk*>::iterator it_end = _lightWithShadow->second.end();
	while ( it != it_end )
	{
		goInstance = (*it)->getInstance();
//CHANGE IT!!!!!!!!!!!!!!!!!!!!
		goInstance->getBoundingRange(&instanceCenter, instanceRadius);

		lightInstanceDistance = lightPosition - instanceCenter;

		lightInstanceDistSq = lightInstanceDistance.x * lightInstanceDistance.x + lightInstanceDistance.y * lightInstanceDistance.y + lightInstanceDistance.z * lightInstanceDistance.z;

		lightRangeInstanceRadiusSq = lightRange + instanceRadius;
		lightRangeInstanceRadiusSq *= lightRangeInstanceRadiusSq;

		if (lightInstanceDistSq > lightRangeInstanceRadiusSq)
		{
			while ( ((++it) != it_end) && ((*it)->getInstance() == goInstance) );
			continue;
		}


		instanceInfront = lightInstanceDistance.z < instanceRadius ? true : false;
		instanceBehind = -lightInstanceDistance.z < instanceRadius ? true : false;

		do
		{
			if (instanceInfront)
				smRenderChunks[0].push_back(*it);

			if (instanceBehind)
				smRenderChunks[1].push_back(*it);
		}
		while ( ((++it) != it_end) && ((*it)->getInstance() == goInstance) )
	}
*/


	D3DXMATRIX lightViewProjectionMx;
	D3DXMatrixLookAtLH(&lightViewProjectionMx, &lightPosition, &D3DXVECTOR3(0.0f, 0.0f, 1.0f), &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	D3DXMatrixMultiply(&lightViewProjectionMx, &lightViewProjectionMx, &(shadowMapsState.lightsProjectionMx[LIGHT_OMNI]));
	D3DXMATRIX worldLightViewProjectionMx;

	ID3DXEffect* effect = shadowMapsState.pSMEffect->getResource();

	float omniDirection[2] = {1.0f, -1.0f};
	int SMIndexUsed[2] = {_SMIndexUsedFront, _SMIndexUsedBack};


	for (UINT i = 0; i < 2; ++i)
	{
		if (FAILED(hr = pDevice->SetRenderTarget(0, shadowMapsState.v_shadowMaps[SMIndexUsed[i]].renderTarget)))
		{
			showError(false, "IDirect3DDevice9::SetRenderTarget error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -101;
		}

		if (FAILED(hr = pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0)))
		{
			showError(false, "IDirect3DDevice9::Clear error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -102;
		}


		if (!_lightWithShadow->second.size())
			continue;


		if (FAILED(hr = effect->SetTechnique(shadowMapsState.h_technique[LIGHT_OMNI])))
		{
			showError(false, "ID3DXEffect::SetTechnique error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -201;
		}


		if (FAILED(hr = effect->SetValue(shadowMapsState.h_vec3NFPlane, &nearFarPlane, sizeof(D3DXVECTOR3))))
		{
			showError(false, "ID3DXEffect::SetValue error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -202;
		}

		if (FAILED(hr = effect->SetFloat(shadowMapsState.h_omniDirection, &(omniDirection[i]))))
		{
			showError(false, "ID3DXEffect::SetFloat error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -203;
		}


		// render to front map
		list<RenderChunk*>::iterator it = _lightWithShadow->second.begin();
		list<RenderChunk*>::iterator it_end = _lightWithShadow->second.end();

		UINT uiPasses = 0;
		if (FAILED(hr = effect->Begin(&uiPasses, 0)))
		{
			showError(false, "ID3DXEffect::Begin error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -204;
		}

		for (UINT i = 0; i < uiPasses; ++i)
		{
			if (FAILED(hr = effect->BeginPass(uiPasses)))
			{
				showError(false, "ID3DXEffect::BeginPass error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
				return -205;
			}

			it = _lightWithShadow->second.begin();

			while (it != it_end)
			{
				GraphicObject* instance = (*it)->getInstance();
				if (instance->getAnimationInfo())
					break;

				D3DXMatrixMultiply(&worldLightViewProjectionMx, instance->getWorldPosition(), &lightViewProjectionMx);

				if (FAILED(hr = effect->SetMatrix(shadowMapsState.h_worldLightViewProjectionMx, &worldLightViewProjectionMx)))
				{
					showError(false, "ID3DXEffect::SetMatrix error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
					return -206;
				}

				if (FAILED(hr = effect->CommitChanges()))
				{
					showError(false, "ID3DXEffect::Begin error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
					return -207;
				}


				for (it; (it != it_end) && ((*it)->getInstance() != instance); it++)
				{
					if (!(*it)->draw())
						return -301;
				}

			}

			if (FAILED(hr = effect->EndPass()))
			{
				showError(false, "ID3DXEffect::Begin error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
				return -208;
			}
		}

		if (FAILED(hr = effect->End()))
		{
			showError(false, "ID3DXEffect::Begin error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -209;
		}



		if (it != it_end)
		{
			if (shadowMapsState.useBlur)
			{
				if (renderVSMBlur(SMIndexUsed[i], shadowMapsState.dualParaboloidFreeMap))
					return 401;
			}

			continue;
		}



		if (FAILED(hr = effect->SetTechnique(shadowMapsState.h_techniqueSkinned[LIGHT_OMNI])))
		{
			showError(false, "ID3DXEffect::SetTechnique error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -201;
		}


		if (FAILED(hr = effect->SetValue(shadowMapsState.h_vec3NFPlane, &nearFarPlane, sizeof(D3DXVECTOR3))))
		{
			showError(false, "ID3DXEffect::SetValue error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -202;
		}

		if (FAILED(hr = effect->SetFloat(shadowMapsState.h_omniDirection, &(omniDirection[i]))))
		{
			showError(false, "ID3DXEffect::SetFloat error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -203;
		}


		list<RenderChunk*>::iterator it_begin = it;

		uiPasses = 0;
		if (FAILED(hr = effect->Begin(&uiPasses, 0)))
		{
			showError(false, "ID3DXEffect::Begin error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -204;
		}

		for (UINT i = 0; i < uiPasses; ++i)
		{
			if (FAILED(hr = effect->BeginPass(uiPasses)))
			{
				showError(false, "ID3DXEffect::BeginPass error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
				return -205;
			}

			it = it_begin;

			while (it != it_end)
			{
				GraphicObject* instance = (*it)->getInstance();
				AnimationInfo* instanceAnimationInfo = instance->getAnimationInfo();

				D3DXMatrixMultiply(&worldLightViewProjectionMx, instance->getWorldPosition(), &lightViewProjectionMx);

				if (FAILED(hr = effect->SetMatrix(shadowMapsState.h_worldLightViewProjectionMx, &worldLightViewProjectionMx)))
				{
					showError(false, "ID3DXEffect::SetMatrix error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
					return -206;
				}

				if (!(*it)->applySkinningInfo(effect, shadowMapsState.h_bonesSkinningPalette, shadowMapsState.h_bonesInfluenceNumber))
					return -302;

				if (FAILED(hr = effect->CommitChanges()))
				{
					showError(false, "ID3DXEffect::Begin error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
					return -207;
				}


				for (it; (it != it_end) && ((*it)->getInstance() != instance); it++)
				{
					if (!(*it)->draw())
						return -301;
				}

			}

			if (FAILED(hr = effect->EndPass()))
			{
				showError(false, "ID3DXEffect::Begin error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
				return -208;
			}
		}

		if (FAILED(hr = effect->End()))
		{
			showError(false, "ID3DXEffect::Begin error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -209;
		}


		if (shadowMapsState.useBlur)
		{
			if (renderVSMBlur(SMIndexUsed[i], shadowMapsState.dualParaboloidFreeMap))
				return 401;
		}
	}


	if (FAILED(hr = pDevice->SetRenderTarget(0, NULL)))
	{
		showError(false, "IDirect3DDevice9::SetRenderTarget error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -103;
	}

	return 0;
};



int Renderer::renderVSMBlur(int _bluredSMIndex, int _tmpSMIndex)
{
	ID3DXEffect* effect = shadowMapsState.pSMEffect->getResource();
	int samplesNumber = shadowMapsState.sampleWeights.size();


	if (FAILED(hr = pDevice->SetFVF(NULL)))
	{
		showError(false, "IDirect3DDevice9::SetFVF error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -101;
	}

	if (FAILED(hr = pDevice->SetVertexDeclaration(p_fullscreenQuadVertexDecl)))
	{
		showError(false, "IDirect3DDevice9::SetVertexDeclaration error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -102;
	}

	if (FAILED(hr = pDevice->SetStreamSource(0, shadowMapsState.p_blurFullscreenQuad, 0, sizeof(FullscreenQuadVertex))))
	{
		showError(false, "IDirect3DDevice9::SetStreamSource error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -103;
	}



	if (FAILED(hr = effect->SetTechnique(shadowMapsState.h_blurTechnique)))
	{
		showError(false, "ID3DXEffect::SetTechnique error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -201;
	}

	if (FAILED(hr = effect->SetInt(shadowMapsState.h_blurSampleNumber, samplesNumber)))
	{
		showError(false, "ID3DXEffect::SetInt error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -202;
	}

	if (FAILED(hr = effect->SetFloatArray(shadowMapsState.h_blurSampleWeights, &(shadowMapsState.sampleWeights[0]), samplesNumber)))
	{
		showError(false, "ID3DXEffect::SetFloatArray error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -203;
	}



	////  HORIZONTAL  //////////////////////

	if (FAILED(hr = pDevice->SetRenderTarget(0, shadowMapsState.v_shadowMaps[_tmpSMIndex].renderTarget)))
	{
		showError(false, "IDirect3DDevice9::SetRenderTarget error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -104;
	}

	if (FAILED(hr = effect->SetTexture(shadowMapsState.h_blurInputTexture, shadowMapsState.v_shadowMaps[_bluredSMIndex].renderTexture)))
	{
		showError(false, "ID3DXEffect::SetTexture error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -204;
	}

	if (FAILED(hr = effect->SetRawValue(shadowMapsState.h_blurSampleOffsets, &(shadowMapsState.sampleOffsetsHorizontal[0]), 0, samplesNumber * sizeof(D3DXVECTOR4))))
	{
		showError(false, "ID3DXEffect::SetRawValue error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -205;
	}



	UINT uiPasses = 0;
	if (FAILED(hr = effect->Begin(&uiPasses, 0)))
	{
		showError(false, "ID3DXEffect::Begin error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -301;
	}

	for (UINT i = 0; i < uiPasses; ++i)
	{
		if (FAILED(hr = effect->BeginPass(i)))
		{
			showError(false, "ID3DXEffect::BeginPass error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -302;
		}

		if (FAILED(hr = pDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2)))
		{
			showError(false, "ID3DXEffect::DrawPrimitive error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -303;
		}

		if (FAILED(hr = effect->EndPass()))
		{
			showError(false, "ID3DXEffect::EndPass error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -304;
		}
	}

	if (FAILED(hr = effect->End()))
	{
		showError(false, "ID3DXEffect::End error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -305;
	}



	////  VERTICAL  //////////////////////

	if (FAILED(hr = pDevice->SetRenderTarget(0, shadowMapsState.v_shadowMaps[_bluredSMIndex].renderTarget)))
	{
		showError(false, "IDirect3DDevice9::SetRenderTarget error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -104;
	}

	if (FAILED(hr = effect->SetTexture(shadowMapsState.h_blurInputTexture, shadowMapsState.v_shadowMaps[_tmpSMIndex].renderTexture)))
	{
		showError(false, "ID3DXEffect::SetTexture error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -204;
	}

	if (FAILED(hr = effect->SetRawValue(shadowMapsState.h_blurSampleOffsets, &(shadowMapsState.sampleOffsetsVertical[0]), 0, samplesNumber * sizeof(D3DXVECTOR4))))
	{
		showError(false, "ID3DXEffect::SetRawValue error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -205;
	}



	UINT uiPasses = 0;
	if (FAILED(hr = effect->Begin(&uiPasses, 0)))
	{
		showError(false, "ID3DXEffect::Begin error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -301;
	}

	for (UINT i = 0; i < uiPasses; ++i)
	{
		if (FAILED(hr = effect->BeginPass(i)))
		{
			showError(false, "ID3DXEffect::BeginPass error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -302;
		}

		if (FAILED(hr = pDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2)))
		{
			showError(false, "ID3DXEffect::DrawPrimitive error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -303;
		}

		if (FAILED(hr = effect->EndPass()))
		{
			showError(false, "ID3DXEffect::EndPass error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -304;
		}
	}

	if (FAILED(hr = effect->End()))
	{
		showError(false, "ID3DXEffect::End error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -305;
	}



	// free local settings

	if (FAILED(hr = pDevice->SetRenderTarget(0, NULL)))
	{
		showError(false, "IDirect3DDevice9::SetRenderTarget error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -105;
	}

	if (FAILED(hr = pDevice->SetVertexDeclaration(NULL)))
	{
		showError(false, "IDirect3DDevice9::SetVertexDeclaration error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -106;
	}

	if (FAILED(hr = pDevice->SetStreamSource(0, NULL, 0, 0)))
	{
		showError(false, "IDirect3DDevice9::SetStreamSource error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -107;
	}


	return 0;
};



int Renderer::renderLightTransportAmbient()
{

	HRESULT hr = S_OK;


	if (FAILED(hr = pDevice->SetRenderTarget(0, lightTransportState.lightTransport.renderTarget)))
	{
		showError(false, "IDirect3DDevice9::SetRenderTarget error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -101;
	}


	// draw ambient transport
	ID3DXEffect* effect = lightTransport.pLTEffect->getResourcePointer();

	if (FAILED(hr = effect->SetTechnique(lightTransportState.h_techniqueAmbient)))
	{
		showError(false, "ID3DXEffect::SetTechnique error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -201;
	}

	if (FAILED(hr = effect->SetVector(lightTransportState.h_lightAmbient, &ambientLight)))
	{
		showError(false, "ID3DXEffect::SetVector error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -202;
	}

	if (FAILED(hr = effect->SetTexture(lightTransportState.h_colorMap, MRTState.v_renderTargets[MRT_COLOR_INDEX].renderTexture)))
	{
		showError(false, "ID3DXEffect::SetTexture error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -203;
	}


	if (FAILED(hr = pDevice->SetFVF(NULL)))
	{
		showError(false, "IDirect3DDevice9::SetFVF error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -102;
	}

	if (FAILED(hr = pDevice->SetVertexDeclaration(p_fullscreenQuadVertexDecl)))
	{
		showError(false, "IDirect3DDevice9::SetVertexDeclaration error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -103;
	}

	if (FAILED(hr = pDevice->SetStreamSource(0, p_fullscreenQuad, 0, sizeof(FullscreenQuadVertex))))
	{
		showError(false, "IDirect3DDevice9::SetStreamSource error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -104;
	}


	UINT uiPasses = 0;
	if (FAILED(hr = effect->Begin(&uiPasses, 0)))
	{
		showError(false, "ID3DXEffect::Begin error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -204;
	}

	for (UINT i = 0; i < uiPasses; ++i)
	{
		if (FAILED(hr = effect->BeginPass(i)))
		{
			showError(false, "ID3DXEffect::BeginPass error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -205;
		}

		if (FAILED(hr = pDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2)))
		{
			showError(false, "IDirect3DDevice9::DrawPrimitive error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -206;
		}

		if (FAILED(hr = effect->EndPass()))
		{
			showError(false, "ID3DXEffect::EndPass error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -207;
		}
	}

	if (FAILED(hr = effect->End()))
	{
		showError(false, "ID3DXEffect::End error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -208;
	}



	if (FAILED(hr = pDevice->SetFVF(NULL)))
	{
		showError(false, "IDirect3DDevice9::SetFVF error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -102;
	}

	if (FAILED(hr = pDevice->SetVertexDeclaration(NULL)))
	{
		showError(false, "IDirect3DDevice9::SetVertexDeclaration error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -103;
	}

	if (FAILED(hr = pDevice->SetStreamSource(0, NULL, 0, 0)))
	{
		showError(false, "IDirect3DDevice9::SetStreamSource error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -104;
	}

	if (FAILED(hr = pDevice->SetRenderTarget(0, NULL)))
	{
		showError(false, "IDirect3DDevice9::SetRenderTarget error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -101;
	}


	return 0;
};



int Renderer::renderLightTransportDirectional( pair< Light*, list< RenderChunk* > >* _lightWithShadow, int _smIndex, const D3DXVECTOR3* _nearFarPlane )
{
	Light* _light = _lightWithShadow->first;

	HRESULT hr = S_OK;


	if (FAILED(hr = pDevice->SetRenderTarget(0, lightTransportState.lightTransport.renderTarget)))
	{
		showError(false, "IDirect3DDevice9::SetRenderTarget error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -101;
	}

	if (FAILED(hr = pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE)))
	{
		showError(false, "IDirect3DDevice9::SetRenderState error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -102;
	}


	// set quad for fullscreen rendering

	if (FAILED(hr = pDevice->SetFVF(NULL)))
	{
		showError(false, "IDirect3DDevice9::SetFVF error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -103;
	}

	if (FAILED(hr = pDevice->SetVertexDeclaration(p_fullscreenQuadVertexDecl)))
	{
		showError(false, "IDirect3DDevice9::SetVertexDeclaration error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -104;
	}

	if (FAILED(hr = pDevice->SetStreamSource(0, p_fullscreenQuad, 0, sizeof(FullscreenQuadVertex))))
	{
		showError(false, "IDirect3DDevice9::SetStreamSource error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -105;
	}



	ID3DXEffect* effect = lightTransportState.pLTEffect->getResourcePointer();

	bool setShadowMaps = ( lightTransportState.useShadowMaps && _smIndex >= 0 ) ? true : false ;

	D3DXHANDLE technique = ( lightTransportState.useShadowMaps && _smIndex < 0 ) ? 
							lightTransportState.h_techniqueNoSM[LIGHT_DIRECTIONAL] : lightTransportState.h_technique[LIGHT_DIRECTIONAL] ;


	// set basic effect values

	if (FAILED(hr = effect->SetTechnique(technique)))
	{
		showError(false, "ID3DXEffect::SetTechnique error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -201;
	}


	if (FAILED(hr = effect->SetValue(lightTransportState.h_cameraPosition, usedCamera->getPosition(), sizeof(D3DXVECTOR3))))
	{
		showError(false, "ID3DXEffect::SetValue error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -202;
	}

	if (FAILED(hr = effect->SetRawValue(lightTransportState.h_cameraInvViewProjectionMx, usedCamera->getPosition(), 0, sizeof(D3DXMATRIX))))
	{
		showError(false, "ID3DXEffect::SetRawValue error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -203;
	}


	UINT lightArraySize = 0;
	D3DXVECTOR4* lightArray = _light->getLightArray(&lightArraySize);

	if (FAILED(hr = effect->SetRawValue(lightTransportState.h_lightTable, lightArray, 0, lightArraySize * sizeof(D3DXVECTOR3))))
	{
		showError(false, "ID3DXEffect::SetRawValue error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -204;
	}


	// set mrt

	if (FAILED(hr = effect->SetTexture(lightTransportState.h_colorMap, MRTState.v_renderTargets[MRT_COLOR_INDEX].renderTexture)))
	{
		showError(false, "ID3DXEffect::SetTexture error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -205;
	}

	if (FAILED(hr = effect->SetTexture(lightTransportState.h_normalMap, MRTState.v_renderTargets[MRT_NORMAL_INDEX].renderTexture)))
	{
		showError(false, "ID3DXEffect::SetTexture error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -206;
	}

	if (FAILED(hr = effect->SetTexture(lightTransportState.h_depthMap, MRTState.v_renderTargets[MRT_DEPTH_INDEX].renderTexture)))
	{
		showError(false, "ID3DXEffect::SetTexture error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -207;
	}


	// set shadowmaps if needed

	if (setShadowMaps)
	{
		D3DXVECTOR3 lightPosition;
		D3DXVec3Normalize(&lightPosition, light->getDirection());
		lightPosition *= DIRECTIONAL_LIGHT_DISTANCE_FROM_CAMERA;
		lightPosition = (*usedCamera->getPosition()) - lightPosition;
		D3DXMATRIX lightViewProjection;
		D3DXMatrixLookAtLH(&lightViewProjection, lightPosition, light->getDirection(), &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
		lightViewProjection *= shadowMapsState.lightsProjectionMx[LIGHT_DIRECTIONAL];

		if (FAILED(hr = effect->SetRawValue(lightTransportState.h_lightViewProjectionMx, &lightViewProjection, 0, sizeof(D3DXMATRIX))))
		{
			showError(false, "ID3DXEffect::SetRawValue error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -208;
		}

		if (FAILED(hr = effect->SetRawValue(lightTransportState.h_lightShadowMapMx, &shadowMapsState.lightShadowMapMx, 0, sizeof(D3DXMATRIX))))
		{
			showError(false, "ID3DXEffect::SetRawValue error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -209;
		}

		if (FAILED(hr = effect->SetValue(lightTransportState.h_lightNearFarPlane, _nearFarPlane, sizeof(D3DXVECTOR3))))
		{
			showError(false, "ID3DXEffect::SetValue error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -210;
		}

		if (FAILED(hr = effect->SetTexture(lightTransportState.h_shadowMap, shadowMapsState.v_shadowMaps[_smIndex])))
		{
			showError(false, "ID3DXEffect::SetTexture error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -211;
		}
	}


	// draw light transport

	UINT uiPasses = 0;
	if (FAILED(hr = effect->Begin(&uiPasses, 0)))
	{
		showError(false, "ID3DXEffect::Begin error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -301;
	}

	for (UINT i = 0; i < uiPasses; ++i)
	{
		if (FAILED(hr = effect->BeginPass(i)))
		{
			showError(false, "ID3DXEffect::BeginPass error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -302;
		}

		if (FAILED(hr = pDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2)))
		{
			showError(false, "IDirect3DDevice9::DrawPrimitive error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -303;
		}

		if (FAILED(hr = effect->EndPass()))
		{
			showError(false, "ID3DXEffect::EndPass error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -304;
		}
	}

	if (FAILED(hr = effect->End()))
	{
		showError(false, "ID3DXEffect::End error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -305;
	}


	// clean device settings

	if (FAILED(hr = pDevice->SetRenderTarget(0, NULL)))
	{
		showError(false, "IDirect3DDevice9::SetRenderTarget error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -101;
	}

	if (FAILED(hr = pDevice->SetFVF(NULL)))
	{
		showError(false, "IDirect3DDevice9::SetFVF error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -103;
	}

	if (FAILED(hr = pDevice->SetVertexDeclaration(NULL)))
	{
		showError(false, "IDirect3DDevice9::SetVertexDeclaration error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -104;
	}

	if (FAILED(hr = pDevice->SetStreamSource(0, NULL, 0, 0)))
	{
		showError(false, "IDirect3DDevice9::SetStreamSource error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -105;
	}


	return 0;
};



int Renderer::renderLightTransportSpot( pair< Light*, list< RenderChunk* > >* _lightWithShadow, int _smIndex, const D3DXVECTOR3* _nearFarPlane )
{
	Light* _light = _lightWithShadow->first;

	HRESULT hr = S_OK;

	if (FAILED(hr = pDevice->SetRenderTarget(0, lightTransportState.lightTransport.renderTarget)))
	{
		showError(false, "IDirect3DDevice9::SetRenderTarget error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -101;
	}

	if (FAILED(hr = pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE)))
	{
		showError(false, "IDirect3DDevice9::SetRenderState error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -102;
	}


// set up scissor rect
	D3DXVECTOR3* lightPosition = _light->getPosition();
	D3DXVECTOR3* lightDir = _light->getDirection();

	D3DXVECTOR3 coneUpDir;
	D3DXVec3Normalize(&coneUpDir, D3DXVec3Cross(&coneUpDir, D3DXVec3Cross(&coneUpDir, usedCamera->getNormalizedUpDirection(), lightDir), lightDir));
	D3DXVECTOR3 coneRightDir;
	D3DXVec3Normalize(&coneRightDir, D3DXVec3Cross(&coneRightDir, D3DXVec3Cross(&coneRightDir, usedCamera->getNormalizedRightDirection(), lightDir), lightDir));

	float r = _light->getRange();
	float cos;
	_light->getSpotlight(NULL, &cos, NULL);
	float c = r / cos;
	float a = pow((c * c) - (r * r), 0.5f);

	D3DXVECTOR3 conePoints[5];
	D3DXVECTOR3 lr = *lightDir * r;
	conePoints[0] = *lightPosition;

	D3DXVECTOR3 tmp = coneUpDir * a;
	conePoints[1] = conePoints[0] + lr + tmp;
	conePoints[2] = conePoints[0] + lr - tmp;

	D3DXVECTOR3 tmp = coneRightDir * a;
	conePoints[3] = conePoints[0] + lr + tmp;
	conePoints[4] = conePoints[0] + lr - tmp;


	D3DXMATRIX* viewProjectionMx = usedCamera->getViewProjectionMatrix();
	D3DXVECTOR4 projPoint;
	LONG minX = 0xFFFFFFFF;
	LONG maxX = 0x00000000;
	LONG minY = 0xFFFFFFFF;
	LONG maxY = 0x00000000;
	LONG x = 0;
	LONG y = 0;

	for (UINT i = 0; i < 5; ++i)
	{
		D3DXVec3Transform(&projPoint, &conePoints[i], viewProjectionMx);
		x = LONG((clip(projPoint.x / projPoint.w, -1.0f, 1.0f) + 1.0f) *  0.5f * displayMode.Width);
		y = LONG((clip(projPoint.y / projPoint.w, -1.0f, 1.0f) - 1.0f) * -0.5f * displayMode.Height);

		if (x < minX)	minX = x;
		if (x > maxX)	maxX = x;
		if (y < minY)	minY = y;
		if (y > maxY)	maxY = y;
	}

	RECT clipRect;
	clipRect.top    = minY;
	clipRect.bottom = maxY;
	clipRect.left   = minX;
	clipRect.right  = maxX;



	if (FAILED(hr = pDevice->SetScissorRect(&clipRect)))
	{
		showError(false, "IDirect3DDevice9::SetScissorRect error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -103;
	}


	// set quad for fullscreen rendering

	if (FAILED(hr = pDevice->SetFVF(NULL)))
	{
		showError(false, "IDirect3DDevice9::SetFVF error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -104;
	}

	if (FAILED(hr = pDevice->SetVertexDeclaration(p_fullscreenQuadVertexDecl)))
	{
		showError(false, "IDirect3DDevice9::SetVertexDeclaration error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -105;
	}

	if (FAILED(hr = pDevice->SetStreamSource(0, p_fullscreenQuad, 0, sizeof(FullscreenQuadVertex))))
	{
		showError(false, "IDirect3DDevice9::SetStreamSource error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -106;
	}



	ID3DXEffect* effect = lightTransportState.pLTEffect->getResourcePointer();

	bool setShadowMaps = ( lightTransportState.useShadowMaps && _smIndex >= 0 ) ? true : false ;

	D3DXHANDLE technique = ( lightTransportState.useShadowMaps && _smIndex < 0 ) ? 
							lightTransportState.h_techniqueNoSM[LIGHT_SPOT] : lightTransportState.h_technique[LIGHT_SPOT] ;


	// set basic effect values

	if (FAILED(hr = effect->SetTechnique(technique)))
	{
		showError(false, "ID3DXEffect::SetTechnique error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -201;
	}


	if (FAILED(hr = effect->SetValue(lightTransportState.h_cameraPosition, usedCamera->getPosition(), sizeof(D3DXVECTOR3))))
	{
		showError(false, "ID3DXEffect::SetValue error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -202;
	}

	if (FAILED(hr = effect->SetRawValue(lightTransportState.h_cameraInvViewProjectionMx, usedCamera->getPosition(), 0, sizeof(D3DXMATRIX))))
	{
		showError(false, "ID3DXEffect::SetRawValue error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -203;
	}


	UINT lightArraySize = 0;
	D3DXVECTOR4* lightArray = _light->getLightArray(&lightArraySize);

	if (FAILED(hr = effect->SetRawValue(lightTransportState.h_lightTable, lightArray, 0, lightArraySize * sizeof(D3DXVECTOR3))))
	{
		showError(false, "ID3DXEffect::SetRawValue error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -204;
	}


	// set mrt

	if (FAILED(hr = effect->SetTexture(lightTransportState.h_colorMap, MRTState.v_renderTargets[MRT_COLOR_INDEX].renderTexture)))
	{
		showError(false, "ID3DXEffect::SetTexture error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -205;
	}

	if (FAILED(hr = effect->SetTexture(lightTransportState.h_normalMap, MRTState.v_renderTargets[MRT_NORMAL_INDEX].renderTexture)))
	{
		showError(false, "ID3DXEffect::SetTexture error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -206;
	}

	if (FAILED(hr = effect->SetTexture(lightTransportState.h_depthMap, MRTState.v_renderTargets[MRT_DEPTH_INDEX].renderTexture)))
	{
		showError(false, "ID3DXEffect::SetTexture error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -207;
	}


	// set shadowmaps if needed

	if (setShadowMaps)
	{
		D3DXVECTOR3 lightPosition;
		D3DXVec3Normalize(&lightPosition, light->getDirection());
		lightPosition *= DIRECTIONAL_LIGHT_DISTANCE_FROM_CAMERA;
		lightPosition = (*usedCamera->getPosition()) - lightPosition;
		D3DXMATRIX lightViewProjection;
		D3DXMatrixLookAtLH(&lightViewProjection, lightPosition, light->getDirection(), &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
		lightViewProjection *= shadowMapsState.lightsProjectionMx[LIGHT_DIRECTIONAL];

		if (FAILED(hr = effect->SetRawValue(lightTransportState.h_lightViewProjectionMx, &lightViewProjection, 0, sizeof(D3DXMATRIX))))
		{
			showError(false, "ID3DXEffect::SetRawValue error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -208;
		}

		if (FAILED(hr = effect->SetRawValue(lightTransportState.h_lightShadowMapMx, &shadowMapsState.lightShadowMapMx, 0, sizeof(D3DXMATRIX))))
		{
			showError(false, "ID3DXEffect::SetRawValue error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -209;
		}

		if (FAILED(hr = effect->SetValue(lightTransportState.h_lightNearFarPlane, _nearFarPlane, sizeof(D3DXVECTOR3))))
		{
			showError(false, "ID3DXEffect::SetValue error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -210;
		}

		if (FAILED(hr = effect->SetTexture(lightTransportState.h_shadowMap, shadowMapsState.v_shadowMaps[_smIndex])))
		{
			showError(false, "ID3DXEffect::SetTexture error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -211;
		}
	}


	// draw light transport

	UINT uiPasses = 0;
	if (FAILED(hr = effect->Begin(&uiPasses, 0)))
	{
		showError(false, "ID3DXEffect::Begin error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -301;
	}

	for (UINT i = 0; i < uiPasses; ++i)
	{
		if (FAILED(hr = effect->BeginPass(i)))
		{
			showError(false, "ID3DXEffect::BeginPass error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -302;
		}

		if (FAILED(hr = pDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2)))
		{
			showError(false, "IDirect3DDevice9::DrawPrimitive error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -303;
		}

		if (FAILED(hr = effect->EndPass()))
		{
			showError(false, "ID3DXEffect::EndPass error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -304;
		}
	}

	if (FAILED(hr = effect->End()))
	{
		showError(false, "ID3DXEffect::End error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -305;
	}


	// clean device settings

	if (FAILED(hr = pDevice->SetRenderTarget(0, NULL)))
	{
		showError(false, "IDirect3DDevice9::SetRenderTarget error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -101;
	}

	if (FAILED(hr = pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE)))
	{
		showError(false, "IDirect3DDevice9::SetRenderState error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -102;
	}

	if (FAILED(hr = pDevice->SetFVF(NULL)))
	{
		showError(false, "IDirect3DDevice9::SetFVF error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -104;
	}

	if (FAILED(hr = pDevice->SetVertexDeclaration(NULL)))
	{
		showError(false, "IDirect3DDevice9::SetVertexDeclaration error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -105;
	}

	if (FAILED(hr = pDevice->SetStreamSource(0, NULL, 0, 0)))
	{
		showError(false, "IDirect3DDevice9::SetStreamSource error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -106;
	}


	return 0;
};



int Renderer::renderLightTransportOmni( pair< Light*, list< RenderChunk* > >* _lightWithShadow, int _smFrontIndex, int _smBackIndex, const D3DXVECTOR3* _nearFarPlane )
{
	Light* _light = _lightWithShadow.first;

	HRESULT hr = S_OK;

	if (FAILED(hr = pDevice->SetRenderTarget(0, lightTransportState.lightTransport.renderTarget)))
	{
		showError(false, "IDirect3DDevice9::SetRenderTarget error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -101;
	}

	if (FAILED(hr = pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE)))
	{
		showError(false, "IDirect3DDevice9::SetRenderState error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -102;
	}


// set up scissor rect
	D3DXVECTOR3* lightPosition = _light->getPosition();

	float r = _light->getRange();

	D3DXVECTOR3 upDist = *(usedCamera->getNormalizedUpDirection()) * r;
	D3DXVECTOR3 rightDist = *(usedCamera->getNormalizedRightDirection()) * r;

	D3DXVECTOR3 conePoints[4];
	conePoints[0] = *lightPosition + upDist;
	conePoints[1] = *lightPosition - upDist;
	conePoints[2] = *lightPosition + rightDist;
	conePoints[3] = *lightPosition - rightDist;


	D3DXMATRIX* viewProjectionMx = usedCamera->getViewProjectionMatrix();
	D3DXVECTOR4 projPoint;
	LONG minX = 0xFFFFFFFF;
	LONG maxX = 0x00000000;
	LONG minY = 0xFFFFFFFF;
	LONG maxY = 0x00000000;
	LONG x = 0;
	LONG y = 0;

	for (UINT i = 0; i < 4; ++i)
	{
		D3DXVec3Transform(&projPoint, &conePoints[i], viewProjectionMx);
		x = LONG((clip(projPoint.x / projPoint.w, -1.0f, 1.0f) + 1.0f) *  0.5f * displayMode.Width);
		y = LONG((clip(projPoint.y / projPoint.w, -1.0f, 1.0f) - 1.0f) * -0.5f * displayMode.Height);

		if (x < minX)	minX = x;
		if (x > maxX)	maxX = x;
		if (y < minY)	minY = y;
		if (y > maxY)	maxY = y;
	}

	RECT clipRect;
	clipRect.top    = minY;
	clipRect.bottom = maxY;
	clipRect.left   = minX;
	clipRect.right  = maxX;



	if (FAILED(hr = pDevice->SetScissorRect(&clipRect)))
	{
		showError(false, "IDirect3DDevice9::SetScissorRect error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -103;
	}


	// set quad for fullscreen rendering

	if (FAILED(hr = pDevice->SetFVF(NULL)))
	{
		showError(false, "IDirect3DDevice9::SetFVF error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -104;
	}

	if (FAILED(hr = pDevice->SetVertexDeclaration(p_fullscreenQuadVertexDecl)))
	{
		showError(false, "IDirect3DDevice9::SetVertexDeclaration error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -105;
	}

	if (FAILED(hr = pDevice->SetStreamSource(0, p_fullscreenQuad, 0, sizeof(FullscreenQuadVertex))))
	{
		showError(false, "IDirect3DDevice9::SetStreamSource error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -106;
	}



	ID3DXEffect* effect = lightTransportState.pLTEffect->getResourcePointer();

	bool setShadowMaps = ( lightTransportState.useShadowMaps && _smFrontIndex >= 0  && _smBackIndex >= 0 ) ? true : false ;

	D3DXHANDLE technique = ( lightTransportState.useShadowMaps && ( _smFrontIndex < 0 || _smBackIndex < 0 ) ) ? 
							lightTransportState.h_techniqueNoSM[LIGHT_OMNI] : lightTransportState.h_technique[LIGHT_OMNI] ;


	// set basic effect values

	if (FAILED(hr = effect->SetTechnique(technique)))
	{
		showError(false, "ID3DXEffect::SetTechnique error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -201;
	}


	if (FAILED(hr = effect->SetValue(lightTransportState.h_cameraPosition, usedCamera->getPosition(), sizeof(D3DXVECTOR3))))
	{
		showError(false, "ID3DXEffect::SetValue error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -202;
	}

	if (FAILED(hr = effect->SetRawValue(lightTransportState.h_cameraInvViewProjectionMx, usedCamera->getPosition(), 0, sizeof(D3DXMATRIX))))
	{
		showError(false, "ID3DXEffect::SetRawValue error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -203;
	}


	UINT lightArraySize = 0;
	D3DXVECTOR4* lightArray = _light->getLightArray(&lightArraySize);

	if (FAILED(hr = effect->SetRawValue(lightTransportState.h_lightTable, lightArray, 0, lightArraySize * sizeof(D3DXVECTOR3))))
	{
		showError(false, "ID3DXEffect::SetRawValue error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -204;
	}


	// set mrt

	if (FAILED(hr = effect->SetTexture(lightTransportState.h_colorMap, MRTState.v_renderTargets[MRT_COLOR_INDEX].renderTexture)))
	{
		showError(false, "ID3DXEffect::SetTexture error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -205;
	}

	if (FAILED(hr = effect->SetTexture(lightTransportState.h_normalMap, MRTState.v_renderTargets[MRT_NORMAL_INDEX].renderTexture)))
	{
		showError(false, "ID3DXEffect::SetTexture error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -206;
	}

	if (FAILED(hr = effect->SetTexture(lightTransportState.h_depthMap, MRTState.v_renderTargets[MRT_DEPTH_INDEX].renderTexture)))
	{
		showError(false, "ID3DXEffect::SetTexture error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -207;
	}


	// set shadowmaps if needed

	if (setShadowMaps)
	{
		D3DXVECTOR3 lightPosition;
		D3DXVec3Normalize(&lightPosition, light->getDirection());
		lightPosition *= DIRECTIONAL_LIGHT_DISTANCE_FROM_CAMERA;
		lightPosition = (*usedCamera->getPosition()) - lightPosition;
		D3DXMATRIX lightViewProjection;
		D3DXMatrixLookAtLH(&lightViewProjection, lightPosition, light->getDirection(), &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
		lightViewProjection *= shadowMapsState.lightsProjectionMx[LIGHT_DIRECTIONAL];

		if (FAILED(hr = effect->SetRawValue(lightTransportState.h_lightViewProjectionMx, &lightViewProjection, 0, sizeof(D3DXMATRIX))))
		{
			showError(false, "ID3DXEffect::SetRawValue error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -208;
		}

		if (FAILED(hr = effect->SetRawValue(lightTransportState.h_lightShadowMapMx, &shadowMapsState.lightShadowMapMx, 0, sizeof(D3DXMATRIX))))
		{
			showError(false, "ID3DXEffect::SetRawValue error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -209;
		}

		if (FAILED(hr = effect->SetValue(lightTransportState.h_lightNearFarPlane, _nearFarPlane, sizeof(D3DXVECTOR3))))
		{
			showError(false, "ID3DXEffect::SetValue error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -210;
		}

		if (FAILED(hr = effect->SetTexture(lightTransportState.h_shadowFrontMap, shadowMapsState.v_shadowMaps[_smFrontIndex])))
		{
			showError(false, "ID3DXEffect::SetTexture error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -211;
		}

		if (FAILED(hr = effect->SetTexture(lightTransportState.h_shadowBackMap, shadowMapsState.v_shadowMaps[_smBackIndex])))
		{
			showError(false, "ID3DXEffect::SetTexture error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -211;
		}
	}


	// draw light transport

	UINT uiPasses = 0;
	if (FAILED(hr = effect->Begin(&uiPasses, 0)))
	{
		showError(false, "ID3DXEffect::Begin error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -301;
	}

	for (UINT i = 0; i < uiPasses; ++i)
	{
		if (FAILED(hr = effect->BeginPass(i)))
		{
			showError(false, "ID3DXEffect::BeginPass error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -302;
		}

		if (FAILED(hr = pDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2)))
		{
			showError(false, "IDirect3DDevice9::DrawPrimitive error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -303;
		}

		if (FAILED(hr = effect->EndPass()))
		{
			showError(false, "ID3DXEffect::EndPass error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -304;
		}
	}

	if (FAILED(hr = effect->End()))
	{
		showError(false, "ID3DXEffect::End error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -305;
	}


	// clean device settings

	if (FAILED(hr = pDevice->SetRenderTarget(0, NULL)))
	{
		showError(false, "IDirect3DDevice9::SetRenderTarget error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -101;
	}

	if (FAILED(hr = pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE)))
	{
		showError(false, "IDirect3DDevice9::SetRenderState error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -102;
	}

	if (FAILED(hr = pDevice->SetFVF(NULL)))
	{
		showError(false, "IDirect3DDevice9::SetFVF error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -104;
	}

	if (FAILED(hr = pDevice->SetVertexDeclaration(NULL)))
	{
		showError(false, "IDirect3DDevice9::SetVertexDeclaration error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -105;
	}

	if (FAILED(hr = pDevice->SetStreamSource(0, NULL, 0, 0)))
	{
		showError(false, "IDirect3DDevice9::SetStreamSource error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -106;
	}


	return 0;
};



int Renderer::renderLightTransport()
{
	HRESULT hr = S_OK;

	if (FAILED(hr = pDevice->SetRenderTarget(0, lightTransportState.lightTransport.renderTarget)))
	{
		showError(false, "IDirect3DDevice9::SetRenderTarget error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -101;
	}

	if (FAILED(hr = pDevice->Clear(0, NULL, D3DCLEAR_TARGET, defaultBackgroundColor, 1.0f, 0)))
	{
		showError(false, "IDirect3DDevice9::Clear error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -102;
	}


	if (renderLightTransportAmbient())
		return -401;

	if (!l_usedLightsWithShadows.size())
		return 0;

	l_usedLightsWithShadows.sort(light_with_shadows_compare);

	float nearPlane = usedCamera->getNearViewPlaneDistance();
	float farPlane = usedCamera->getFarViewPlaneDistance();

	// update light matrices for shadow mapping
	shadowMapsState.lightsProjectionMx[LIGHT_DIRECTIONAL]._33 = 1.0f / (farPlane - nearPlane);
	shadowMapsState.lightsProjectionMx[LIGHT_DIRECTIONAL]._43 = nearPlane / (nearPlane - farPlane);
	shadowMapsState.lightsProjectionMx[LIGHT_POINT]._33 = shadowMapsState.lightsProjectionMx[LIGHT_SPOT]._33 = farPlane / (farPlane - nearPlane);
	shadowMapsState.lightsProjectionMx[LIGHT_POINT]._43 = shadowMapsState.lightsProjectionMx[LIGHT_SPOT]._43 = (-nearPlane * farPlane) / (farPlane - nearPlane);

	D3DXVECTOR3 nearFarPlane = D3DXVECTOR3(nearPlane, farPlane, 1.0f / (farPlane - nearPlane));

	int smIndices[2] = {0,0};

	UINT lightsNumber = 0;
	list< pair< Light*, list< RenderChunk* > > >::iterator it_end = l_usedLightsWithShadows.end();
	for (list< pair< Light*, list< RenderChunk* > > >::iterator it = l_usedLightsWithShadows.begin(); (it != it_end) && (lightsNumber < maxLightsUsed) ; it++)
	{
		lightsNumber++;

		BYTE lightType = (*it)->getLightType();
		bool generateShadows = (*it)->getGenerateShadow();
		smIndices[0] = -1;
		smIndices[1] = -1;

		switch (lightType)
		{
			case LIGHT_DIRECTIONAL:
			{
				if ( (*it)->getGenerateShadow() )
				{
					if ( renderVSMLightDirectional( &(*it), &nearFarPlane, 0 ) )
						return -511;

					if ( shadowMapsState.useBlur )
						if ( renderVSMBlur(0, 1) )
							return -601;

					smIndices[0] = 0;
				}

				if ( renderLightTransportDirectional( &(*it), smIndices[0], &nearFarPlane) )
					return -711;

				break;
			};

			case LIGHT_OMNI:
			{
				if ( (*it)->getGenerateShadow() )
				{
					if ( renderVSMLightOmni( &(*it), &nearFarPlane, 0, 1 ) )
						return -521;

					if ( shadowMapsState.useBlur )
						if ( renderVSMBlur(0, 2) || renderVSMBlur(1, 2) )
							return -601;

					smIndices[0] = 0;
					smIndices[1] = 1;
				}

				if ( renderLightTransportOmni( &(*it), smIndices[0], smIndices[1], &nearFarPlane) )
					return -721;

				break;
			};

			case LIGHT_SPOT:
			{
				if ( (*it)->getGenerateShadow() )
				{
					if ( renderVSMLightSpot( &(*it), &nearFarPlane, 0 ) )
						return -531;

					if ( shadowMapsState.useBlur )
						if ( renderVSMBlur(0, 1) )
							return -601;

					smIndices[0] = 0;
				}

				if ( renderLightTransportSpot( &(*it), smIndices[0], &nearFarPlane ) )
					return -731;

				break;
			};
		}
	}


	return 0;

};



int Renderer::renderParticles()
{
	if (!l_particles.size())
		return 0;


	HRESULT hr = S_OK;

	if (FAILED(hr = pDevice->SetRenderTarget(0, p_backBuffer)))
	{
		showError(false, "IDirect3DDevice9::SetRenderTarget error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -101;
	}

	if (FAILED(hr = pDevice->SetDepthStencilSurface(0, p_depthBuffer)))
	{
		showError(false, "IDirect3DDevice9::SetDepthStencilSurface error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -102;
	}


	// add sort function
//	l_particles.sort();


	ID3DXEffect* effect = particleSystemEffectState.pPSEffect->getResourcePointer();

	if (FAILED(hr = pDevice->SetStreamSource(0, ParticleSystem_v4::getParticleVertexBuffer(), 0, sizeof(ParticleSystem_v4::ParticleVertex))))
	{
		showError(false, "IDirect3DDevice9::SetStreamSource error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -103;
	}

	if (FAILED(hr = pDevice->SetStreamSourceFreq(1, D3DSTREAMSOURCE_INSTANCEDATA | 1)))
	{
		showError(false, "IDirect3DDevice9::SetStreamSourceFreq error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -104;
	}

	if (FAILED(hr = pDevice->SetVertexDeclaration(ParticleSystem_v4::getParticleVertexDeclaration())))
	{
		showError(false, "IDirect3DDevice9::SetVertexDeclaration error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -105;
	}


	if (FAILED(hr = effect->SetTechnique(particleSystemEffectState.h_technique)))
	{
		showError(false, "ID3DXEffect::SetTechnique error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -201;
	}

	if (FAILED(hr = effect->SetValue(particleSystemEffectState.h_ambient, &ambientLight, sizeof(D3DXVECTOR4))))
	{
		showError(false, "ID3DXEffect::SetValue error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -202;
	}

	if (FAILED(hr = effect->SetMatrix(particleSystemEffectState.h_matrixViewProjection, usedCamera->getViewProjectionMatrix())))
	{
		showError(false, "ID3DXEffect::SetMatrix error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -203;
	}

	if (FAILED(hr = effect->SetMatrix(particleSystemEffectState.h_matrixViewInversion, _camera->getViewInverseMatrix())))
	{
		showError(false, "ID3DXEffect::SetMatrix error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -204;
	}


	UINT particlesToRender = 0;
	DWORD billboardingType = BILLBOARDING_NONE;
	DWORD billboardingTypeTmp = 0;

	list<ParticleSystem_v4*>::iterator it_end = l_particles.end();

	UINT uiPasses = 0;

	if (FAILED(hr = effect->Begin(&uiPasses, 0)))
	{
		showError(false, "ID3DXEffect::Begin error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -301;
	}


	for (UINT i = 0; i < uiPasses; ++i)
	{
		if (FAILED(hr = effect->BeginPass(i)))
		{
			showError(false, "ID3DXEffect::BeginPass error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -302;
		}

		for (list<ParticleSystem_v4*> it = l_particles.begin(); it != it_end; it++)
		{

			if (res = (*it)->updateBeforeDrawing(pDevice, usedCamera))
			{
				showError(false, "ParticleSystem_v4::updateBeforeDrawing error: %d\n", res);
				return -401;
			}


			if (!(particlesToRender = (*it)->getNumberParticlesToRender()))
				continue;


			if (FAILED(hr = pDevice->SetStreamSource(1, (*it)->getInstanceBuffer(), 0, sizeof(ParticleSystem_v4::ParticleInstance))))
			{
				showError(false, "IDirect3DDevice9::SetStreamSource error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
				return -106;
			}

			if (FAILED(hr = pDevice->SetStreamSourceFreq(0, D3DSTREAMSOURCE_INDEXEDDATA | particlesToRender)))
			{
				showError(false, "IDirect3DDevice9::SetStreamSourceFreq error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
				return -107;
			}


			if ( (billboardingTypeTmp =(*it)->getBillboardingType()) != billboardingType)
			{
				if (billboardingTypeTmp == BILLBOARDING_VIEWPLANE)
				{
					if (FAILED(hr = effect->SetMatrix(particleSystemEffectState.h_matrixViewInversion, _camera->getBillboardingSphericalMatrix())))
					{
						showError(false, "ID3DXEffect::SetMatrix error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
						return -205;
					}
				}
				else if (billboardingType == BILLBOARDING_VIEWPLANE)
				{
					if (FAILED(hr = effect->SetMatrix(particleSystemEffectState.h_matrixViewInversion, _camera->getViewInverseMatrix())))
					{
						showError(false, "ID3DXEffect::SetMatrix error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
						return -206;
					}
				}

				billboardingType = billboardingTypeTmp;
			}


			if (FAILED(hr = effect->SetInt(particleSystemEffectState.h_billboardind, (*it)->getBillboardingType())))
			{
				showError(false, "ID3DXEffect::SetInt error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
				return -207;
			}

			if (FAILED(hr = effect->SetFloat(particleSystemEffectState.h_emissiveFactor, (*it)->getEmissiveFactor())))
			{
				showError(false, "ID3DXEffect::SetFloat error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
				return -208;
			}

			if (FAILED(hr = effect->SetMatrix(particleSystemEffectState.h_matrixWorld, (*it)->getWorldPosition()->getWorldPosition())))
			{
				showError(false, "ID3DXEffect::SetMatrix error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
				return -209;
			}

			if (FAILED(hr = effect->SetValue(particleSystemEffectState.h_sprites, &((*it)->getSpritesVector()), sizeof(D3DXVECTOR2))))
			{
				showError(false, "ID3DXEffect::SetValue error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
				return -210;
			}

			if (FAILED(hr = effect->SetTexture(particleSystemEffectState.h_texture, (*it)->getTexture()->getResourcePointer())))
			{
				showError(false, "ID3DXEffect::SetTexture error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
				return -211;
			}


			if (FAILED(hr = effect->CommitChanges()))
			{
				showError(false, "ID3DXEffect::CommitChanges error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
				return -212;
			}


			if (FAILED(hr = _pDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2)))
			{
				showError(false, "IDirect3DDevice9::DrawPrimitive error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
				return -108;
			}
		}

		if (FAILED(hr = effect->EndPass()))
		{
			showError(false, "ID3DXEffect::EndPass error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -303;
		}
	}

	if (FAILED(hr = effect->End()))
	{
		showError(false, "ID3DXEffect::End error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -304;
	}



	if (FAILED(hr = pDevice->SetStreamSource(0, NULL, 0, 0)))
	{
		showError(false, "IDirect3DDevice9::SetStreamSource error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -108;
	}

	if (FAILED(hr = pDevice->SetStreamSourceFreq(0, 0)))
	{
		showError(false, "IDirect3DDevice9::SetStreamSourceFreq error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -109;
	}


	if (FAILED(hr = pDevice->SetStreamSource(1, NULL, 0, 0)))
	{
		showError(false, "IDirect3DDevice9::SetStreamSource error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -110;
	}

	if (FAILED(hr = pDevice->SetStreamSourceFreq(1, 0)))
	{
		showError(false, "IDirect3DDevice9::SetStreamSourceFreq error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -111;
	}


	if (FAILED(hr = pDevice->SetVertexDeclaration(NULL)))
	{
		showError(false, "IDirect3DDevice9::SetVertexDeclaration error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -112;
	}


	if (FAILED(hr = pDevice->SetRenderTarget(0, NULL)))
	{
		showError(false, "IDirect3DDevice9::SetRenderTarget error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -113;
	}

	if (FAILED(hr = pDevice->SetDepthStencilSurface(0, NULL)))
	{
		showError(false, "IDirect3DDevice9::SetRenderTarget error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -114;
	}



	return 0;
};


int Renderer::passLightTransportToBackBuffer()
{
	HRESULT hr = S_OK;

	if (FAILED(hr = pDevice->SetRenderTarget(0, p_backBuffer)))
	{
		showError(false, "IDirect3DDevice9::SetRenderTarget error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -101;
	}

	if (FAILED(hr = pDevice->Clear(0, NULL, D3DCLEAR_TARGET, defaultBackgroundColor, 1.0f, 0)))
	{
		showError(false, "IDirect3DDevice9::Clear error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -102;
	}


	if (FAILED(hr = pDevice->SetFVF(0)))
	{
		showError(false, "IDirect3DDevice9::SetFVF error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -103;
	}

	if (FAILED(hr = pDevice->SetVertexDeclaration(p_fullscreenQuadVertexDecl)))
	{
		showError(false, "IDirect3DDevice9::SetVertexDeclaration error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -104;
	}

	if (FAILED(hr = pDevice->SetStreamSource(0, p_fullscreenQuad, 0, sizeof(FullscreenQuadVertex))))
	{
		showError(false, "IDirect3DDevice9::SetStreamSource error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -105;
	}


	ID3DXEffect* effect = MRTState.postMRTeffect->getResourcePointer();

	if (FAILED(hr = effect->SetTechnique(MRTState.h_techniquePassThroughLTtoBB)))
	{
		showError(false, "ID3DXEffect::SetTechnique error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -201;
	}

	if (FAILED(hr = effect->SetTexture(MRTState.h_lightTransportTexture, lightTransportState.lightTransport.renderTexture)))
	{
		showError(false, "ID3DXEffect::SetTexture error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -202;
	}

	UINT uiPasses = 0;
	if (FAILED(hr = effect->Begin(&uiPasses, 0)))
	{
		showError(false, "ID3DXEffect::Begin error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -301;
	}

	for (UINT i = 0; i < uiPasses; ++i)
	{
		if (FAILED(hr = effect->BeginPass(i)))
		{
			showError(false, "ID3DXEffect::BeginPass error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -302;
		}

		if (FAILED(hr = pDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2)))
		{
			showError(false, "IDirect3DDevice9::DrawPrimitive error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -303;
		}

		if (FAILED(hr = effect->EndPass()))
		{
			showError(false, "ID3DXEffect::EndPass error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -304;
		}
	}

	if (FAILED(hr = effect->End()))
	{
		showError(false, "ID3DXEffect::End error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -305;
	}


	if (FAILED(hr = pDevice->SetRenderTarget(0, NULL)))
	{
		showError(false, "IDirect3DDevice9::SetRenderTarget error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -101;
	}

	if (FAILED(hr = pDevice->SetFVF(0)))
	{
		showError(false, "IDirect3DDevice9::SetFVF error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -103;
	}

	if (FAILED(hr = pDevice->SetVertexDeclaration(NULL)))
	{
		showError(false, "IDirect3DDevice9::SetVertexDeclaration error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -104;
	}

	if (FAILED(hr = pDevice->SetStreamSource(0, NULL, 0, 0)))
	{
		showError(false, "IDirect3DDevice9::SetStreamSource error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -105;
	}


	return 0;
};


int Renderer::renderBackToFront()
{
	if (!l_backToFrontRenderedChunks.size())
		return 0;


	l_backToFrontRenderedChunks.sort(render_chunk_compare);

	D3DXVECTOR4 ambient = D3DXVECTOR4(ambientLight.r, ambientLight.g, ambientLight.b, ambientLight.a);

	D3DXVECTOR4* lightArray = NULL;
	UINT lightArrayLength = 0;

	UINT lightsNumber = l_usedLightsWithShadows.size();
	vector <D3DXVECTOR4*> lightsVector;
	vector <UINT> lightsVectorLength;
	lightsVector.resize(MAX_LIGHT_ARRRAY);
	UINT lightsNumber = 0;

	lightsVectorLength.resize(lightsNumber);
	UINT lightVectorPointer = 0;

	list<Light*>::iterator it_end = l_usedLightsWithShadows.end();
	for (list<Light*>::iterator it = l_usedLightsWithShadows.begin(); it != it_end; it++)
	{
		if (lightVectorPointer + lightArrayLength > MAX_LIGHT_ARRRAY)
			break;

		lightArray = (*it)->getLightArray(&lightArrayLength);
		memcpy(&(lightsVector[lightVectorPointer]), lightArray, lightArrayLength * sizeof(D3DXVECTOR4));
		lightVectorPointer += lightArrayLength;
		lightsVectorLength[ lightsNumber ++ ] = lightVectorPointer;
	}



	if (FAILED(hr = pDevice->SetRenderTarget(0, p_backBuffer);
	{
		showError(false, "IDirect3DDevice9::SetRenderTarget error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -101;
	}

	if (FAILED(hr = pDevice->SetDepthStencilSurface(p_depthBuffer);
	{
		showError(false, "IDirect3DDevice9::SetDepthStencilSurface error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -102;
	}


	list<RenderChunk*>::iterator it = l_backToFrontRenderedChunks.begin();
	list<RenderChunk*>::iterator it2;
	list<RenderChunk*>::iterator it_end = l_backToFrontRenderedChunks.end();

	while (it != it_end)
	{
		Effect* baseEffect = (*it)->getEffect();
		ID3DXEffect* effect = baseEffect->getResourcePointer();
		GraphicObject* instance = NULL;

		UINT lightVecCount = baseEffect->getLightsVectorSize();
		UINT lightsUsed = 0;
		for (lightsUsed; (lightsUsed < lightsNumber) && (lightsVectorLength[lightsUsed] <= lightVecCount) ; ++lightsUsed);
		lightsUsed --;


		if (baseEffect->setLights(&ambient, &(lightsVector[0]), lightsVectorLength[lightsUsed], lightsUsed);
		{
			showError(false, "Effect::setLights error\n");
			return -103;
		}


		UINT uiPasses = 0;

		if (FAILED(hr = effect->Begin(&uiPasses, 0);
		{
			showError(false, "ID3DXEffect::Begin error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -104;
		}

		for (UINT i = 0; i < uiPasses; ++i)
		{
			if (FAILED(hr = effect->BeginPass(i);
			{
				showError(false, "ID3DXEffect::BeginPass error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
				return -105;
			}

			for (it2 = it; it2 != it_end && (*it2)->getEffect() != baseEffect; it2 ++)
			{
				if ((*it2)->getInstance() != instance)
				{
					if (FAILED(hr = (*it2)->applyInstanceVariables();
					{
						showError(false, "RenderChunk::applyInstanceVariables error\n");
						return -106;
					}

					instance = (*it2)->getInstance();
				}

				if ((*it2)->applySkinningInfo();
				{
					showError(false, "RenderChunk::applySkinningInfo error\n");
					return -107;
				}

				if ((*it2)->draw();
				{
					showError(false, "RenderChunk::draw error\n");
					return -108;
				}
			}

			if (FAILED(hr = effect->EndPass();
			{
				showError(false, "ID3DXEffect::EndPass error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
				return -109;
			}

		}

		if (FAILED(hr = effect->End();
		{
			showError(false, "ID3DXEffect::End error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -110;
		}

		it = it2;
	}

	
	if (FAILED(hr = pDevice->SetRenderTarget(0, NULL);
	{
		showError(false, "IDirect3DDevice9::SetRenderTarget error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -111;
	}

	if (FAILED(hr = pDevice->SetDepthStencilSurface(NULL);
	{
		showError(false, "IDirect3DDevice9::SetDepthStencilSurface error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -112;
	}


	return 0;

};




int Renderer::addChunkToRender(RenderChunk* _chunk)
{
	if (_chunk->renderBTF())
		l_backToFrontRenderedChunks.push_back(_chunk);
	else
		l_renderChunks.push_back(_chunk);

	return 0;
};


int Renderer::addChunksToRender(GraphicObject* _graphicObject)
{
	UINT size = _graphicObject->getRenderChunksNumber();

	for (UINT i = 0; i < size; ++i)
	{
		RenderChunk* chunk = _graphicObject->getRenderChunk(i);
		if (chunk->renderBTF())
			l_backToFrontRenderedChunks.push_back(chunk);
		else
			l_renderChunks.push_back(chunk);
	}

	return 0;
};


int Renderer::addChunkToRenderBTF(RenderChunk* _chunk)
{
	l_backToFrontRenderedChunks.push_back(_chunk);

	return 0;
};


int Renderer::addChunksToRenderBTF(GraphicObject* _graphicObject)
{
	UINT size = _graphicObject->getRenderChunksNumber();

	for (UINT i = 0; i < size; ++i)
	{
		transparentChunks.push_back(_graphicObject->getRenderChunk(i));
	}

	return 0;
};


int Renderer::addChunkToShadowRender(RenderChunk* _chunk)
{
	if ( !l_usedLightsWithShadows.size() )
		return 1;

	pair< Light*, list< RenderChunk* > > p = l_usedLightsWithShadows.back();

	p.second.push_back(_chunk);

	return 0;
};


int Renderer::addChunksToShadowRender(GraphicObject* _graphicObject)
{
	if ( !l_usedLightsWithShadows.size() )
		return 1;

	pair< Light*, list< RenderChunk* > > p = l_usedLightsWithShadows.back();

	UINT size = _graphicObject->getRenderChunksNumber();

	for (UINT i = 0; i < size; ++i)
	{
		p.second.push_back(_graphicObject->getRenderChunk(i));
	}

	return 0;
};


int Renderer::addChunkToShadowRenderNoCheck(RenderChunk* _chunk)
{
	l_usedLightsWithShadows.back().second.push_back(_chunk);
	return 0;
};


int Renderer::addChunksToShadowRenderNoCheck(GraphicObject* _graphicObject)
{
	pair< Light*, list< RenderChunk* > > p = l_usedLightsWithShadows.back();

	UINT size = _graphicObject->getRenderChunksNumber();

	for (UINT i = 0; i < size; ++i)
	{
		p.second.push_back(_graphicObject->getRenderChunk(i));
	}

	return 0;
};


int Renderer::addLight(Light* _light)
{
	pair< Light*, list< RenderChunk* > > p;
	p.first = _light;
	l_usedLightsWithShadows.push_back( p );
	return 0;
};


int Renderer::addParticles(ParticleSystem_v4* _particles)
{
	l_particles.push_back(_particles);
	return 0;
};


int Renderer::setGUISystem(GUISystem* _guiSystem)
{
	 guiSystem = _ guiSystem;
	 return 0;
};


int Renderer::setCamera(Camera* _camera)
{
	usedCamera = _camera;
	return 0;
};


int Renderer::setAmbient(DWORD _color)
{
	ambientLight = D3DXCOLOR(_color);
	return 0;
};


int Renderer::setAmbient(D3DXCOLOR _color)
{
	ambientLight = _color;
	return 0;
};


int Renderer::add2DElement(GUIElement* _2dElements)
{
	l_2dElements.push_back(_2dElements);
	return 0;
};

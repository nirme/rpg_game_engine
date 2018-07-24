#pragma once

#ifndef _CORE_RENDERER
#define _CORE_RENDERER

#include "utility.h"
#include "Core_Logger.h"
#include "Core_Variable_Library.h"
#include "Core_Defines.h"

#include "Core_Graphic_Resource.h"
#include "Core_Graphic_Object.h"
#include "Particle_System_v4.h"
#include "Core_Light.h"
#include "Core_Camera.h"
#include "GUI_System.h"








//getDeviceWeakPtr()







#define		EFFECT_PS					"particle_system_we"
#define		EFFECT_PS_TECHNIQUE			"ParticleSystemWE"

#define		EFFECT_PS_AMBIENT			"lightAmbient"

//WORLD;
//VIEWPROJECTION;
//VIEWINV;

#define		EFFECT_PS_EMISSIVE_FACTOR		"emissiveFactor"
#define		EFFECT_PS_TEXTURE				"p_ptTexture"
#define		EFFECT_PS_SPRITES				"p_sprites"
#define		EFFECT_PS_BILLBOARDING			"p_billboardind"






















bool light_with_shadows_compare ( const pair< Light*, list< RenderChunk* > > &_first, const pair< Light*, list< RenderChunk* > > &_second)
{
	return *(_first.first) < *(_second.first);
};




class Renderer
{
public:

	struct RenderTarget
	{
		IDirect3DTexture9* renderTexture;
		IDirect3DSurface9* renderTarget;

		RenderTarget() : renderTexture(NULL), renderTarget(NULL) {};
		int release()
		{
			int res = 0;
			res += IDX_SAFE_RELEASE(renderTarget);
			res += IDX_SAFE_RELEASE(renderTexture);
			return res;
		};
		~RenderTarget()
		{
			release();
		};
	};


	struct FullscreenQuadVertex
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;

		FullscreenQuadVertex(D3DXVECTOR3 const & _position, D3DXVECTOR2 const & _texture) : position(_position), Texture (_texture) {};
		FullscreenQuadVertex(float const & _positionX, 
					float const & _positionY, 
					float const & _positionZ, 
					float const & _textureU, 
					float const & _textureV)
		{
			position.x = _positionX;
			position.y = _positionY;
			position.z = _positionZ;
			texture.x = _textureU;
			texture.y = _textureV;
		};
	};
////

private:

	bool initialized;

	IDirect3DDevice9* pDevice;
	D3DDISPLAYMODE displayMode;

	IDirect3DSurface9* p_backBuffer;
	IDirect3DSurface9* p_depthBuffer;

// triangle fan
	IDirect3DVertexBuffer9* p_fullscreenQuad;
	IDirect3DVertexDeclaration9* p_fullscreenQuadVertexDecl;

	D3DCOLOR defaultBackgroundColor;

	UINT maxLightsUsed;


	//deferred rendering
	bool supportsHWShadowMaps;			// D3DUSAGE_DEPTHSTENCIL  :  D3DFMT_D32F_LOCKABLE
	bool supports64bitRT;				// D3DUSAGE_RENDERTARGET & D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING  :  D3DFMT_A16B16G16R16F && D3DFMT_G32R32F


	struct ShadowMapsState
	{
		bool useShadowMapping;

		Effect* pSMEffect;
		UINT shadowMapResolution;

		bool useVarianceShadowMapping; //FSM otherwise

		bool use64bitShadowMaps;

		bool useBlur;

		// blur horizontal then vertical for cheaper results
		vector<D3DXVECTOR4> sampleOffsetsHorizontal;
		vector<D3DXVECTOR4> sampleOffsetsVertical;
		vector<float> sampleWeights;


		// for Variance Shadow Mapping
		// D3DFMT_G16R16F || D3DFMT_G32R32F    // add D3DFMT_A16B16G16R16F for rendering 2 lights in one pass
		// for FSM
		// D3DFMT_D32F_LOCKABLE / D3DFMT_R32F    // add D3DFMT_A16B16G16R16F for rendering up to 4 lights in one pass

		// we use 2 maps for rendering dual paraboloid + at least 1 map for blur application
		vector<RenderTarget> v_shadowMaps;

		//FSM indexes
		int shadowMapIndex;
		int shadowMapDummyIndex;

		//dual-paraboloid indexes
		int dualParaboloidFront;
		int dualParaboloidBack;
		int dualParaboloidFreeMap;

		//single map index
		int mapUsed;


		D3DXMATRIX lightsProjectionMx[LIGHT_TYPES];
		D3DXMATRIX lightShadowMapMx;


		D3DXHANDLE h_technique[LIGHT_TYPES]; //technique per light type
		D3DXHANDLE h_techniqueSkinned[LIGHT_TYPES]; //technique per light type

		D3DXHANDLE h_omniDirection;
		D3DXHANDLE h_vec3NFPlane; //vec3(Near plane, Far plane, 1 / (Far plane - Near plane))

		D3DXHANDLE h_worldLightViewProjectionMx;

		D3DXHANDLE h_bonesInfluenceNumber;
		D3DXHANDLE h_bonesSkinningPalette;


		D3DXHANDLE h_blurTechnique;

		D3DXHANDLE h_blurSampleOffsets;
		D3DXHANDLE h_blurSampleWeights;
		D3DXHANDLE h_blurSampleNumber;
		D3DXHANDLE h_blurInputTexture;

		IDirect3DVertexBuffer9* p_blurFullscreenQuad;


		ShadowMapsState() :	useShadowMapping(false), 
							pSMEffect(NULL), 
							shadowMapResolution(0), 
							useVarianceShadowMapping(false), 
							use64bitShadowMaps(false), 
							useBlur(false), 
							shadowMapIndex(0), 
							shadowMapDummyIndex(0), 
							dualParaboloidFront(0), 
							dualParaboloidBack(0), 
							dualParaboloidFreeMap(0), 
							mapUsed(0), 
							h_omniDirection(NULL), 
							h_vec3NFPlane(NULL), 
							h_worldLightViewProjectionMx(NULL), 
							h_bonesInfluenceNumber(NULL), 
							h_bonesSkinningPalette(NULL), 
							h_blurTechnique(NULL), 
							h_blurSampleOffsets(NULL), 
							h_blurSampleWeights(NULL), 
							h_blurSampleNumber(NULL), 
							h_blurInputTexture(NULL), 
							p_blurFullscreenQuad(NULL)
		{
			h_technique = {0};
			h_techniqueSkinned = {0};

			ZeroMemory(lightsProjectionMx, LIGHT_TYPES * sizeof(D3DXMATRIX));
		};
	};

	ShadowMapsState shadowMapsState;


	struct MultipleRenderTargetsState
	{
		bool use64bitRT;
		vector<RenderTarget> v_renderTargets;

		//    G-Buffer standard precision (32bit) setup:
		// +-------------------------------------------+------------+------------+------------+----------------------+
		// |  RenderTarget # (D3DFORMAT) [value]       |    8bit    |    8bit    |    8bit    |      8bit            |
		// +-------------------------------------------+------------+------------+------------+----------------------+
		// |  RenderTarget 0 (D3DFMT_A8R8G8B8) [byte]  |  color.R   |  color.G   |  color.B   |  specular intensity  |
		// +-------------------------------------------+------------+------------+------------+----------------------+
		// |  RenderTarget 1 (D3DFMT_A8R8G8B8) [byte]  |  normal.X  |  normal.Y  |  normal.Z  |  specular power      |
		// +-------------------------------------------+------------+------------+------------+----------------------+
		// |  RenderTarget 2 (D3DFMT_G16R16F) [half]   |      depth (16bit)      |      emissive factor (16bit)      |
		// +-------------------------------------------+-------------------------------------------------------------+
		//
		//    G-Buffer higher precision (64bit) setup:
		// +------------------------------------------------+------------+------------+------------+----------------------+
		// |  RenderTarget # (D3DFORMAT) [value]            |   16bit    |   16bit    |   16bit    |     16bit            |
		// +------------------------------------------------+------------+------------+------------+----------------------+
		// |  RenderTarget 0 (D3DFMT_A16B16G16R16F) [half]  |  color.R   |  color.G   |  color.B   |  specular intensity  |
		// +------------------------------------------------+------------+------------+------------+----------------------+
		// |  RenderTarget 1 (D3DFMT_A16B16G16R16F) [half]  |  normal.X  |  normal.Y  |  normal.Z  |  specular power      |
		// +------------------------------------------------+------------+------------+------------+----------------------+
		// |  RenderTarget 2 (D3DFMT_G32R32F) [float]       |      depth (32bit)      |      emissive factor (32bit)      |
		// +------------------------------------------------+-------------------------+-----------------------------------+

		Effect* clearMRTeffect;

		Effect* postMRTeffect;
		D3DXHANDLE h_techniquePassThroughLTtoBB;
		D3DXHANDLE h_lightTransportTexture;

		MultipleRenderTargetsState() :	use64bitRT(false), 
										clearMRTeffect(NULL), 
										postMRTeffect(NULL), 
										h_techniquePassThroughLTtoBB(NULL), 
										h_lightTransportTexture(NULL)
		{};
	};

	MultipleRenderTargetsState MRTState;


	// used for blurring aliased pixels
	// RenderTarget lightTransportFinal;
	// RenderTarget blurSurfaceHorizontal;	
	// RenderTarget blurSurfaceVertical;
	//
	//     Blur targets setup:
	// +-------------------+--------------------+----------+---------------+
	// |      Width        |      Height        |  Levels  |   D3DFORMAT   |
	// +-------------------+--------------------+----------+---------------+
	// |  screenWidth / 2  |  screenHeight / 2  |    1     |  LightFormat  |
	// +-------------------+--------------------+----------+---------------+

	struct LightTransportState
	{
		bool use64bitLightTransport;
		RenderTarget lightTransport;

		// use64bitLightTransport ? D3DFMT_A16B16G16R16F : D3DFMT_A8R8G8B8;
		// 
		//    Light Transport standard precision simple specular (D3DFMT_A8R8G8B8) (32bit) setup:
		// +---------------------------------------------+-----------+-----------+-----------+------------+
		// |  LightTransport # (D3DFORMAT) [value]       |   8bit    |   8bit    |   8bit    |   8bit     |
		// +---------------------------------------------+-----------+-----------+-----------+------------+
		// |  LightTransport 0 (D3DFMT_A8R8G8B8) [byte]  |  diff.R   |  diff.G   |  diff.B   |     x      |
		// +---------------------------------------------+-----------+-----------+-----------+------------+
		//
		//    Light Transport higher precision simple specular (D3DFMT_A16B16G16R16F) (64bit) setup:
		// +--------------------------------------------------+-----------+-----------+-----------+------------+
		// |  LightTransport # (D3DFORMAT) [value]            |   16bit   |   16bit   |   16bit   |   16bit    |
		// +--------------------------------------------------+-----------+-----------+-----------+------------+
		// |  LightTransport 0 (D3DFMT_A16B16G16R16F) [half]  |  diff.R   |  diff.G   |  diff.B   |     x      |
		// +--------------------------------------------------+-----------+-----------+-----------+------------+

		Effect* pLTEffect;

		D3DXHANDLE h_techniqueAmbient;

		D3DXHANDLE h_technique[LIGHT_TYPES];
		D3DXHANDLE h_techniqueNoSM[LIGHT_TYPES];

		D3DXHANDLE h_cameraPosition;
		D3DXHANDLE h_cameraInvViewProjectionMx;
		D3DXHANDLE h_cameraInvViewMx;
		D3DXHANDLE h_cameraInvProjectionMx;

		D3DXHANDLE h_lightAmbient;

		D3DXHANDLE h_lightTable;
		// vector4[6 max]:
		//     vec4 position
		//     vec4 color_diffuse
		//     vec4 color_specular
		//     vec4 {range, attenuation0, attenuation1, attenuation2}

		//   spotlight part
		//     vec4 {is_spot, cos_hotspot, cos_cutoff, falloff}
		//     vec4 spot_direction


		D3DXHANDLE h_lightViewProjectionMx;
		D3DXHANDLE h_lightShadowMapMx;
		D3DXHANDLE h_lightNearFarPlane;
		// h_lightNearFarPlane.x = near plane
		// h_lightNearFarPlane.y = far plane
		// h_lightNearFarPlane.z = (far plane) - (near plane)

		D3DXHANDLE h_colorMap;
		D3DXHANDLE h_normalMap;
		D3DXHANDLE h_depthMap;


		bool useShadowMaps;
		bool useVSM;

		D3DXHANDLE h_shadowMap;
		D3DXHANDLE h_shadowFrontMap;
		D3DXHANDLE h_shadowBackMap;


		LightTransportState() :	use64bitLightTransport(false), 
								pLTEffect(NULL), 
								h_techniqueAmbient(NULL), 
								h_cameraPosition(NULL), 
								h_cameraInvViewProjectionMx(NULL), 
								h_cameraInvViewMx(NULL), 
								h_cameraInvProjectionMx(NULL), 
								h_lightAmbient(NULL), 
								h_lightTable(NULL), 
								h_lightViewProjectionMx(NULL), 
								h_lightShadowMapMx(NULL), 
								h_lightNearFarPlane(NULL), 
								h_colorMap(NULL), 
								h_normalMap(NULL), 
								h_depthMap(NULL), 
								useShadowMaps(false), 
								useVSM(false), 
								h_shadowMap(NULL), 
								h_shadowFrontMap(NULL), 
								h_shadowBackMap(NULL)
		{
			h_technique = {NULL};
			h_techniqueNoSM = {NULL};
		};
	};

	LightTransportState lightTransportState;


	struct ParticleSystemEffectState
	{
		Effect* pPSEffect;

		D3DXHANDLE h_technique;

		D3DXHANDLE h_ambient;
		D3DXHANDLE h_emissiveFactor;
		D3DXHANDLE h_texture;
		D3DXHANDLE h_sprites;
		D3DXHANDLE h_billboardind;
		D3DXHANDLE h_matrixWorld;
		D3DXHANDLE h_matrixViewProjection;
		D3DXHANDLE h_matrixViewInversion;


		ParticleSystemEffectState() :	pPSEffect(NULL), 
										h_technique(NULL), 
										h_ambient(NULL), 
										h_emissiveFactor(NULL), 
										h_texture(NULL), 
										h_sprites(NULL), 
										h_billboardind(NULL), 
										h_matrixWorld(NULL), 
										h_matrixViewProjection(NULL), 
										h_matrixViewInversion(NULL)
		{};
	};

	ParticleSystemEffectState particleSystemEffectState;


	ID3DXSprite* pGUISprite;
	GUISystem* guiSystem;


	Camera* usedCamera;


//	list<RenderChunk*> l_shadowRenderChunks;
	list<RenderChunk*> l_renderChunks;

	// include any 3d rendered UI elements, transparent chunks and models without normals or tangents/normal/binormal(bitangent)
	list<RenderChunk*> l_backToFrontRenderedChunks;

	D3DXCOLOR ambientLight;
	list< pair< Light*, list< RenderChunk* > > > l_usedLightsWithShadows;


	// rendering particles use hw instancing and is renderred differently from normal chunks
	// it can't cast any shadows and in most cases use partially transparrent textures
	list<ParticleSystem_v4*> l_particles;


	list<GUIElement*> l_2dElements;


protected:

	int setupFullscreenQuad();
	int releaseFullscreenQuad();


public:

	Renderer();
	~Renderer();

	int initialize(	IDirect3DDevice9* _pDevice, 
					D3DCAPS9* _deviceCaps, 
					D3DDISPLAYMODE* _displayMode, 
					bool _use64bitRenderTargets, 
					bool _use64bitLightTransport, 
					bool _useShadowMapping, 
					//bool _useVarianceShadowMapping = true, 
					bool _use64bitShadowMaps = false, 
					UINT _shadowMapResolution = 0, 
					//bool _useBlur = false, 
					UINT _blurSamples = 0);

	int shutdown();

	int render();


protected:

	int checkDeviceCapabilities(IDirect3DDevice9* _pDevice, D3DCAPS9* _deviceCaps, D3DDISPLAYMODE* _displayMode);

	int setupRenderTargets(bool _use64bitRenderTargets);
	int releaseRenderTargets();

	int setupShadowMaps(bool _useVarianceShadowMapping, bool _use64bitShadowMaps = false, UINT _shadowMapResolution = 0, bool _useBlur = false, UINT _blurSamples = 0);
	int releaseShadowMaps();

	int setupLightTransport(bool _use64bitLightTransport, bool _useShadowMaps, bool _useVSM);
	int releaseLightTransport();

	int setupParticleSystemEffect();
	int releaseParticleSystemEffect();


	int renderGUI();
	int render2DElements();


	int renderToMRT();


	int renderVSMLightDirectional( pair< Light*, list< RenderChunk* > >* _lightWithShadow, const D3DXVECTOR3* _nearFarPlane, int _SMIndexUsed );
	int renderVSMLightSpot( pair< Light*, list< RenderChunk* > >* _lightWithShadow, const D3DXVECTOR3* _nearFarPlane, int _SMIndexUsed );
	int renderVSMLightOmni( pair< Light*, list< RenderChunk* > >* _lightWithShadow, const D3DXVECTOR3* _nearFarPlane, int _SMIndexUsedFront, int _SMIndexUsedBack );

	int renderVSMBlur( int _bluredSMIndex, int _tmpSMIndex );

	int renderLightTransportAmbient();
	int renderLightTransportDirectional( pair< Light*, list< RenderChunk* > >* _lightWithShadow, int _smIndex = -1, const D3DXVECTOR3* _nearFarPlane = NULL );
	int renderLightTransportSpot( pair< Light*, list< RenderChunk* > >* _lightWithShadow, int _smIndex = -1, const D3DXVECTOR3* _nearFarPlane = NULL );
	int renderLightTransportOmni( pair< Light*, list< RenderChunk* > >* _lightWithShadow, int _smFrontIndex = -1, int _smBackIndex = -1, const D3DXVECTOR3* _nearFarPlane = NULL );

	int renderLightTransport();


	int renderParticles();

	int passLightTransportToBackBuffer();

	int renderBackToFront();



public:

	int addChunkToRender(RenderChunk* _chunk);
	int addChunksToRender(GraphicObject* _graphicObject);

	int addChunkToRenderBTF(RenderChunk* _chunk);
	int addChunksToRenderBTF(GraphicObject* _graphicObject);

	int addChunkToShadowRender(RenderChunk* _chunk);
	int addChunksToShadowRender(GraphicObject* _graphicObject);

	int addChunkToShadowRenderNoCheck(RenderChunk* _chunk);
	int addChunksToShadowRenderNoCheck(GraphicObject* _graphicObject);

	int addLight(Light* _light);

	int addParticles(ParticleSystem_v4* _particles);

	int add2DElement(GUIElement* _2dElements);

	int setGUISystem(GUISystem* _guiSystem);

	int setCamera(Camera* _camera);

	int setAmbient(DWORD _color);
	int setAmbient(D3DXCOLOR _color);

};



#endif //_CORE_RENDERER
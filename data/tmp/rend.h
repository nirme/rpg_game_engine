#pragma once

#ifndef _CORE_RENDERER_V2
#define _CORE_RENDERER_V2






class Renderer_v2
{
public:

	struct RenderTarget
	{
		IDirect3DTexture9* renderTexture;
		IDirect3DSurface9* renderTarget;


		int release()
		{
			int res = 0;
			res += IDX_SAFE_RELEASE(renderTarget);
			res += IDX_SAFE_RELEASE(renderTexture);
			return res;
		};

		RenderTarget() : renderTexture(NULL), renderTarget(NULL) {};
		inline ~RenderTarget()	{	release();	};
	};

/*
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
*/


	bool initialized;

	IDirect3DDevice9* pDevice;
	D3DDISPLAYMODE displayMode;


	// renderer resources

	



};
////




#endif //_CORE_RENDERER_V2

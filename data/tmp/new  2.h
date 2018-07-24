
class Material
{
private:
	ProjectingEffect* pEffect;

public:

	Material( ProjectingEffect* _pEffect, bool _renderBackToFront = false ) : pEffect(_pEffect), renderBackToFront(_renderBackToFront)
	{};

	virtual ~Material()
	{};

	inline bool isMRT()	{	return pEffect->outputToMRT();	};

	void setMatrices( const D3DXMATRIX* _pWorld, const D3DXMATRIX* _pView, const D3DXMATRIX* _pProjection, const D3DXMATRIX* _pViewProjection, const D3DXMATRIX* _pViewInverse, UINT _uiFrameTimeStamp )
	{
		pEffect->setMatrices( _pWorld );
		pEffect->setMatrices( _pView, _pProjection, _pViewProjection, _pViewInverse, _mFrameTimeStamp );
	};

	void setWorld( const D3DXMATRIX* _pWorld )
	{
		pEffect->setMatrices( _pWorld );
	};

	void setViewProjection( const D3DXMATRIX* _pView, const D3DXMATRIX* _pProjection, const D3DXMATRIX* _pViewProjection, const D3DXMATRIX* _pViewInverse, UINT _uiFrameTimeStamp )
	{
		pEffect->setMatrices( _pView, _pProjection, _pViewProjection, _pViewInverse, _mFrameTimeStamp );
	};

	virtual void apply()
	{};
};


class MaterialTextured : public virtual Material
{
private:
	BYTE texturesNumber;
	Texture** ppTextures;
	IDirect3DTexture9** ppCashedTextureResource;

	D3DXVECTOR3 pMrtMaterial;

public:

	MaterialTextured( ProjectingTexturingEffect* _pEffect, BYTE _texturesNumber, Texture** _ppTextures, float _specularIntensity, float _specularPower, float _emissiveFactor ) : Material( _pEffect ), texturesNumber(_texturesNumber), pTextures(_ppTextures)
	{
		pCashedTextureResource = new IDirect3DTexture9* [ texturesNumber ];
		for ( BYTE i = 0; i < texturesNumber; ++i )
		{
			ppTextures[i]->addRef();
			ppCashedTextureResource[i] = ppTextures[i]->getResource();
		}

		pMrtMaterial.x = _specularIntensity;
		pMrtMaterial.y = _specularPower;
		pMrtMaterial.z = _emissiveFactor;
	};
	
	~MaterialTextured()
	{
		for ( BYTE i = 0; i < texturesNumber; ++i )
		{
			SAFE_RELEASE( ppTextures[i] );
		}

		SAFE_DELETE_ARRAY( ppCashedTextureResource );
		SAFE_DELETE_ARRAY( ppTextures );
	};

	virtual void apply()
	{
		ProjectingTexturingEffect* pTmpEffect = reinterpret_cast<ProjectingEffect*> (_pEffect);
		for ( BYTE i = 0; i < texturesNumber; ++i )
			pTmpEffect->setTexture( i, ppCashedTextureResource[i] );

		pTmpEffect->setMaterial( &pMrtMaterial );
	};
};


class MaterialSkinned : public virtual Material
{
public:

	MaterialSkinned( ProjectingSkinningEffect* _pEffect ) : Material( _pEffect )
	{};

	inline void applySkinningPalette( const D3DXMATRIX* const * _ppPalette, USHORT _paletteSize, BYTE _boneInfluenceNumber  )
	{
		SkinningEffect* pTmpEffect = reinterpret_cast<SkinningEffect*> (_pEffect);
		pTmpEffect->setSkinningPalette( _ppPalette, _paletteSize, _boneInfluenceNumber );
	};

	inline void applySkinningPalette( const D3DXMATRIX* _pPalette, USHORT _paletteSize, BYTE _boneInfluenceNumber  )
	{
		SkinningEffect* pTmpEffect = reinterpret_cast<SkinningEffect*> (_pEffect);
		pTmpEffect->setSkinningPalette( _pPalette, _paletteSize, _boneInfluenceNumber );
	};
};


class MaterialTexturedSkinned : public MaterialTextured, public MaterialSkinned
{
public:
	MaterialTexturedSkinned( ProjectingTexturingSkinningEffect* _pEffect, BYTE _texturesNumber, Texture** _ppTextures, float _specularIntensity, float _specularPower, float _emissiveFactor ) : Material( _pEffect ), MaterialTextured( _pEffect, _texturesNumber, _ppTextures, _specularIntensity, _specularPower, _emissiveFactor ), MaterialSkinned( _pEffect )
	{};
};




class MaterialLighted : public virtual Material
{
public:
	MaterialLighted( LightingEffect* _pEffect ) : Material( _pEffect, true )
	{};

	void applyLights( const D3DXVECTOR4* _pLightAmbient, const D3DXVECTOR4* _pLightsVector = NULL, BYTE _lightsVectorLength = 0, BYTE _lightsNumber = 0 )
	{
		LightingEffect* pTmpEffect = reinterpret_cast<LightingEffect*> (_pEffect);
		pTmpEffect->setLights( _pLightAmbient, _pLightsVector, _lightsVectorLength, _lightsNumber );
	};

	void applyLights( const D3DXVECTOR4* _pLightAmbient, const Light* const * _ppLights, BYTE _lightsNumber )
	{
		LightingEffect* pTmpEffect = reinterpret_cast<LightingEffect*> (_pEffect);
		pTmpEffect->setLights( _pLightAmbient, _ppLights, _lightsNumber );
	};
};




class MaterialTexturedLighted : public MaterialTextured, public MaterialLighted
{
public:
	MaterialTexturedLighted( ProjectingTexturingLightingEffect* _pEffect, BYTE _texturesNumber, Texture** _ppTextures, float _specularIntensity, float _specularPower, float _emissiveFactor ) : Material( _pEffect, true ), MaterialTextured( _pEffect, _texturesNumber, _ppTextures, _specularIntensity, _specularPower, _emissiveFactor ), MaterialLighted( _pEffect )
	{};
};


class MaterialSkinnedLighted : public MaterialSkinned, public MaterialLighted
{
public:
	MaterialSkinnedLighted( ProjectingSkinningLightingEffect* _pEffect ) : Material( _pEffect, true ), MaterialSkinned( _pEffect ), MaterialLighted( _pEffect )
	{};
};

class MaterialTexturedSkinnedLighted : public MaterialTexturedSkinned, public MaterialLighted
{
public:
	MaterialTexturedSkinnedLighted( ProjectingTexturingSkinningLightingEffect* _pEffect, BYTE _texturesNumber, Texture** _ppTextures, float _specularIntensity, float _specularPower, float _emissiveFactor ) : Material( _pEffect, true ), MaterialTexturedSkinned( _pEffect, _texturesNumber, _ppTextures, _specularIntensity, _specularPower, _emissiveFactor ), MaterialLighted( _pEffect )
	{};
};


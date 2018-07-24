#include "Core_Graphic_Object.h"




/////////////////////////////////////////////
////////           Material          ////////
/////////////////////////////////////////////

Material::~Material()
{
	loaded = false;

	SAFE_RELEASE(pEffect);
	for (InstanceTextures::iterator it = m_instanceTextures.begin();it != m_instanceTextures.end(); it++)
		SAFE_RELEASE((*it).second.second);

	m_effectInstanceParameters.clear();
	m_instanceTextures.clear();
	pD3DXEffect = NULL;
	SAFE_RELEASE(pEffect);
	renderBackToFront = true;

	delete this;
};

void Material::setEffect(Effect* _pEffect)
{
	pEffect = _pEffect;
};

Effect* Material::getEffect()
{
	return pEffect;
};

int Material::setEffectParameter(string _parameterName, vector<BYTE>* _parameterValue)
{
	return setEffectParameter(_parameterName, &((*_parameterValue)[0]), _parameterValue->size());
};

int Material::setEffectParameter(string _parameterName, const void* _parameterValue, UINT _parameterValueSize)
{
	if (pD3DXEffect && !pD3DXEffect->GetParameterByName(NULL, _parameterName.c_str()))
		return 1;

	EffectInstanceParameters::iterator it = m_effectInstanceParameters.find(_parameterName);
	if (it != m_effectInstanceParameters.end())
	{
		(*it).second.second.resize(_parameterValueSize);
		memcpy(&((*it).second.second[0]), _parameterValue, _parameterValueSize);
	}
	else
	{
		it = m_effectInstanceParameters.insert(EffectInstanceParameters::value_type(_parameterName, pair<D3DXHANDLE, InstanceValue>::pair(NULL, InstanceValue((BYTE*)_parameterValue, ((BYTE*)_parameterValue) + _parameterValueSize)))).first;
	}

	if (!(*it).second.first && pD3DXEffect)
		(*it).second.first = pD3DXEffect->GetParameterByName(NULL, (*it).first.c_str());

	return 0;
};

void* Material::getEffectParameter(string _parameterName, UINT* _parameterValueSize)
{
	EffectInstanceParameters::iterator it = m_effectInstanceParameters.find(_parameterName);

	if (it != m_effectInstanceParameters.end())
	{
		if (_parameterValueSize)
			*_parameterValueSize = (*it).second.second.size();
		return &((*it).second.second[0]);

	}

	return NULL;
};



int Material::setTexture(string _textureParameterName, Texture* _pTexture)
{
	if (pD3DXEffect && !pD3DXEffect->GetParameterByName(NULL, _textureParameterName.c_str()))
		return 1;

	InstanceTextures::iterator it = m_instanceTextures.find(_textureParameterName);
	if (it != m_instanceTextures.end())
	{
		if ((*it).second.second->getId() != _pTexture->getId())
		{
			(*it).second.second->release();
			(*it).second.second = _pTexture;
		}
	}
	else
	{
		it = m_instanceTextures.insert(InstanceTextures::value_type(_textureParameterName, pair<D3DXHANDLE, Texture*>::pair(NULL, _pTexture))).first;
	}

	D3DXHANDLE w = (*it).second.first;

	if (!(*it).second.first && pD3DXEffect)
	{
		D3DXHANDLE h = pD3DXEffect->GetParameterByName(NULL, (*it).first.c_str());
		(*it).second.first = pD3DXEffect->GetParameterByName(NULL, (*it).first.c_str());
	}

	if (!_pTexture->isLoaded())
		loaded = false;

	return 0;
};

UINT Material::getTextureNumber()
{
	return m_instanceTextures.size();
};

Texture* Material::getTexture(string _textureParameterName)
{
	InstanceTextures::iterator it = m_instanceTextures.find(_textureParameterName);

	if (it != m_instanceTextures.end())
		return (*it).second.second;
	return NULL;
};

Texture* Material::getTexture(UINT _index)
{
	if (_index >= m_instanceTextures.size())
		return NULL;

	InstanceTextures::iterator it = m_instanceTextures.begin();

	for (UINT i=0;i<_index;++i,it++);

	return(*it).second.second;
};


void Material::setRenderBTF(bool _renderBackToFront)
{
	renderBackToFront = _renderBackToFront;
};


bool Material::getRenderBTF()
{
	return renderBackToFront;
};


bool Material::isLoaded()
{
	if (loaded)
		return true;

	if (!pEffect->isLoaded())
		return false;

	pD3DXEffect = pEffect->getResourcePointer();

	{
		InstanceTextures::iterator it_end = m_instanceTextures.end();
		for (InstanceTextures::iterator it = m_instanceTextures.begin(); it != it_end; it++)
		{
			if (!(*it).second.second->isLoaded())
				return false;
		}

		if (!pEffect->isLoaded())
			return false;
	}

	pD3DXEffect = pEffect->getResourcePointer();

	{
		EffectInstanceParameters::iterator it = m_effectInstanceParameters.begin();
		EffectInstanceParameters::iterator it_end = m_effectInstanceParameters.end();
		while (it != it_end)
		{
			(*it).second.first = pD3DXEffect->GetParameterByName(NULL, (*it).first.c_str());
			if (!(*it).second.first)
			{
				EffectInstanceParameters::iterator tmp = it;
				tmp++;
				m_effectInstanceParameters.erase(it);
				it = tmp;
			}
			else
				it++;
		}
	}

	{
		InstanceTextures::iterator it = m_instanceTextures.begin();
		InstanceTextures::iterator it_end = m_instanceTextures.end();
		while (it != it_end)
		{
			(*it).second.first = pD3DXEffect->GetParameterByName(NULL, (*it).first.c_str());
			if (!(*it).second.first)
			{
				InstanceTextures::iterator tmp = it;
				tmp++;
				m_instanceTextures.erase(it);
				it = tmp;
			}
			else
				it++;
		}
	}

	loaded = true;
	return true;
};


void Material::setNotLoaded()
{
	loaded = false;
	pD3DXEffect = NULL;
};


int Material::apply()
{
	HRESULT hr;

	{
		EffectInstanceParameters::const_iterator it_end = m_effectInstanceParameters.end();
		for (EffectInstanceParameters::const_iterator it = m_effectInstanceParameters.begin(); it != it_end; it++)
		{
			if (FAILED(hr = pD3DXEffect->SetValue((*it).second.first, &((*it).second.second[0]), (*it).second.second.size())))
			{
				showError(false, "ID3DXEffect::SetValue error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
				return -1;
			}
		}
	}

	{
		InstanceTextures::const_iterator it_end = m_instanceTextures.end();
		for (InstanceTextures::const_iterator it = m_instanceTextures.begin(); it_end; it++)
		{
			if (FAILED(hr = pD3DXEffect->SetTexture((*it).second.first, (*it).second.second->getResourcePointer())))
			{
				showError(false, "ID3DXEffect::SetTexture error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
				return -2;
			}
		}
	}

	return 0;
};

int Material::applySkinningPalette(UINT _paletteArraySize, const D3DXMATRIX** _paletteArray, UINT _boneInfluenceNumber)
{
	if (FAILED(HRESULT hr = pEffect->setSkinninginfo(_paletteArraySize, _paletteArray, _boneInfluenceNumber)))
	{
		showError(false, "Effect::setSkinninginfo error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -1;
	}

	return 0;
};



/////////////////////////////////////////////
////////         RenderChunk         ////////
/////////////////////////////////////////////


int RenderChunk::setupRenderChunk(GraphicObject* _pInstance, DWORD _dwSubset)
{
	pInstance = _pInstance;
	dwSubset = _dwSubset;

	pWorldPosition = pInstance->getWorldPosition();

	if (!pInstance->getMesh()->isAnimated())
	{
		pMesh = pInstance->getMesh()->getResourcePointer();
	}
	else //animated
	{
		pMesh = pInstance->getAnimationInfo()->getBlendedMesh();
	}

	pInstance->getSubsetInfo(dwSubset, &dwMaterialIndex, &dwRenderPass);

	pInstance->getMaterial(dwMaterialIndex, &pMaterial);
	pEffect = pMaterial->getEffect();

	ppSkinningPalette = pInstance->getAnimationInfo()->getSubsetPalettePointers(dwSubset);
	paletteSize = pInstance->getAnimationInfo()->getSubsetPaletteSize(dwSubset);
	maxBonesInfluence = pInstance->getAnimationInfo()->getMaxFaceBonesInfluence();

	renderBTF = pMaterial->getRenderBTF();

	return 0;
};


int RenderChunk::applyAllVariables()
{
	int res = pMaterial->apply();
	res += pEffect->setMatrices(pWorldPosition->getWorldPosition());

	if (paletteSize)
		res += pEffect->setSkinninginfo(paletteSize, ppSkinningPalette, maxBonesInfluence);

	return res;
};


int RenderChunk::applyInstanceVariables()
{
	int res = pEffect->setMatrices(pWorldPosition->getWorldPosition());
	res += pMaterial->apply();

	return res;
};


int RenderChunk::applyMaterial()
{
	return pMaterial->apply();
};

int RenderChunk::applyWorldPosition()
{
	return pEffect->setMatrices(pWorldPosition->getWorldPosition());
};

int RenderChunk::applySkinningInfo()
{
	if (paletteSize)
		return pEffect->setSkinninginfo(paletteSize, ppSkinningPalette, maxBonesInfluence);

	return 0;
};

int RenderChunk::applySkinningInfo(ID3DXEffect* _effect, D3DXHANDLE _hPalette, D3DXHANDLE _hBonesInfluence)
{
	if (paletteSize)
	{
		HRESULT hr = S_OK;

		if (FAILED(hr = _effect->SetMatrixPointerArray(_hPalette, ppSkinningPalette, paletteSize)))
		{
			showError(false, "ID3DXEffect::SetMatrixPointerArray error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -1;
		}

		if (FAILED(hr = _effect->SetInt(_hBonesInfluence, _boneInfluenceNumber)))
		{
			showError(false, "ID3DXEffect::SetInt error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -2;
		}
	}

	return 0;
};


int RenderChunk::draw()
{
	if (FAILED(hr = pMesh->DrawSubset(dwSubset)))
	{
		showError(false, "ID3DXMesh::DrawSubset error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return -1;
	}

	return 0;
};

Material* RenderChunk::getMaterial()
{
	return pMaterial;
};

Effect* RenderChunk::getEffect()
{
	return pEffect;
};

GraphicObject* RenderChunk::getInstance()
{
	return pInstance;
};

WorldPosition* RenderChunk::getWorldPosition()
{
	return pWorldPosition;
};


bool RenderChunk::getRenderBTF()
{
	return renderBTF;
};

int RenderChunk::getRenderPass()
{
	return dwRenderPass;
};



/////////////////////////////////////////////
////////        GraphicObject        ////////
/////////////////////////////////////////////


GraphicObject::~GraphicObject()
{};


int GraphicObject::isLoaded()
{
	if (loaded)
		return 0;

	if (!meshLoaded)
	{
		int res = loadRenderChunks();
		if (!res)
			meshLoaded = true;
		else if (res < 0)
			return -100 + res;
	}

	if (!materialLoaded)
	{
		materialLoaded = true;
		for (UINT i = 0; i < v_pMaterial.size(); ++i)
			materialLoaded = materialLoaded && v_pMaterial[i]->isLoaded();
	}

	
	if (!(loaded = meshLoaded && materialLoaded))
		return 1;

	return 0;
};


int GraphicObject::loadRenderChunks()
{
	if (!pMesh->isLoaded())
		return 1;

	if (!pMesh->isAnimated())
	{
		pAnimationInstance = NULL;

		ID3DXMesh* mesh = pMesh->getResourcePointer();

		DWORD attr = 0;
		HRESULT hr = S_OK;
		if (FAILED(hr = mesh->GetAttributeTable(NULL, &attr)))
		{
			showError(false, "ID3DXMesh::GetAttributeTable error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -2;
		}

		v_renderChunks.resize(attr);

		for (UINT i = 0; i < attr; ++i)
		{
			v_renderChunks[i].setupRenderChunk(this, i, v_subsetsInfo[i].usedMaterial, v_subsetsInfo[i].defaultRenderPass);
		}
	}
	else //animated
	{
		pMesh->getAnimationInfoInstance(&pAnimationInstance);

		ID3DXMesh* mesh = pAnimationInstance->getBlendedMesh();

		UINT animAttr = 0;
		HRESULT hr = S_OK;
		if (FAILED(hr = mesh->GetAttributeTable(NULL, &animAttr)))
		{
			showError(false, "ID3DXMesh::GetAttributeTable error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
			return -2;
		}

		v_renderChunks.resize(animAttr);

		for (UINT i = 0; i < animAttr; ++i)
		{
			int defaultSubsetId = pAnimationInstance->getBaseSubsetId(i);
			if (defaultSubsetId < 0)
				return -3;
			v_renderChunks[i].setupRenderChunk(this, i, v_subsetsInfo[defaultSubsetId].usedMaterial, v_subsetsInfo[defaultSubsetId].defaultRenderPass);
		}
	}

	return 0;
};


int GraphicObject::setup(const string &_name, WorldPosition* _pWorldPosition, Mesh* _pMesh, Material** _arrMaterial, UINT _materialNum, int _dwRenderPass, RENDER_CHUNK_DECL* _arrChunkDecl, UINT _chunksNum)
{
	loaded = false;
	meshLoaded = false;
	materialLoaded = false;

	name = _name;
	pMesh = _pMesh;

	v_pMaterial.resize(_materialNum);

	for (UINT i = 0; i < _materialNum; ++i)
	{
		v_pMaterial[i] = _arrMaterial[i];
	}

	UINT chmax = 0;
	for (UINT i = 0; i < _chunksNum; ++i)
		if (_arrChunkDecl[i].usedSubset > chmax)
			chmax = _arrChunkDecl[i].usedSubset;
	chmax ++;

	chmax = max(chmax, _chunksNum);

	v_subsetsInfo.resize(chmax);

	for (UINT i = 0; i < _chunksNum; ++i)
	{
		v_subsetsInfo[_arrChunkDecl[i].usedSubset].usedMaterial = _arrChunkDecl[i].usedMaterial;
		v_subsetsInfo[_arrChunkDecl[i].usedSubset].defaultRenderPass = _arrChunkDecl[i].defaultRenderPass;
	}

	pWorldPosition = _pWorldPosition;

	return 0;
};


int GraphicObject::release()
{
	SAFE_RELEASE(pAnimationInstance);
	SAFE_RELEASE(pMesh);

	for (UINT i = 0; i < v_pMaterial.size(); ++i)
		delete v_pMaterial[i];

	delete this;
};


string GraphicObject::getName()
{
	return name;
};


UINT GraphicObject::getMaterialsNumber()
{
	return v_pMaterial.size();
};


int GraphicObject::getMaterial(UINT _index, Material** _ppMaterial)
{
	if (_index >= v_pMaterial.size())
		return -1;

	*_ppMaterial = v_pMaterial[_index];
	return 0;
};


int GraphicObject::getSubsetInfo(UINT _subset, UINT* _materialIndex, int* _defaultRenderPass)
{
	if (!pAnimationInstance)
	{
		if (_subset >= v_subsetsInfo.size())
			return -1;

		*_materialIndex = v_subsetsInfo[_subset].usedMaterial;
		*_defaultRenderPass = v_subsetsInfo[_subset].defaultRenderPass;
	}
	else
	{
		if (_subset >= pAnimationInstance->getAnimatedSubsetsNumber())
			return -1;

		_subset = pAnimationInstance->getBaseSubsetId(_subset);
		*_materialIndex = v_subsetsInfo[_subset].usedMaterial;
		*_defaultRenderPass = v_subsetsInfo[_subset].defaultRenderPass;
	}

	return 0;
};


Mesh* GraphicObject::getMesh()
{
	return pMesh;
};


AnimationInfo* GraphicObject::getAnimationInfo()
{
	return pAnimationInstance;
};


int GraphicObject::setWorldPosition(WorldPosition* _pWorldPosition)
{
	if (!_pWorldPosition)
		return -1;

	pWorldPosition = _pWorldPosition;
	return 0;
};


WorldPosition* GraphicObject::getWorldPosition()
{
	return pWorldPosition;
};


UINT GraphicObject::getRenderChunksNumber()
{
	return v_renderChunks.size();
};


int GraphicObject::getRenderChunk( DWORD _index, RenderChunk** _ppRenderChunk )
{
	if (_index >= v_renderChunks.size())
		return -1;

	*_ppRenderChunk = &(v_renderChunks[_index]);
	return 0;
};


int GraphicObject::getRenderChunkArray( const RenderChunk** _ppRenderChunkArray, UINT* _renderChunkArraySize )
{
	*_ppRenderChunk = &(v_renderChunks[0]);

	*_renderChunkArraySize = v_renderChunks.size();
	return 0;
};


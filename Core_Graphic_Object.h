#pragma once

#ifndef _CORE_GRAPHIC_OBJECT
#define _CORE_GRAPHIC_OBJECT

#include "utility.h"
#include "Core_Graphic_Resource.h"
#include "Core_World_Position_v3.h"
#include "Core_Graphic.h"
#include "AnimationController.h"


class Material;
class GraphicObject;
class RenderChunk;



class Material
{
	friend class RenderChunk;

public:
	typedef vector<BYTE> InstanceValue;
	typedef map<string, pair<D3DXHANDLE, InstanceValue>> EffectInstanceParameters;
	typedef map<string, pair<D3DXHANDLE, Texture*>> InstanceTextures;

protected:
	Effect* pEffect;
	EffectInstanceParameters m_effectInstanceParameters;
	InstanceTextures m_instanceTextures;

	// in case of transparency and other variations
	bool renderBackToFront;

	bool loaded;

	//cashed pointer to effect
	ID3DXEffect* pD3DXEffect;

public:
	Material() : pEffect(NULL), renderBackToFront(true), loaded(false), pD3DXEffect(NULL) {};
	~Material();

	void setEffect(Effect* _pEffect);
	Effect* getEffect();
	int setEffectParameter(string _parameterName, vector<BYTE>* _parameterValue);
	int setEffectParameter(string _parameterName, const void* _parameterValue, UINT _parameterValueSize);
	void* getEffectParameter(string _parameterName, UINT* _parameterValueSize);

	int setTexture(string _textureParameterName, Texture* _pTexture);

	UINT getTextureNumber();
	Texture* getTexture(string _textureParameterName);
	Texture* getTexture(UINT _index);

	void setRenderBTF(bool _renderBackToFront);
	bool getRenderBTF();

	bool isLoaded();
	void setNotLoaded();

	int apply();

	int applySkinningPalette(UINT _paletteArraySize, const D3DXMATRIX** _paletteArray, UINT _boneInfluenceNumber);

};




class RenderChunk
{
	friend bool render_chunk_compare (RenderChunk* first, RenderChunk* second);
	friend bool render_chunk_shadowmapping_compare (RenderChunk* first, RenderChunk* second);

protected:
	// values that uniquely define renderable item
	GraphicObject* pInstance;

	const WorldPosition* pWorldPosition;

	const ID3DXMesh* pMesh;
	DWORD dwSubset;

	UINT dwMaterialIndex;
	int dwRenderPass;
	const Material* pMaterial;
	const Effect* pEffect;

	const D3DXMATRIX** ppSkinningPalette;
	UINT paletteSize;
	UINT maxBonesInfluence;


	bool renderBTF;


public:

	RenderChunk() :	pInstance(NULL), 
					pWorldPosition(NULL), 
					pMesh(NULL), 
					dwSubset(0), 
					dwMaterialIndex(0), 
					dwRenderPass(0), 
					pMaterial(NULL), 
					pEffect(NULL), 
					ppSkinningPalette(NULL), 
					paletteSize(0), 
					maxBonesInfluence(0), 
					renderBTF(false)
	{};

	int setupRenderChunk(GraphicObject* _pInstance, DWORD _dwSubset);

	int applyAllVariables();
	int applyInstanceVariables();
	int applyMaterial();
	int applyWorldPosition();
	int applySkinningInfo();
	int applySkinningInfo(ID3DXEffect* _effect, D3DXHANDLE _hPalette, D3DXHANDLE _hBonesInfluence);

	int draw();

	Material* getMaterial();
	Effect* getEffect();
	GraphicObject* getInstance();
	WorldPosition* getWorldPosition();


	bool getRenderBTF();
	int getRenderPass();

};


bool render_chunk_compare (RenderChunk* first, RenderChunk* second)
{
	return UINT(first->pEffect) < UINT(second->pEffect);
};

bool render_chunk_shadowmapping_compare (RenderChunk* first, RenderChunk* second)
{
	if (bool(first->paletteSize) == bool(second->paletteSize))
		return UINT(first->pInstance) < UINT(second->pInstance);

	return (bool(first->paletteSize) < bool(second->paletteSize));
};



struct RENDER_CHUNK_DECL
{
	DWORD usedSubset;
	DWORD usedMaterial;
	int defaultRenderPass;
};


class GraphicObject
{
protected:

	struct RenderSubsetInfo
	{
		UINT usedMaterial;
		int defaultRenderPass;

		RenderSubsetInfo() : usedMaterial(0), defaultRenderPass(0) {};
	};


protected:

	string name;

	Mesh* pMesh;
	AnimationInfo* pAnimationInstance;

	vector <Material*> v_pMaterial;
	vector <RenderSubsetInfo> v_subsetsInfo;

	vector<RenderChunk> v_renderChunks;

	WorldPosition* pWorldPosition;


	bool loaded;
	bool meshLoaded;
	bool materialLoaded;

public:

	GraphicObject() :	pMesh(NULL), 
						pAnimationInstance(NULL), 
						pWorldPosition(NULL), 
						loaded(false), 
						meshLoaded(false), 
						materialLoaded(false)
	{};

	~GraphicObject();

	int isLoaded();
	int loadRenderChunks();

	int setup(const string &_name, WorldPosition* _pWorldPosition, Mesh* _pMesh, Material** _arrMaterial, UINT _materialNum, int _dwRenderPass, RENDER_CHUNK_DECL* _arrChunkDecl, UINT _chunksNum);
	int release();

	string getName();
	UINT getMaterialsNumber();
	int getMaterial(UINT _index, Material** _ppMaterial);
	int getSubsetInfo(UINT _subset, UINT* _materialIndex, int* _defaultRenderPass);
	Mesh* getMesh();
	AnimationInfo* getAnimationInfo();

	int setWorldPosition(WorldPosition* _pWorldPosition);
	WorldPosition* getWorldPosition();

	UINT getRenderChunksNumber();
	int getRenderChunk(DWORD _index, RenderChunk** _ppRenderChunk);
	int getRenderChunkArray(RenderChunk** _ppRenderChunkArray, UINT* _renderChunkArraySize);

};




#endif //_CORE_GRAPHIC_OBJECT

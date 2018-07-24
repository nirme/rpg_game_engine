
#define		RENDER_CHUNK_TYPE_BTF			0x80000000
#define		RENDER_CHUNK_TYPE_LIGHTED		0x40000000
#define		RENDER_CHUNK_TYPE_SKINNED		0x20000000


//MaterialLighted

class RenderChunk
{
private:
	GraphicObject* pGraphicObject;
	DWORD subsetId;

	Model* pModel;
	ID3DXMesh* pMesh;

	USHORT materialIndex;
	Material* pMaterial;

	WorldPosition* pWorldPosition;


	DWORD typeStamp;

	BYTE renderPass;


public:

	RenderChunk()
	{};

	virtual ~RenderChunk()
	{};


	void draw()
	{};

	virtual void applyAllVariables()
	{};

};


class RenderChunkSkinned : public RenderChunk
{
private:

public:

	RenderChunkSkinned() : RenderChunk()
	{};
};

#pragma once

#include "engineMath.h"
#include "Graphics.h"

class AssetManager;
class Material;
class VertexBuffer;

class Mesh
{
public:
	Mesh(AssetManager* pAssetManager, const VertexBuffer* pVertexBuffer, const Shader* pShader);
	~Mesh();

	void Draw() const;

	bool Load(const WCHAR* fileName, AssetManager* pAssetManager);
	static Mesh* StaticLoad(const WCHAR* fileName, AssetManager* pAssetManager);

protected:
	Material* mMaterial;
	const VertexBuffer* mVertexBuffer;
	bool mIsSkinned;
};
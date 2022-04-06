#pragma once
#include "assetCache.h"
#include "Shader.h"
#include "texture.h"
#include "Material.h"
#include "mesh.h"
#include "skeleton.h"
#include "Animation.h"
#include "bonetransform.h"

class AssetManager
{
public:
    AssetManager();
    ~AssetManager();

    Shader* GetShader(const std::wstring& shaderName);
    void SetShader(const std::wstring& shaderName, Shader* pShader);
    Texture* LoadTexture(const std::wstring& fileName);
    //Material* LoadMaterial(const std::wstring& materialName);
    Mesh* LoadMesh(const std::wstring& fileName);
    Material* LoadMaterial(const std::wstring& fileName);
    Skeleton* LoadSkeleton(const std::wstring& fileName);
    Animation* LoadAnimation(const std::wstring& fileName);
    void Clear();

private:
    AssetCache<Shader>* ShaderAssetCache;
    AssetCache<Texture>* TextureAssetCache;
    AssetCache<Material>* MaterialAssetCache;
    AssetCache<Mesh>* MeshAssetCache;
    AssetCache<Skeleton>* SkeletonAssetCache;
    AssetCache<Animation>* AnimationAssetCache;
};

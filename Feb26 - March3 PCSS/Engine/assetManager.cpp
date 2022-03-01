#include "stdafx.h"
#include "assetManager.h"

AssetManager::AssetManager()
{
	ShaderAssetCache = new AssetCache<Shader>(this);
	MaterialAssetCache = new AssetCache<Material>(this);
	TextureAssetCache = new AssetCache<Texture>(this);
	MeshAssetCache = new AssetCache<Mesh>(this);
	SkeletonAssetCache = new AssetCache<Skeleton>(this);
	AnimationAssetCache = new AssetCache<Animation>(this);
}

AssetManager::~AssetManager()
{

}

void AssetManager::Clear()
{
	ShaderAssetCache->Clear();
	delete ShaderAssetCache;
	TextureAssetCache->Clear();
	delete TextureAssetCache;
	MaterialAssetCache->Clear();
	delete MaterialAssetCache;
	MeshAssetCache->Clear();
	delete MeshAssetCache;
	SkeletonAssetCache->Clear();
	delete SkeletonAssetCache;
	AnimationAssetCache->Clear();
	delete AnimationAssetCache;
}

Shader* AssetManager::GetShader(const std::wstring& shaderName)
{
	return ShaderAssetCache->Get(shaderName);
}

void AssetManager::SetShader(const std::wstring& shaderName, Shader* pShader)
{
	ShaderAssetCache->Cache(shaderName, pShader);
}

Texture* AssetManager::LoadTexture(const std::wstring& fileName)
{
	return TextureAssetCache->Load(fileName);
}

/*
Material* AssetManager::LoadMaterial(const std::wstring& materialName)
{
	return MaterialAssetCache->Load(materialName);
}*/

Mesh* AssetManager::LoadMesh(const std::wstring& fileName)
{
	return MeshAssetCache->Load(fileName);
}

Material* AssetManager::LoadMaterial(const std::wstring& fileName)
{
	return MaterialAssetCache->Load(fileName);
}

Skeleton* AssetManager::LoadSkeleton(const std::wstring& fileName)
{
	return SkeletonAssetCache->Load(fileName);
}


Animation* AssetManager::LoadAnimation(const std::wstring& fileName) {
	return AnimationAssetCache->Load(fileName);
}
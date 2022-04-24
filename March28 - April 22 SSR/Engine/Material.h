#pragma once	
#include "engineMath.h"

#include <vector>

class AssetManager;
class Texture;
class Shader;
class Graphics;
class GeometryShader;

class Material {

	struct matchMaterial {
		Vector3 c_diffuseColor;
		float padding;
		Vector3 c_specularColor;
		float c_specularPower;
	};

public:
	Material();
	~Material();
	void SetActive();
	void SetActive(Shader* name);
	void SetShader(const Shader* shader);
	void SetTexture(int slot, const Texture* texture);
	void SetDiffuseColor(const Vector3& color);
	void SetSpecularColor(const Vector3& color);
	void SetSpecularPower(float power);
	static Material* StaticLoad(const WCHAR* fileName, AssetManager* pManager);
	bool Load(const WCHAR* fileName, AssetManager* pAssetManager);

	matchMaterial match;
private:
	ID3D11Buffer* constBuffer;
	const Shader* mShader;
	std::vector<const Texture*> mTextureList;
	Graphics* mGraphic;
	//const Texture* mTextureList[Graphics::TEXTURE_SLOT_TOTAL];
	//Shader* shaderForGbufferPass;
};


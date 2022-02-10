#include "stdafx.h"
#include "Material.h"
#include "texture.h"
#include "Shader.h"
#include "Graphics.h"
#include <fstream>
#include <sstream>
#include "stringUtil.h"
#include "rapidjson\include\rapidjson\rapidjson.h"
#include "rapidjson\include\rapidjson\document.h"
#include <jsonUtil.h>
#include "assetManager.h"
#include <codecvt>
#include "GeometryShader.h"


Material::Material()
{
	mGraphic = Graphics::Get();
	//initialize mTextureList with nullptr
	for (int i = 0; i < mGraphic->TEXTURE_SLOT_TOTAL; i++) {
		mTextureList.push_back(nullptr);
	}
	//initialize const buffer
	constBuffer = mGraphic->CreateGraphicsBuffer(&match, sizeof(matchMaterial),
		D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC
	);
}

Material::~Material()
{
	constBuffer->Release();
}

void Material::SetActive()
{
	//set the shader active
	mShader->SetActive();
	//set each texture active on its slot
	for (int i = 0; i < mTextureList.size(); i++) {
		if (mTextureList[i] != nullptr) {
			mTextureList[i]->SetActive(i);
		}
	}
	//bind buffer to correct correct constant buffer on the pixel shader
	mGraphic->UploadBuffer(constBuffer, &match, sizeof(matchMaterial));
	mGraphic->GetDeviceContext()->PSSetConstantBuffers(mGraphic->CONSTANT_BUFFER_MATERIAL, 1, &constBuffer);
}

void Material::SetShader(const Shader* shader)
{
	mShader = shader;
}

void Material::SetTexture(int slot, const Texture* texture)
{
	mTextureList[slot] = texture;
}

void Material::SetDiffuseColor(const Vector3& color)
{
	match.c_diffuseColor = color;
}

void Material::SetSpecularColor(const Vector3& color)
{
	match.c_specularColor = color;
}

void Material::SetSpecularPower(float power)
{                                         
	match.c_specularPower = power;
}

Material* Material::StaticLoad(const WCHAR* fileName, AssetManager* pManager)
{
	Material* material = new Material();
	if (material->Load(fileName, pManager) == false) {
		delete material;
		return new Material();
	}
	return material;
}

bool Material::Load(const WCHAR* fileName, AssetManager* pAssetManager)
{
	std::ifstream file(fileName);
	if (!file.is_open())
	{
		return false;
	}

	std::stringstream fileStream;
	fileStream << file.rdbuf();
	std::string contents = fileStream.str();
	rapidjson::StringStream jsonStr(contents.c_str());
	rapidjson::Document doc;
	doc.ParseStream(jsonStr);

	if (!doc.IsObject())
	{
		DbgAssert(false, "Unable to open Meterial file");
		return false;
	}

	std::string str = doc["metadata"]["type"].GetString();
	int ver = doc["metadata"]["version"].GetInt();

	// Check the metadata
	if (!doc["metadata"].IsObject() || str != "itpmat" || ver != 1)
	{
		DbgAssert(false, "Material File Incorrect Version");
		return false;
	}

	//create shader
	std::wstring shaderName;
	GetWStringFromJSON(doc, "shader", shaderName);
	Shader* shader = pAssetManager->GetShader(shaderName);
	SetShader(shader);

	//get color
	Vector3 diffuseColor;
	Vector3 specularColor;
	GetVectorFromJSON(doc, "diffuseColor", diffuseColor);
	GetVectorFromJSON(doc, "specularColor", specularColor);
	SetDiffuseColor(diffuseColor);
	SetSpecularColor(specularColor);

	//get power
	float specularPower;
	GetFloatFromJSON(doc, "specularPower", specularPower);
	SetSpecularPower(specularPower);

	//setup texture
	const rapidjson::Value& texJson = doc["textures"];
	if (!texJson.IsArray() || texJson.Size() < 1)
	{
		DbgAssert(false, "Material File Invalid Textures");
		return false;
	}
	mTextureList.reserve(texJson.Size() * 3);
	auto temp = texJson.Size();
	for (rapidjson::SizeType i = 0; i < texJson.Size(); i++) {
		const rapidjson::Value& tex = texJson[i];
		if (!tex.IsString()) {
			DbgAssert(false, "Material File Invalid Textures String");
			return false;
		}
		std::wstring textureName = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(tex.GetString());
		Texture* texture = pAssetManager->LoadTexture(textureName);
		SetTexture(i, texture);
	}
	return true;
}

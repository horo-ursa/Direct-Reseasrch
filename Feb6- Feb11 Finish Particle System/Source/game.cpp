#include "stdafx.h"
#include "Game.h"
#include "engineMath.h"
#include "Graphics.h"
#include "Shader.h"
#include "stringUtil.h"
#include "rapidjson\include\rapidjson\rapidjson.h"
#include "rapidjson\include\rapidjson\document.h"
#include <fstream>
#include <sstream>
#include <VertexFormat.h>
#include "mesh.h"
#include "jsonUtil.h"
#include "Components/PointLight.h"
#include "Component.h"
#include "SkinnedObj.h"
#include "Components\character.h"
#include "Components\player.h"
#include "Components\followCam.h"
#include "CollisionBox.h"


float rotationAngel = 0;

Game::Game()
{

}

Game::~Game()
{
}

void Game::Init(HWND hWnd, float width, float height)
{
	mGraphics.InitD3D(hWnd, width, height);

	//initialize the asset manager
	assetManager = new AssetManager();
	//load shaders into cache
	{
		//Mesh.hlsl shader
		D3D11_INPUT_ELEMENT_DESC inputElem[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosColor, pos),
			D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(VertexPosColor, color),
			D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		Shader* meshShader = new Shader();
		meshShader->Load(L"Shaders/Mesh.hlsl", inputElem, ARRAY_SIZE(inputElem));
		assetManager->SetShader(L"Mesh", meshShader);
	
		//BasicMesh.hlsl
		D3D11_INPUT_ELEMENT_DESC inputTextureElem[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexWithNormal, pos),
			D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexWithNormal, normal),
			D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(VertexWithNormal, color),
			D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(VertexWithNormal, uv),
			D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		Shader* BasicMeshShader = new Shader();
		BasicMeshShader->Load(L"Shaders/BasicMesh.hlsl", inputTextureElem, ARRAY_SIZE(inputTextureElem));
		assetManager->SetShader(L"BasicMesh", BasicMeshShader);

		//Phong.hlsl
		D3D11_INPUT_ELEMENT_DESC inputWithoutColor[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexWithoutColor, pos),
			D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexWithoutColor, normal),
			D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(VertexWithoutColor, uv),
			D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		Shader* PhongShader = new Shader();
		PhongShader->Load(L"Shaders/Phong.hlsl", inputWithoutColor, ARRAY_SIZE(inputWithoutColor));
		assetManager->SetShader(L"Phong", PhongShader);

		//Unlit.hlsl
		Shader* UnlitShader = new Shader();
		UnlitShader->Load(L"Shaders/Unlit.hlsl", inputWithoutColor, ARRAY_SIZE(inputWithoutColor));
		assetManager->SetShader(L"Unlit", UnlitShader);

		//skinned.hlsl
		D3D11_INPUT_ELEMENT_DESC SkinningDataDesc[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(SkinningFormat, pos),
			D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(SkinningFormat, normal),
			D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"BLENDINDICES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, offsetof(SkinningFormat,indice),
			D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"BLENDWEIGHT", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, offsetof(SkinningFormat,weight),
			D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(SkinningFormat, uv),
			D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		Shader* SkinShader = new Shader();
		SkinShader->Load(L"Shaders/Skinned.hlsl", SkinningDataDesc, ARRAY_SIZE(SkinningDataDesc));
		assetManager->SetShader(L"Skinned", SkinShader);
	}

	mPhysics = new Physics();
	//load level
	LoadLevel(L"Assets/Levels/Level08.itplevel");

	//set up lighting buffer
	lightingBuffer = mGraphics.CreateGraphicsBuffer(&lightConst, 
		sizeof(Lights::LightingConstants),D3D11_BIND_CONSTANT_BUFFER, 
		D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC
	);

	assetManager->LoadSkeleton(L"Assets/Anims/SK_Mannequin.itpskel");
	assetManager->LoadAnimation(L"Assets/Anims/ThirdPersonRun.itpanim2");

	inputhandler = new InputHandler(this);

	//for a sample particle
	D3D11_INPUT_ELEMENT_DESC GeometryShaderDesc[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(GeometryShaderInput, pos),
			D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	gsShader = new GeometryShader();
	gsShader->Load(L"Shaders/Particle.hlsl", GeometryShaderDesc, ARRAY_SIZE(GeometryShaderDesc));

	mTex = new Texture();
	mTex->Load(L"Assets/Textures/Cloud.png");

	mMaterial = new Material();
	mMaterial->SetShader(gsShader);
	mMaterial->SetTexture(Graphics::TEXTURE_SLOT_GSSHADER, mTex);

	GeometryShaderInput vert[] =
	{
		{ Vector3(100.0f, 100.f, 150.0f)},
		{ Vector3(300.f, -200.f, 80.f)},
		{ Vector3(500.f, 12.5f, 50.0f)},
		{ Vector3(200.f, 54.5f, 20.0f)},
		{ Vector3(160.f, 345.5f, 400.0f)},
		{ Vector3(240.f, 430.5f, 120.0f)},
		{ Vector3(340.f, -236.5f, 340.0f)},
		{ Vector3(54.f, -76.5f, 170.0f)},
		{ Vector3(23.f, -324.5f, 30.0f)},
		{ Vector3(15.f, -54.5f, 250.0f)},
		{ Vector3(672.f, -670.5f, 650.0f)}
	};
	VertexBuffer* vBuffer = new VertexBuffer(vert, sizeof(vert) / sizeof(vert[0]), sizeof(GeometryShaderInput), nullptr, NULL, NULL);
	mMesh = new Mesh(vBuffer, mMaterial);
	particles = new RenderObj(mMesh);
	objectList.push_back(particles);
}

void Game::Shutdown()
{
	delete camera;

	for (auto& object : objectList) {
		delete object;
	}
	lightingBuffer->Release();

	delete inputhandler;
	delete gsShader;
	delete mTex;
	delete mMesh;
	delete mMaterial;
	assetManager->Clear();
	mGraphics.CleanD3D();
}

void Game::Update(float deltaTime)
{
	for (auto& obj : objectList) {
		obj->Update(deltaTime);
	}
	inputhandler->Execute();
}

void Game::RenderFrame()
{
	// Set the render target
	mGraphics.SetRenderTarget(mGraphics.GetBackBuffer(), mGraphics.GetView());

	{	// Clear the screen to blue
		Graphics::Color4 clearColor(0.0f, 0.2f, 0.4f, 1.0f);
		mGraphics.ClearRenderTarget(clearColor);
		mGraphics.ClearDepthBuffer(mGraphics.GetView(), 1.0f);
	}

	//set up camera
	camera->SetActive();

	mGraphics.UploadBuffer(lightingBuffer, &lightConst, sizeof(Lights::LightingConstants));
	mGraphics.GetDeviceContext()->PSSetConstantBuffers(mGraphics.CONSTANT_BUFFER_LIGHTING, 1, &lightingBuffer);

	//draw objects
  	for (auto& object : objectList) {
		object->Draw();
	}

	mGraphics.EndFrame();
}

void Game::OnKeyDown(uint32_t key)
{
	m_keyIsHeld[key] = true;
}

void Game::OnKeyUp(uint32_t key)
{
	m_keyIsHeld[key] = false;
}

void Game::GetMousePos(uint32_t xpos, uint32_t ypos) {
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	camera->ProcessMouseMovement(xoffset, yoffset);
}

bool Game::IsKeyHeld(uint32_t key) const
{
	const auto find = m_keyIsHeld.find(key);
	if (find != m_keyIsHeld.end())
		return find->second;
	return false;
}

bool Game::LoadLevel(const WCHAR* fileName)
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
		return false;
	}

	std::string str = doc["metadata"]["type"].GetString();
	int ver = doc["metadata"]["version"].GetInt();

	// Check the metadata
	if (!doc["metadata"].IsObject() ||
		str != "itplevel" ||
		ver != 2)
	{
		return false;
	}

	//load camera
	const rapidjson::Value& cameraData = doc["camera"];
	if (!cameraData.IsObject()) {
		DbgAssert(false, "Camera Format Wrong");
		return false;
	}
	Vector3 cameraPos;
	GetVectorFromJSON(cameraData, "position", cameraPos);
	Quaternion cameraRot;
	GetQuaternionFromJSON(cameraData, "rotation", cameraRot);
	//Camera* cam = new Camera();
	//cam->SetPositionAndRotation(cameraPos, cameraRot);
	camera = new Camera();
	//camera->SetPositionAndRotation(cameraPos, cameraRot);

	//load lighting data
	const rapidjson::Value& light = doc["lightingData"];
	if (!light.IsObject()) {
		DbgAssert(false, "lighting data error");
		return false;
	}
	Vector3 ambient;
	GetVectorFromJSON(light, "ambient", ambient);
	SetAmbientLight(ambient);

	//load rendering objects
	const rapidjson::Value& renderObj = doc["renderObjects"];
	if (!renderObj.IsArray() || renderObj.Size() < 1) {
		DbgAssert(false, "Renderobjs error");
		return false;
	}
	Vector3 position;
	Quaternion rotation;
	float scale;
	std::wstring MeshAddress;
	for (rapidjson::SizeType i = 0; i < renderObj.Size(); i++) {
		GetVectorFromJSON(renderObj[i], "position", position);
		GetQuaternionFromJSON(renderObj[i], "rotation", rotation);
		GetFloatFromJSON(renderObj[i], "scale", scale);
		GetWStringFromJSON(renderObj[i], "mesh", MeshAddress);
		RenderObj* obj;
		Mesh* mesh = assetManager->LoadMesh(MeshAddress);
		if (mesh->IsSkinned()) {
			obj = new SkinnedObj(mesh);
		}
		else {
			obj = new RenderObj(mesh);
		}
		
		obj->position = position;
		obj->match.c_modelToWorld = Matrix4::CreateScale(scale)
			* Matrix4::CreateFromQuaternion(rotation)
			* Matrix4::CreateTranslation(position);
		objectList.push_back(obj);

		//load component
		const rapidjson::Value& componentData = doc["renderObjects"][i]["components"];
		if (!componentData.IsArray()) {
			DbgAssert(false, "component error");
			return false;
		}
		else if (componentData.Size() < 1) {
			continue; 
		}

		for (rapidjson::SizeType j = 0; j < componentData.Size(); j++) {
			std::wstring type;
			GetWStringFromJSON(componentData[j], "type", type);
			if (type == L"PointLight") {
				Vector3 lightColor;
				float inner;
				float outer;
				GetVectorFromJSON(componentData[j], "lightColor", lightColor);
				GetFloatFromJSON(componentData[j], "innerRadius", inner);
				GetFloatFromJSON(componentData[j], "outerRadius", outer);
				PointLight* light = new PointLight(this, obj);
				light->LoadProperties(lightColor, inner, outer);
				obj->AddComponent(light);
			}
			else if (type == L"Character") {
				Character* character = new Character(obj, assetManager);
				character->LoadProperties(componentData[j]);
				obj->AddComponent(character);
			}
			else if (type == L"Player") {
				Player* player = new Player(obj, this);
				player->LoadProperties(componentData[j]);
				obj->AddComponent(player);
			}
			else if (type == L"FollowCam") {
				FollowCam* cam = new FollowCam(obj, this);
				cam->LoadProperties(componentData[j]);
				obj->AddComponent(cam);
			}
			else if (type == L"CollisionBox") {
				CollisionBox* colliBox = new CollisionBox(obj, assetManager, mPhysics);
				colliBox->LoadProperties(componentData[j]);
				obj->AddComponent(colliBox);
			}
		}
	}

	return true;
}

Lights::PointLightData* Game::AllocateLight()
{
	for (int i = 0; i < 8; i++) {
		if (!lightConst.c_pointLight[i].isEnabled) {
			lightConst.c_pointLight[i].isEnabled = true;
			return &lightConst.c_pointLight[i];
		}
	}
}

void Game::FreeLight(Lights::PointLightData* pLight)
{
	pLight->isEnabled = false;
}

void Game::SetAmbientLight(const Vector3& color)
{
	lightConst.c_ambient = color;
}

const Vector3& Game::GetAmbientLight() const
{
	return lightConst.c_ambient;
}
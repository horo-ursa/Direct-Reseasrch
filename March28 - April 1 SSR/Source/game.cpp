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
#include "OBJ_Loader.h"

//#include "DirectXTK\Inc\DDSTextureLoader.h"
//#include "DirectXTK\Inc\WICTextureLoader.h"
//#include "DirectXTK\Inc\ScreenGrab.h"

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
	device = mGraphics.GetDevice();
	devContext = mGraphics.GetDeviceContext();

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
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(SkinningFormat, pos),
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

		//gpass, lightpass
		D3D11_INPUT_ELEMENT_DESC inputForDefferedRenderring[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexForGBuffer, pos),
		D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexForGBuffer, normal),
		D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(VertexForGBuffer, uv),
		D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		Shader* gpShader = new Shader();
		gpShader->Load(L"Shaders/GPass.hlsl", inputForDefferedRenderring, ARRAY_SIZE(inputForDefferedRenderring));
		assetManager->SetShader(L"GPass", gpShader);

		Shader* lpShader = new Shader();
		lpShader->Load(L"Shaders/lightPass.hlsl", inputForDefferedRenderring, ARRAY_SIZE(inputForDefferedRenderring));
		assetManager->SetShader(L"lightPass", lpShader);

		//GPassPhong
		Shader* GPPShader = new Shader();
		GPPShader->Load(L"Shaders/GPassPhong.hlsl", inputForDefferedRenderring, ARRAY_SIZE(inputForDefferedRenderring));
		assetManager->SetShader(L"GPassPhong", GPPShader);

		//GPassSkin
		Shader* GPassSkin = new Shader();
		GPassSkin->Load(L"Shaders/GPassSkin.hlsl", SkinningDataDesc, ARRAY_SIZE(SkinningDataDesc));
		assetManager->SetShader(L"GPassSkin", GPassSkin);

		//DepthPass shader
		Shader* DPShader = new Shader();
		DPShader->Load(L"Shaders/DepthPass.hlsl", inputForDefferedRenderring, ARRAY_SIZE(inputForDefferedRenderring));
		assetManager->SetShader(L"DepthPass", DPShader);
		//SSR

		Shader* SSRShader = new Shader();
		SSRShader->Load(L"Shaders/SSR.hlsl", inputForDefferedRenderring, ARRAY_SIZE(inputForDefferedRenderring));
		assetManager->SetShader(L"SSR", SSRShader);


		//quad
		D3D11_INPUT_ELEMENT_DESC quad[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(quadVertexBuffer, pos),
			D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(quadVertexBuffer, uv),
			D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		Shader* quadShader = new Shader();
		quadShader->Load(L"Shaders/quadShader.hlsl", quad, ARRAY_SIZE(quad));
		assetManager->SetShader(L"quadShader", quadShader);
	}



	mPhysics = new Physics();
	//load level
	LoadLevel(L"Assets/Levels/Level1.itplevel");

	//set up lighting buffer
	lightingBuffer = mGraphics.CreateGraphicsBuffer(
		&lightConst, 
		sizeof(Lights::LightingConstants),
		D3D11_BIND_CONSTANT_BUFFER, 
		D3D11_CPU_ACCESS_WRITE, 
		D3D11_USAGE_DYNAMIC
	);

	//mask for shaders
	maskBuffer = mGraphics.CreateGraphicsBuffer(
		&mask,
		sizeof(shaderMask),
		D3D11_BIND_CONSTANT_BUFFER,
		D3D11_CPU_ACCESS_WRITE,
		D3D11_USAGE_DYNAMIC
	);

	assetManager->LoadSkeleton(L"Assets/Anims/SK_Mannequin.itpskel");
	assetManager->LoadAnimation(L"Assets/Anims/ThirdPersonRun.itpanim2");

	inputhandler = new InputHandler(this);

	Pass1depthTexture = new Texture();
	Pass1depthTarget = Pass1depthTexture->CreateRenderTarget(
		mGraphics.GetScreenWidth(),
		mGraphics.GetScreenHeight(),
		DXGI_FORMAT_R32_FLOAT
	);

	//for second pass
	mGraphics.CreateDepthStencil(mGraphics.GetScreenWidth(), mGraphics.GetScreenHeight(), &mDepthTexture, &mDSV);
	DSS = mGraphics.CreateDepthStencilState(D3D11_COMPARISON_LESS);

	//for g-buffer
	normalTexture = new Texture();
	normalTarget = normalTexture->CreateRenderTarget(width, height, DXGI_FORMAT_R32G32B32A32_FLOAT);
	albedoSpecTexture = new Texture();
	albedoSpecTarget = albedoSpecTexture->CreateRenderTarget(width, height, DXGI_FORMAT_R32G32B32A32_FLOAT);
	positionTexture = new Texture();
	positionTarget = positionTexture->CreateRenderTarget(width, height, DXGI_FORMAT_R32G32B32A32_FLOAT);
	GPdepthTexture = new Texture();
	GPdepthTarget = GPdepthTexture->CreateRenderTarget(width, height, DXGI_FORMAT_R32G32B32A32_FLOAT);
	shadowTexture = new Texture();
	shadowTarget = shadowTexture->CreateRenderTarget(width, height, DXGI_FORMAT_R32G32B32A32_FLOAT);
	RTVArray[0] = positionTarget;
	RTVArray[1] = albedoSpecTarget;
	RTVArray[2] = normalTarget;
	RTVArray[3] = GPdepthTarget;
	RTVArray[4] = shadowTarget;

	////obj loader
	//objl::Loader Loader;
	//bool loadresult = Loader.LoadFile("Assets/Meshes/woman.obj");
	//if (loadresult)
	//{
	//	// Create/Open e1Out.txt
	//	std::ofstream file("woman.itpmesh3");
	//	file << "\"vertices\":[\n";
	//	// Go through each loaded mesh and out its contents
	//	for (int i = 0; i < Loader.LoadedMeshes.size(); i++)
	//	{
	//		// Copy one of the loaded meshes to be our current mesh
	//		objl::Mesh curMesh = Loader.LoadedMeshes[i];
	//		// Print Mesh Name
	//		//file << "Mesh " << i << ": " << curMesh.MeshName << "\n";

	//		// Print Vertices
	//		// Go through each vertex and print its number, position, normal, and texture coordinate
	//		for (int j = 0; j < curMesh.Vertices.size(); j++)
	//		{
	//			file << "	["
	//				<< curMesh.Vertices[j].Position.X << "," << curMesh.Vertices[j].Position.Y << "," << curMesh.Vertices[j].Position.Z << ","
	//				<< curMesh.Vertices[j].Normal.X << "," << curMesh.Vertices[j].Normal.Y << "," << curMesh.Vertices[j].Normal.Z << ","
	//				<< curMesh.Vertices[j].TextureCoordinate.X << "," << curMesh.Vertices[j].TextureCoordinate.Y
	//				<< "],\n";
	//		}
	//	}
	//	file << "],\n";

	//	file << "\"indices\":[\n";
	//	int count = 0;
	//	for (int i = 0; i < Loader.LoadedMeshes.size(); i++) {
	//		objl::Mesh curMesh = Loader.LoadedMeshes[i];
	//		for (int j = 0; j < curMesh.Indices.size(); j+=3) {
	//			file << "	[" 
	//				 << curMesh.Indices[j]  + count<< "," << curMesh.Indices[j + 1] + count << "," << curMesh.Indices[j + 2] + count
	//				 << "],\n";
	//		}
	//		count += curMesh.Indices.size();
	//	}

	//	// Close File
	//	file.close();
	//}

}

void Game::Shutdown()
{
	delete camera;

	for (auto& object : objectList) {
		delete object;
	}
	lightingBuffer->Release();
	//direcLightBuffer->Release();

	delete inputhandler;
	assetManager->Clear();


	maskBuffer->Release();

	mDSV->Release();
	mDepthTexture->Release();
	DSS->Release();

	//delete depthTexture;
	//depthTarget->Release();

	delete positionTexture;
	positionTarget->Release();
	delete normalTexture;
	normalTarget->Release();
	delete albedoSpecTexture;
	albedoSpecTarget->Release();
	delete GPdepthTexture;
	GPdepthTarget->Release();
	delete shadowTexture;
	shadowTarget->Release();

	mGraphics.CleanD3D();
}

void Game::Update(float deltaTime)
{
	for (auto& obj : objectList) {
		obj->Update(0.0f);
	}
	inputhandler->Execute();
}

void Game::RenderFrame()
{
	bool DEBUG = true;

	camera->SetActive();
	//upload light buffer
	mGraphics.UploadBuffer(lightingBuffer, &lightConst, sizeof(Lights::LightingConstants));
	mGraphics.GetDeviceContext()->PSSetConstantBuffers(mGraphics.CONSTANT_BUFFER_LIGHTING, 1, &lightingBuffer);

	//Pass 1: DepthMap pass, create the depth texture from light                                  CHECKED
	mGraphics.ClearDepthBuffer(mDSV, 1.0);
	mGraphics.SetRenderTarget(Pass1depthTarget, mDSV);
	mGraphics.ClearRenderTarget(Graphics::Color4(1.0f, 1.0f, 1.0f, 1.0f));
	for (auto& object : objectList) {
		object->Draw(assetManager->GetShader(L"DepthPass"));
	}
	devContext->OMSetRenderTargets(0, nullptr, nullptr);
	//Pass 2: G-Buffer pass, create position/normal/albedo/shadow/depth texture
	Pass1depthTexture->SetActive(Graphics::TEXTURE_SLOT_DEPTHMAP);
	mGraphics.ClearDepthBuffer(mGraphics.GetDepthView(), 1.0f);
	devContext->OMSetRenderTargets(5, RTVArray, mGraphics.GetDepthView());
	for (int i = 0; i < 5; i++) {
		mGraphics.ClearRenderTarget(RTVArray[i], Graphics::Color4(1.0f, 1.0f, 1.0f, 1.0f));
	}
	//mGraphics.SetRenderTarget(mGraphics.GetBackBuffer(), mGraphics.GetDepthView());
	//mGraphics.ClearRenderTarget(Graphics::Color4(0.0f, 0.0f, 0.0f, 1.0f));

	objectList[0]->Draw(assetManager->GetShader(L"GPass"));		//light sphere
	objectList[1]->Draw(assetManager->GetShader(L"GPass"));		//platform
	objectList[2]->Draw(assetManager->GetShader(L"GPassSkin"));	//robot

	mGraphics.SetActiveTexture(Graphics::TEXTURE_SLOT_DEPTHMAP, nullptr);
	devContext->OMSetRenderTargets(0, nullptr, nullptr);

	//Pass 3: Lighting pass, do SSR here
	mGraphics.ClearDepthBuffer(mGraphics.GetDepthView(), 1.0f);
	mGraphics.SetRenderTarget(mGraphics.GetBackBuffer(), mGraphics.GetDepthView());
	mGraphics.ClearRenderTarget(Graphics::Color4(0.5f, 0.5f, 0.5f, 1.0f));

	positionTexture->SetActive(Graphics::TEXTURE_SLOT_POSITION);
	albedoSpecTexture->SetActive(Graphics::TEXTURE_SLOT_ALBEDO);
	normalTexture->SetActive(Graphics::TEXTURE_SLOT_NORMAL);
	GPdepthTexture->SetActive(Graphics::TEXTURE_SLOT_GPDEPTH);
	shadowTexture->SetActive(Graphics::TEXTURE_SLOT_SHADOW);

	//quadVertexBuffer vert[] =
	//{
	//	{Vector3(0,0,0), Vector2(0,0)},
	//	{Vector3(1,0,0),Vector2(1,0)},
	//	{Vector3(1,1,0),Vector2(1,1)},
	//	{Vector3(0,1,0),Vector2(0,1)}
	//};
	//uint16_t indice[] = { 2,1,0,3,2,0 };
	//VertexBuffer* quadVBuffer = new VertexBuffer(
	//	vert, sizeof(vert)/sizeof(vert[0]), sizeof(vert[0]),
	//	indice, sizeof(indice)/sizeof(indice[0]), sizeof(indice[0])
	//);
	//Material* mat = new Material();
	//mat->SetShader(assetManager->GetShader(L"quadShader"));
	//Mesh* quadMesh = new Mesh(quadVBuffer, mat);
	//RenderObj* quad = new RenderObj(quadMesh);
	//quad->Draw();


	objectList[0]->Draw(assetManager->GetShader(L"lightPass"));		//light sphere
	objectList[2]->Draw(assetManager->GetShader(L"lightPass"));		//robot
	objectList[1]->Draw(assetManager->GetShader(L"SSR"));			//platform

	mGraphics.SetActiveTexture(Graphics::TEXTURE_SLOT_POSITION, nullptr);
	mGraphics.SetActiveTexture(Graphics::TEXTURE_SLOT_ALBEDO, nullptr);
	mGraphics.SetActiveTexture(Graphics::TEXTURE_SLOT_NORMAL, nullptr);
	mGraphics.SetActiveTexture(Graphics::TEXTURE_SLOT_GPDEPTH, nullptr);
	mGraphics.SetActiveTexture(Graphics::TEXTURE_SLOT_SHADOW, nullptr);
	devContext->OMSetRenderTargets(0, nullptr, nullptr);

	mGraphics.EndFrame();





	/*Pass 1, the depth buffer contains the depth values for the scene*/
	// Set the render target
	//mGraphics.SetRenderTarget(depthTarget, mGraphics.GetDepthView());
	//mGraphics.SetRenderTarget(mGraphics.GetBackBuffer(), mGraphics.GetDepthView());
	//{
	//	Graphics::Color4 clearColor(1.0f, 1.0f, 1.0f, 1.0f);
	//	mGraphics.ClearRenderTarget(clearColor);
	//	mGraphics.ClearDepthBuffer(mGraphics.GetDepthView(), 1.0f);
	//}

	////set up camera
	//camera->SetActive();

	////upload light buffer
	//mGraphics.UploadBuffer(lightingBuffer, &lightConst, sizeof(Lights::LightingConstants));
	//mGraphics.GetDeviceContext()->PSSetConstantBuffers(mGraphics.CONSTANT_BUFFER_LIGHTING, 1, &lightingBuffer);
	////upload depthmask buffer
	//mask.isDepth = true;
	//mGraphics.UploadBuffer(maskBuffer, &mask, sizeof(shaderMask));
	//mGraphics.GetDeviceContext()->VSSetConstantBuffers(mGraphics.CONSTANT_BUFFER_DEPTHMASK, 1, &maskBuffer);
	//mGraphics.GetDeviceContext()->PSSetConstantBuffers(mGraphics.CONSTANT_BUFFER_DEPTHMASK, 1, &maskBuffer);

	////draw objects
 // 	for (auto& object : objectList) {
	//	object->Draw();
	//}

	///*Pass 2*/
	////mGraphics.GetDeviceContext()->OMSetDepthStencilState(DSS, 0);
	////mGraphics.SetRenderTarget(mGraphics.GetBackBuffer(), mDSV);
	////{
	////	Graphics::Color4 clearColor(0.5f, 0.5f, 0.5f, 1.0f);
	////	mGraphics.ClearRenderTarget(clearColor);
	////	mGraphics.ClearDepthBuffer(mDSV, 1.0f);
	////}

	////mask.isDepth = false;
	////mGraphics.UploadBuffer(maskBuffer, &mask, sizeof(depthMask));
	////mGraphics.GetDeviceContext()->VSSetConstantBuffers(mGraphics.CONSTANT_BUFFER_DEPTHMASK, 1, &maskBuffer);
	////mGraphics.GetDeviceContext()->PSSetConstantBuffers(mGraphics.CONSTANT_BUFFER_DEPTHMASK, 1, &maskBuffer);
	////depthTexture->SetActive(mGraphics.TEXTURE_SLOT_DEPTHMAP);
	////for (auto& object : objectList) {
	////	object->Draw();
	////}
	////mGraphics.SetActiveTexture(mGraphics.TEXTURE_SLOT_DEPTHMAP, nullptr);
	//mGraphics.EndFrame();
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
	camera = new Camera();

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

void Game::DeferredRenderingInit() {

}
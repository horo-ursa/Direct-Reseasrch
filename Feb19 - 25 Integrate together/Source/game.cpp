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


float rotationAngel = 1;

Game::Game()
{
	//buffer = NULL;
	shader = NULL;
	vBuffer = NULL;
	render = NULL;	
}

Game::~Game()
{
}

void Game::Init(HWND hWnd, float width, float height)
{
	mGraphics.InitD3D(hWnd, width, height);
	//Setup imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	// Setup Dear ImGui style
	ImGui::StyleColorsClassic();
	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(mGraphics.GetDevice(), mGraphics.GetDeviceContext());

#pragma region Basic Patch
	VertexPos vert[] =
	{
		{Vector3(-500.0f, 500.0f, 1000.0f) },
		{Vector3(500.0f, 500.0f, 1000.0f) },
		{Vector3(500.f, -500.f, 1000.0f)},
		{Vector3(-500.f, -500.0f, 1000.0f)}
	};

	uint16_t indice[] = { 0, 1, 2, 0, 2, 3 };
	D3D11_INPUT_ELEMENT_DESC triangle[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPos, pos),
		D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	mShaderHDS = new HullDomainShader();
	mShaderHDS->Load(L"Shaders/HullDomain.hlsl", triangle, ARRAY_SIZE(triangle));
	HDBuffer = new VertexBuffer(vert, sizeof(vert) / sizeof(vert[0]), sizeof(VertexPos),
		indice, sizeof(indice) / sizeof(indice[0]), sizeof(uint16_t));
	HDRender = new RenderObj(HDBuffer);
#pragma endregion

#pragma region Adding normal, tangent, and uv

	VertexInput vhdpInputData[] =
	{
		{Vector3(-5000.0f, 5000.0f, 1000.0f), Vector3(0,1,0), Vector3(1, 0, 0), Vector2(0, 0) },
		{Vector3(5000.0f, 5000.0f, 1000.0f), Vector3(0,1,0), Vector3(1, 0, 0),Vector2(1, 0) },
		{Vector3(5000.f, -5000.f, 1000.0f), Vector3(0,1,0), Vector3(1, 0, 0),Vector2(1, 1)},
		{Vector3(-5000.f, -5000.0f, 1000.0f), Vector3(0,1,0), Vector3(1, 0, 0),Vector2(0, 1)}
	};
	uint16_t vhdpIndice[] = { 0, 1, 2, 0, 2, 3 };
	D3D11_INPUT_ELEMENT_DESC vhdpInputDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexInput, pos),
		D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexInput, normal),
		D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexInput, tangent),
		D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(VertexInput, uv),
		D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	vhdpShader = new HullDomainShader();
	vhdpShader->Load(L"Shaders/VHDP.hlsl", vhdpInputDesc, ARRAY_SIZE(vhdpInputDesc));
	VertexBuffer* vhdpVBuffer = new VertexBuffer(vhdpInputData, sizeof(vhdpInputData) / sizeof(vhdpInputData[0]), sizeof(VertexInput),
		vhdpIndice, sizeof(vhdpIndice) / sizeof(vhdpIndice[0]), sizeof(uint16_t));
	vhdpRender = new RenderObj(vhdpVBuffer);
#pragma endregion

	//setup textures and sampler
	WallTexture = new Texture();
	WallTexture->Load(L"Assets/Textures/stoneWall.tif");
	WallNormalTexture = new Texture();
	WallNormalTexture->Load(L"Assets/Textures/stoneWallNormal.tif");
	WallHeightTexture = new Texture();
	WallHeightTexture->Load(L"Assets/Textures/stoneWallHeight.tif");
	WallSampler = mGraphics.CreateSamplerState();
	WallNormalSampler = mGraphics.CreateSamplerState();
	WallHeightSampler = mGraphics.CreateSamplerState();

	//setup camera and input handler
	camera = new Camera();
	inputhandler = new InputHandler(this);

	//particle system
	partEmitter = new ParticleEmitter();
	partEmitter->SetRange(8000, 8000, 4000);
	LastPartNum = 100;
	CurrPartNum = LastPartNum;
	partEmitter->GenerateParticle(100);
}

void Game::Shutdown()
{
	//delete all shaders
	delete shader;
	delete vhdpShader;
	delete mShaderHDS;
	//delete all RenderObj;
	delete render;
	delete HDRender;
	delete vhdpRender;
	//delete particleEmitter
	delete partEmitter;
	//delete Camera
	delete camera;
	//delete inputhandler
	delete inputhandler;
	//delete all textures
	delete snowTexture;
	delete WallHeightTexture;
	delete WallTexture;
	delete WallNormalTexture;
	//release all sampler
	WallSampler->Release();
	WallHeightSampler->Release();
	WallNormalSampler->Release();

	//ImGui_ImplDX11_Shutdown();
	mGraphics.CleanD3D();
}

void Game::Update(float deltaTime)
{
	//HDRender->shaderData.tess_factor = tess_factor;
	camera->shaderData.tess_factor = tess_factor;
	if (rasterizer_state == 0) {
		mGraphics.SetupRasterizerState(D3D11_FILL_SOLID);
	}
	else {
		mGraphics.SetupRasterizerState(D3D11_FILL_WIREFRAME);
	}
	inputhandler->Execute();
	if (RenderParticle) {
		partEmitter->Update(deltaTime);
	}
}

void Game::RenderFrame()
{
	// Set the render target
	//mGraphics.SetRenderTarget(mGraphics.GetBackBuffer(), mGraphics.GetView());

	//{	// Clear the screen to blue
	//	Graphics::Color4 clearColor(0.0f,0.5f, 0.5f, 1.0f);
	//	mGraphics.ClearRenderTarget(clearColor);
	//	auto view = mGraphics.GetView();
	//	mGraphics.ClearDepthBuffer(view, 1.0f);
	//}

	Graphics::Color4 clearColor(0.3f, 0.3f, 0.3f, 1.0f);
	mGraphics.BeginFrame(clearColor);

	camera->SetActive(); 

	if (RenderFloor) {
		//Setup texture and sampler for pixel shader and domain shader
		mGraphics.PSSetActiveSampler(Graphics::SAMPLER_SLOT_WALL, WallSampler);
		WallTexture->PSSetActive(Graphics::TEXTURE_SLOT_WALL);
		mGraphics.DSSetActiveSampler(Graphics::SAMPLER_SLOT_WALLHEIGHT, WallHeightSampler);
		WallHeightTexture->DSSetActive(Graphics::TEXTURE_SLOT_WALLHEIGHT);
		//draw with VHDP shader
		vhdpShader->SetActive();
		vhdpRender->Draw();
	}

	mGraphics.BeginAlpha();
	//render particle
	if (RenderParticle) {
		if (LastPartMaxNum != CurrPartMaxNum) {
			partEmitter->SetMaxPartNum(CurrPartMaxNum);
			LastPartMaxNum = CurrPartMaxNum;
		}
		//update emitter position
		bool updated = false;
		if (LastParticleXPosition != particleXPosition) {
			LastParticleXPosition = particleXPosition;
			updated = true;
		}
		if (LastParticleYPosition != particleYPosition) {
			LastParticleYPosition = particleYPosition;
			updated = true;
		}
		if (LastParticleZPosition != particleZPosition) {
			LastParticleZPosition = particleZPosition;
			updated = true;
		}
		if (updated) {
			partEmitter->SetPosition(particleXPosition, particleYPosition, particleZPosition);
			partEmitter->UpdatePosition();
		}
		//update particle numbers
		if (LastPartNum != CurrPartNum) {
			partEmitter->GenerateParticle(CurrPartNum);
			LastPartNum = CurrPartNum;
		}
		//draw call
		partEmitter->Render();
	}


	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui_ImplDX11_NewFrame();
	{
		static float f = 50.0f;
		ImGui::Text("Render Particle"); ImGui::SameLine();
		ImGui::Checkbox("##particle checkbox", &RenderParticle);
		ImGui::Text("Render Floor"); ImGui::SameLine();
		ImGui::Checkbox("##Floor checkbox", &RenderFloor);
		ImGui::Text("Tesselation"); ImGui::SameLine();
		ImGui::SliderFloat("##float", &tess_factor, 1.0f, 64.0f);
		ImGui::Text("Rasterizer State");
		ImGui::RadioButton("Solid", &rasterizer_state, 0);
		ImGui::RadioButton("Wireframe", &rasterizer_state, 1);
		ImGui::Text("Max Particle Number: "); 
		ImGui::SliderInt("##maxnum", &CurrPartMaxNum, 1024, 10000);
		ImGui::Text("Particle X Position");
		ImGui::SliderFloat("##EmitterXPosition", &particleXPosition, -2000, 2000);
		ImGui::Text("Particle Y Position");
		ImGui::SliderFloat("##EmitterYPosition", &particleYPosition, -2000, 2000);
		ImGui::Text("Particle Z Position");
		ImGui::SliderFloat("##EmitterZPosition", &particleZPosition, -2000, 2000);
		ImGui::Text("Particle Number: "); 
		ImGui::SliderInt("##curnum", &CurrPartNum, 1, CurrPartMaxNum);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		/*ImGui::InputInt()*/
	}
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

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

	return true;
}
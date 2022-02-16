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


	//VertexPosColor vert1[] =
	//{
	//	{ Vector3(-500.0f, 500.0f, 0.0f), Graphics::Color4(1.0f, 0.0f, 0.0f, 1.0f) },
	//	{ Vector3(500.0f, 500.0f, 0.0f), Graphics::Color4(0.0f, 1.0f, 0.0f, 1.0f) },
	//	{ Vector3(500.f, -500.f, 0.0f), Graphics::Color4(0.0f, 0.0f, 1.0f, 1.0f) },
	//	{ Vector3(-500.f, -500.0f, 0.0f), Graphics::Color4(0.0f, 0.0f, 1.0f, 1.0f) },
	//};

	//uint16_t indice1[] = { 0, 1, 2, 0, 2, 3 };
	//
	//D3D11_INPUT_ELEMENT_DESC inputElem[] =
	//{ 
	//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosColor, pos),
	//	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(VertexPosColor, color),
	//	D3D11_INPUT_PER_VERTEX_DATA, 0 }
	//};
	//shader = new Shader();
	//shader->Load(L"Shaders/Mesh.hlsl", inputElem, ARRAY_SIZE(inputElem));
	//vBuffer = new VertexBuffer(vert1, sizeof(vert1)/sizeof(vert1[0]), sizeof(VertexPosColor), indice1, sizeof(indice1) / sizeof(indice1[0]), sizeof(uint16_t));
	//render = new RenderObj(vBuffer);
	
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
		{Vector3(-500.0f, 500.0f, 1000.0f), Vector3(0,1,0), Vector3(1, 0, 0), Vector2(0, 0) },
		{Vector3(500.0f, 500.0f, 1000.0f), Vector3(0,1,0), Vector3(1, 0, 0),Vector2(1, 0) },
		{Vector3(500.f, -500.f, 1000.0f), Vector3(0,1,0), Vector3(1, 0, 0),Vector2(1, 1)},
		{Vector3(-500.f, -500.0f, 1000.0f), Vector3(0,1,0), Vector3(1, 0, 0),Vector2(0, 1)}
	};
	/*VertexInput vhdpInputData[] =
	{
		{Vector3(-500.0f, 500.0f, 1000.0f),  Vector2(0, 0) },
		{Vector3(500.0f, 500.0f, 1000.0f), Vector2(0, 1) },
		{Vector3(500.f, -500.f, 1000.0f), Vector2(1, 0)},
		{Vector3(-500.f, -500.0f, 1000.0f), Vector2(1, 1)}
	};*/
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
	//D3D11_INPUT_ELEMENT_DESC vhdpInputDesc[] =
	//{
	//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexInput, pos),
	//	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(VertexInput, uv),
	//	D3D11_INPUT_PER_VERTEX_DATA, 0 }
	//};
	vhdpShader = new HullDomainShader();
	vhdpShader->Load(L"Shaders/VHDP.hlsl", vhdpInputDesc, ARRAY_SIZE(vhdpInputDesc));
	VertexBuffer* vhdpVBuffer = new VertexBuffer(vhdpInputData, sizeof(vhdpInputData) / sizeof(vhdpInputData[0]), sizeof(VertexInput),
		vhdpIndice, sizeof(vhdpIndice) / sizeof(vhdpIndice[0]), sizeof(uint16_t));
	//VertexBuffer* vhdpVBuffer = new VertexBuffer(vhdpInputData, sizeof(vhdpInputData) / sizeof(vhdpInputData[0]), sizeof(VertexInput), nullptr, 0, 0);
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
}

void Game::Shutdown()
{
	delete shader;
	delete render;
	delete HDRender;
	delete camera;
	mGraphics.CleanD3D();
}

void Game::Update(float deltaTime)
{
	HDRender->shaderData.tess_factor = tess_factor;
	vhdpRender->shaderData.tess_factor = tess_factor;
	if (rasterizer_state == 0) {
		mGraphics.SetupRasterizerState(D3D11_FILL_SOLID);
	}
	else {
		mGraphics.SetupRasterizerState(D3D11_FILL_WIREFRAME);
	}
	inputhandler->Execute();
}

void Game::RenderFrame()
{
	// Set the render target
	mGraphics.SetRenderTarget(mGraphics.GetBackBuffer(), mGraphics.GetView());

	{	// Clear the screen to blue
		Graphics::Color4 clearColor(0.0f,0.5f, 0.5f, 1.0f);
		mGraphics.ClearRenderTarget(clearColor);
		auto view = mGraphics.GetView();
		mGraphics.ClearDepthBuffer(view, 1.0f);
	}
	camera->SetActive(); 

	//Setup texture and sampler for pixel shader and domain shader
	mGraphics.PSSetActiveSampler(Graphics::SAMPLER_SLOT_WALL, WallSampler);
	WallTexture->PSSetActive(Graphics::TEXTURE_SLOT_WALL);
	mGraphics.DSSetActiveSampler(Graphics::SAMPLER_SLOT_WALLHEIGHT, WallHeightSampler);
	WallHeightTexture->DSSetActive(Graphics::TEXTURE_SLOT_WALLHEIGHT);
	/*
	mGraphics.PSSetActiveSampler(Graphics::SAMPLER_SLOT_WALLNORMAL, WallNormalSampler);
	WallNormalTexture->PSSetActive(Graphics::TEXTURE_SLOT_WALLNORMAL);*/

	//draw the square patch for tessollation
	//mShaderHDS->SetActive();
	//HDRender->Draw();

	//draw with VHDP shader
	vhdpShader->SetActive();
	vhdpRender->Draw();


	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui_ImplDX11_NewFrame();
	{
		static float f = 50.0f;
		ImGui::Text("Tesselation");                        
		ImGui::SliderFloat("float", &tess_factor, 1.0f, 64.0f);
		ImGui::Text("Rasterizer State");
		ImGui::RadioButton("Solid", &rasterizer_state, 0);
		ImGui::RadioButton("Wireframe", &rasterizer_state, 1);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
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
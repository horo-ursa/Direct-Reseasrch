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


float rotationAngel = 0;

Game::Game()
{
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

	VertexPosColor vert[] =
	{
		{ Vector3(0.0f, 0.5f, 0.0f), Graphics::Color4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ Vector3(0.45f, -0.5f, 0.0f), Graphics::Color4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ Vector3(-0.45f, -0.5f, 0.0f), Graphics::Color4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ Vector3(0.7f, -0.6f, 0.0f), Graphics::Color4(1.0f, 0.0f, 1.0f, 1.0f) },
		{ Vector3(0.3f, -0.2f, 0.0f), Graphics::Color4(0.3f, 0.4f, 0.2f, 1.0f) },
		{ Vector3(-0.4f, 0.3f, 0.0f), Graphics::Color4(0.0f, 0.4f, 1.0f, 1.0f) },
		{ Vector3(0.1f, 0.1f, 0.0f), Graphics::Color4(0.3f, 0.2f, 1.0f, 1.0f) }
	};

	shader = new Shader();
	D3D11_INPUT_ELEMENT_DESC inputElem[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosColor, pos),
		D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(VertexPosColor, color),
		D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	shader->Load(L"Shaders/Particle.hlsl", inputElem, ARRAY_SIZE(inputElem));
	UINT pStride = sizeof(VertexPosColor);
	vBuffer = new VertexBuffer(vert, sizeof(vert) / sizeof(vert[0]), pStride);
	render = new RenderObj(vBuffer);
	inputHandler = new InputHandler(this);
	camera = new Camera();
	
	snowTexture = new Texture();
	snowTexture->Load(L"Assets/Textures/snow.png");


	/*simple = new Shader();
	simple->Load(L"Shaders/Simple.hlsl", inputElem, ARRAY_SIZE(inputElem));
	rcube = new RenderCube();*/
}

void Game::Shutdown()
{
	delete shader;
	delete render;
	delete camera;
	delete snowTexture;
	delete rcube;
	mGraphics.CleanD3D();
}

void Game::Update(float deltaTime)
{
	rotationAngel += Math::Pi * deltaTime;
	//rotationAngel = 25;
	Matrix4 scale = Matrix4::CreateScale(300.f);
	render->match.c_modelToWorld = Matrix4::CreateRotationZ(rotationAngel) * scale;
	inputHandler->Execute();
}

void Game::RenderFrame()
{
	// Set the render target
	mGraphics.SetRenderTarget(mGraphics.GetBackBuffer(), mGraphics.GetView());

	{	// Clear the screen to blue
		Graphics::Color4 clearColor(0.0f, 0.2f, 0.4f, 1.0f);
		mGraphics.ClearRenderTarget(clearColor);
		auto view = mGraphics.GetView();
		mGraphics.ClearDepthBuffer(view, 1.0f);
	}

	camera->SetActive();
	shader->SetActive();
	//setup changes for geometry shader
	mGraphics.GetDeviceContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	snowTexture->SetActive(Graphics::TEXTURE_SLOT_SNOW);
	render->Draw();
	//simple->SetActive();
	//rcube->Draw();
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

bool Game::IsKeyHeld(uint32_t key) const
{
	const auto find = m_keyIsHeld.find(key);
	if (find != m_keyIsHeld.end())
		return find->second;
	return false;
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
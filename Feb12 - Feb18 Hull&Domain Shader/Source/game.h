#pragma once
#include "Graphics.h"
#include "VertexBuffer.h"
#include "RenderObj.h"
#include "Camera.h"
#include "RenderCube.h"
#include "InputHandler.h"
#include "texture.h"
#include "GeometryShader.h"
#include "HullDomainShader.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

class Shader;

class Game
{
public:
    Game();
    ~Game();

    void Init(HWND hWnd, float width, float height);
    void Shutdown();
	void Update(float deltaTime);
    void RenderFrame();

	void OnKeyDown(uint32_t key);
	void OnKeyUp(uint32_t key);
	void GetMousePos(uint32_t xPos, uint32_t yPos);
	bool IsKeyHeld(uint32_t key) const;
	bool LoadLevel(const WCHAR* fileName);

	Camera* camera;
	Graphics mGraphics;
private:
	std::unordered_map<uint32_t, bool> m_keyIsHeld;

	RenderCube* rCube;
	
	InputHandler* inputhandler;
	VertexBuffer* vBufferParticle;
	RenderObj* particles;
	GeometryShader* particleShader;

	//mouse input
	bool firstMouse = true;
	float lastX; float lastY;
	
	//shaders
	Shader* shader;
	HullDomainShader* mShaderHDS;
	HullDomainShader* vhdpShader;

	//vertexbuffers
	VertexBuffer* vBuffer;
	VertexBuffer* HDBuffer;

	//RenderObj
	RenderObj* render;
	RenderObj* HDRender;
	RenderObj* vhdpRender;

	//Textures
	Texture* snowTexture;
	Texture* WallTexture;
	Texture* WallNormalTexture;
	Texture* WallHeightTexture;

	//SamplerState
	ID3D11SamplerState* WallSampler;
	ID3D11SamplerState* WallNormalSampler;
	ID3D11SamplerState* WallHeightSampler;

	//some data for imgui
	float tess_factor = 1.0f;
	int rasterizer_state = 0;
};
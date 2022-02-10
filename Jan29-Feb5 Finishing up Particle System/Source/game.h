#pragma once
#include "Graphics.h"
#include "VertexBuffer.h"
#include "RenderObj.h"
#include "Camera.h"
#include "InputHandler.h"
#include "texture.h"
#include "RenderCube.h"

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
	bool IsKeyHeld(uint32_t key) const;
	void GetMousePos(uint32_t xpos, uint32_t ypos);
	Camera* camera;

private:
	std::unordered_map<uint32_t, bool> m_keyIsHeld;
	Graphics mGraphics;

	Shader* shader;
	Shader* simple;


	VertexBuffer* vBuffer;
	RenderObj* render;

	InputHandler* inputHandler;

	Texture* snowTexture;
	
	bool LoadLevel(const WCHAR* fileName);

	//for testing
	RenderCube* rcube;

	//mouse input
	bool firstMouse = true;
	float lastX; float lastY;
};
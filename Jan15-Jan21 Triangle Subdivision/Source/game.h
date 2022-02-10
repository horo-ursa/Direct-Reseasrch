#pragma once
#include "Graphics.h"
#include "VertexBuffer.h"
#include "RenderObj.h"

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

private:
	std::unordered_map<uint32_t, bool> m_keyIsHeld;
	Graphics mGraphics;

	// TODO Lab 02b
	//ID3D11Buffer* buffer;
	// TODO Lab 02c
	Shader* shader;
	// TODO Lab 02e
	VertexBuffer* vBuffer;
	// TODO Lab 02f
	RenderObj* render;
	bool LoadLevel(const WCHAR* fileName);
};
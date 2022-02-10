#pragma once
#include "Graphics.h"
#include "VertexBuffer.h"
#include "RenderObj.h"
#include "Camera.h"
#include "RenderCube.h"
#include "texture.h"
#include "Material.h"
#include "Lights.h"
#include "assetManager.h"
#include "Physics.h"
#include "InputHandler.h"
#include "GeometryShader.h"

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

	Lights::PointLightData* AllocateLight();
	void FreeLight(Lights::PointLightData* pLight);
	void SetAmbientLight(const Vector3& color);
	const Vector3& GetAmbientLight() const;
	void GetMousePos(uint32_t xPos, uint32_t yPos);
	AssetManager* assetManager;
	Camera* camera;
	Physics* mPhysics;
private:
	std::unordered_map<uint32_t, bool> m_keyIsHeld;
	Graphics mGraphics;

	bool LoadLevel(const WCHAR* fileName);
	
	ID3D11Buffer* lightingBuffer;
	Lights::LightingConstants lightConst;
	std::vector<RenderObj*> objectList;
	InputHandler* inputhandler;

	GeometryShader* gsShader;
	RenderObj* particles;
	Mesh* mMesh;

	//mouse input
	bool firstMouse = true;
	float lastX; float lastY;
};
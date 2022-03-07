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

	//allocate point light
	Lights::PointLightData* AllocateLight();
	void FreeLight(Lights::PointLightData* pLight);
	//allocate directional light
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
	
	//lighting stuff
	ID3D11Buffer* lightingBuffer;
	Lights::LightingConstants lightConst;
	//ID3D11Buffer* direcLightBuffer;
	//Lights::DirectionalLightConstants direcLightConst;


	std::vector<RenderObj*> objectList;
	InputHandler* inputhandler;


	//mouse input
	bool firstMouse = true;
	float lastX; float lastY;

	//
	struct depthMask
	{
		bool isDepth;
		Vector3 pad;
	};
	depthMask mask;
	ID3D11Buffer* maskBuffer;

	Texture* depthTexture;
	ID3D11RenderTargetView* depthTarget;

	//for second pass depthstencilview
	ID3D11DepthStencilView* mDSV;
	ID3D11Texture2D* mDepthTexture;
	ID3D11DepthStencilState* DSS;
};
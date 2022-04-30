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
#include "ParticleEmitter.h"

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

	void DeferredRenderingInit();
private:
	std::unordered_map<uint32_t, bool> m_keyIsHeld;
	Graphics mGraphics;

	bool saved = false;

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
	struct shaderMask
	{
		bool isDepth;
		bool isGBufferPass;
		bool isLightPass;
		bool padding1;
		Vector3 padding2;
	};
	shaderMask mask;
	ID3D11Buffer* maskBuffer;

	Texture* Pass1depthTexture;
	ID3D11RenderTargetView* Pass1depthTarget;

	//for second pass depthstencilview
	ID3D11DepthStencilView* mDSV;
	ID3D11Texture2D* mDepthTexture;
	ID3D11DepthStencilState* DSS;

	//---------------------------------------Deferred Rendering-------------------------------------//
	//for G-buffer pass
	ID3D11RenderTargetView* positionTarget;
	ID3D11RenderTargetView* albedoSpecTarget;
	ID3D11RenderTargetView* normalTarget;
	ID3D11RenderTargetView* GPdepthTarget;
	ID3D11RenderTargetView* shadowTarget;
	Texture* positionTexture;
	Texture* albedoSpecTexture;
	Texture* normalTexture;
	Texture* GPdepthTexture;
	Texture* shadowTexture;
	ID3D11Device* device;
	ID3D11DeviceContext* devContext;
	ID3D11RenderTargetView* RTVArray[5];

	//---------------------------------------Particle System---------------------------------------//
	ParticleEmitter* partEmitter;

};
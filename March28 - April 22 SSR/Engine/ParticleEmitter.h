#pragma once
#include "engineMath.h"
#include "Graphics.h"
#include "texture.h"
#include "GeometryShader.h"

class ParticleEmitter {
public:
    enum { Max_Part_Num = 1000000 };    // can't create particles more than 1000000
    struct PerParticleRenderData        // the unique render data per particle
    {
        Vector3 pos;                    // center position of the particle in world space
        float size;                     // current width of the particle
        Vector4 endColor;
        float timeout;
        float lifeTimer;
        float padding[2];
    };
    struct PerParticleUpdateData        // extra data needed for updating but not render
    {
        Vector3 vel;
        float timer;
    };
    struct ParticleConst                // these are the emitter constants
    {                                   // they do not vary per-particle
        float deltaTime = 0.0f;
        float gravity = 0.0f;
        float startSize = 0.0f;
        float endSize = 0.0f;
        Graphics::Color4 startColor;
        Graphics::Color4 endColor;
        float timeOut = 0.0f;
        float pad[3] = {};
    };

	ParticleEmitter();
	~ParticleEmitter();
	void SetRange(float x, float y, float z);
	void SetParticleNumber(int num);
	void GenerateParticle(int num);
    Vector3 PosRandomGenerator();
    int IntRandomGenerator(int from, int to);
    void SetTexture(Texture* mTexture);
    void Render();
    void SetMaxPartNum(int num);
    void SetPosition(float x, float y, float z);
    void UpdatePosition();
    void Update(float deltaTime);
private:
    int MAX_PARTICLES = 1024;      // the maximum particles per emitter
	int particleNumber;            // how many particles we are rendering
    float xRange = 100.f;          //default range that generate random particles;
	float yRange = 100.f;
	float zRange = 100.f;

    Graphics* pGraphics;
    Texture* partTexture;
    ID3D11SamplerState* particleSampler;
    GeometryShader* gsShader;
    Vector3 emitterPosition;
    Vector3 PreEmitterPosition;

    ID3D11BlendState* mBlend; 

    // current particles
    PerParticleRenderData mPartRender[Max_Part_Num];
    PerParticleUpdateData mPartUpdate[Max_Part_Num];
    ID3D11Buffer* mPartBuffer;
};
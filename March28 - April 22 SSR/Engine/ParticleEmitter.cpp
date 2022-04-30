#include "stdafx.h"
#include "ParticleEmitter.h"
#include <algorithm>
#include "VertexBuffer.h"
#include <random>
#include "GeometryShader.h"



ParticleEmitter::ParticleEmitter(): 
	partTexture(nullptr),
	particleNumber(0),
	mPartBuffer(nullptr)
{
	pGraphics = Graphics::Get();
	mBlend = pGraphics->CreateBlendState(true, D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA);
	D3D11_INPUT_ELEMENT_DESC GeometryShaderDesc[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(PerParticleRenderData, pos),
		D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"SIZE", 0, DXGI_FORMAT_R32_FLOAT, 0, offsetof(PerParticleRenderData, size),
		D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(PerParticleRenderData, endColor),
		D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"TIMEOUT", 0, DXGI_FORMAT_R32_FLOAT, 0, offsetof(PerParticleRenderData, timeout),
		D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"LIFETIMER", 0, DXGI_FORMAT_R32_FLOAT, 0, offsetof(PerParticleRenderData, lifeTimer),
		D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	gsShader = new GeometryShader();
	gsShader->Load(L"Shaders/Particle.hlsl", GeometryShaderDesc, ARRAY_SIZE(GeometryShaderDesc));

	partTexture = new Texture();
	partTexture->Load(L"Assets/Textures/Cloud.png");
	particleSampler = pGraphics->CreateSamplerState();
}

ParticleEmitter::~ParticleEmitter() {
	// release the resources in use by this emitter
	mPartBuffer->Release();
	delete gsShader;
	delete partTexture;
	if (particleSampler) {
		particleSampler->Release();
	}
	mBlend->Release();
}

void ParticleEmitter::SetRange(float x, float y, float z)
{
	xRange = x;
	yRange = y;
	zRange = z;
}

void ParticleEmitter::SetParticleNumber(int num) {
	particleNumber = num;
}

void ParticleEmitter::GenerateParticle(int num) {
	if (mPartBuffer) {
		mPartBuffer->Release();
	}
	if (num < MAX_PARTICLES) { 
		particleNumber = num;
	}
	else { 
		particleNumber = MAX_PARTICLES;
	}
	for (int i = 0; i < particleNumber; i++) {
		mPartRender[i].pos = PosRandomGenerator() + emitterPosition;
		mPartRender[i].size = IntRandomGenerator(1, 10);
		mPartRender[i].timeout = IntRandomGenerator(1, 5);
		mPartRender[i].lifeTimer = 0.0f;
		mPartRender[i].endColor = Vector4(0.0, 0.0, 0.0, 0.0);
	}
	// Each emitter has a vertex buffer it uses to render all the particles that belong to this emitter.
	mPartBuffer = pGraphics->CreateGraphicsBuffer(&mPartRender, sizeof(mPartRender[0]) * particleNumber,
		D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
}

Vector3 ParticleEmitter::PosRandomGenerator() {
	const int range_from_x = -xRange / 2;
	const int range_to_x = xRange / 2;
	const int range_from_y = -yRange / 2;
	const int range_to_y = yRange / 2;
	//const int range_from_z = 0;
	const int range_from_z = zRange - 10;
	const int range_to_z = zRange;
	std::random_device					rand_dev;
	std::mt19937						generator(rand_dev());
	std::uniform_int_distribution<int>  distrX(range_from_x, range_to_x);
	std::uniform_int_distribution<int>  distrY(range_from_y, range_to_y);
	std::uniform_int_distribution<int>  distrZ(range_from_z, range_to_z);
	return Vector3(distrX(generator), distrY(generator), -distrZ(generator));
}

int ParticleEmitter::IntRandomGenerator(int from, int to) {
	const int range_from = from;
	const int range_to = to;
	std::random_device					  rand_dev;
	std::mt19937						  generator(rand_dev());
	std::uniform_int_distribution<int>  distr(range_from, range_to);
	return distr(generator);
}

void ParticleEmitter::SetTexture(Texture* mTexture) {
	partTexture = mTexture;
}

void ParticleEmitter::Render()
{
	//Input Assembler stage, Geometry Shader already handled this
	//pGraphics->GetDeviceContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	//set up shader
	gsShader->SetActive();
	//set up sampler
	pGraphics->PSSetActiveSampler(Graphics::SAMPLER_SLOT_PARTICLE, particleSampler);
	//set up texture
	//partTexture =  new Texture();
	//partTexture->Load(L"Assets/Textures/Cloud.png");
	partTexture->PSSetActive(Graphics::TEXTURE_SLOT_PARTICLE);
	//set up blend state
	pGraphics->SetBlendState(mBlend);
	//set up vertex data
	UINT stride = sizeof(PerParticleRenderData);
	UINT offset = 0;
	pGraphics->UploadBuffer(mPartBuffer, &mPartRender, sizeof(mPartRender[0]) * particleNumber);
	pGraphics->GetDeviceContext()->IASetVertexBuffers(0, 1, &mPartBuffer, &stride, &offset);
	//draw call
	pGraphics->GetDeviceContext()->Draw(particleNumber, 0);

}

void ParticleEmitter::SetMaxPartNum(int num) {
	MAX_PARTICLES = num;
}

void ParticleEmitter::SetPosition(float x, float y, float z) {
	emitterPosition = Vector3(x, y, z);
}

void ParticleEmitter::UpdatePosition()
{
	for (int i = 0; i < particleNumber; i++) {
		mPartRender[i].pos += (emitterPosition - PreEmitterPosition);
	}
	PreEmitterPosition = emitterPosition;
}

void ParticleEmitter::Update(float deltaTime)
{
	for (int i = 0; i < particleNumber; i++) {
		mPartRender[i].pos.z -= IntRandomGenerator(5, 20);
		mPartRender[i].lifeTimer += deltaTime;
		if (mPartRender[i].lifeTimer >= mPartRender[i].timeout) {
			mPartRender[i].pos = PosRandomGenerator() + emitterPosition;
			mPartRender[i].size = IntRandomGenerator(1, 100);
			mPartRender[i].timeout = IntRandomGenerator(1, 10);
			mPartRender[i].lifeTimer = 0.0f;
			mPartRender[i].endColor = Vector4(0.0, 0.0, 0.0, 0.0);
		}
	}
}
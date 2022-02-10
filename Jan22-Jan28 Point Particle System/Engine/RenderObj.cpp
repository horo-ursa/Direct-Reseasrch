#include "stdafx.h"
#include "RenderObj.h"
#include "VertexBuffer.h"
#include "Graphics.h"

RenderObj::RenderObj(const VertexBuffer* vertexBuffer) : vBuffer(vertexBuffer){
	mGraphics = Graphics::Get();
	bufferData.time = 0.0f;
	constBuffer = mGraphics->CreateGraphicsBuffer(
		&bufferData,
		sizeof(bufferData),
		D3D11_BIND_CONSTANT_BUFFER,
		D3D11_CPU_ACCESS_WRITE,
		D3D11_USAGE_DYNAMIC
	);
}

RenderObj::~RenderObj() {
	delete vBuffer;
}

void RenderObj::Update(float deltaTime)
{
	bufferData.time += deltaTime;
}

void RenderObj::Draw() {
	mGraphics->UploadBuffer(constBuffer, &bufferData, sizeof(bufferData));
	mGraphics->GetDeviceContext()->VSSetConstantBuffers(Graphics::CONSTANT_VERTEX_ANIMATION, 1, &constBuffer);
	vBuffer->Draw();
}
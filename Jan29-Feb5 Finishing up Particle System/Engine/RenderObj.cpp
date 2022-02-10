#include "stdafx.h"
#include "RenderObj.h"
#include "VertexBuffer.h"
#include "Graphics.h"

RenderObj::RenderObj() {
	mGraphics = Graphics::Get();
	constBuffer = mGraphics->CreateGraphicsBuffer(
		&match,
		sizeof(match),
		D3D11_BIND_CONSTANT_BUFFER,
		D3D11_CPU_ACCESS_WRITE,
		D3D11_USAGE_DYNAMIC
	);

	//match.c_modelToWorld = Matrix4::CreateRotationZ(Math::ToRadians(45.0f));
	match.c_modelToWorld = Matrix4::Identity;
}

RenderObj::RenderObj(const VertexBuffer* vertexBuffer) : vBuffer(vertexBuffer){
	mGraphics = Graphics::Get();
	match.c_modelToWorld = Matrix4::Identity;
	constBuffer = mGraphics->CreateGraphicsBuffer(
		&match,
		sizeof(match),
		D3D11_BIND_CONSTANT_BUFFER,
		D3D11_CPU_ACCESS_WRITE,
		D3D11_USAGE_DYNAMIC
	);
}

RenderObj::~RenderObj() {
	constBuffer->Release();
	delete vBuffer;
}


void RenderObj::Draw() {
	mGraphics->UploadBuffer(constBuffer, &match, sizeof(match));
	mGraphics->GetDeviceContext()->VSSetConstantBuffers(Graphics::CONSTANT_BUFFER_RENDEROBJ, 1, &constBuffer);
	vBuffer->Draw();
}
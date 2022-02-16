#include "stdafx.h"
#include "RenderObj.h"
#include "VertexBuffer.h"
#include "Graphics.h"

RenderObj::RenderObj(){
	graphic = Graphics::Get();
	objectBuffer = graphic->CreateGraphicsBuffer(
		&match,
		sizeof(match),
		D3D11_BIND_CONSTANT_BUFFER,
		D3D11_CPU_ACCESS_WRITE,
		D3D11_USAGE_DYNAMIC
	);

	match.c_modelToWorld = Matrix4::CreateRotationZ(Math::ToRadians(45.0f));
}

RenderObj::RenderObj(const VertexBuffer* vertexBuffer) : vBuffer(vertexBuffer){
	graphic = Graphics::Get();
	objectBuffer = graphic->CreateGraphicsBuffer(
		&match, 
		sizeof(match), 
		D3D11_BIND_CONSTANT_BUFFER, 
		D3D11_CPU_ACCESS_WRITE, 
		D3D11_USAGE_DYNAMIC
	);
	shaderData.tess_factor = 1.f;
	tessBuffer = graphic->CreateGraphicsBuffer(
		&shaderData,
		sizeof(shaderData),
		D3D11_BIND_CONSTANT_BUFFER,
		D3D11_CPU_ACCESS_WRITE,
		D3D11_USAGE_DYNAMIC
	);
	match.c_modelToWorld = Matrix4::Identity;
	//match.c_modelToWorld = Matrix4::CreateRotationX(Math::ToRadians(45.0f));
}

RenderObj::~RenderObj() {
	objectBuffer->Release();
	tessBuffer->Release();
	delete vBuffer;
}

void RenderObj::Draw() {
	const void* data = &match.c_modelToWorld;
	size_t size = sizeof(match.c_modelToWorld);
	graphic->UploadBuffer(objectBuffer, data, size);
	graphic->GetDeviceContext()->VSSetConstantBuffers(Graphics::ConstantBuffer::CONSTANT_BUFFER_RENDEROBJ, 1, &objectBuffer);
	graphic->GetDeviceContext()->HSSetConstantBuffers(Graphics::ConstantBuffer::CONSTANT_BUFFER_RENDEROBJ, 1, &objectBuffer);
	graphic->GetDeviceContext()->DSSetConstantBuffers(Graphics::ConstantBuffer::CONSTANT_BUFFER_RENDEROBJ, 1, &objectBuffer);
	graphic->GetDeviceContext()->PSSetConstantBuffers(Graphics::ConstantBuffer::CONSTANT_BUFFER_RENDEROBJ, 1, &objectBuffer);

	graphic->UploadBuffer(tessBuffer, &shaderData, sizeof(shaderData));
	graphic->GetDeviceContext()->VSSetConstantBuffers(Graphics::CONSTANT_BUFFER_TESS, 1, &tessBuffer);
	graphic->GetDeviceContext()->HSSetConstantBuffers(Graphics::CONSTANT_BUFFER_TESS, 1, &tessBuffer);
	graphic->GetDeviceContext()->DSSetConstantBuffers(Graphics::CONSTANT_BUFFER_TESS, 1, &tessBuffer);
	graphic->GetDeviceContext()->PSSetConstantBuffers(Graphics::CONSTANT_BUFFER_TESS, 1, &tessBuffer);
	vBuffer->Draw();

}
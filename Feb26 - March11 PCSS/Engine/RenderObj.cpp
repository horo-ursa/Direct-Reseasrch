#include "stdafx.h"
#include "RenderObj.h"
#include "VertexBuffer.h"
#include "Graphics.h"
#include "Material.h"
#include "mesh.h"

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

RenderObj::RenderObj(const Mesh* mesh) : mMesh(mesh)
{
	graphic = Graphics::Get();
	objectBuffer = graphic->CreateGraphicsBuffer(
		&match,
		sizeof(match),
		D3D11_BIND_CONSTANT_BUFFER,
		D3D11_CPU_ACCESS_WRITE,
		D3D11_USAGE_DYNAMIC
	);
	match.c_modelToWorld = Matrix4::Identity;
}

RenderObj::~RenderObj() {
	objectBuffer->Release();
	//delete vBuffer;
}

void RenderObj::Draw() {
	const void* data = &match.c_modelToWorld;
	size_t size = sizeof(match.c_modelToWorld);
	graphic->UploadBuffer(objectBuffer, data, size);
	graphic->GetDeviceContext()->VSSetConstantBuffers(Graphics::ConstantBuffer::CONSTANT_BUFFER_RENDEROBJ, 1, &objectBuffer);
	mMesh->Draw();
}

void RenderObj::AddComponent(Component* pComp)
{
	mComponent.push_back(pComp);
}
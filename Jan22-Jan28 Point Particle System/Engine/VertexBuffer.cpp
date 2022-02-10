#include "stdafx.h"
#include "VertexBuffer.h"
#include "Graphics.h"
#include "VertexFormat.h"

VertexBuffer::VertexBuffer(const void* vertData, uint32_t vertCount, uint32_t vertStride)
{
	stride = vertStride;
	vertices = vertCount;
	offset = 0;
	mGraphic = Graphics::Get();
	verBuffer = mGraphic->CreateGraphicsBuffer(vertData, vertices * stride ,
		D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
}


VertexBuffer::~VertexBuffer() 
{
	verBuffer->Release();
}

void VertexBuffer::Draw() const
{
	mGraphic->GetDeviceContext()->IASetVertexBuffers(0, 1, &verBuffer, &stride, &offset);
	mGraphic->GetDeviceContext()->Draw(vertices, 0);
}
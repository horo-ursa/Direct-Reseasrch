#include "stdafx.h"
#include "VertexBuffer.h"
#include "Graphics.h"
#include "VertexFormat.h"

VertexBuffer::VertexBuffer(
	const void* vertData, uint32_t vertCount, uint32_t vertStride,
	const void* indexData, uint32_t indexCount, uint32_t indexStride)
{
	// not sure for this DXGI_FORMAT
	format = (indexStride == sizeof(uint32_t)) ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;

	data = vertData;
	stride = vertStride;
	vertices = vertCount;
	offset = 0;
	datasize = vertices * stride;

	index_stride = indexStride;
	index = indexCount;

	mGraphic = Graphics::Get();
	verBuffer = mGraphic->CreateGraphicsBuffer(data, datasize,
		D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
	if (indexData) {
		indexBuffer = mGraphic->CreateGraphicsBuffer(indexData, indexCount * indexStride, D3D11_BIND_INDEX_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
	}
	else {
		indexBuffer = nullptr;
	}
}


VertexBuffer::~VertexBuffer()
{
	verBuffer->Release();
	if (indexBuffer) {
		indexBuffer->Release();
	}
}

void VertexBuffer::Draw() const
{
	auto context = mGraphic->GetDeviceContext();
	context->IASetVertexBuffers(0, 1, &verBuffer, &stride, &offset);
	if (indexBuffer) {
		context->IASetIndexBuffer(indexBuffer, format, 0);
		context->DrawIndexed(index, 0, 0);
	}
	else {
		context->Draw(vertices, 0);
	}
}


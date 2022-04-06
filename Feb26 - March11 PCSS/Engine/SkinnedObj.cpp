#include "stdafx.h"
#include "SkinnedObj.h"
#include "Graphics.h"

SkinnedObj::SkinnedObj(Mesh* mesh) : RenderObj(mesh) {
	for (int i = 0; i < matrixNum; i++) {
		constant.c_skinMatrix[i] = Matrix4::Identity;
	}
	mGraphic = Graphics::Get();
	constBuffer = graphic->CreateGraphicsBuffer(
		&constant,
		sizeof(constant),
		D3D11_BIND_CONSTANT_BUFFER,
		D3D11_CPU_ACCESS_WRITE,
		D3D11_USAGE_DYNAMIC
	);
}

SkinnedObj::~SkinnedObj() {
	constBuffer->Release();
}

void SkinnedObj::Draw()
{
	const void* data = &constant.c_skinMatrix;
	size_t size = sizeof(constant.c_skinMatrix);
	mGraphic->UploadBuffer(constBuffer, data, size);
	mGraphic->GetDeviceContext()->VSSetConstantBuffers(mGraphic->CONSTANT_BUFFER_Skin, 1, &constBuffer);
	RenderObj::Draw();
}

void SkinnedObj::BindtoConstant(std::vector<Matrix4> data) {
	for (int i = 0; i < data.size(); i++) {
		constant.c_skinMatrix[i] = data[i];
	}
}
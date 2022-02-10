#include "stdafx.h"
#include "Camera.h"
#include "Graphics.h"

Camera::Camera() {
	pGraphics = Graphics::Get();
	projMatrix = Matrix4::CreateRotationY(-Math::PiOver2)
		* Matrix4::CreateRotationZ(-Math::PiOver2)
		* Matrix4::CreatePerspectiveFOV(Math::ToRadians(70.0f),
			pGraphics->GetScreenWidth(), pGraphics->GetScreenHeight(),
			25.0f, 10000.0f);
	constBuffer = pGraphics->CreateGraphicsBuffer(&pcc, sizeof(pcc), D3D11_BIND_CONSTANT_BUFFER,
		D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
}

Camera::~Camera() {
	constBuffer->Release();
}

void Camera::SetActive() {
	pcc.c_viewProj = worldToCameraMatrix * projMatrix;
	Matrix4 invMatrix = worldToCameraMatrix;
	invMatrix.Invert();
	pcc.c_cameraPosition = invMatrix.GetTranslation();
	pGraphics->UploadBuffer(constBuffer, &pcc, sizeof(pcc));
	pGraphics->GetDeviceContext()->VSSetConstantBuffers(
		Graphics::CONSTANT_BUFFER_CAMERA, 1, &constBuffer
	);
	pGraphics->GetDeviceContext()->PSSetConstantBuffers(
		Graphics::CONSTANT_BUFFER_CAMERA, 1, &constBuffer
	);
	pGraphics->GetDeviceContext()->GSSetConstantBuffers(
		Graphics::CONSTANT_BUFFER_CAMERA, 1, &constBuffer
	);
}

void Camera::SetPositionAndRotation(Vector3 pos, Quaternion rot)
{
	//worldToCameraMatrix = Matrix4::CreateFromQuaternion(rot) * Matrix4::CreateTranslation(pos);
	Vector3 newPos = Vector3(-pos.x, -pos.y, -pos.z);
	worldToCameraMatrix = Matrix4::CreateFromQuaternion(rot) * Matrix4::CreateTranslation(newPos);
}
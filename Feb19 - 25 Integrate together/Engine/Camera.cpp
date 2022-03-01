#include "stdafx.h"
#include "Camera.h"
#include "Graphics.h"

Camera::Camera() : MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
	WorldUp = Vector3(0, 1, 0);
	Position = Vector3(-40.f, -4832.f, -1772.f);
	Yaw = YAW;
	Pitch = PITCH;
	updateCameraVectors();
	//worldToCameraMatrix = Matrix4::CreateTranslation(Vector3(0.0f, 0.0f, -100.f));
	pGraphics = Graphics::Get();
	/*projMatrix = Matrix4::CreateOrtho(
		pGraphics->GetScreenWidth(), pGraphics->GetScreenHeight(), 1000.0f, -1000.0f
	);*/
	projMatrix = Matrix4::CreatePerspectiveFOV(Math::ToRadians(90.0f),
		pGraphics->GetScreenWidth(), pGraphics->GetScreenHeight(),
		25.0f, 100000.0f);
	constBuffer = pGraphics->CreateGraphicsBuffer(&pcc, sizeof(pcc), D3D11_BIND_CONSTANT_BUFFER,
		D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);

	shaderData.tess_factor = 1.f;
	tessBuffer = pGraphics->CreateGraphicsBuffer(
		&shaderData,
		sizeof(shaderData),
		D3D11_BIND_CONSTANT_BUFFER,
		D3D11_CPU_ACCESS_WRITE,
		D3D11_USAGE_DYNAMIC
	);
}

Camera::~Camera() {
	constBuffer->Release();
	tessBuffer->Release();
}

void Camera::SetActive() {
	/*pcc.c_viewProj = worldToCameraMatrix * projMatrix;
	Matrix4 invMatrix = worldToCameraMatrix;
	invMatrix.Invert();
	pcc.c_cameraPosition = invMatrix.GetTranslation();*/

	pcc.c_cameraPosition = Position;
	Matrix4 cameraToWorld = Matrix4::CreateLookAt(Position, Position + Front, Up);
	cameraToWorld.Invert();
	worldToCameraMatrix = cameraToWorld;
	pcc.c_viewProj = worldToCameraMatrix * projMatrix;
	pcc.UpVector = Up;

	//update camera
	updateCameraVectors();

	//update tess_factor
	auto length = (Position - floorPos).Length();
	if (length <= 1000) {
		shaderData.tess_factor = 64.0f;
	}
	else if (length >= 10000) {
		shaderData.tess_factor = 1.0f;
	}
	else {
		shaderData.tess_factor = Math::Lerp(64, 1, length / 10000);
	}

	pGraphics->UploadBuffer(constBuffer, &pcc, sizeof(pcc));
	pGraphics->GetDeviceContext()->VSSetConstantBuffers(Graphics::CONSTANT_BUFFER_CAMERA, 1, &constBuffer);
	pGraphics->GetDeviceContext()->HSSetConstantBuffers(Graphics::CONSTANT_BUFFER_CAMERA, 1, &constBuffer);
	pGraphics->GetDeviceContext()->DSSetConstantBuffers(Graphics::CONSTANT_BUFFER_CAMERA, 1, &constBuffer);
	pGraphics->GetDeviceContext()->PSSetConstantBuffers(Graphics::CONSTANT_BUFFER_CAMERA, 1, &constBuffer);
	pGraphics->GetDeviceContext()->GSSetConstantBuffers(Graphics::CONSTANT_BUFFER_CAMERA, 1, &constBuffer);

	pGraphics->UploadBuffer(tessBuffer, &shaderData, sizeof(shaderData));
	pGraphics->GetDeviceContext()->VSSetConstantBuffers(Graphics::CONSTANT_BUFFER_TESS, 1, &tessBuffer);
	pGraphics->GetDeviceContext()->HSSetConstantBuffers(Graphics::CONSTANT_BUFFER_TESS, 1, &tessBuffer);
	pGraphics->GetDeviceContext()->DSSetConstantBuffers(Graphics::CONSTANT_BUFFER_TESS, 1, &tessBuffer);
	pGraphics->GetDeviceContext()->PSSetConstantBuffers(Graphics::CONSTANT_BUFFER_TESS, 1, &tessBuffer);
}
void Camera::MoveForward()
{
	Position += Front * MovementSpeed;
}

void Camera::MoveBackward()
{
	Position -= Front * MovementSpeed;
}

void Camera::MoveLeft()
{
	Position -= Right * MovementSpeed;
}

void Camera::MoveRight()
{
	Position += Right * MovementSpeed;
}

void Camera::RotateYawUp()
{
	Yaw += 2.0f;
}

void Camera::RotateYawDown()
{
	Yaw -= 2.0f;
}

void Camera::RotatePitchUp()
{
	Pitch += 2.0f;
	if (Pitch >= 90.f) { Pitch = 90.f; }
}

void Camera::RotatePitchDown()
{
	Pitch -= 2.0f;
	if (Pitch <= -90.f) { Pitch = -90.f; }
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset) {
	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;

	Yaw += xoffset;
	Pitch += yoffset;

	//make sure screen doesn't flip
	if (Pitch > 89.0f) { Pitch = 89.0f; }
	if (Pitch < -89.0f) { Pitch = -89.0f; }

	updateCameraVectors();
}

void Camera::updateCameraVectors() {
	Matrix4 yawMat = Matrix4::CreateRotationY(Math::ToRadians(Yaw));
	Matrix4 pitchMat = Matrix4::CreateRotationX(Math::ToRadians(-Pitch));
	Matrix4 yawpitchMat = yawMat * pitchMat;
	//Matrix4 yawpitchMat = pitchMat * yawMat;
	Front = yawpitchMat.GetZAxis();
	Right = yawpitchMat.GetXAxis();
	Up = yawpitchMat.GetYAxis();
}
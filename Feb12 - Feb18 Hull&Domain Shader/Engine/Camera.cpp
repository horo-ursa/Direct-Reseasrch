#include "stdafx.h"
#include "Camera.h"
#include "Graphics.h"

Camera::Camera() : MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
	WorldUp = Vector3(0, 1, 0);
	Position = Vector3(-40, -827, 540.f);
	Yaw = YAW;
	Pitch = PITCH;
	updateCameraVectors();
	//worldToCameraMatrix = Matrix4::CreateTranslation(Vector3(0.0f, 0.0f, -100.f));
	pGraphics = Graphics::Get();
	/*projMatrix = Matrix4::CreateOrtho(
		pGraphics->GetScreenWidth(), pGraphics->GetScreenHeight(), 1000.0f, -1000.0f
	);*/
	projMatrix = Matrix4::CreatePerspectiveFOV(Math::ToRadians(70.0f),
		pGraphics->GetScreenWidth(), pGraphics->GetScreenHeight(),
		25.0f, 10000.0f);
	constBuffer = pGraphics->CreateGraphicsBuffer(&pcc, sizeof(pcc), D3D11_BIND_CONSTANT_BUFFER,
		D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
}

Camera::~Camera() {
	constBuffer->Release();
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

	pGraphics->UploadBuffer(constBuffer, &pcc, sizeof(pcc));
	pGraphics->GetDeviceContext()->VSSetConstantBuffers(
		Graphics::CONSTANT_BUFFER_CAMERA, 1, &constBuffer
	);
	pGraphics->GetDeviceContext()->HSSetConstantBuffers(
		Graphics::CONSTANT_BUFFER_CAMERA, 1, &constBuffer
	);
	pGraphics->GetDeviceContext()->DSSetConstantBuffers(
		Graphics::CONSTANT_BUFFER_CAMERA, 1, &constBuffer
	);
	pGraphics->GetDeviceContext()->PSSetConstantBuffers(
		Graphics::CONSTANT_BUFFER_CAMERA, 1, &constBuffer
	);
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
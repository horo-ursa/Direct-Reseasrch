//#include "stdafx.h"
//#include "Camera.h"
//#include "Graphics.h"
//
//Camera::Camera() {
//	worldToCameraMatrix = Matrix4::CreateTranslation(Vector3(0.0f, 0.0f, -500.f));
//	pGraphics = Graphics::Get();
//	projMatrix = Matrix4::CreateOrtho(
//		pGraphics->GetScreenWidth(), pGraphics->GetScreenHeight(), 1000.0f, -1000.0f
//	);
//	constBuffer = pGraphics->CreateGraphicsBuffer(&pcc, sizeof(pcc), D3D11_BIND_CONSTANT_BUFFER,
//		D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
//}
//
//Camera::~Camera() {
//	constBuffer->Release();
//}
//
//void Camera::SetActive() {
//	pcc.c_viewProj = worldToCameraMatrix * projMatrix;
//	Matrix4 invMatrix = worldToCameraMatrix;
//	invMatrix.Invert();
//	pcc.c_cameraPosition = invMatrix.GetTranslation();
//	pGraphics->UploadBuffer(constBuffer, &pcc, sizeof(pcc));
//	pGraphics->GetDeviceContext()->VSSetConstantBuffers(
//		Graphics::CONSTANT_BUFFER_CAMERA, 1, &constBuffer
//	);
//	pGraphics->GetDeviceContext()->GSSetConstantBuffers(
//		Graphics::CONSTANT_BUFFER_CAMERA, 1, &constBuffer
//	);
//}

#include "stdafx.h"
#include "Camera.h"
#include "Graphics.h"

Camera::Camera() : MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
	//x forward, y is to the right, z is up
	WorldUp = Vector3(0, 0, 1);
	Position = Vector3(0, 0, 100.f);
	Yaw = YAW;
	Pitch = PITCH;
	updateCameraVectors();
	pGraphics = Graphics::Get();
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
	pcc.c_cameraPosition = Position;
	Matrix4 cameraToWorld = Matrix4::CreateLookAt(Position, Position + Front, Up);
	cameraToWorld.Invert();
	worldToCameraMatrix = cameraToWorld;
	pcc.c_viewProj = worldToCameraMatrix * projMatrix;

	pGraphics->UploadBuffer(constBuffer, &pcc, sizeof(pcc));
	pGraphics->GetDeviceContext()->VSSetConstantBuffers(
		Graphics::CONSTANT_BUFFER_CAMERA, 1, &constBuffer
	);
	pGraphics->GetDeviceContext()->PSSetConstantBuffers(
		Graphics::CONSTANT_BUFFER_CAMERA, 1, &constBuffer
	);
}

void Camera::SetPositionAndRotation(Vector3 pos, Quaternion rot)
{
	//worldToCameraMatrix = Matrix4::CreateFromQuaternion(rot) * Matrix4::CreateTranslation(pos);
	Vector3 newPos = Vector3(-pos.x, -pos.y, -pos.z);
	worldToCameraMatrix = Matrix4::CreateFromQuaternion(rot) * Matrix4::CreateTranslation(newPos);
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
	Position += Left * MovementSpeed;
}

void Camera::MoveRight()
{
	Position -= Left * MovementSpeed;
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
	/*Vector3 front;*/
	/*front.x = cos(Math::ToRadians(Yaw)) * cos(Math::ToRadians(Pitch));
	front.y = sin(Math::ToRadians(Pitch));
	front.z = sin(Math::ToRadians(Yaw)) * cos(Math::ToRadians(Pitch));
	front.Normalize();
	Front = front;*/
	/*Front = Transform(Vector3::UnitX, yawpitchMat, 0.0f);
	Left = Cross(WorldUp, Front); Left.Normalize();
	Up = Cross(Front, Left); Up.Normalize();*/

	Matrix4 yawMat = Matrix4::CreateRotationZ(Math::ToRadians(Yaw));
	Matrix4 pitchMat = Matrix4::CreateRotationY(Math::ToRadians(-Pitch));
	Matrix4 yawpitchMat = pitchMat * yawMat;
	Front = yawpitchMat.GetXAxis();
	Left = -1.0 * yawpitchMat.GetYAxis();
	Up = yawpitchMat.GetZAxis();
}
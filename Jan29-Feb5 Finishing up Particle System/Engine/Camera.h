//#pragma once
//#include "engineMath.h"
//
//struct PerCameraConstants {
//	Matrix4 c_viewProj;
//	Vector3 c_cameraPosition;
//	float padding = 0.0f;
//};
//
//class Graphics;
//class Camera {
//public:
//    Camera();
//    virtual ~Camera();
//    //virtual void Update(float deltaTime);
//    void SetActive();
//    void SetPositionAndRotation(Vector3 pos, Quaternion rot);
//    void MoveForward();
//protected:
//    PerCameraConstants pcc;
//    ID3D11Buffer* constBuffer;
//    Matrix4 worldToCameraMatrix;
//    Matrix4 projMatrix;
//    Graphics* pGraphics;
//    Vector3 position;
//    float moveSpeed = 0.1f;
//    float zoom;
//};

#pragma once
#include "engineMath.h"
struct PerCameraConstants
{
    Matrix4 c_viewProj;
    Vector3 c_cameraPosition;
    float padding = 0.0f;
};

class Graphics;
class Camera {
public:
    Camera();
    virtual ~Camera();
    void SetActive();
    void MoveForward();
    void MoveBackward();
    void MoveLeft();
    void MoveRight();
    void ProcessMouseMovement(float xoffset, float yoffset);
    void SetPositionAndRotation(Vector3 pos, Quaternion rot);
    Matrix4 worldToCameraMatrix;
protected:
    PerCameraConstants pcc;
    ID3D11Buffer* constBuffer;
    Matrix4 projMatrix;
    Graphics* pGraphics;
    float speed;

    // Default camera values
    const float YAW = 0.0f;
    const float PITCH = 0.0F;
    const float SPEED = 2.5f;
    const float SENSITIVITY = 0.1f;
    const float ZOOM = 45.0f;

    //Attributes
    Vector3 Position;
    Vector3 Front;
    Vector3 Up;
    Vector3 Left;
    Vector3 WorldUp;
    //Euler Angles
    float Yaw;
    float Pitch;
    //Camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    void updateCameraVectors();
};
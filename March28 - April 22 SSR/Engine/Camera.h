#pragma once
#include "engineMath.h"
struct PerCameraConstants 
{
    Matrix4 c_lightSpaceViewProj;
    Matrix4 c_cameraSpaceViewProj;
    Matrix4 c_viewMatrix;
    Matrix4 c_projMatrix;
    Vector3 c_cameraPosition;
    float padding3 = 0.0f;
    Vector3 UpVector;
    float padding2 = 0.0f;
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
    void TransformToLightSpace();
    void TransformToCameraSpace();
    Matrix4 worldToCameraMatrix;
protected:
    PerCameraConstants pcc;
    ID3D11Buffer* constBuffer;
    Matrix4 projMatrix;
    Graphics* pGraphics;
    float speed;

    // Default camera values
    const float YAW = 0.0f;
    const float PITCH = -30.0f;
    const float SPEED = 2.5f;
    const float SENSITIVITY = 0.1f;
    const float ZOOM = 45.0f;

    //Attributes
    Vector3 Position;
    Vector3 Front;
    Vector3 Up;
    Vector3 Right;
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
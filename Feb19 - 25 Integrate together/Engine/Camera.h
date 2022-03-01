#pragma once
#include "engineMath.h"
struct PerCameraConstants 
{
    Matrix4 c_viewProj;
    Vector3 c_cameraPosition;
    float padding = 0.0f;
    Vector3 UpVector;
    float padding2 = 0.0f;
};

struct tess {
    float tess_factor;
    Vector3 padding;
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
    void RotateYawUp();
    void RotateYawDown();
    void RotatePitchUp();
    void RotatePitchDown();
    void ProcessMouseMovement(float xoffset, float yoffset);
    Matrix4 worldToCameraMatrix;

    tess shaderData;
protected:
    PerCameraConstants pcc;
    ID3D11Buffer* constBuffer;
    ID3D11Buffer* tessBuffer;
    Matrix4 projMatrix;
    Graphics* pGraphics;
    float speed;
    const Vector3 floorPos = Vector3(0, 0, 1000);

    // Default camera values
    const float YAW =  0.0f;
    const float PITCH = 60.0f;
    const float SPEED = 25.f;
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
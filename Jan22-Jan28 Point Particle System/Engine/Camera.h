#pragma once
#include "engineMath.h"

struct PerCameraConstants {
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
    void SetPositionAndRotation(Vector3 pos, Quaternion rot);
protected:
    PerCameraConstants pcc;
    ID3D11Buffer* constBuffer;
    Matrix4 worldToCameraMatrix;
    Matrix4 projMatrix;
    Graphics* pGraphics;
};
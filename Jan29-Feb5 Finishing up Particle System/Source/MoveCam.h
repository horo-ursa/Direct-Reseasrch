#pragma once
#include "Camera.h"

class MoveCam : public Camera
{
	MoveCam();
	~MoveCam();
	void Update(float deltaTime);
};
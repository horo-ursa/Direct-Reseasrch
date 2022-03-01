#pragma once
#include "engineMath.h"
#include "Graphics.h"

struct VertexPosColor
{
    Vector3 pos;
    Graphics::Color4 color;
};

struct VertexPos
{
    Vector3 pos;
};

struct VertexInput
{
    Vector3 pos;
    Vector3 normal;
    Vector3 tangent;
    Vector2 uv;
};

struct GeometryShaderInput
{
    Vector3 pos;
    float size;
};
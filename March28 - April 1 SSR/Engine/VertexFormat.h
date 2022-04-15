#pragma once
#include "engineMath.h"
#include "Graphics.h"

struct VertexPosColor
{
    Vector3 pos;
    Graphics::Color4 color;
};

struct ParticleVertex {
    Vector3 pos;
};

struct VertexPosTexture
{
    Vector3 pos;
    Graphics::Color4 color;
    Vector2 uv;
};

struct VertexWithNormal
{
    Vector3 pos;
    Vector3 normal;
    Graphics::Color4 color;
    Vector2 uv;
};

struct VertexWithoutColor
{
    Vector3 pos;
    Vector3 normal;
    Vector2 uv;
};

struct SkinningFormat
{
    Vector3 pos;
    Vector3 normal;
    char indice[4];
    char weight[4];
    Vector2 uv;
};

struct GeometryShaderInput {
    Vector3 pos;
};

struct VertexForGBuffer {
    Vector3 pos;
    Vector3 normal;
    Vector2 uv;
};

struct quadVertexBuffer {
    Vector3 pos;
    Vector2 uv;
};
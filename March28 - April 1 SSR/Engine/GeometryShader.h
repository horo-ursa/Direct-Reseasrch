#pragma once
#include "Shader.h"

class GeometryShader : public Shader
{
public:
    GeometryShader();
    ~GeometryShader();

    bool Load(const WCHAR* fileName, const D3D11_INPUT_ELEMENT_DESC* layoutArray, int numLayoutElements) override;
    void SetActive() const override;

private:
    ID3D11VertexShader* mShaderVS;
    ID3D11PixelShader* mShaderPS;
    ID3D11GeometryShader* mShaderGS;
    ID3D11InputLayout* mInputLayout;
};
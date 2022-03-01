#pragma once

class Shader
{
public:
    Shader();
    ~Shader();

    virtual bool Load(const WCHAR* fileName, const D3D11_INPUT_ELEMENT_DESC* layoutArray, int numLayoutElements);
    virtual void SetActive() const;

private:
    ID3D11VertexShader* mShaderVS;
    ID3D11PixelShader* mShaderPS;
    ID3D11InputLayout* mInputLayout;
    bool haveGS;
};
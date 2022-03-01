#pragma once

class HullShader
{
public:
    HullShader();
    ~HullShader();

    bool Load(const WCHAR* fileName);
    void SetActive() const;

private:
    ID3D11HullShader* mShaderHS;
};
#include "stdafx.h"
#include "DomainShader.h"
#include "Graphics.h"
#include <d3dcompiler.h>

DomainShader::DomainShader():mShaderDS(nullptr)
{
}

DomainShader::~DomainShader()
{
    mShaderDS->Release();
}

static bool LoadShader(const WCHAR* filename, const char* entryPoint, const char* model, ID3DBlob*& pBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;

    // Disable optimizations to further improve shader debugging
    dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ID3DBlob* pErrorBlob = nullptr;
    hr = D3DCompileFromFile(filename, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint, model,
        dwShaderFlags, 0, &pBlobOut, &pErrorBlob);
    if (FAILED(hr))
    {
        if (pErrorBlob)
        {
            static wchar_t szBuffer[4096];
            _snwprintf_s(szBuffer, 4096, _TRUNCATE,
                L"%hs",
                (char*)pErrorBlob->GetBufferPointer());
            OutputDebugString(szBuffer);
#ifdef _WINDOWS
            MessageBox(nullptr, szBuffer, L"Error", MB_OK);
#endif
            pErrorBlob->Release();
        }
        return false;
    }
    if (pErrorBlob)
    {
        pErrorBlob->Release();
    }

    return true;
}

bool DomainShader::Load(const WCHAR* fileName)
{

    ID3DBlob* pBlobDS = nullptr;
    if (LoadShader(fileName, "DS", "ds_5_0", pBlobDS))
    {
        Graphics* pGraphics = Graphics::Get();
        HRESULT hr = pGraphics->GetDevice()->CreateDomainShader(pBlobDS->GetBufferPointer(), pBlobDS->GetBufferSize(), nullptr, &mShaderDS);
        DbgAssert(hr == S_OK, "Failed to create hull shader");
        if (nullptr != mShaderDS){ 
            return true;
        }
        return false;
    }
    DbgAssert(false, "Failed to load shader");
    return false;
}

void DomainShader::SetActive() const
{
    Graphics* pGraphics = Graphics::Get();
    pGraphics->GetDeviceContext()->DSSetShader(mShaderDS, nullptr, 0);

}
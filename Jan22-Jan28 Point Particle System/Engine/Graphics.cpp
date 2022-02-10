#include "stdafx.h"
#include "Graphics.h"
#include "Shader.h"
#include "engineMath.h"

#pragma comment (lib, "d3d11.lib") 

/*static*/ Graphics* Graphics::s_theGraphics = nullptr;


Graphics::Graphics()
    : mScreenWidth(0)
    , mScreenHeight(0)
    , mSwapChain(nullptr)
    , mDev(nullptr)
    , mDevCon(nullptr)
    , mBackBuffer(nullptr)
    , mCurrentRenderTarget(nullptr)
{
    DbgAssert(nullptr == s_theGraphics, "You can only have 1 Graphics");
    s_theGraphics = this;
}

Graphics::~Graphics()
{
    DbgAssert(this == s_theGraphics, "There can only be 1 Graphics");
    s_theGraphics = nullptr;
}

void Graphics::InitD3D(HWND hWnd, float width, float height)
{
    mScreenWidth = width;
    mScreenHeight = height;

    HRESULT hr = S_OK;

    { // set up the swap chain
        DXGI_SWAP_CHAIN_DESC sd;
        ZeroMemory(&sd, sizeof(sd));
        sd.BufferCount = 2;
        sd.BufferDesc.Width = static_cast<UINT>(width);
        sd.BufferDesc.Height = static_cast<UINT>(height);
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = hWnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;
        sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        D3D_FEATURE_LEVEL  FeatureLevelsRequested = D3D_FEATURE_LEVEL_11_0;
        UINT               numLevelsRequested = 1;
        D3D_FEATURE_LEVEL  FeatureLevelsSupported;
        hr = D3D11CreateDeviceAndSwapChain(NULL,
            D3D_DRIVER_TYPE_HARDWARE,
            NULL,
#ifdef _DEBUG
            D3D11_CREATE_DEVICE_DEBUG,
#else
            0,
#endif
            & FeatureLevelsRequested,
            numLevelsRequested,
            D3D11_SDK_VERSION,
            &sd,
            &mSwapChain,
            &mDev,
            &FeatureLevelsSupported,
            &mDevCon);
        DbgAssert(hr == S_OK, "Failed to create device");
    }

    { // set up the viewport (full screen)
        SetViewport(0.0f, 0.0f, width, height);
    }

    { // grab the back-buffer as our render target
        ID3D11Texture2D* pBackBuffer;
        hr = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
        DbgAssert(hr == S_OK, "Something wrong with your back buffer");
        hr = mDev->CreateRenderTargetView(pBackBuffer, nullptr, &mBackBuffer);
        DbgAssert(hr == S_OK, "Something went wrong with your back buffer");
        pBackBuffer->Release();
    }

    // we'll be drwaing triangle lists
    mDevCon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Graphics::CleanD3D()
{
#ifdef _DEBUG
    ID3D11Debug* pDbg = nullptr;
    HRESULT hr = mDev->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&pDbg));
    DbgAssert(S_OK == hr, "Unable to create debug device");
#endif

    mSwapChain->Release();
    mDev->Release();
    mDevCon->Release();

    mBackBuffer->Release();

#ifdef _DEBUG
    pDbg->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL | D3D11_RLDO_IGNORE_INTERNAL);
    pDbg->Release();
#endif
}

void Graphics::SetRenderTarget(ID3D11RenderTargetView* renderTarget)
{
    mCurrentRenderTarget = renderTarget;
    mDevCon->OMSetRenderTargets(1, &mCurrentRenderTarget, nullptr);
}

void Graphics::ClearRenderTarget(const Color4& clearColor)
{
    mDevCon->ClearRenderTargetView(mCurrentRenderTarget, reinterpret_cast<const float*>(&clearColor));
}

void Graphics::EndFrame()
{
    mSwapChain->Present(1, 0);
}

ID3D11Buffer* Graphics::CreateGraphicsBuffer(const void* initialData, int inDataSize, D3D11_BIND_FLAG inBindFlags, D3D11_CPU_ACCESS_FLAG inCPUAccessFlags, D3D11_USAGE inUsage)
{
    ID3D11Buffer* buffer = nullptr;
    D3D11_BUFFER_DESC desc;
    ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
    desc.Usage = inUsage;
    desc.ByteWidth = inDataSize;
    desc.BindFlags = inBindFlags;
    desc.CPUAccessFlags = inCPUAccessFlags;
    HRESULT hr = GetDevice()->CreateBuffer(&desc, nullptr, &buffer);
    DbgAssert(hr == S_OK, "Unable to create graphics buffer");

    if (nullptr != initialData)
        UploadBuffer(buffer, initialData, inDataSize);

    return buffer;
}

void Graphics::UploadBuffer(ID3D11Buffer* buffer, const void* data, size_t dataSize)
{
    D3D11_MAPPED_SUBRESOURCE map;
    HRESULT hr = GetDeviceContext()->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
    DbgAssert(hr == S_OK, "Map failed");
    memcpy(map.pData, data, dataSize);
    GetDeviceContext()->Unmap(buffer, 0);
}

void Graphics::SetViewport(float x, float y, float width, float height)
{
    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
    viewport.TopLeftX = x;
    viewport.TopLeftY = y;
    viewport.Width = width;
    viewport.Height = height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    mDevCon->RSSetViewports(1, &viewport);
}
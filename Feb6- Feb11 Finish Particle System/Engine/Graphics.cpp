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

    //setup state and create depthstencil
    state = CreateDepthStencilState(D3D11_COMPARISON_LESS);
    mDevCon->OMSetDepthStencilState(state, 0);
    CreateDepthStencil(width, height, &texture, &DepthView);
    
    //setup SamplerState
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    mDev->CreateSamplerState(&sampDesc, &SamplerState);

    
    //setup RasterizerState
    ID3D11RasterizerState* RasterizerState;
    D3D11_RASTERIZER_DESC rastDesc;
    ZeroMemory(&rastDesc, sizeof(rastDesc));
    rastDesc.FillMode = D3D11_FILL_SOLID;
    rastDesc.CullMode = D3D11_CULL_BACK;
    rastDesc.FrontCounterClockwise = true;
    rastDesc.SlopeScaledDepthBias = 0.0f;
    rastDesc.DepthBiasClamp = 0.0f;
    rastDesc.DepthClipEnable = true;
    rastDesc.ScissorEnable = false;
    rastDesc.MultisampleEnable = false;
    rastDesc.AntialiasedLineEnable = false;
    mDev->CreateRasterizerState(&rastDesc, &RasterizerState);
    mDevCon->RSSetState(RasterizerState);
    RasterizerState->Release();
    

    //set active
    SetActiveSampler(0, SamplerState);
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

    state->Release();
    texture->Release();
    DepthView->Release();
    SamplerState->Release();

#ifdef _DEBUG
    pDbg->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL | D3D11_RLDO_IGNORE_INTERNAL);
    pDbg->Release();
#endif
}

void Graphics::SetRenderTarget(ID3D11RenderTargetView* renderTarget, ID3D11DepthStencilView* view)
{
    mCurrentRenderTarget = renderTarget;
    mDevCon->OMSetRenderTargets(1, &mCurrentRenderTarget, view);
}

void Graphics::ClearRenderTarget(const Color4 &clearColor)
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

ID3D11DepthStencilState* Graphics::CreateDepthStencilState(D3D11_COMPARISON_FUNC inDepthComparisonFunction)
{
    D3D11_DEPTH_STENCIL_DESC dsDesc;
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = inDepthComparisonFunction;
    
    // Stencil test parameters
    dsDesc.StencilEnable = false;
    dsDesc.StencilReadMask = 0xFF;
    dsDesc.StencilWriteMask = 0xFF;

    // Stencil operations if pixel is front-facing
    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Stencil operations if pixel is back-facing
    dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    ID3D11DepthStencilState* pState;
    mDev->CreateDepthStencilState(&dsDesc, &pState);
    return pState;
}

bool Graphics::CreateDepthStencil(int inWidth, int inHeight,
    ID3D11Texture2D** pDepthTexture, ID3D11DepthStencilView** pDepthView)
{

    D3D11_TEXTURE2D_DESC descDepth;
    ZeroMemory(&descDepth, sizeof(D3D11_TEXTURE2D_DESC));
    descDepth.Width = inWidth;
    descDepth.Height = inHeight;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    //not sure for these NULLs
    mDev->CreateTexture2D(&descDepth, NULL, pDepthTexture);

    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory(&descDSV, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
    descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    mDev->CreateDepthStencilView(*pDepthTexture, &descDSV, pDepthView);
    return true;
}

void Graphics::ClearDepthBuffer(ID3D11DepthStencilView* depthView, float depth)
{
    //guess here for the 4th parameter
    mDevCon->ClearDepthStencilView(depthView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, 0);
}

void Graphics::SetActiveTexture(int slot, ID3D11ShaderResourceView* pView)
{
    //assume 1 for NumViews
    mDevCon->PSSetShaderResources(slot, 1, &pView);
}

void Graphics::SetActiveSampler(int slot, ID3D11SamplerState* pSampler)
{
    //assume 1 again
    mDevCon->PSSetSamplers(slot, 1, &pSampler);
}
#pragma once

#pragma warning(disable : 6387)

class Graphics
{
public:
    enum TextureSlot
    {
        TEXTURE_SLOT_WALL,
        TEXTURE_SLOT_WALLNORMAL,
        TEXTURE_SLOT_WALLHEIGHT,
        TEXTURE_SLOT_PARTICLE
    };

    enum ConstantBuffer
    {
        CONSTANT_BUFFER_CAMERA,
        CONSTANT_BUFFER_RENDEROBJ,
        CONSTANT_BUFFER_TESS,
        CONSTANT_VERTEX_ANIMATION
    };

    enum SamplerSlot
    {
        SAMPLER_SLOT_WALL,
        SAMPLER_SLOT_WALLNORMAL,
        SAMPLER_SLOT_WALLHEIGHT,
        SAMPLER_SLOT_PARTICLE
    };

    class Color4
    {
    public:
        Color4()
            : r(0), g(0), b(0), a(0)
        {}
        Color4(float _r, float _g, float _b, float _a = 1.0f)
            : r(_r), g(_g), b(_b), a(_a)
        {}
        float r, g, b, a;
    };

    Graphics();
    ~Graphics();

    static Graphics* Get() { return s_theGraphics; }

    void InitD3D(HWND hWnd, float width, float height);
    void CleanD3D();
    ID3D11RenderTargetView* GetBackBuffer() { return mBackBuffer; }

    void SetRenderTarget(ID3D11RenderTargetView* renderTarget, ID3D11DepthStencilView* view);
    void ClearRenderTarget(const Color4& clearColor);
    void EndFrame();

    float GetScreenWidth() const { return mScreenWidth; }
    float GetScreenHeight() const { return mScreenHeight; }

    ID3D11Device* GetDevice() { return mDev; }
    ID3D11DeviceContext* GetDeviceContext() { return mDevCon; }

    ID3D11Buffer* CreateGraphicsBuffer(const void* initialData, int inDataSize, D3D11_BIND_FLAG inBindFlags, D3D11_CPU_ACCESS_FLAG inCPUAccessFlags, D3D11_USAGE inUsage);
    void UploadBuffer(ID3D11Buffer* buffer, const void* data, size_t dataSize);

    void SetViewport(float x, float y, float width, float height);

    ID3D11DepthStencilState* CreateDepthStencilState(D3D11_COMPARISON_FUNC inDepthComparisonFunction);
    bool CreateDepthStencil(int inWidth, int inHeight, ID3D11Texture2D** pDepthTexture, ID3D11DepthStencilView** pDepthView);
    ID3D11DepthStencilView* GetView() { return DepthView; }
    void ClearDepthBuffer(ID3D11DepthStencilView* depthView, float depth);
    void PSSetActiveTexture(int slot, ID3D11ShaderResourceView* pView);
    void PSSetActiveSampler(int slot, ID3D11SamplerState* pSampler);
    void DSSetActiveTexture(int slot, ID3D11ShaderResourceView* pView);
    void DSSetActiveSampler(int slot, ID3D11SamplerState* pSampler);

    ID3D11SamplerState* CreateSamplerState();
    void SetupRasterizerState(D3D11_FILL_MODE mode);

    ID3D11BlendState* CreateBlendState(bool enable, D3D11_BLEND srcBlend, D3D11_BLEND dstBlend);
    void SetBlendState(ID3D11BlendState* inBlendState);
    void BeginAlpha();
    void BeginFrame(const Color4& clearColor);
private:
    static Graphics* s_theGraphics;
    float mScreenWidth;
    float mScreenHeight;

    IDXGISwapChain* mSwapChain;
    ID3D11Device* mDev;
    ID3D11DeviceContext* mDevCon;

    ID3D11RenderTargetView* mBackBuffer;
    ID3D11RenderTargetView* mCurrentRenderTarget;

    ID3D11DepthStencilState* dsState;
    ID3D11DepthStencilState* mDepthAlphaState;

    ID3D11Texture2D* texture;
    ID3D11DepthStencilView* DepthView;
    //ID3D11SamplerState* SamplerState;
};



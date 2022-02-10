#pragma once

#pragma warning(disable : 6387)

class Graphics
{
public:
    enum TextureSlot
    {
        TEXTURE_SLOT_DIFFUSE,

		TEXTURE_SLOT_TOTAL
    };

	enum ConstantBuffer
	{
		CONSTANT_BUFFER_CAMERA,
		CONSTANT_BUFFER_RENDEROBJ,
        CONSTANT_VERTEX_ANIMATION
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

    void SetRenderTarget(ID3D11RenderTargetView* renderTarget);
    void ClearRenderTarget(const Color4 &clearColor);
    void EndFrame();

	float GetScreenWidth() const { return mScreenWidth; }
	float GetScreenHeight() const { return mScreenHeight; }

    ID3D11Device* GetDevice() { return mDev; }
    ID3D11DeviceContext* GetDeviceContext() { return mDevCon; }

    ID3D11Buffer* CreateGraphicsBuffer(const void* initialData, int inDataSize, D3D11_BIND_FLAG inBindFlags, D3D11_CPU_ACCESS_FLAG inCPUAccessFlags, D3D11_USAGE inUsage);
    void UploadBuffer(ID3D11Buffer* buffer, const void* data, size_t dataSize);

    void SetViewport(float x, float y, float width, float height);

private:
    static Graphics* s_theGraphics;
    float mScreenWidth;
    float mScreenHeight;

    IDXGISwapChain* mSwapChain;
    ID3D11Device* mDev;
    ID3D11DeviceContext* mDevCon;

    ID3D11RenderTargetView* mBackBuffer;
    ID3D11RenderTargetView* mCurrentRenderTarget;
};
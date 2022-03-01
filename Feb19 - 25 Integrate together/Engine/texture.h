#pragma once

class AssetManager;
class Texture
{
public:
    Texture();
    ~Texture();
    
    void Free();
    bool Load(const WCHAR* fileName);
    static Texture* StaticLoad(const WCHAR* fileName, AssetManager* pManager);
    int GetWidth() const { return mWidth; }
    int GetHeight() const { return mHeight; }
    void PSSetActive(int slot) const;
    void DSSetActive(int slot) const;

private:
    ID3D11Resource *mResource;
    ID3D11ShaderResourceView *mView;
    int mWidth;
    int mHeight;
};
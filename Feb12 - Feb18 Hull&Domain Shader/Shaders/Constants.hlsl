// We want to use row major matrices
#pragma pack_matrix(row_major)

cbuffer PerCameraConstants : register(b0)
{
    float4x4 c_viewProj;
    float3 c_cameraPosition;
};

cbuffer PerObjectConstants : register(b1)
{
    float4x4 c_modelToWorld;
};

cbuffer TessFactor : register(b2)
{
    float tess_factor;
}

SamplerState WallSampler : register(s0);

SamplerState WallNormalSampler : register(s1);

SamplerState WallHeightSampler : register(s2);

Texture2D WallTexture : register(t0);

Texture2D WallNormalTexture : register(t1);

Texture2D WallHeightTexture : register(t2);
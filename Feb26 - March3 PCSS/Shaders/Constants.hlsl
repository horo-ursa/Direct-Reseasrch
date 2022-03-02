// We want to use row major matrices
#pragma pack_matrix(row_major)

cbuffer PerCameraConstants : register(b0)
{
    float4x4 c_viewProj;
    float4 Right;
    float4 Up;
    float4x4 c_view;
    float4x4 c_proj;
    float3 c_cameraPosition;
};

cbuffer PerObjectConstants : register(b1)
{
    float4x4 c_modelToWorld;
};

SamplerState DefaultSampler : register(s0);


Texture2D DiffuseTexture : register(t0);

Texture2D SnowTexture : register(t1);

cbuffer MaterialConstants : register(b2)
{
    float3 c_diffuseColor;
    float3 c_specularColor;
    float c_specularPower;
};

#define MAX_POINT_LIGHTS 8
struct PointLightData
{
    float3 lightColor;
    float3 position;
    float innerRadius;
    float outerRadius;
    bool isEnabled;
};

cbuffer PointLightConstants : register(b3)
{
    float3 c_ambient;
    PointLightData c_pointLight[MAX_POINT_LIGHTS];
};

#define MAX_SKELETON_BONES 80 
cbuffer SkinConstants : register(b4)
{
    float4x4 c_skinMatrix[MAX_SKELETON_BONES];
};
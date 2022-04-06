#include "Constants.hlsl"

struct VertexInput
{
    float3 position : POSITION0;
    float3 normal   : NORMAL0;
    float2 uv       : TEXCOORD0;
};

struct VertexToPixel
{
    float4 position        : SV_POSITION;
    float4 normal          : NORMAL0;
    float2 uv              : TEXCOORD0;
    float4 worldPosition   : POSITION1;
};

struct GPassOut {
    float4 Position : SV_Target0;
    float4 Albedo   : SV_Target1;
    float4 Normal   : SV_Target2;
};

VertexToPixel VS(in VertexInput input) {
    VertexToPixel output;
    output.worldPosition = mul(float4(input.position, 1.0), c_modelToWorld);
    output.position = mul(output.worldPosition, c_cameraSpaceViewProj);
    output.normal = mul(float4(input.normal, 0.0), c_modelToWorld);
    output.uv = input.uv;
    return output;
}

GPassOut PS(in VertexToPixel input)
{
    GPassOut output;
    float3 albedo = DiffuseTexture.Sample(DefaultSampler, input.uv).rgb;
    output.Albedo = float4(albedo, 1.0f);
    output.Position = input.worldPosition;
    output.Normal = input.normal;
    return output;
}
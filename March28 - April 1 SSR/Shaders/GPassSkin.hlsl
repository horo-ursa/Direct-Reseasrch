#include "Constants.hlsl"

struct VertexInput
{
    float3 position : POSITION0;
    float3 normal : NORMAL0;
    uint4 boneIndices : BLENDINDICES0;
    float4 boneWeight : BLENDWEIGHT0;
    float2 uv : TEXCOORD0;
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

    float4x4 transformMatric =
        c_skinMatrix[input.boneIndices.x] * input.boneWeight.x +
        c_skinMatrix[input.boneIndices.y] * input.boneWeight.y +
        c_skinMatrix[input.boneIndices.z] * input.boneWeight.z +
        c_skinMatrix[input.boneIndices.w] * input.boneWeight.w;

    output.position = mul(float4(input.position, 1.0), transformMatric);
    output.worldPosition = mul(output.position, c_modelToWorld);
    output.position = mul(output.worldPosition, c_cameraSpaceViewProj);

    output.normal = mul(float4(input.normal, 0.0), transformMatric);
    output.normal = mul(output.normal, c_modelToWorld);
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
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
    float4 Depth    : SV_Target3;
    float4 Shadow   : SV_Target4;
};

float SimpleShadowMap(float4 posWorld, float bias) {
    float4 posLight = mul(posWorld, c_lightSpaceViewProj);
    float3 projCoords = posLight.xyz / posLight.w;
    projCoords.xy = projCoords.xy * 0.5 + 0.5;
    projCoords.y = 1 - projCoords.y;
    float cloestDepth = DepthMap.Sample(DefaultSampler, projCoords.xy).r + bias;
    float currentDepth = projCoords.z;
    float inShadow = currentDepth > cloestDepth ? 0.0 : 1.0;
    return inShadow;
}

VertexToPixel VS(in VertexInput input) {
    VertexToPixel output;
    output.worldPosition = mul(float4(input.position, 1.0), c_modelToWorld);
    output.worldPosition = output.worldPosition.xyzw / output.worldPosition.w;
    output.position = mul(output.worldPosition, c_cameraSpaceViewProj);
    output.normal = normalize(mul(float4(input.normal, 0.0), c_modelToWorld));
    output.uv = input.uv;
    return output;
}

GPassOut PS(in VertexToPixel input)
{
    GPassOut output;
    output.Albedo = float4(DiffuseTexture.Sample(DefaultSampler, input.uv).rgb, 1.0);
    output.Position = float4(input.worldPosition.xyz, 1.0);
    output.Normal = float4(normalize(input.normal.xyz), 1.0);                   // DIFFERENT HERE
    float depth = input.position.z;
    float camtopoint = distance(c_cameraPosition, input.worldPosition.xyz);
    output.Depth = float4(depth, camtopoint, depth, 1.0);                            // SEEMS WRONG HERE
    float inshadow = SimpleShadowMap(input.worldPosition, 0.001);
    output.Shadow = float4(inshadow, inshadow, inshadow, 1.0);

    return output;
}

//float4 PS(in VertexToPixel input) : SV_TARGET
//{
//    float depth = input.position.z;
//    return float4(depth, depth, depth, 1.0);
//}
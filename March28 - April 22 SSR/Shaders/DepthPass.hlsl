#include "Constants.hlsl"

struct VIn
{
    float3 position : POSITION0;
    float3 normal : NORMAL0;
    float2 uv : TEXCOORD0;
};

struct VOut
{
    float4 position : SV_POSITION;
    float4 normal : NORMAL0;
    float2 uv : TEXCOORD0;
    float4 worldPosition : POSITION1;
};

VOut VS(VIn vIn)
{
    VOut output;

    output.worldPosition = mul(float4(vIn.position, 1.0), c_modelToWorld);
    output.position = mul(output.worldPosition, c_lightSpaceViewProj);
    output.uv = vIn.uv;
    output.normal = normalize(mul(float4(vIn.normal, 0.0), c_modelToWorld));
    return output;
}

float4 PS(VOut pIn) : SV_TARGET
{
    float4 position = mul(pIn.worldPosition, c_lightSpaceViewProj);
    float depth = position.z / position.w;
    //float depth = position.w;
    return float4(depth, depth, depth, 1.0);
}
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
    float2 uv : TEXCOORD0;
    float4 normal : NORMAL0;
    float4 worldPosition : POSITION1;
};

VOut VS(VIn vIn)
{
    VOut output;

    output.position = mul(float4(vIn.position, 1.0), c_modelToWorld);
    output.position = mul(output.position, c_viewProj);
    output.uv = vIn.uv;
    output.worldPosition = mul(float4(vIn.position, 1.0), c_modelToWorld);

    output.normal = mul(float4(vIn.normal, 0.0), c_modelToWorld);
    output.normal = normalize(output.normal);
    return output;
}

float4 PS(VOut pIn) : SV_TARGET
{
    float4 diffuse = DiffuseTexture.Sample(DefaultSampler, pIn.uv);
   
    return diffuse;
}

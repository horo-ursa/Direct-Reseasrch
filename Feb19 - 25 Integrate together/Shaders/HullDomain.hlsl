#include "Constants.hlsl"

struct VIn {
    float3 position : POSITION;
};

// Input control point
struct VS_CONTROL_POINT_OUTPUT
{
    float4 worldPos		: POSITION;
};


// Output control point
struct HULL_OUT
{
    float4 worldPos		: POSITION;
};

struct HS_CONSTANT_DATA_OUTPUT
{
    float EdgeTessFactor[3]      : SV_TessFactor; // e.g. would be [4] for a quad domain
    float InsideTessFactor       : SV_InsideTessFactor; // e.g. would be Inside[2] for a quad domain
};

struct DomainOut
{
    float4 vPosition	: SV_POSITION;
    float4 worldPos		: POSITION;
};

//--------------------------Vertex Shader
VS_CONTROL_POINT_OUTPUT VS(VIn vIn) {
    VS_CONTROL_POINT_OUTPUT output;
    output.worldPos = mul(float4(vIn.position, 1.0f), c_modelToWorld);
    return output;
}

//--------------------------Hull Shader
#define MAX_POINTS 32
// Patch Constant Function
HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(
    InputPatch<VS_CONTROL_POINT_OUTPUT, MAX_POINTS> ip,
    uint PatchID : SV_PrimitiveID)
{
    HS_CONSTANT_DATA_OUTPUT Output;
    Output.EdgeTessFactor[0] = tess_factor;
    Output.EdgeTessFactor[1] = tess_factor;
    Output.EdgeTessFactor[2] = tess_factor;
    Output.InsideTessFactor = tess_factor;
    return Output;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("CalcHSPatchConstants")]
HULL_OUT HS(
    InputPatch<VS_CONTROL_POINT_OUTPUT, MAX_POINTS> ip,
    uint i : SV_OutputControlPointID,
    uint PatchID : SV_PrimitiveID)
{
    HULL_OUT Output;
    Output.worldPos = ip[i].worldPos;
    return Output;
}

//--------------------------Domain Shader
#define NUM_CONTROL_POINTS 3
[domain("tri")]
DomainOut DS(
    HS_CONSTANT_DATA_OUTPUT input,
    float3 domain : SV_DomainLocation, // this is the barycentric coordinate
    const OutputPatch<HULL_OUT, NUM_CONTROL_POINTS> patch)
{
    DomainOut output;
    output.worldPos = domain.x * patch[0].worldPos + domain.y * patch[1].worldPos + domain.z * patch[2].worldPos;
    output.vPosition = mul(output.worldPos, c_viewProj);
    return output;
}

//--------------------------Pixel Shader
float4 PS(DomainOut vIn) : SV_TARGET
{
    return float4(1.0, 1.0, 1.0, 1.0);
}
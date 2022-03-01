#include "Constants.hlsl"

struct VertexIn
{
    float3 position	: POSITION;
    float3 normal   : NORMAL;
    float3 tangent  : TANGENT;
    float2 uv		: TEXCOORD;
};

struct VertexOut
{
    float4 worldPos		: POSITION;
    float4 normal		: NORMAL;
    float4 tangent		: TANGENT;
    float2 uv			: TEXCOORD;
};


// Output control point
struct HULL_OUT
{
    float4 worldPos		: POSITION;
    float4 normal		: NORMAL;
    float4 tangent		: TANGENT;
    float2 uv			: TEXCOORD;
};

struct HS_CONSTANT_DATA_OUTPUT
{
    float EdgeTessFactor[3]      : SV_TessFactor; // e.g. would be [4] for a quad domain
    float InsideTessFactor : SV_InsideTessFactor; // e.g. would be Inside[2] for a quad domain
};

struct DomainOut
{
    float4 vPosition	: SV_POSITION;
    float4 normal		: NORMAL;
    float4 tangent		: TANGENT;
    float4 worldPos		: POSITION;
    float2 uv			: TEXCOORD;
};

//--------------------------Vertex Shader
VertexOut VS(VertexIn vIn) {
    VertexOut output;
    output.worldPos = mul(float4(vIn.position, 1.0f), c_modelToWorld);
    output.normal = mul(float4(vIn.normal, 1.0f), c_modelToWorld);
    output.tangent = mul(float4(vIn.tangent, 1.0f), c_modelToWorld);
    output.uv = vIn.uv;
    return output;
}

//--------------------------Hull Shader
#define MAX_POINTS 3
// Patch Constant Function
HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(
    InputPatch<VertexOut, MAX_POINTS> ip,
    uint PatchID : SV_PrimitiveID)
{
    HS_CONSTANT_DATA_OUTPUT Output;
    Output.EdgeTessFactor[0] = tess_factor;
    Output.EdgeTessFactor[1] = tess_factor;
    Output.EdgeTessFactor[2] = tess_factor;
    Output.InsideTessFactor= tess_factor;
    return Output;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("CalcHSPatchConstants")]
HULL_OUT HS(
    InputPatch<VertexOut, MAX_POINTS> ip,
    uint i : SV_OutputControlPointID,
    uint PatchID : SV_PrimitiveID)
{
    HULL_OUT Output;
    Output.worldPos = ip[i].worldPos;
    Output.normal = ip[i].normal;
    Output.tangent = ip[i].tangent;
    Output.uv = ip[i].uv;
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
    DomainOut Output;
    Output.normal = domain.x * patch[0].normal + domain.y * patch[1].normal + domain.z * patch[2].normal;
    Output.tangent = domain.x * patch[0].tangent + domain.y * patch[1].tangent + domain.z * patch[2].tangent;
    Output.worldPos = domain.x * patch[0].worldPos + domain.y * patch[1].worldPos + domain.z * patch[2].worldPos;
    Output.uv = domain.x * patch[0].uv + domain.y * patch[1].uv + domain.z * patch[2].uv;
    
    Output.normal = normalize(Output.normal);

    float hScale = 0.5f;
    float hBias = 0.0f;
    float4 vDir = Output.normal;
    float hMap = WallHeightTexture.SampleLevel(WallHeightSampler, Output.uv.xy, 0).r;
    hMap *= hScale;
    hMap += hBias;
    Output.worldPos += hMap * vDir;
    Output.vPosition = mul(Output.worldPos, c_viewProj);
    return Output;
}

//--------------------------Pixel Shader
float4 PS(DomainOut input) : SV_TARGET
{
    float4 textureColor = WallTexture.Sample(WallSampler, input.uv);
    //return float4(1.0, 1.0, 1.0, 1.0);
    return textureColor;
}
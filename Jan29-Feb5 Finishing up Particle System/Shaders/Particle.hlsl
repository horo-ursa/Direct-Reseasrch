#include "Constants.hlsl"
struct VIn
{
    float3 position : POSITION0;
};

struct v2g
{
    float4 position : SV_POSITION;
};

struct g2p
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

v2g VS(VIn vIn)
{
    v2g output;
    //output.worldPosition = mul(float4(vIn.position, 1.0), c_modelToWorld);
    //output.position = mul(output.worldPosition, c_viewProj);
    output.position = mul(float4(vIn.position, 1.0), c_modelToWorld);
    return output;
}


[maxvertexcount(10)]
void GS(point v2g InData[1], inout TriangleStream<g2p> triStream)
{
    //create new vertices
    g2p newPoints[4];
   /* newPoints[0].worldPos = InData[0].worldPosition - float4(20, -20, 0.f, 0.f);
    newPoints[0].uv = float2(0, 0);
    newPoints[1].worldPos = InData[0].worldPosition - float4(-20, -20, 0.f, 0.f);
    newPoints[1].uv = float2(1, 0);
    newPoints[2].worldPos = InData[0].worldPosition - float4(20, 20, 0.f, 0.f);
    newPoints[2].uv = float2(0, 1);
    newPoints[3].worldPos = InData[0].worldPosition - float4(-20, 20, 0.f, 0.f);
    newPoints[3].uv = float2(1, 1);*/

    //float disToCamera = distance(c_cameraPosition, InData.worldPosition);

    newPoints[0].position = InData[0].position - float4(80, -50, 0.f, 0.f);
    newPoints[0].uv = float2(0, 0);
    newPoints[1].position = InData[0].position - float4(-50, -50, 0.f, 0.f);
    newPoints[1].uv = float2(1, 0);
    newPoints[2].position = InData[0].position - float4(50, 50, 0.f, 0.f);
    newPoints[2].uv = float2(0, 1);
    newPoints[3].position = InData[0].position - float4(-50, 50, 0.f, 0.f);
    newPoints[3].uv = float2(1, 1);

    for (int j = 0; j < 4; j++) {
        newPoints[j].position = mul(newPoints[j].position, c_viewProj);
    }

    for (int i = 0; i < 4; i++) {
        triStream.Append(newPoints[i]);
    }
}


float4 PS(g2p pIn) : SV_TARGET
{
    float4 color = SnowTexture.Sample(DefaultSampler, pIn.uv);
    if (color.x == 1.f && color.y == 1.f && color.z == 1.f) {
        color = float4(0.0f, 0.2f, 0.4f, 1.0f);
    }
    return color;
}

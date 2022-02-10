#include "Constants.hlsl"
struct VIn
{
    float4 position : POSITION0;
    float4 color : COLOR0;
};

struct v2g
{
    float4 position : SV_POSITION;
    float4 color : COLOR0;
};

struct g2p
{
    float4 position : SV_POSITION;
    float4 color : COLOR0;
    float2 uv : TEXCOORD0;
};

v2g VS(VIn vIn)
{
    v2g output;
    float delta = sin(time) / 5;
    float beta = cos(time) / 5;
    output.position = vIn.position + float4(delta, beta, 0, 0);
    output.color = vIn.color + float4(sin(time), cos(time), 0.f, 0.f);
    return output;
}


[maxvertexcount(10)]
void GS(point v2g InData[1], inout TriangleStream<g2p> triStream)
{
    //create new vertices
    v2g newPoints[6];
    newPoints[0].position = InData[0].position - float4(0.012f, -0.012f, 0.f, 0.f);
    newPoints[0].uv = float2(0, 0);
    newPoints[1].position = InData[0].position - float4(-0.012f, -0.012f, 0.f, 0.f);
    newPoints[1].uv = float2(1, 0);
    newPoints[2].position = InData[0].position - float4(0.012f, 0.012f, 0.f, 0.f);
    newPoints[2].uv = float2(0, 1);
    newPoints[3].position = InData[0].position - float4(-0.012f, 0.012f, 0.f, 0.f);
    newPoints[3].uv = float2(1, 1);

    //change to g2p
    g2p output[4];
    for (int i = 0; i < 4; i++) {
        output[i].position = newPoints[i].position;
        output[i].color = InData[0].color;
    }
    //append to triStream
    for (int i = 0; i < 4; i++) {
        triStream.Append(output[i]);
    }
}


float4 PS(g2p pIn) : SV_TARGET
{
    return pIn.color;
}

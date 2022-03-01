#include "Constants.hlsl"
struct VIn
{
    float3 position : POSITION;
    float size : SIZE;
    float4 endColor : COLOR;
    float timeout : TIMEOUT;
    float lifeTimer : LIFETIMER;
};

struct v2g
{
    float3 position : POSITION;
    float size : SIZE;
    float4 endColor : COLOR;
    float timeout : TIMEOUT;
    float lifeTimer : LIFETIMER;
};

struct g2p
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 endColor : COLOR;
    float timeout : TIMEOUT;
    float lifeTimer : LIFETIMER;
};


//-----------------Vertex Shader-----------------//
v2g VS(VIn vIn)
{
    v2g output;
    output.position = mul(float4(vIn.position, 1.0), c_modelToWorld);
    output.position = vIn.position;
    output.size = vIn.size;
    output.timeout = vIn.timeout;
    output.endColor = vIn.endColor;
    output.lifeTimer = vIn.lifeTimer;
    return output;
}

//-----------------Geometry Shader-----------------//
[maxvertexcount(4)]
void GS(point v2g InData[1], inout TriangleStream<g2p> triStream)
{
    //create new vertices
    g2p newPoints[4];

    ////local coordinate
    float3 up = c_upVector;
    float3 look = c_cameraPosition - InData[0].position;
    look = normalize(look);
    float3 right = cross(up, look);


    float size = InData[0].size;
    /*float size = 30;*/

    float3 pos1 = InData[0].position + (right - up) * size;
    float3 pos2 = InData[0].position + (right + up) * size;
    float3 pos3 = InData[0].position - (right + up) * size;
    float3 pos4 = InData[0].position - (right - up) * size;

    newPoints[0].position = float4(pos1, 1.0f);
    newPoints[0].uv = float2(1, 1);
    newPoints[1].position = float4(pos2, 1.0f);
    newPoints[1].uv = float2(1, 0);
    newPoints[2].position = float4(pos3, 1.0f);
    newPoints[2].uv = float2(0, 1);
    newPoints[3].position = float4(pos4, 1.0f);
    newPoints[3].uv = float2(0, 0);


    for (int j = 0; j < 4; j++) {
        newPoints[j].position = mul(newPoints[j].position, c_viewProj);
        newPoints[j].timeout = InData[0].timeout;
        newPoints[j].lifeTimer = InData[0].lifeTimer;
        newPoints[j].endColor = InData[0].endColor;
    }

    for (int i = 0; i < 4; i++) {
        triStream.Append(newPoints[i]);
    }
}


float4 PS(g2p pIn) : SV_TARGET
{
    float4 color = ParticleTexture.Sample(ParticleSampler, pIn.uv);
    float lerpValue = pIn.lifeTimer / pIn.timeout;
    float tempR = lerp(color.r, pIn.endColor.r, lerpValue);
    float tempG = lerp(color.g, pIn.endColor.g, lerpValue);
    float tempB = lerp(color.b, pIn.endColor.b, lerpValue);
    float tempA = lerp(color.a, pIn.endColor.a, lerpValue);
    return float4(tempR, tempG, tempB, tempA);
}

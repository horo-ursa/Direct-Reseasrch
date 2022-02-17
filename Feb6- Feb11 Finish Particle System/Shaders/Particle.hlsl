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
    output.position = mul(float4(vIn.position, 1.0), c_modelToWorld);
    return output;
}


[maxvertexcount(4)]
void GS(point v2g InData[1], inout TriangleStream<g2p> triStream)
{
    //create new vertices
    g2p newPoints[4];
    
    //local coordinate
    float4 eye_pos = InData[0].position;
    float4 cam_pos = float4(c_cameraPosition, 1.0);
    float3 w = normalize(float3(cam_pos.x - eye_pos.x, cam_pos.y - eye_pos.y, cam_pos.z - eye_pos.z));
    float3 v = float3(0, 0, 1);
    float3 u = cross(w, v);
    float4 look = float4(w, 0.0f);
    float4 up = float4(v, 0.0f);
    float4 right = float4(u, 0.0f);
     
    float size = 3;

    newPoints[0].position = InData[0].position + (right - up)* size;
    newPoints[0].uv = float2(1, 1);
    newPoints[1].position = InData[0].position + (right + up)* size;
    newPoints[1].uv = float2(1, 0);
    newPoints[2].position = InData[0].position - (right + up) * size;
    newPoints[2].uv = float2(0, 1);
    newPoints[3].position = InData[0].position - (right - up) * size;
    newPoints[3].uv = float2(0, 0);

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
    /*if (color.x == 1.f && color.y == 1.f && color.z == 1.f) {
        color = float4(0.0f, 0.2f, 0.4f, 1.0f);
    }*/
    //color = float4(0, 0, 0, 1);
    return color.aaaa;
    return color;
}

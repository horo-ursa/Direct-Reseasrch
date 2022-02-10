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
};

v2g VS(VIn vIn)
{
    v2g output;

    output.position = vIn.position;
    output.color = vIn.color;

    return output;
}

[maxvertexcount(8)]
void GS(triangle v2g InData[3], inout TriangleStream<g2p> triStream)
{
    //generate new vertices
    v2g m[3];
    m[0].position = (InData[0].position + InData[1].position) / 2.f;
    m[0].color = float4(1.0, 1.0, 1.0, 0.0);
    m[1].position = (InData[1].position + InData[2].position) / 2.f;
    m[1].color = float4(1.0, 1.0, 1.0, 0.0);
    m[2].position = (InData[2].position + InData[0].position) / 2.f;
    m[2].color = float4(1.0, 1.0, 1.0, 0.0);
    v2g allVertices[6];
    allVertices[0] = InData[0]; 
    allVertices[1] = m[0]; 
    allVertices[2] = m[2]; 
    allVertices[3] = m[1]; 
    allVertices[4] = InData[2]; 
    allVertices[5] = InData[1];

    //create new triangles
    g2p output[6];
    for (int i = 0; i < 6; i++) {
        output[i].position = allVertices[i].position;
        output[i].color = allVertices[i].color;
    }

    for (int i = 0; i < 5; i++) {
        triStream.Append(output[i]);
    }
    triStream.RestartStrip();
    triStream.Append(output[1]);
    triStream.Append(output[5]);
    triStream.Append(output[3]);

}

float4 PS(g2p pIn) : SV_TARGET
{
    return pIn.color;
}

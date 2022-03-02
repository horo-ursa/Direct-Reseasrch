#include "Constants.hlsl"

struct VIn
{
    float3 position : POSITION0;
    float3 normal : NORMAL0;
    float4 color : COLOR0;
    float2 uv : TEXCOORD0;
};

struct VOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR0;
    float2 uv : TEXCOORD0;
    float4 normal : NORMAL0;
    float4 worldPosition : POSITION1;
};

VOut VS(VIn vIn)
{
    VOut output;

    output.position = mul(float4(vIn.position, 1.0), c_modelToWorld);
    output.position = mul(output.position, c_viewProj);
    output.color = vIn.color;
    output.uv = vIn.uv;
    output.worldPosition = mul(float4(vIn.position, 1.0), c_modelToWorld);

    output.normal = mul(float4(vIn.normal, 0.0), c_modelToWorld);
    output.normal = normalize(output.normal);
    return output;
}

float4 PS(VOut pIn) : SV_TARGET
{
    float4 diffuse = DiffuseTexture.Sample(DefaultSampler, pIn.uv);
    float4 color = pIn.color * diffuse;
    float4 reNormal = normalize(pIn.normal);
    
    float4 accumulatedColor = float4(0.0, 0.0, 0.0, 0.0);
    for (int i = 0; i < MAX_POINT_LIGHTS; i++) {
        if (c_pointLight[i].isEnabled) {
            float4 Li = normalize(float4(c_pointLight[i].position, 1.0));
            float4 Ri = normalize(reflect(-Li, reNormal));
            float4 V = normalize(float4(c_cameraPosition, 1.0) - pIn.worldPosition);
            float Distance = distance(float4(c_pointLight[i].position, 1.0), pIn.worldPosition);
            float4 Falloff = smoothstep(c_pointLight[i].outerRadius, c_pointLight[i].innerRadius, Distance);
            
            //float4(c_pointLight[i].lightColor, 1.0)
            float4 diffuseColor = Falloff * float4(c_diffuseColor, 1.0) * max(dot(reNormal, Li), 0.0);
            float4 specularColor = Falloff * float4(c_specularColor, 1.0) * max(pow(dot(Ri, V), c_specularPower), 0.0);
            float4 lightColor = float4(c_pointLight[i].lightColor, 1.0) * (diffuseColor + specularColor);
            accumulatedColor = accumulatedColor + lightColor;
            //color += lightColor;
        }
    }
    accumulatedColor += float4(c_ambient, 1.0);
    color = color * accumulatedColor;
    return color;
}

#include "Constants.hlsl"

struct VertexInput
{
    float3 position : POSITION0;
    float3 normal : NORMAL0;
    float2 uv : TEXCOORD0;
};

struct VertexToPixel
{
    float4 position : SV_POSITION;
    float4 worldPos : POSITION1;
};


VertexToPixel VS(in VertexInput input) {
    VertexToPixel output;
    output.worldPos = mul(float4(input.position, 1.0), c_modelToWorld);
    output.position = mul(output.worldPos, c_cameraSpaceViewProj);
    return output;
}

float4 PS(in VertexToPixel input) : SV_Target
{
    float3 sampleIndices = float3(input.position.xy, 0);
    float4 pixelWorldPosition = float4(positionGP.Load(sampleIndices).xyz, 1.0);
    float4 albedo = float4(albedoGP.Load(sampleIndices).xyz, 1.0);
    float4 normal = float4(normalize(normalGP.Load(sampleIndices).xyz), 0.0);

    //pixelWorldPosition = positionGP.Sample(DefaultSampler, uv);
    //albedo = albedoGP.Sample(DefaultSampler, uv);
    //normal = normalize(normalGP.Sample(DefaultSampler, uv));

    float4 accumulatedColor = float4(0.0, 0.0, 0.0, 0.0);
    for (int i = 0; i < MAX_POINT_LIGHTS; i++) {
        if (c_pointLight[i].isEnabled) {
            float4 viewDir = normalize(float4(c_cameraPosition, 1.0) - pixelWorldPosition);
            float4 lightDir = normalize(float4(c_pointLight[i].position, 0.0));
            float4 reflectedLightDir = normalize(reflect(-lightDir, normal));
            float4 diffuseColor = float4(c_diffuseColor, 1.0) * max(dot(normal, lightDir), 0.0);
            //return float4(c_pointLight[i].lightColor, 1.0) * diffuseColor * albedo;
            float4 specularColor = float4(c_specularColor, 1.0) * max(pow(dot(reflectedLightDir, viewDir), c_specularPower), 0.0);
            //return float4(c_pointLight[i].lightColor, 1.0) * specularColor * albedo;
            float4 lightColor = float4(c_pointLight[i].lightColor, 1.0) * (diffuseColor + specularColor);
            accumulatedColor += lightColor;
        }
    }

    //return pixelWorldPosition;
    //return albedo;
    //return normal;
    //return accumulatedColor;
    accumulatedColor += float4(c_ambient, 1.0);
    return albedo * accumulatedColor;
}
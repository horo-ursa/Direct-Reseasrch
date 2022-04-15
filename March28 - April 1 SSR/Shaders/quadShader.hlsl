#include "Constants.hlsl"

struct VertexInput
{
    float3 position : POSITION;
    float2 uv       : TEXCOORD0;
};

struct VertexToPixel
{
    float4 position		: SV_POSITION;
    float2 uv           : TEXCOORD0;
};



VertexToPixel VS(uint id : SV_VertexID) {
    // Set up output
    VertexToPixel output;

    // Calculate the UV (0,0) to (2,2) via the ID
    output.uv = float2(
        (id << 1) & 2,  // id % 2 * 2
        id & 2);

    // Adjust the position based on the UV
    output.position = float4(output.uv, 0, 1);
    output.position.x = output.position.x * 2 - 1;
    output.position.y = output.position.y * -2 + 1;

    return output;
}

float4 PS(in VertexToPixel input) : SV_Target
{
    float4 pixelWorldPosition = positionGP.Sample(DefaultSampler, input.uv);
    float4 albedo = albedoGP.Sample(DefaultSampler, input.uv);
    float4 normal = normalize(normalGP.Sample(DefaultSampler, input.uv));

    float4 accumulatedColor = float4(0.0, 0.0, 0.0, 0.0);
    for (int i = 0; i < MAX_POINT_LIGHTS; i++) {
        if (c_pointLight[i].isEnabled) {
            float4 viewDir = normalize(float4(c_cameraPosition, 1.0) - pixelWorldPosition);
            float4 lightDir = normalize(float4(c_pointLight[i].position, 1.0));
            float4 reflectedLightDir = normalize(reflect(-lightDir, normal));
            float4 diffuseColor = float4(c_diffuseColor, 1.0) * max(dot(normal, lightDir), 0.0);
            float4 specularColor = float4(c_specularColor, 1.0) * max(pow(dot(reflectedLightDir, viewDir), c_specularPower), 0.0);
            float4 lightColor = float4(c_pointLight[i].lightColor, 1.0) * (diffuseColor + specularColor);
            accumulatedColor += lightColor;
        }
    }
    accumulatedColor += float4(c_ambient, 1.0);
    return albedo * accumulatedColor;
}
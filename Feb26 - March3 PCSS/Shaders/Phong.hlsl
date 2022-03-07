#include "Constants.hlsl"


//// Shadow map related variables
//#define NUM_SAMPLES 20
//#define BLOCKER_SEARCH_NUM_SAMPLES NUM_SAMPLES
//#define PCF_NUM_SAMPLES NUM_SAMPLES
//#define NUM_RINGS 10
//#define EPS 1e-3
//#define PI 3.141592653589793
//#define PI2 6.283185307179586
//
//float2 poissonDisk[NUM_SAMPLES];
//
//float unpack(float4 rgbaDepth) {
//    float4 bitShift = float4(1.0, 1.0 / 256.0, 1.0 / (256.0 * 256.0), 1.0 / (256.0 * 256.0 * 256.0));
//    return dot(rgbaDepth, bitShift);
//}
//
//float rand_1to1(float x) {
//    // -1 -1
//    return frac(sin(x) * 10000.0);
//}
//
//float rand_2to1(float2 uv) {
//    // 0 - 1
//    const float a = 12.9898, b = 78.233, c = 43758.5453;
//    float dt = dot(uv.xy, float2(a, b));
//    //float sn = mod(dt, PI);
//    float sn = dt - PI * floor(dt / PI);
//    return frac(sin(sn) * c);
//}
//
//void poissonDiskSamples(float2 randomSeed) {
//
//    float ANGLE_STEP = PI2 * float(NUM_RINGS) / float(NUM_SAMPLES);
//    float INV_NUM_SAMPLES = 1.0 / float(NUM_SAMPLES);
//
//    float angle = rand_2to1(randomSeed) * PI2;
//    float radius = INV_NUM_SAMPLES;
//    float radiusStep = radius;
//
//    for (int i = 0; i < NUM_SAMPLES; i++) {
//        poissonDisk[i] = float2(cos(angle), sin(angle)) * pow(radius, 0.75);
//        radius += radiusStep;
//        angle += ANGLE_STEP;
//    }
//}
//
//void uniformDiskSamples(float2 randomSeed) {
//
//    float randNum = rand_2to1(randomSeed);
//    float sampleX = rand_1to1(randNum);
//    float sampleY = rand_1to1(sampleX);
//
//    float angle = sampleX * PI2;
//    float radius = sqrt(sampleY);
//
//    for (int i = 0; i < NUM_SAMPLES; i++) {
//        poissonDisk[i] = float2(radius * cos(angle), radius * sin(angle));
//
//        sampleX = rand_1to1(sampleY);
//        sampleY = rand_1to1(sampleX);
//
//        angle = sampleX * PI2;
//        radius = sqrt(sampleY);
//    }
//}
//
//float findBlocker(sampler2D shadowMap, float2 uv, float zReceiver) {
//    return 1.0;
//}
//
//float PCF(float4 psPosLightSpace) {
//
//    float3 projCoords = psPosLightSpace.xyz / psPosLightSpace.w;
//    projCoords.xy = projCoords.xy * 0.5 + 0.5;
//    projCoords.y = 1 - projCoords.y;
//    uniformDiskSamples(projCoords.xy);
//
//    float textureSizeX = 2000;
//    float textureSizeY = 1200;
//    float filterStride = 5;
//    float filterRangeX = 1.0 / textureSizeX * filterStride;
//    float filterRangeY = 1.0 / textureSizeY * filterStride;
//
//    int noShadowCount = 0;
//    for (int i = 0; i < NUM_SAMPLES; i++) {
//        float2 sampleCoord = poissonDisk[i] * float2(filterRangeX, filterRangeY) + projCoords.xy;
//        float closestDepth = DepthMap.Sample(DefaultSampler, sampleCoord.xy).r + 0.005;
//        float currentDepth = projCoords.z;
//        if (currentDepth < closestDepth) {
//            noShadowCount++;
//        }
//    }
//    return float(noShadowCount) / float(NUM_SAMPLES);
//}
//
//float PCSS(float4 psPosLightSpace) {
//
//    // STEP 1: avgblocker depth
//
//    // STEP 2: penumbra size
//
//    // STEP 3: filtering
//
//    return 1.0;
//
//}



struct VIn
{
    float3 position : POSITION0;
    float3 normal : NORMAL0;
    float2 uv : TEXCOORD0;
};

struct VOut
{
    float4 position : SV_POSITION;
    float4 normal : NORMAL0;
    float2 uv : TEXCOORD0;
    float4 worldPosition : POSITION1;
    float4 psPosLightSpace : POSITION2;
};

VOut VS(VIn vIn)
{
    VOut output;

    output.position = mul(float4(vIn.position, 1.0), c_modelToWorld);
    output.psPosLightSpace = mul(output.position, c_lightSpaceViewProj);
    if (isDepth) {
        output.position = output.psPosLightSpace;
    }
    else {
        output.position = mul(output.position, c_cameraSpaceViewProj);
    }
    output.uv = vIn.uv;
    output.worldPosition = mul(float4(vIn.position, 1.0), c_modelToWorld);

    output.normal = mul(float4(vIn.normal, 0.0), c_modelToWorld);
    output.normal = normalize(output.normal);
    return output;
}

float4 PS(VOut pIn) : SV_TARGET
{
    if (isDepth) {
        float depth = pIn.psPosLightSpace.z / pIn.psPosLightSpace.w;
        return float4(depth, depth, depth, 1.0);
    }
    float4 diffuse = DiffuseTexture.Sample(DefaultSampler, pIn.uv);
    float4 color = diffuse;
    float4 reNormal = normalize(pIn.normal);

    float4 accumulatedColor = float4(0.0, 0.0, 0.0, 0.0);
    for (int i = 0; i < MAX_POINT_LIGHTS; i++) {
        if (c_pointLight[i].isEnabled) {
            //float4 Li = normalize(float4(c_pointLight[i].position, 1.0) - pIn.worldPosition);
            float4 Li = normalize(float4(c_pointLight[i].position, 1.0));
            float4 Ri = normalize(reflect(-Li, reNormal));
            float4 V = normalize(float4(c_cameraPosition, 1.0) - pIn.worldPosition);
            float Distance = distance(float4(c_pointLight[i].position, 1.0), pIn.worldPosition);
            //float4 Falloff = smoothstep(c_pointLight[i].outerRadius, c_pointLight[i].innerRadius, Distance);

            //float4(c_pointLight[i].lightColor, 1.0)
            float4 diffuseColor =/*Falloff */  float4(c_diffuseColor, 1.0) * max(dot(reNormal, Li), 0.0);
            float4 specularColor =/*Falloff */ float4(c_specularColor, 1.0) * max(pow(dot(Ri, V), c_specularPower), 0.0);
            float4 lightColor = float4(c_pointLight[i].lightColor, 1.0) * (diffuseColor + specularColor);
            accumulatedColor = accumulatedColor + lightColor;
            //color += lightColor;
        }
    }

    float shadow = ShadowCalculation(pIn.psPosLightSpace);
    shadow = PCSS(pIn.psPosLightSpace);
    //shadow = 0;
    accumulatedColor = shadow * accumulatedColor + float4(c_ambient, 1.0);
    color = color * accumulatedColor;
    
    //return float4(shadow, shadow, shadow, 1);

    return color;
}

// We want to use row major matrices
#pragma pack_matrix(row_major)

cbuffer PerCameraConstants : register(b0)
{
    float4x4 c_lightSpaceViewProj;
    float4x4 c_cameraSpaceViewProj;
    float3 c_cameraPosition;
};

cbuffer PerObjectConstants : register(b1)
{
    float4x4 c_modelToWorld;
};

cbuffer MaterialConstants : register(b2)
{
    float3 c_diffuseColor;
    float3 c_specularColor;
    float c_specularPower;
};

#define MAX_POINT_LIGHTS 8
struct PointLightData
{
    float3 lightColor;
    float3 position;
    float innerRadius;
    float outerRadius;
    bool isEnabled;
};

cbuffer PointLightConstants : register(b3)
{
    float3 c_ambient;
    PointLightData c_pointLight[MAX_POINT_LIGHTS];
};

#define MAX_SKELETON_BONES 80 
cbuffer SkinConstants : register(b4)
{
    float4x4 c_skinMatrix[MAX_SKELETON_BONES];
};

cbuffer depthMask : register(b5)
{
    bool isDepth;
}

SamplerState DefaultSampler : register(s0);


Texture2D DiffuseTexture : register(t0);

Texture2D DepthMap : register(t1);

//constant functions
float ShadowCalculation(float4 psPosLightSpace) {
    float3 projCoords = psPosLightSpace.xyz / psPosLightSpace.w;
    // transform to [0,1] range
    projCoords.xy = projCoords.xy * 0.5 + 0.5;
    projCoords.y = 1 - projCoords.y;
    float cloestDepth = DepthMap.Sample(DefaultSampler, projCoords.xy).r + 0.0001;
    float currentDepth = projCoords.z;
    float inShadow = currentDepth > cloestDepth ? 1.0 : 0.0;
    return inShadow;
}


// Shadow map related variables
#define NUM_SAMPLES  50
#define BLOCKER_SEARCH_NUM_SAMPLES NUM_SAMPLES
#define PCF_NUM_SAMPLES NUM_SAMPLES
#define NUM_RINGS 10
#define EPS 1e-3
#define PI 3.141592653589793
#define PI2 6.283185307179586
#define wLight 10

float2 poissonDisk[NUM_SAMPLES];

float unpack(float4 rgbaDepth) {
    float4 bitShift = float4(1.0, 1.0 / 256.0, 1.0 / (256.0 * 256.0), 1.0 / (256.0 * 256.0 * 256.0));
    return dot(rgbaDepth, bitShift);
}

float rand_1to1(float x) {
    // -1 -1
    return frac(sin(x) * 10000.0);
}

float rand_2to1(float2 uv) {
    // 0 - 1
    const float a = 12.9898, b = 78.233, c = 43758.5453;
    float dt = dot(uv.xy, float2(a, b));
    //float sn = mod(dt, PI);
    float sn = dt - PI * floor(dt / PI);
    return frac(sin(sn) * c);
}

void poissonDiskSamples(float2 randomSeed) {

    float ANGLE_STEP = PI2 * float(NUM_RINGS) / float(NUM_SAMPLES);
    float INV_NUM_SAMPLES = 1.0 / float(NUM_SAMPLES);

    float angle = rand_2to1(randomSeed) * PI2;
    float radius = INV_NUM_SAMPLES;
    float radiusStep = radius;

    for (int i = 0; i < NUM_SAMPLES; i++) {
        poissonDisk[i] = float2(cos(angle), sin(angle)) * pow(radius, 0.75);
        radius += radiusStep;
        angle += ANGLE_STEP;
    }
}

void uniformDiskSamples(float2 randomSeed) {

    float randNum = rand_2to1(randomSeed);
    float sampleX = rand_1to1(randNum);
    float sampleY = rand_1to1(sampleX);

    float angle = sampleX * PI2;
    float radius = sqrt(sampleY);

    for (int i = 0; i < NUM_SAMPLES; i++) {
        poissonDisk[i] = float2(radius * cos(angle), radius * sin(angle));

        sampleX = rand_1to1(sampleY);
        sampleY = rand_1to1(sampleX);

        angle = sampleX * PI2;
        radius = sqrt(sampleY);
    }
}

float PCF(float4 psPosLightSpace, float wPenumbra) {

    float3 projCoords = psPosLightSpace.xyz / psPosLightSpace.w;
    projCoords.xy = projCoords.xy * 0.5 + 0.5;
    projCoords.y = 1 - projCoords.y;
    poissonDiskSamples(projCoords.xy);

    float textureSize = 1000;
    float filterStride = 2;
    float filterRange = 1.0 / textureSize * filterStride * wPenumbra;

    int noShadowCount = 0;
    for (int i = 0; i < NUM_SAMPLES; i++) {
        float2 sampleCoord = poissonDisk[i] * filterRange + projCoords.xy;
        float closestDepth = DepthMap.Sample(DefaultSampler, sampleCoord.xy).r + 0.005;
        float currentDepth = projCoords.z;
        if (currentDepth < closestDepth) {
            noShadowCount++;
        }
    }
    return float(noShadowCount) / float(NUM_SAMPLES);
}

float findAverageBlockerDepth(float4 psPosLightSpace, float zReceiver) {
    float3 projCoords = psPosLightSpace.xyz / psPosLightSpace.w;
    projCoords.xy = projCoords.xy * 0.5 + 0.5;
    projCoords.y = 1 - projCoords.y;
    poissonDiskSamples(projCoords.xy);

    float textureSize = 1000;
    float filterStride = 20;
    float filterRange = 1.0 / textureSize * filterStride;

    int shadowCount = 0;
    float blockDepth = 0.0f;
    for (int i = 0; i < NUM_SAMPLES; i++) {
        float2 sampleCoord = poissonDisk[i] * filterRange + projCoords.xy;
        float closestDepth = DepthMap.Sample(DefaultSampler, sampleCoord.xy).r + 0.005;
        if (zReceiver > closestDepth) {
            shadowCount++;
            blockDepth += closestDepth;
        }
    }
    return float(blockDepth) / float(shadowCount);
}

float PCSS(float4 psPosLightSpace) {
    float3 projVec = psPosLightSpace.xyz / psPosLightSpace.w;
    float zReceiver = projVec.z;
    // STEP 1: avgblocker depth
    float zBlocker = findAverageBlockerDepth(psPosLightSpace, zReceiver);
    if (zBlocker < EPS) return 1.0f;
    if (zBlocker > 1.0) return 0.0f;
    // STEP 2: penumbra size
    float wPenumbra = (zReceiver - zBlocker) * wLight / zBlocker;
    // STEP 3: filtering
    return PCF(psPosLightSpace, wPenumbra);

}
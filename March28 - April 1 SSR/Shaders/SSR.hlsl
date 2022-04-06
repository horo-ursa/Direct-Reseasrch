#include "Constants.hlsl"

#define RAY_MARCH_TIME 40
#define MAX_MARCH_DIS 1000
#define M_PI 3.1415926535897932384626433832795
#define TWO_PI 6.283185307
#define INV_PI 0.31830988618
#define INV_TWO_PI 0.15915494309
#define SAMPLE_NUM 50

float Rand1(inout float p) {
	p = frac(p * .1031);
	p *= p + 33.33;
	p *= p + p;
	return frac(p);
}

float2 Rand2(inout float p) {
	return float2(Rand1(p), Rand1(p));
}

//初始化随机数得到随机数的状态 s
float InitRand(float2 uv) {
	float3 p3 = frac(float3(uv.xyx) * .1031);
	p3 += dot(p3, p3.yzx + 33.33);
	return frac((p3.x + p3.y) * p3.z);
}

float3 SampleHemisphereUniform(inout float s, out float pdf) {
	float2 uv = Rand2(s);
	float z = uv.x;
	float phi = uv.y * TWO_PI;
	float sinTheta = sqrt(1.0 - z * z);
	float3 dir = float3(sinTheta * cos(phi), sinTheta * sin(phi), z);
	pdf = INV_TWO_PI;
	return dir;
}

float4 Project(float4 a) {
	return a / a.w;
}

void LocalBasis(float3 n, out float3 b1, out float3 b2) {
	float sign_ = sign(n.z);
	if (n.z == 0.0) {
		sign_ = 1.0;
	}
	float a = -1.0 / (sign_ + n.z);
	float b = n.x * n.y * a;
	b1 = float3(1.0 + sign_ * n.x * n.x * a, sign_ * b, -sign_ * n.x);
	b2 = float3(b, sign_ + n.y * n.y * a, -n.y);
}

float GetDepth(float3 posWorld) {
	return = mul(float4(posWorld, 1.0), c_worldToScreen).w; //????????????????
}

float GetGBufferDepth(float2 uv) {
	float depth = depthGP.Load(float3(uv, 0)).r;
	if (depth < 1e-2) {
		depth = 1000.0;
	}
	return depth;
}

float3 GetGBufferNormalWorld(float2 uv) {
	return normalGP.Load(float3(uv, 0)).xyz;
}

float2 GetScreenCoordinate(float3 posWorld) {
	return = Project(mul(c_cameraSpaceViewProj, float4(posWorld, 1.0)).xy * 0.5 + 0.5;
}

float3 GetGBufferDiffuse(float2 uv) {
	float3 diffuse = albedoGP.Load(float3(uv, 0)).xyz;
	return pow(diffuse, float3(2.2));   //?????????????????????
}

float3 GetGBufferShadow(float2 uv) {
	return shadowGP.Load(float3(uv, 0)).x;
}

float3 EvalDirectionalLight(float2 uv) {
	float visibility = GetGBufferShadow(uv);
	return c_pointLight[0].lightColor /*uLightRadiance*/ * visibility;
}

/* 
 * ori: origin of the reflected light
 * dir: direction of the light
 * hitPoint: intersection point
 */

bool RayMarch(float3 ori, float3 dir, out float3 hitPoint) {
	float step = 0.05;
	float3 endPoint = ori;

	for (int i = 0; i < RAY_MARCH_TIME; i++) {
		float3 testPoint = endPoint + dir * step;
		float testDepth = GetDepth(testPoint);                                   //depth of the test point
		float depthInGBuffer = GetGBufferDepth(GetScreenCoordinate(testPoint));  //depth in the depth map

		float marchedDis = distance(testPoint, ori);

		if (marchedDis > MAX_MARCH_DIS) {                      //no intersection
			return false;
		}
		if (abs(testDepth - depthInGBuffer) <= 1e-6) {    //close enough, intersection
			hitPoint = testPoint;
			return true;
		}
		if (testDepth < depthInGBuffer) {                 //too shallow, no intersection
			endPoint = testPoint;                         //Linear Raymarch for now
		}
		else {                                            //too depth, count as intersection for now
			hitPoint = testPoint;
			return true;
		}
	}
	return false;
}

/*
 * Evaluate diffuse bsdf value.
 * wi, wo are all in world space.
 * uv is in screen space, [0, 1] x [0, 1].
 * wi,wo入射方向和出射方向
 */
float3 EvalDiffuse(float3 wi, float3 wo, float2 uv) {
	float3 albedo = GetGBufferDiffuse(uv);//获取漫反射率
	float3 normal = normalize(GetGBufferNormalWorld(uv));//获取法线
	float cosTheta = max(0.0, dot(normalize(wi), normal));//漫反射夹角不能为负! 不然间接光存在黑边
	return albedo * INV_PI * cosTheta;
}

struct vsIn {
	float3 position : POSITION0;
	float3 normal : NORMAL0;
	float2 uv : TEXCOORD0;
};

struct vsOut {
	float4 worldPos  : WORLDPOS;
	float4 screenPos : SV_POSITION;
};

vsOut VS(in vsIn input) {
	vsOut output;
	output.worldPos = mul(float4(input.position, 1.0), c_modelToWorld);
	output.worldPos = output.worldPos.xyzw / output.worldPos.w;           //???????????????????
	output.screenPos = mul(output.worldPos, c_cameraSpaceViewProj);
	return output;
}

float4 PS(in vsOut input) : SV_Target
{
	float seed = InitRand(input.worldPos.xy);

	float2 uv = GetScreenCoordinate(input.worldPos.xyz);
	float3 viewDir = c_cameraPosition - input.worldPos;
	float3 lightDir = c_pointLight[0].position;
	float3 normal = normalize(GetGBufferNormalWorld(uv));
	float3 totalLighting = float3(0, 0, 0);
	float3 indirectLighting = float3(0, 0, 0);
	float3 directLighting = EvalDiffuse(lightDir, viewDir, uv) * EvalDirectionalLight(uv); 

	for (int i = 0; i < SAMPLE_NUM; i++) {
		Rand1(seed);
		float pdf;
		float3 dir = SampleHemisphereUniform(seed, pdf);	//return a local direction in the hemisphere
		float3 b1, b2;
		float3 hitPos;										//the origin of indirect light
		LocalBasis(normal, b1, b2);							//construct a local coordinate using normal
		dir = normalize(mul(float3x3(b1, b2, normal),dir)); //transform local dir to world space
		if (RayMarch(input.worldPos, dir, hitPos)) {
			float2 uv1 = GetScreenCoordinate(hitPos);
			indirectLighting += EvalDiffuse(dir, viewDir, uv) / pdf * EvalDiffuse(lightDir, dir, uv1) * EvalDirectionalLight(uv1) * 1 / SAMPLE_NUM;
		}
	}
	totalLighting = indirectLighting + directLighting;

	float3 color = pow(clamp(L, float3(0.0), float3(1.0)), float3(1.0 / 2.2));//clamp 三个参数中大小处在中间的那个值
	return float4(color.rgb, 1.0);
}
#include "Constants.hlsl"

#define RAY_MARCH_TIME 1
#define MAX_MARCH_DIS 100
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
	float4 temp = mul(float4(posWorld, 1.0), c_cameraSpaceViewProj);
	//return mul(float4(posWorld, 1.0), c_cameraSpaceViewProj).w; //????????????????
	return temp.z / temp.w;
}

float GetGBufferDepth(float3 testPoint) {
	float4 screenSpace = mul(float4(testPoint, 1.0), c_cameraSpaceViewProj);
	screenSpace = screenSpace.xyzw / screenSpace.w;
	float2 uv = screenSpace.xy * 0.5 + 0.5;
	uv.y = 1 - uv.y;
	//float depth = depthGP.Load(float3(uv, 0)).r;
	float depth = depthGP.Sample(DefaultSampler, uv).x;
	return depth;
}

float3 GetGBufferNormalWorld(float2 uv) {
	return normalGP.Load(float3(uv, 0)).xyz;
}

float2 GetScreenCoordinate(float3 posWorld) {
	return Project(mul(float4(posWorld, 1.0), c_cameraSpaceViewProj)).xy * 0.5 + 0.5;
}

float3 GetGBufferDiffuse(float2 uv) {
	float3 diffuse = albedoGP.Load(float3(uv, 0)).xyz;
	return diffuse;
	//return pow(diffuse, float3(2.2));   //?????????????????????
}

float GetGBufferShadow(float2 uv) {
	return shadowGP.Load(float3(uv, 0)).x;
}

float3 EvalDirectionalLight(float2 uv) {
	float visibility = GetGBufferShadow(uv);
	//return c_pointLight[0].lightColor  * visibility;
	return c_pointLight[0].lightColor * visibility;
}

/* 
 * ori: origin of the reflected light
 * dir: direction of the light
 * hitPoint: intersection point
 */

bool RayMarch(float3 ori, float3 dir, out float3 hitPoint) {
	float step = 10;
	float3 endPoint = ori;

	for (int i = 0; i < RAY_MARCH_TIME; i++) {
		float3 testPoint = endPoint + dir * step;
		float4 screenSpace = mul(float4(testPoint, 1.0), c_cameraSpaceViewProj);
		screenSpace = screenSpace.xyzw / screenSpace.w;
		float testDepth = screenSpace.z;          //depth of the test point
		float2 uv = screenSpace.xy * 0.5 + 0.5;
		uv.y = 1 - uv.y;
		float depthInGBuffer = depthGP.Sample(DefaultSampler, uv).x;  //depth in the depth map
		if (depthInGBuffer < 0.001) { //black
			depthInGBuffer = 1000;
		}

		float marchedDis = distance(testPoint, ori);

		if (marchedDis > MAX_MARCH_DIS) {                      //no intersection
			return false;
		}
		if (testDepth - depthInGBuffer > 1e-6) {    //close enough, intersection
			hitPoint = testPoint;
			return true;
		}
		//if (testDepth < depthInGBuffer) {                 //too shallow, no intersection
		//	endPoint = testPoint;                         //Linear Raymarch for now
		//}
		//else {                                            //too depth, count as intersection for now
		//	hitPoint = testPoint;
		//	return true;
		//}
		endPoint = testPoint;
	}
	return false;
}

/*
 * Evaluate diffuse bsdf value.
 * wi, wo are all in world space.
 * uv is in screen space, [0, 1] x [0, 1].
 * wi: in dir,wo: out dir
 */
float3 EvalDiffuse(float3 wi, float3 wo, float2 uv) {
	float3 albedo = GetGBufferDiffuse(uv);					//Get albedo from albedoGP
	float3 normal = normalize(GetGBufferNormalWorld(uv));	//Get normal from normalGP
	float cosTheta = max(0.0, dot(normalize(wi), normal));	//Cosine
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
	//output.worldPos = output.worldPos.xyzw / output.worldPos.w;           //???????????????????
	output.screenPos = mul(output.worldPos, c_cameraSpaceViewProj);
	return output;
}

float4 PS(in vsOut input) : SV_Target
{
	float seed = InitRand(input.worldPos.xy);

	//float2 uv = GetScreenCoordinate(input.worldPos.xyz);
	float2 uv = input.screenPos.xy;
	float3 viewDir = normalize(c_cameraPosition - input.worldPos.xyz);
	float3 lightDir = c_pointLight[0].position;
	float3 normal = normalize(GetGBufferNormalWorld(uv));
	float3 totalLighting = float3(0, 0, 0);
	float3 indirectLighting = float3(0, 0, 0);
	float3 dif = EvalDiffuse(lightDir, viewDir, uv); 
	float3 direc = EvalDirectionalLight(uv);
	float3 directLighting = dif * direc;

	float3 dir = reflect(-viewDir, normal);
	//float3 b1, b2;
	//LocalBasis(normal, b1, b2);
	//dir = normalize(mul(dir, float3x3(normal, b2, b1)));

	
	float3 testPoint = input.worldPos.xyz + dir * 10;
	float4 screenSpace = mul(float4(testPoint, 1.0), c_cameraSpaceViewProj);
	screenSpace = screenSpace.xyzw / screenSpace.w;
	float testDepth = screenSpace.z;          //depth of the test point
	float2 tempUV = screenSpace.xy * 0.5 + 0.5;
	tempUV.y = 1 - tempUV.y;
	float4 testNormal = normalGP.Sample(DefaultSampler, tempUV);
	return testNormal;

	float3 hitPos;
	if (RayMarch(input.worldPos.xyz, dir, hitPos)) {
		float2 uv1 = mul(float4(hitPos, 1.0), c_cameraSpaceViewProj).xy;
		indirectLighting = albedoGP.Load(float3(uv1, 0)).xyz;
		return float4(1, 0, 0, 1);
	}
	else {
		return float4(0, 1, 0, 1);
	}
	totalLighting = indirectLighting + directLighting;
	return float4(totalLighting.rgb, 1.0);

	//for (int i = 0; i < SAMPLE_NUM; i++) {
	//	Rand1(seed);
	//	float pdf;
	//	float3 dir = SampleHemisphereUniform(seed, pdf);	//return a local direction in the hemisphere
	//	float3 b1, b2;
	//	float3 hitPos;										//the origin of indirect light
	//	LocalBasis(normal, b1, b2);							//construct a local coordinate using normal
	//	dir = normalize(mul(dir, float3x3(b1, b2, normal))); //transform local dir to world space
	//	if (RayMarch(input.worldPos.xyz, dir, hitPos)) {
	//		float2 uv1 = GetScreenCoordinate(hitPos);
	//		indirectLighting += EvalDiffuse(dir, viewDir, uv) / pdf * EvalDiffuse(lightDir, dir, uv1) * EvalDirectionalLight(uv1) * 1 / SAMPLE_NUM;
	//	}
	//}
	//totalLighting = indirectLighting + directLighting;

	//float3 color = pow(clamp(totalLighting, float3(0, 0, 0), float3(1, 1, 1)), float3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2));//clamp 三个参数中大小处在中间的那个值
	//return float4(color.rgb, 1.0);
}
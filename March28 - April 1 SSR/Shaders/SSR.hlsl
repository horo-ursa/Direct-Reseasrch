#include "Constants.hlsl"

#define RAY_MARCH_TIME 30
#define MAX_MARCH_DIS 500
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
	float2 uv = screenSpace.xy * 0.5+ 0.5;
	uv.y = 1 - uv.y;
	//float depth = depthGP.Load(float3(uv, 0)).r;
	float depth = depthGP.Sample(DefaultSampler, uv).x;
	return depth;
}

float3 GetGBufferNormalWorld(float2 uv) {
	//return normalGP.Sample(DefaultSampler, uv).xyz;
	return normalGP.Load(float3(uv, 0)).xyz;
}

float2 GetScreenCoordinate(float3 posWorld) {
	return Project(mul(float4(posWorld, 1.0), c_cameraSpaceViewProj)).xy * 0.5 + 0.5;
}

float3 GetGBufferDiffuse(float2 uv) {
	//return albedoGP.Sample(DefaultSampler, uv).xyz;
	return albedoGP.Load(float3(uv, 0)).xyz;
}

float GetGBufferShadow(float2 uv) {
	//return shadowGP.Sample(DefaultSampler, uv).x;
	return shadowGP.Load(float3(uv, 0)).x;
}


/*
 * ori: origin of the reflected light
 * dir: direction of the light
 * hitPoint: intersection point
 */

bool RayMarch(float4 ori, float4 dir, out float4 hitPoint) {
	float step = 0.3;
	float4 endPoint = ori;

	for (int i = 0; i < RAY_MARCH_TIME; i++) {
		float4 testPoint = endPoint + dir * step;
		float4 screenSpace = mul(testPoint, c_cameraSpaceViewProj);
		float testDepth = screenSpace.z/screenSpace.w;
		//float testDepth = screenSpace.z * 0.5 + 0.5;          //depth of the test point
		screenSpace = screenSpace.xyzw / screenSpace.w;
		float2 uv = screenSpace.xy * float2(0.5, -0.5) + 0.5;
		float depthInGBuffer = depthGP.Sample(DefaultSampler, uv).z;  //depth in the depth map

		float marchedDis = distance(testPoint, ori);
		if (marchedDis > MAX_MARCH_DIS) {                      //no intersection
			return false;
		}
		if (testDepth - depthInGBuffer > 1e-5) {    //close enough, intersection
			hitPoint = testPoint;
			return true;
		}
		endPoint = testPoint;
	}
	return false;
}

//float3 testPoint = endPoint + dir * step;
//float4 screenSpace = mul(float4(testPoint, 1.0), c_cameraSpaceViewProj);
//float depthInGBuffer = depthGP.Load(float3(screenSpace.xy, 0));
//float testDepth = screenSpace.z / screenSpace.w;

//if (testDepth < depthInGBuffer) {                 //too shallow, no intersection
//	endPoint = testPoint;                         //Linear Raymarch for now
//}
//else {                                            //too depth, count as intersection for now
//	hitPoint = testPoint;
//	return true;
//}

/*
 * Evaluate diffuse bsdf value.
 * wi, wo are all in world space.
 * uv is in screen space, [0, 1] x [0, 1].
 * wi: in dir,wo: out dir
 */
float4 EvalDiffuse(float4 wi, float4 normal, float4 worldPos) {
	float4 screenPos = mul(worldPos, c_cameraSpaceViewProj);
	float2 uv = screenPos.xy / screenPos.w * 0.5 + 0.5;
	uv.y = 1 - uv.y;
	float4 albedo = albedoGP.Sample(DefaultSampler, uv);
	//float3 normal = normalize(GetGBufferNormalWorld(uv));	//Get normal from normalGP
	float cosTheta = max(0.0, dot(normal, wi));	//Cosine
	return albedo * cosTheta;
}



float4 EvalDirectionalLight(float4 worldPos) {
	float4 screenPos = mul(worldPos, c_cameraSpaceViewProj);
	float2 uv = screenPos.xy / screenPos.w * 0.5 + 0.5;
	uv.y = 1 - uv.y;
	float visibility = shadowGP.Sample(DefaultSampler, uv).x;
	return float4(c_pointLight[0].lightColor, 1.0) * visibility;
}

struct vsIn {
	float3 position : POSITION0;
	float3 normal : NORMAL0;
	float2 uv : TEXCOORD0;
};

struct vsOut {
	float4 worldPos  : WORLDPOS;
	float4 screenPos : SV_POSITION;
	float4 worldNormal    : NORMAL1;
};

vsOut VS(in vsIn input) {
	vsOut output;
	output.worldPos = mul(float4(input.position, 1.0), c_modelToWorld);
	output.worldPos = output.worldPos.xyzw / output.worldPos.w;
	output.screenPos = mul(output.worldPos, c_cameraSpaceViewProj);
	output.worldNormal = mul(float4(input.normal, 0), c_modelToWorld);
	return output;
}

float4 PS(in vsOut input) : SV_Target
{
	/*
	float3 normal = normalize(input.worldNormal.xyz);
	float3 position = input.worldPos.xyz;

	float3 viewPos = mul(float4(position, 1.0), c_viewMatrix).xyz;
	float3 viewNormal = mul(float4(normal, 0.0), c_viewMatrix).xyz;

	float3 reflection = reflect(position, normal);

	float3 outputColor = float3(0, 0, 0);

	float3 step = reflection;
	float3 newPosition = position + step;

	for (int i = 0; i < 50; ++i)
	{
		//grab new position and convert to post projection space
		float4 vPos = float4(viewPos, 1.0);
		float4 samplePosition = mul(vPos, c_projMatrix);

		// adjust from NDC to DirectX space..
		samplePosition.xy = (samplePosition.xy / samplePosition.w) * float2(0.5, -0.5) + 0.5;

		float currentDepth = abs(samplePosition.w);
		//I multiply with far plane because we have our depth stored linearly by using viewPos.z / farPlane
		float sampleDepth = abs(depthGP.Sample(DefaultSampler, samplePosition.xy).y);

		//if depth is close enough then sample pixel color
		if (abs(currentDepth - sampleDepth) < 0.001)
		{
			outputColor = albedoGP.Sample(DefaultSampler, samplePosition.xy).xyz;
		}

		//else keep stepping
		step *= 1.0 - 0.5 * max(sign(currentDepth - sampleDepth), 0.0); //progress the step
		newPosition += step * (sign(sampleDepth - currentDepth) + 0.000001); //set new position and loop again
		outputColor = float3(0, 0, 0);
	}
	return float4(outputColor, 1.0);
*/
	float4 camPos = mul(float4(c_cameraPosition, 1.0), c_modelToWorld);
	float4 viewDir = normalize(camPos - input.worldPos);
	//float4 viewDir = normalize(float4(c_cameraPosition, 1.0) - input.worldPos);
	float4 lightDir = float4(normalize(c_pointLight[0].position), 0.0);
	float4 normal = float4(normalize(input.worldNormal.xyz), 0.0);

	float3 totalLighting = float3(0, 0, 0);
	float3 indirectLighting = float3(0, 0, 0);

	float4 diffuseColor = EvalDiffuse(lightDir, normal, input.worldPos);
	float4 direcLight = EvalDirectionalLight(input.worldPos); // in shadow?
	float4 directLighting = diffuseColor * direcLight;

	float4 temp = mul(input.worldPos, c_viewMatrix);

	//float4 dir = float4(normalize(reflect(-viewDir, normal)).xyz, 0);
	float4 dir = float4((float3(30, 0, 0)), 0.0);
	
	float4 hitPos;
	if (RayMarch(input.worldPos, dir, hitPos)) {
		//return float4(1, 0, 0, 1);
		float4 tempScreen = mul(hitPos, c_cameraSpaceViewProj);
		float2 uv = tempScreen.xy / tempScreen.w * float2(0.5, -0.5) + 0.5;
		indirectLighting = albedoGP.Sample(DefaultSampler, uv).xyz;
		//indirectLighting *= EvalDirectionalLight(hitPos).xyz;
	}
	//return float4(0, 1, 0, 1);
	totalLighting = indirectLighting + directLighting.xyz;
	return float4(totalLighting, 1.0);

	//float s = InitRand(input.worldPos.xy);
	//for (int i = 0; i < SAMPLE_NUM; i++) {
	//	float pdf;//採樣的概率
	//	Rand1(s);
	//	vec3 dir = SampleHemisphereUniform(s, pdf);//返回一個局部坐標系

	//	vec3 b1, b2;
	//	vec3 hitPos;//间接光照第一次反射的点，之后反射到我们要着色的像素点上
	//	LocalBasis(N, b1, b2);//根据法线n构建局部坐标系

	//	dir = normalize(mat3(b1, b2, N) * dir);//将采样获取的局部坐标转化到世界坐标
	//	if (RayMarch(vPosWorld.xyz, dir, hitPos)) {
	//		vec3 woNew = vPosWorld.xyz - hitPos;
	//		vec2 uv1 = GetScreenCoordinate(hitPos);
	//		L_indirect += EvalDiffuse(dir, wo, uv) / pdf * EvalDiffuse(wi, dir, uv1) * EvalDirectionalLight(uv1);
	//	}
	//}

	//float seed = InitRand(input.worldPos.xy);
	//float3 viewDir = normalize(c_cameraPosition - input.worldPos.xyz);
	//float3 lightDir = normalize(c_pointLight[0].position);
	//float3 normal = normalize(normalGP.Sample(DefaultSampler, uv).xyz);
	//float4 diffuseColor = float4(c_diffuseColor, 1.0) * max(dot(normal, lightDir), 0.0);
	//float visibility = shadowGP.Load(float3(input.screenPos.xy, 0)).x;
	//float3 direc = c_pointLight[0].lightColor * visibility;

	//float3 dif = EvalDiffuse(lightDir, viewDir, uv); 
	//return diffuseColor;

	//float3 direc = EvalDirectionalLight(uv);
	//float3 directLighting = diffuseColor.xyz * direc;


	//float3 dir = reflect(-viewDir, normal);


	//float3 b1, b2;
	//LocalBasis(normal, b1, b2);
	//dir = normalize(mul(dir, float3x3(normal, b2, b1)));


	//float3 testPoint = input.worldPos.xyz + dir * 10;
	//float4 screenSpace = mul(float4(testPoint, 1.0), c_cameraSpaceViewProj);
	//screenSpace = screenSpace.xyzw / screenSpace.w;
	//float testDepth = screenSpace.z;          //depth of the test point
	//float2 tempUV = screenSpace.xy * 0.5 + 0.5;
	//tempUV.y = 1 - tempUV.y;
	//float4 testNormal = normalGP.Sample(DefaultSampler, tempUV);
	//return testNormal;


	///HERE HERE HERE
	//float3 hitPos;
	//if (RayMarch(input.worldPos.xyz, dir, hitPos)) {
	//	float2 uv1 = mul(float4(hitPos, 1.0), c_cameraSpaceViewProj).xy * 0.5 + 0.5;
	//	uv1.y = 1 - uv1.y;
	//	indirectLighting = albedoGP.Sample(DefaultSampler, uv1).xyz;
	//}
	//else {
	//}
	//totalLighting = indirectLighting + directLighting;
	//return float4(totalLighting.rgb, 1.0);






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
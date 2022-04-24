#pragma once
#include "engineMath.h"

namespace Lights
{
	struct PointLightData
	{
		Vector3 lightColor;
		float padding;

		Vector3 position;
		float innerRadius;

		float outerRadius;
		bool isEnabled;
		float padding2;
		float padding3;
	};

	const int lightNumbers = 8;

	struct LightingConstants
	{
		Vector3 c_ambient;
		float padding;

		PointLightData c_pointLight[lightNumbers];
	};
}


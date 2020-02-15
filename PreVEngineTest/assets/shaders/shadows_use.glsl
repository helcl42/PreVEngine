// shadows client code

const uint SHADOW_MAP_CASCADE_COUNT = 4;
const bool enablePCF = true;
const float defaultShadowFactor = 0.2;

float getShadowRaw(const sampler2DArray depthSampler, const vec4 shadowCoord, const vec2 shadowCoordOffset, const uint cascadeIndex)
{
	const float bias = 0.005;
	float shadow = 1.0;
	if (shadowCoord.z > -1.0 && shadowCoord.z < 1.0)
	{
		float depth = texture(depthSampler, vec3(shadowCoord.xy + shadowCoordOffset, cascadeIndex)).r;
		if (shadowCoord.w > 0.0 && depth < shadowCoord.z - bias) 
		{
			shadow = defaultShadowFactor;
		}
	}
	return shadow;
}

float getShadowPCF(const sampler2DArray depthSampler, const vec4 shadowCoord, const uint cascadeIndex)
{
	const ivec2 texDim = textureSize(depthSampler, 0).xy;
	const float scale = 0.75;
	const float dx = scale * 1.0 / float(texDim.x);
	const float dy = scale * 1.0 / float(texDim.y);
	const int range = 1; // 3 x 3

	float shadowFactor = 0.0;
	int count = 0;
	for (int x = -range; x <= range; x++)
	{
		for (int y = -range; y <= range; y++)
		{
			shadowFactor += getShadowRaw(depthSampler, shadowCoord, vec2(dx * x, dy * y), cascadeIndex);
			count++;
		}
	
	}
	return shadowFactor / count;
}

float getShadow(const sampler2DArray depthSampler, const vec4 shadowCoord, const uint cascadeIndex)
{
	float shadow = 1.0f;
	if(enablePCF)
	{
		shadow = getShadowPCF(depthSampler, shadowCoord, cascadeIndex);
	}
	else
	{
		shadow = getShadowRaw(depthSampler, shadowCoord, vec2(0.0), cascadeIndex);
	}
	return shadow;
}

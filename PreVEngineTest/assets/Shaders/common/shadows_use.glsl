// shadows client code

const bool enablePCF = true;
const float defaultShadowFactor = 0.2;
const float cascadeBiasDamper = 1.5;
const uint SHADOW_MAP_CASCADE_COUNT = 4;

struct ShadowsCascade {
	mat4 viewProjectionMatrix;
	vec4 split;
};

struct Shadows {
	ShadowsCascade cascades[SHADOW_MAP_CASCADE_COUNT];
	uint enabled;
	uint useReverseDepth;
};

float GetShadowRawInternal(in sampler2DArray depthSampler, in vec4 shadowCoord, in vec2 shadowCoordOffset, in uint cascadeIndex, in float depthBias, in uint useReverseDepth)
{
	float shadow = 1.0;
	if (shadowCoord.z >= 0.0 && shadowCoord.z <= 1.0)
	{
		float depth = texture(depthSampler, vec3(shadowCoord.xy + shadowCoordOffset, cascadeIndex)).r;
		if(useReverseDepth != 0)
		{
			if (depth > shadowCoord.z + depthBias)
			{
				shadow = defaultShadowFactor;
			}
		}
		else
		{
			if (depth < shadowCoord.z - depthBias)
			{
				shadow = defaultShadowFactor;
			}
		}
	}
	return shadow;
}

float GetShadowPCFInternal(in sampler2DArray depthSampler, in vec4 shadowCoord, in uint cascadeIndex, in float depthBias, in uint useReverseDepth)
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
			shadowFactor += GetShadowRawInternal(depthSampler, shadowCoord, vec2(dx * x, dy * y), cascadeIndex, depthBias, useReverseDepth);
			count++;
		}
	}
	return shadowFactor / count;
}

float GetShadow(in sampler2DArray depthSampler, in vec4 shadowCoord, in uint cascadeIndex, in float depthBias, in uint useReverseDepth)
{
	float shadow = 1.0f;
	if(enablePCF)
	{
		shadow = GetShadowPCFInternal(depthSampler, shadowCoord, cascadeIndex, depthBias, useReverseDepth);
	}
	else
	{
		shadow = GetShadowRawInternal(depthSampler, shadowCoord, vec2(0.0), cascadeIndex, depthBias, useReverseDepth);
	}
	return shadow;
}

float GetShadow(in sampler2DArray depthSampler, in Shadows shadows, in vec3 viewPosition, in vec3 worldPosition, in float depthBias)
{
	float shadow = 1.0;
	float bias = depthBias;
	if(shadows.enabled != 0)
	{
		uint cascadeIndex = 0;
		for(uint i = 0; i < SHADOW_MAP_CASCADE_COUNT - 1; i++)
		{
			if(viewPosition.z < shadows.cascades[i].split.x)
			{
				cascadeIndex = i + 1;
				bias /= cascadeBiasDamper;
			}
		}
	    vec4 shadowCoord = shadows.cascades[cascadeIndex].viewProjectionMatrix * vec4(worldPosition, 1.0);
		vec4 normalizedShadowCoord = shadowCoord / shadowCoord.w;
		shadow = GetShadow(depthSampler, normalizedShadowCoord, cascadeIndex, bias, shadows.useReverseDepth);
	}
	return shadow;
}
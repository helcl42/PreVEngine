// shadows client code

#extension GL_EXT_nonuniform_qualifier : enable

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
		float depth = textureLod(depthSampler, vec3(shadowCoord.xy + shadowCoordOffset, cascadeIndex), 0.0).r;
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
	const vec2 textureDim = vec2(textureSize(depthSampler, 0).xy);
	const vec2 texelSize = 1.0 / textureDim;

	float shadow = 0.0;

	//// Naive PCF impl -> 16 samples
	// int sampleCount = 0;
	// for(float y = -1.5; y <= 1.5; y += 1.0)
	// {
	// 	for(float x = -1.5; x <= 1.5; x += 1.0)
	// 	{
	// 		if(x >= 0.0 && x < textureDim.x && y >= 0.0 && y < textureDim.y)
	// 		{
	// 			shadow += GetShadowRawInternal(depthSampler, shadowCoord, vec2(texelSize.x * x, texelSize.y * y), cascadeIndex, depthBias, useReverseDepth);
	// 			++sampleCount;
	// 		}
	// 	}
	// }
	// return shadow / max(sampleCount, 1); // avoid zero division

	// Optimized PCF impl -> 4 samples
	vec2 offset = fract(gl_FragCoord.xy * 0.5) + 0.25;  // mod offset.y += offset.x;  // y ^= x in floating point
   	if (offset.y > 1.1)
	{
		offset.y = 0;
	}
	shadow = (
				GetShadowRawInternal(depthSampler, shadowCoord, texelSize * (offset + vec2(-1.5, 0.5)), cascadeIndex, depthBias, useReverseDepth)
				+ GetShadowRawInternal(depthSampler, shadowCoord, texelSize * (offset + vec2(0.5, 0.5)), cascadeIndex, depthBias, useReverseDepth)
				+ GetShadowRawInternal(depthSampler, shadowCoord, texelSize * (offset + vec2(-1.5, -1.5)), cascadeIndex, depthBias, useReverseDepth)
				+ GetShadowRawInternal(depthSampler, shadowCoord, texelSize * (offset + vec2(0.5, -1.5)), cascadeIndex, depthBias, useReverseDepth)
				) * 0.25;
	return shadow;
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
	    vec4 shadowCoord = shadows.cascades[nonuniformEXT(cascadeIndex)].viewProjectionMatrix * vec4(worldPosition, 1.0);
		vec4 normalizedShadowCoord = shadowCoord / shadowCoord.w;
		shadow = GetShadow(depthSampler, normalizedShadowCoord, nonuniformEXT(cascadeIndex), bias, shadows.useReverseDepth);
	}
	return shadow;
}
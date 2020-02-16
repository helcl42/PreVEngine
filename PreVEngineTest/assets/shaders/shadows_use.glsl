// shadows client code

const bool enablePCF = true;
const float defaultShadowFactor = 0.2;
const uint SHADOW_MAP_CASCADE_COUNT = 4;

struct ShadowsCascade {
	mat4 viewProjectionMatrix;
	vec4 split;
};

struct Shadows {
	ShadowsCascade cascades[SHADOW_MAP_CASCADE_COUNT];
	uint enabled;
};

float getShadowRaw(in sampler2DArray depthSampler, in vec4 shadowCoord, in vec2 shadowCoordOffset, in uint cascadeIndex)
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

float getShadowPCF(in sampler2DArray depthSampler, in vec4 shadowCoord, in uint cascadeIndex)
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

float getShadowInternal(in sampler2DArray depthSampler, in vec4 shadowCoord, in uint cascadeIndex)
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

float getShadow(in sampler2DArray depthSampler, in Shadows shadows, in vec3 viewPosition, in vec3 worldPosition)
{
	float shadow = 1.0;	
	if(shadows.enabled != 0)
	{
		uint cascadeIndex = 0;
		for(uint i = 0; i < SHADOW_MAP_CASCADE_COUNT - 1; i++) 
		{
			if(viewPosition.z < shadows.cascades[i].split.x)
			{
				cascadeIndex = i + 1;
			}
		}

	    vec4 shadowCoord = shadows.cascades[cascadeIndex].viewProjectionMatrix * vec4(worldPosition, 1.0);
		vec4 normalizedShadowCoord = shadowCoord / shadowCoord.w;
		shadow = getShadowInternal(depthSampler, normalizedShadowCoord, cascadeIndex);
	}
	return shadow;
}
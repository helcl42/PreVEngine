#version 450
#extension GL_ARB_separate_shader_objects : enable

#define SHADOW_MAP_CASCADE_COUNT 4

layout(std140, binding = 1) uniform UniformBufferObject {
    vec4 cascadeSplits[SHADOW_MAP_CASCADE_COUNT];
	mat4 cascadeViewProjecionMatrix[SHADOW_MAP_CASCADE_COUNT];
	vec4 lightDirection;
	bool isCastedByShadows;
} uboFS;

layout(binding = 2) uniform sampler2D textureSampler;
layout(binding = 3) uniform sampler2DArray depthSampler;

layout(location = 0) in vec2 inTextureCoord;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inViewPosition;
layout(location = 3) in vec3 inWorldPosition;

layout(location = 0) out vec4 outColor;

const float ambient = 0.2;
const vec3 lightColor = vec3(1.0);

const bool enablePCF = true;
const mat4 biasMat = mat4( 
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.0, 1.0 
);

float getShadowInternal(const vec4 shadowCoord, const vec2 shadowCoordOffset, const uint cascadeIndex)
{
	const float bias = 0.005;
	float shadow = 1.0;
	if (shadowCoord.z > -1.0 && shadowCoord.z < 1.0)
	{
		float depth = texture(depthSampler, vec3(shadowCoord.xy + shadowCoordOffset, cascadeIndex)).r;
		if (shadowCoord.w > 0.0 && depth < shadowCoord.z - bias) 
		{
			shadow = ambient;
		}
	}
	return shadow;
}

float getShadowPCFInternal(const vec4 shadowCoord, const uint cascadeIndex)
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
			shadowFactor += getShadowInternal(shadowCoord, vec2(dx * x, dy * y), cascadeIndex);
			count++;
		}
	
	}
	return shadowFactor / count;
}

float getShadow(const vec4 shadowCoord, const uint cascadeIndex)
{
	float shadow = 1.0f;
	if(enablePCF)
	{
		shadow = getShadowPCFInternal(shadowCoord, cascadeIndex);
	}
	else
	{
		shadow = getShadowInternal(shadowCoord, vec2(0.0), cascadeIndex);
	}
	return shadow;
}

void main() 
{
	// Discard too transparent materials
	vec4 textureColor = texture(textureSampler, inTextureCoord);
	if (textureColor.a < 0.5) 
	{
		discard;
	}

	// Get cascade index for the current fragment's view position
	uint cascadeIndex = 0;
	for(uint i = 0; i < SHADOW_MAP_CASCADE_COUNT - 1; i++) 
	{
		if(inViewPosition.z < uboFS.cascadeSplits[i].x) 
		{	
			cascadeIndex = i + 1;
		}
	}

	// Depth compare for shadowing
	vec4 shadowCoord = (biasMat * uboFS.cascadeViewProjecionMatrix[cascadeIndex]) * vec4(inWorldPosition, 1.0);

	vec4 normalizedShadowCoord = shadowCoord / shadowCoord.w;

	float shadow = 1.0f;
	 if(uboFS.isCastedByShadows)
	{
		shadow = getShadow(normalizedShadowCoord, cascadeIndex);
	}

	const vec3 N = normalize(inNormal);
	const vec3 L = normalize(-uboFS.lightDirection.xyz);
	const vec3 diffuse = max(dot(N, L), ambient) * textureColor.rgb;

	outColor = vec4(diffuse * lightColor, 1.0);
	outColor *= shadow;
	outColor.a = textureColor.a;
}
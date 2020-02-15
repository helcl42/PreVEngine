#version 450
#extension GL_ARB_separate_shader_objects : enable

const uint SHADOW_MAP_CASCADE_COUNT = 4;
const uint MAX_LIGHT_COUNT = 4;

layout(std140, binding = 1) uniform UniformBufferObject {
	mat4 cascadeViewProjecionMatrices[SHADOW_MAP_CASCADE_COUNT];
    
	vec4 cascadeSplits[SHADOW_MAP_CASCADE_COUNT];

	vec4 lightDirection;

	vec4 lightColors[MAX_LIGHT_COUNT];

	vec4 attenuations[MAX_LIGHT_COUNT];

	uint realCountOfLights;
	float ambientLight;
	float shineDamper;
	float reflectivity;

	vec4 fogColor;

	vec4 selectedColor;

	uint selected;
	uint castedByShadows;
	uint shadowsEnabled;
} uboFS;

layout(binding = 2) uniform sampler2D textureSampler;
layout(binding = 3) uniform sampler2DArray depthSampler;
layout(binding = 4) uniform sampler2D extraInfoSampler;

layout(location = 0) in vec2 inTextureCoord;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inWorldPosition;
layout(location = 3) in vec3 inViewPosition;
layout(location = 4) in vec3 inToLightVectors[MAX_LIGHT_COUNT];
layout(location = 8) in vec3 inToCameraVector;
layout(location = 9) in float inVisibility;

layout(location = 0) out vec4 outColor;

const bool enablePCF = true;

float getShadowInternal(const vec4 shadowCoord, const vec2 shadowCoordOffset, const uint cascadeIndex)
{
	const float bias = 0.005;
	float shadow = 1.0;
	if (shadowCoord.z > -1.0 && shadowCoord.z < 1.0)
	{
		float depth = texture(depthSampler, vec3(shadowCoord.xy + shadowCoordOffset, cascadeIndex)).r;
		if (shadowCoord.w > 0.0 && depth < shadowCoord.z - bias) 
		{
			shadow = uboFS.ambientLight;
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
	vec4 textureColor = texture(textureSampler, inTextureCoord);
	if (textureColor.a < 0.5) 
	{
		discard;
	}

	float shadow = 1.0;	
	if(uboFS.castedByShadows != 0)
	{
		uint cascadeIndex = 0;
		for(uint i = 0; i < SHADOW_MAP_CASCADE_COUNT - 1; i++) 
		{
			if(inViewPosition.z < uboFS.cascadeSplits[i].x) 
			{	
				cascadeIndex = i + 1;
			}
		}

		vec4 shadowCoord = uboFS.cascadeViewProjecionMatrices[cascadeIndex] * vec4(inWorldPosition, 1.0);
		vec4 normalizedShadowCoord = shadowCoord / shadowCoord.w;
		shadow = getShadow(normalizedShadowCoord, cascadeIndex);
	}

	vec3 unitNormal = normalize(inNormal);
	vec3 unitToCameraVector = normalize(inToCameraVector);

	vec3 totalDiffuse = vec3(0.0);
	vec3 totalSpecular = vec3(0.0);

	for (int i = 0; i < uboFS.realCountOfLights; i++)
	{
		float toLightDistance = length(inToLightVectors[i]);
		float attenuationFactor = uboFS.attenuations[i].x + (uboFS.attenuations[i].y * toLightDistance) + (uboFS.attenuations[i].z * toLightDistance * toLightDistance);

		vec3 unitToLightVector = normalize(inToLightVectors[i]);

		// diffuse
		float nDotL = dot(unitNormal, unitToLightVector);

		float brightness = max(nDotL, 0.0);
		totalDiffuse = totalDiffuse + (brightness * uboFS.lightColors[i].xyz) / attenuationFactor;

		// specular
		vec3 lightDirection = -unitToLightVector;
		vec3 reflectedLightDirection = reflect(lightDirection, unitNormal);
		float specularFactor = dot(reflectedLightDirection, unitToCameraVector);
		specularFactor = max(specularFactor, 0.0);
		float dampedFactor = pow(specularFactor, uboFS.shineDamper);

		totalSpecular = totalSpecular + (dampedFactor * uboFS.reflectivity * uboFS.lightColors[i].xyz) / attenuationFactor;
	}
	totalDiffuse = max(totalDiffuse * shadow, 0.0) + uboFS.ambientLight;
	totalSpecular = totalSpecular * shadow;

	vec4 baseResultColor = vec4(totalDiffuse, 1.0) * textureColor + vec4(totalSpecular, 1.0);
	vec4 resultColor = mix(vec4(uboFS.fogColor.xyz, 1.0), baseResultColor, inVisibility);

	if (uboFS.selected != 0)
	{
		resultColor = mix(resultColor, uboFS.selectedColor, 0.5);
	}

	outColor = resultColor;
}
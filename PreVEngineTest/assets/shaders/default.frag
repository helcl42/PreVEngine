#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "shadows_use.glsl"
#include "lights.glsl"

const uint MAX_LIGHT_COUNT = 4;

struct Light {
	vec4 position;

	vec4 color;

	vec4 attenuation;
};

struct Lightning {
	Light lights[MAX_LIGHT_COUNT];

	uint realCountOfLights;

	float ambientFactor;
};


layout(std140, binding = 1) uniform UniformBufferObject {
	Shadows shadows;

	Lightning lightning;

	vec4 fogColor;

	vec4 selectedColor;

	uint selected;
	uint castedByShadows;	
	float shineDamper;
	float reflectivity;
} uboFS;

layout(binding = 2) uniform sampler2D textureSampler;
layout(binding = 3) uniform sampler2DArray depthSampler;
layout(binding = 4) uniform sampler2D extraInfoSampler;

layout(location = 0) in vec2 inTextureCoord;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inWorldPosition;
layout(location = 3) in vec3 inViewPosition;
layout(location = 4) in vec3 inToCameraVector;
layout(location = 5) in float inVisibility;

layout(location = 0) out vec4 outColor;

// vec4 GetShadowCoord(in mat4 cascadeViewProjecionMatrices[SHADOW_MAP_CASCADE_COUNT], in uint cascadeIndex, in vec3 worldPosition)
// {
// 	vec4 shadowCoord = cascadeViewProjecionMatrices[cascadeIndex] * vec4(worldPosition, 1.0);
// 	vec4 normalizedShadowCoord = shadowCoord / shadowCoord.w;
// 	return normalizedShadowCoord;
// }

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
		shadow = getShadow(depthSampler, uboFS.shadows, inViewPosition, inWorldPosition);
	}

	const vec3 unitNormal = normalize(inNormal);
	const vec3 unitToCameraVector = normalize(inToCameraVector);

	vec3 totalDiffuse = vec3(0.0);
	vec3 totalSpecular = vec3(0.0);
	for (uint i = 0; i < uboFS.lightning.realCountOfLights; i++)
	{
		const Light light = uboFS.lightning.lights[i];

		const vec3 toLightVector = light.position.xyz - inWorldPosition.xyz;
		const float attenuationFactor = getAttenuationFactor(light.attenuation.xyz, toLightVector);
		const vec3 unitToLightVector = normalize(toLightVector);
		totalDiffuse += getDiffuseLight(unitNormal, unitToLightVector, light.color.xyz, attenuationFactor);
		totalSpecular += getSpecularLight(unitNormal, unitToLightVector, unitToCameraVector, light.color.xyz, attenuationFactor, uboFS.shineDamper, uboFS.reflectivity);
	}
	totalDiffuse = max(totalDiffuse * shadow, 0.0) + uboFS.lightning.ambientFactor;
	totalSpecular = totalSpecular * shadow;

	vec4 baseResultColor = vec4(totalDiffuse, 1.0) * textureColor + vec4(totalSpecular, 1.0);

	vec4 resultColor = mix(vec4(uboFS.fogColor.xyz, 1.0), baseResultColor, inVisibility);

	if (uboFS.selected != 0)
	{
		resultColor = mix(resultColor, uboFS.selectedColor, 0.5);
	}

	outColor = resultColor;
}
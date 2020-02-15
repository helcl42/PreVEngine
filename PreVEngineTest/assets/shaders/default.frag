#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "shadows_use.glsl"
#include "lights.glsl"

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

void main() 
{
	vec4 textureColor = texture(textureSampler, inTextureCoord);
	if (textureColor.a < 0.5) 
	{
		discard;
	}

	float shadow = 1.0;	
	if(uboFS.castedByShadows != 0 && uboFS.shadowsEnabled != 0)
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
		shadow = getShadow(depthSampler, normalizedShadowCoord, cascadeIndex);
	}

	const vec3 unitNormal = normalize(inNormal);
	const vec3 unitToCameraVector = normalize(inToCameraVector);

	vec3 totalDiffuse = vec3(0.0);
	vec3 totalSpecular = vec3(0.0);
	for (int i = 0; i < uboFS.realCountOfLights; i++)
	{
		float attenuationFactor = getAttenuationFactor(uboFS.attenuations[i].xyz, inToLightVectors[i]);
		vec3 unitToLightVector = normalize(inToLightVectors[i]);
		totalDiffuse += getDiffuseLight(unitNormal, unitToLightVector, uboFS.lightColors[i].xyz, attenuationFactor);
		totalSpecular += getSpecularLight(unitNormal, unitToLightVector, unitToCameraVector, uboFS.lightColors[i].xyz, attenuationFactor, uboFS.shineDamper, uboFS.reflectivity);
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
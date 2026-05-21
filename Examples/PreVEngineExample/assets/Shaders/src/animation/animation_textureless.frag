#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "../common/shadows_use.glsl"
#include "../common/lights.glsl"

layout(std140, binding = 1) uniform UniformBufferObject {
	Shadows shadows;

	Lightning lightning;

	Material material;

	vec4 fogColor;

	vec4 selectedColor;

	uint selected;
	uint castedByShadows;
} uboFS;

layout(binding = 2) uniform texture2DArray depthTexture;
layout(binding = 3) uniform sampler depthSampler;

layout(location = 0) in vec2 inTextureCoord;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inWorldPosition;
layout(location = 3) in vec3 inViewPosition;
layout(location = 4) in vec3 inToCameraVector;
layout(location = 5) in float inVisibility;
layout(location = 6) in vec3 inToLightVector0;
layout(location = 7) in vec3 inToLightVector1;
layout(location = 8) in vec3 inToLightVector2;
layout(location = 9) in vec3 inToLightVector3;
layout(location = 10) in float inClipDistance;

layout(location = 0) out vec4 outColor;

void main()
{
	vec3 inToLightVector_arr[4] = vec3[4](inToLightVector0, inToLightVector1, inToLightVector2, inToLightVector3);
	if (inClipDistance < 0.0)
	{
		discard; 
	}

	float shadow = 1.0;	
	if(uboFS.castedByShadows != 0)
	{
			shadow = GetShadow(depthTexture, depthSampler, uboFS.shadows, inViewPosition, inWorldPosition, 0.005);
	}

	const vec3 unitNormal = normalize(inNormal);
	const vec3 unitToCameraVector = normalize(inToCameraVector);

	vec3 totalDiffuse = vec3(0.0);
	vec3 totalSpecular = vec3(0.0);
	for (uint i = 0; i < uboFS.lightning.realCountOfLights; i++)
	{
		const Light light = uboFS.lightning.lights[i];

		const vec3 toLightVector = inToLightVector_arr[i];
		const vec3 unitToLightVector = normalize(toLightVector);

		const float attenuationFactor = GetAttenuationFactor(light.attenuation.xyz, toLightVector);
		totalDiffuse += GetDiffuseColor(unitNormal, unitToLightVector, light.color.xyz, attenuationFactor);
		totalSpecular += GetSpecularColor(unitNormal, unitToLightVector, unitToCameraVector, light.color.xyz, attenuationFactor, uboFS.material.shineDamper, uboFS.material.reflectivity);
	}
	totalDiffuse = totalDiffuse * shadow + uboFS.lightning.ambientFactor;
	totalSpecular = totalSpecular * shadow;

	vec4 baseResultColor = vec4(totalDiffuse, 1.0) * uboFS.material.color + vec4(totalSpecular, 0.0);
	vec4 resultColor = mix(vec4(uboFS.fogColor.xyz, 1.0), baseResultColor, inVisibility);

	if (uboFS.selected != 0)
	{
		resultColor = mix(resultColor, uboFS.selectedColor, 0.5);
	}

	outColor = resultColor;
}
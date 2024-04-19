#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_nonuniform_qualifier : enable

#include "../common/common.glsl"
#include "../common/shadows_use.glsl"
#include "../common/lights.glsl"
#include "../common/cone_step_mapping_use.glsl"
#include "../common/normal_mapping_use.glsl"

const uint MATERIAL_COUNT = 4;

layout(std140, binding = 1) uniform UniformBufferObject {
	Shadows shadows;

	Lightning lightning;

	Material material[MATERIAL_COUNT];

	vec4 fogColor;

	vec4 selectedColor;

	uint selected;
	uint castedByShadows;
    float minHeight;
    float maxHeight;

	vec4 heightSteps[MATERIAL_COUNT];

	vec4 heightScale[MATERIAL_COUNT];

	float heightTransitionRange;
	uint numLayers;
} uboFS;

layout(binding = 2) uniform sampler2D colorSampler[MATERIAL_COUNT];
layout(binding = 3) uniform sampler2D normalSampler[MATERIAL_COUNT];
layout(binding = 4) uniform sampler2D heightSampler[MATERIAL_COUNT];
layout(binding = 5) uniform sampler2DArray depthSampler;

layout(location = 0) in vec2 inTextureCoord;
layout(location = 1) in vec3 inWorldPosition;
layout(location = 2) in vec3 inViewPosition;
layout(location = 3) in float inVisibility;
layout(location = 4) in vec3 inToCameraVectorTangentSpace;
layout(location = 5) in vec3 inPositionTangentSpace;
layout(location = 6) in vec3 inToLightVectorTangentSpace[MAX_LIGHT_COUNT];

layout(location = 0) out vec4 outColor;

void main()
{
    const float heightRange = abs(uboFS.maxHeight) + abs(uboFS.minHeight);
    const float normalizedHeight = (inWorldPosition.y + abs(uboFS.minHeight)) / heightRange;

	const vec3 rayDirection = normalize(inPositionTangentSpace);

    vec4 textureColor = vec4(1.0, 1.0, 1.0, 1.0);
	vec3 normal = vec3(0.0, 1.0, 0.0);
	float shineDamper = 1.0f;
	float reflectivity = 1.0f;

    for(uint i = 0; i < MATERIAL_COUNT; ++i)
    {
		const uint uniformIndex = nonuniformEXT(i);
		const uint nextUniformIndex = nonuniformEXT(i + 1);

        if(i < MATERIAL_COUNT - 1)
        {
            if(normalizedHeight > uboFS.heightSteps[uniformIndex].x - uboFS.heightTransitionRange && normalizedHeight < uboFS.heightSteps[uniformIndex].x + uboFS.heightTransitionRange)
            {
                float ratio = (normalizedHeight - uboFS.heightSteps[uniformIndex].x + uboFS.heightTransitionRange) / (2.0 * uboFS.heightTransitionRange);

				vec2 uv1 = RelaxedConeStepMapping(heightSampler[uniformIndex], uboFS.heightScale[uniformIndex].x, uboFS.numLayers, inTextureCoord, rayDirection);
				vec2 uv2 = RelaxedConeStepMapping(heightSampler[nextUniformIndex], uboFS.heightScale[nextUniformIndex].x, uboFS.numLayers, inTextureCoord, rayDirection);

				vec3 normal1 = NormalMapping(normalSampler[uniformIndex], uv1);
				vec3 normal2 = NormalMapping(normalSampler[nextUniformIndex], uv2);
				normal = mix(normal1, normal2, ratio);

                vec4 color1 = texture(colorSampler[uniformIndex], uv1);
                vec4 color2 = texture(colorSampler[nextUniformIndex], uv2);
                textureColor = mix(color1, color2, ratio);

				float shineDamper1 = uboFS.material[uniformIndex].shineDamper;
				float shineDamper2 = uboFS.material[nextUniformIndex].shineDamper;
				shineDamper = mix(shineDamper1, shineDamper2, ratio);

				float reflectivity1 = uboFS.material[uniformIndex].reflectivity;
				float reflectivity2 = uboFS.material[nextUniformIndex].reflectivity;
				reflectivity = mix(reflectivity1, reflectivity2, ratio);
                break;
            }
			else if(normalizedHeight < uboFS.heightSteps[uniformIndex].x - uboFS.heightTransitionRange)
			{
				vec2 uv = RelaxedConeStepMapping(heightSampler[uniformIndex], uboFS.heightScale[uniformIndex].x, uboFS.numLayers, inTextureCoord, rayDirection);

				normal = NormalMapping(normalSampler[uniformIndex], uv);
				textureColor = texture(colorSampler[uniformIndex], uv);
				shineDamper = uboFS.material[uniformIndex].shineDamper;
				reflectivity = uboFS.material[uniformIndex].reflectivity;
				break;
			}
        }
        else
        {
			vec2 uv = RelaxedConeStepMapping(heightSampler[uniformIndex], uboFS.heightScale[uniformIndex].x, uboFS.numLayers, inTextureCoord, rayDirection);

			normal = NormalMapping(normalSampler[uniformIndex], uv);
			textureColor = texture(colorSampler[uniformIndex], uv);
			shineDamper = uboFS.material[uniformIndex].shineDamper;
			reflectivity = uboFS.material[uniformIndex].reflectivity;
        }
    }

	float shadow = 1.0;
	if(uboFS.castedByShadows != 0)
	{
		shadow = GetShadow(depthSampler, uboFS.shadows, inViewPosition, inWorldPosition, 0.02);
	}

	const vec3 unitToCameraVector = normalize(inToCameraVectorTangentSpace - inPositionTangentSpace);

	vec3 totalDiffuse = vec3(0.0);
	vec3 totalSpecular = vec3(0.0);
	for (uint i = 0; i < uboFS.lightning.realCountOfLights; ++i)
	{
		const Light light = uboFS.lightning.lights[i];

		const vec3 toLightVector = inToLightVectorTangentSpace[i] - inPositionTangentSpace;
		const vec3 unitToLightVector = normalize(toLightVector);

		const float attenuationFactor = GetAttenuationFactor(light.attenuation.xyz, toLightVector);
		totalDiffuse += GetDiffuseColor(normal, unitToLightVector, light.color.xyz, attenuationFactor);
		totalSpecular += GetSpecularColor(normal, unitToLightVector, unitToCameraVector, light.color.xyz, attenuationFactor, shineDamper, reflectivity);
	}
	totalDiffuse = totalDiffuse * shadow + uboFS.lightning.ambientFactor;
	totalSpecular = totalSpecular * shadow;

	vec4 baseResultColor = vec4(totalDiffuse, 1.0) * textureColor + vec4(totalSpecular, 0.0);
	vec4 resultColor = mix(vec4(uboFS.fogColor.xyz, 1.0), baseResultColor, inVisibility);

	if (uboFS.selected != 0)
	{
		resultColor = mix(resultColor, uboFS.selectedColor, 0.5);
	}
	outColor = resultColor;
}
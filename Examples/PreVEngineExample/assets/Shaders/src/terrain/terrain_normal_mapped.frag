#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "../common/shadows_use.glsl"
#include "../common/lights.glsl"

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

	float heightTransitionRange;
	uint hasNormalMap;
} uboFS;

layout(binding = 2) uniform texture2DArray colorTextures;
layout(binding = 3) uniform sampler colorSampler;
layout(binding = 4) uniform texture2DArray normalTextures;
layout(binding = 5) uniform sampler normalSampler;
layout(binding = 6) uniform texture2DArray depthTexture;
layout(binding = 7) uniform sampler depthSampler;

vec4 sampleColorTexture(uint idx, vec2 uv, vec2 ddx, vec2 ddy) {
    return textureGrad(sampler2DArray(colorTextures, colorSampler), vec3(uv, float(idx)), ddx, ddy);
}

vec3 sampleNormalMap(uint idx, vec2 uv, vec2 ddx, vec2 ddy) {
    vec3 n = textureGrad(sampler2DArray(normalTextures, normalSampler), vec3(uv, float(idx)), ddx, ddy).xyz;
    return normalize(2.0 * normalize(n) - 1.0);
}

layout(location = 0) in vec2 inTextureCoord;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inWorldPosition;
layout(location = 3) in vec3 inViewPosition;
layout(location = 4) in float inVisibility;
layout(location = 5) in vec3 inToCameraVectorTangentSpace;
layout(location = 6) in vec3 inPositionTangentSpace;
layout(location = 7) in vec3 inToLightVectorTangentSpace0;
layout(location = 8) in vec3 inToLightVectorTangentSpace1;
layout(location = 9) in vec3 inToLightVectorTangentSpace2;
layout(location = 10) in vec3 inToLightVectorTangentSpace3;
layout(location = 11) in float inClipDistance;

layout(location = 0) out vec4 outColor;

void main()
{
	vec3 inToLightVectorTangentSpace_arr[4] = vec3[4](inToLightVectorTangentSpace0, inToLightVectorTangentSpace1, inToLightVectorTangentSpace2, inToLightVectorTangentSpace3);
	if (inClipDistance < 0.0)
	{
		discard; 
	}

	const vec3 unitToCameraVector = normalize(inToCameraVectorTangentSpace - inPositionTangentSpace);

    const float heightRange = abs(uboFS.maxHeight) + abs(uboFS.minHeight);
    const float normalizedHeight = (inWorldPosition.y + abs(uboFS.minHeight)) / heightRange;

    vec4 textureColor = vec4(1.0, 1.0, 1.0, 1.0);
	vec3 normal = vec3(0.0, 1.0, 0.0);
	float shineDamper = 1.0f;
	float reflectivity = 1.0f;
	vec2 ddx = dFdx(inTextureCoord);
	vec2 ddy = dFdy(inTextureCoord);
    for(uint i = 0; i < MATERIAL_COUNT; i++)
    {
        if(i < MATERIAL_COUNT - 1)
        {
            if(normalizedHeight > uboFS.heightSteps[i].x - uboFS.heightTransitionRange && normalizedHeight < uboFS.heightSteps[i].x + uboFS.heightTransitionRange)
            {
                float ratio = (normalizedHeight - uboFS.heightSteps[i].x + uboFS.heightTransitionRange) / (2 * uboFS.heightTransitionRange);

                vec4 color1 = sampleColorTexture(i, inTextureCoord, ddx, ddy);
                vec4 color2 = sampleColorTexture(i + 1, inTextureCoord, ddx, ddy);
                textureColor = mix(color1, color2, ratio);

				vec3 normal1;
				vec3 normal2;
				if(uboFS.hasNormalMap != 0)
				{
					normal1 = sampleNormalMap(i, inTextureCoord, ddx, ddy);
					normal2 = sampleNormalMap(i + 1, inTextureCoord, ddx, ddy);
				}
				else
				{
					normal1 = inNormal;
					normal2 = inNormal;
				}
				normal = mix(normal1, normal2, ratio);

				float shineDamper1 = uboFS.material[i].shineDamper;
				float shineDamper2 = uboFS.material[i + 1].shineDamper;
				shineDamper = mix(shineDamper1, shineDamper2, ratio);

				float reflectivity1 = uboFS.material[i].reflectivity;
				float reflectivity2 = uboFS.material[i + 1].reflectivity;
				reflectivity = mix(reflectivity1, reflectivity2, ratio);
                break;
            }
			else if(normalizedHeight < uboFS.heightSteps[i].x - uboFS.heightTransitionRange)
			{
				textureColor = sampleColorTexture(i, inTextureCoord, ddx, ddy);
				normal = sampleNormalMap(i, inTextureCoord, ddx, ddy);
				shineDamper = uboFS.material[i].shineDamper;
				reflectivity = uboFS.material[i].reflectivity;
				break;
			}
        }
        else
        {
			textureColor = sampleColorTexture(i, inTextureCoord, ddx, ddy);
			normal = sampleNormalMap(i, inTextureCoord, ddx, ddy);
			shineDamper = uboFS.material[i].shineDamper;
			reflectivity = uboFS.material[i].reflectivity;
        }
    }

	float shadow = 1.0;
	if(uboFS.castedByShadows != 0)
	{
		shadow = GetShadow(depthTexture, depthSampler, uboFS.shadows, inViewPosition, inWorldPosition, 0.005);
	}

	vec3 totalDiffuse = vec3(0.0);
	vec3 totalSpecular = vec3(0.0);
	for (uint i = 0; i < uboFS.lightning.realCountOfLights; i++)
	{
		const Light light = uboFS.lightning.lights[i];

		const vec3 toLightVector = inToLightVectorTangentSpace_arr[i] - inPositionTangentSpace;
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
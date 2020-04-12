#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "shadows_use.glsl"
#include "lights.glsl"

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
} uboFS;

layout(binding = 2) uniform sampler2D textureSampler[MATERIAL_COUNT];
layout(binding = 3) uniform sampler2D normalSampler[MATERIAL_COUNT];
layout(binding = 4) uniform sampler2DArray depthSampler;

layout(location = 0) in vec2 inTextureCoord;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inWorldPosition;
layout(location = 3) in vec3 inViewPosition;
layout(location = 4) in float inVisibility;
layout(location = 5) in vec3 inToCameraVectorTangentSpace;
layout(location = 6) in vec3 inWorldPositionTangentSpace;
layout(location = 7) in vec3 inToLightVectorTangentSpace[MAX_LIGHT_COUNT];

layout(location = 0) out vec4 outColor;

void main() 
{
	const vec3 viewDirectionTangentSpace = normalize(inToCameraVectorTangentSpace - inWorldPositionTangentSpace);

    const float heightRange = abs(uboFS.maxHeight) + abs(uboFS.minHeight);
    const float normalizedHeight = (inWorldPosition.y + abs(uboFS.minHeight)) / heightRange;

    vec4 textureColor = vec4(1.0, 1.0, 1.0, 1.0);
	vec3 normal = vec3(0.0, 1.0, 0.0);
	float shineDamper = 1.0f;
	float reflectivity = 1.0f;
    for(uint i = 0; i < MATERIAL_COUNT; i++) 
    {
        if(i < MATERIAL_COUNT - 1)
        {
            if(normalizedHeight > uboFS.heightSteps[i].x - uboFS.heightTransitionRange && normalizedHeight < uboFS.heightSteps[i].x + uboFS.heightTransitionRange)
            {				
                float ratio = (normalizedHeight - uboFS.heightSteps[i].x + uboFS.heightTransitionRange) / (2 * uboFS.heightTransitionRange);

                vec4 color1 = texture(textureSampler[i], inTextureCoord);
                vec4 color2 = texture(textureSampler[i + 1], inTextureCoord);
                textureColor = mix(color1, color2, ratio);

				vec3 normal1 = normalize(2.0 * texture(normalSampler[i], inTextureCoord).xyz - 1.0);
				vec3 normal2 = normalize(2.0 * texture(normalSampler[i + 1], inTextureCoord).xyz - 1.0);
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
				textureColor = texture(textureSampler[i], inTextureCoord);
				normal = normalize(2.0 * texture(normalSampler[i], inTextureCoord).xyz - 1.0);
				shineDamper = uboFS.material[i].shineDamper;
				reflectivity = uboFS.material[i].reflectivity;
				break;
			}
            else if(normalizedHeight > uboFS.heightSteps[i].x + uboFS.heightTransitionRange && normalizedHeight < uboFS.heightSteps[i + 1].x - uboFS.heightTransitionRange)
            {
				textureColor = texture(textureSampler[i], inTextureCoord);
				normal = normalize(2.0 * texture(normalSampler[i], inTextureCoord).xyz - 1.0);
				shineDamper = uboFS.material[i].shineDamper;
				reflectivity = uboFS.material[i].reflectivity;
            }
        }
        else
        {
			textureColor = texture(textureSampler[i], inTextureCoord);
			normal = normalize(2.0 * texture(normalSampler[i], inTextureCoord).xyz - 1.0);
			shineDamper = uboFS.material[i].shineDamper;
			reflectivity = uboFS.material[i].reflectivity;
        }
    }

	float shadow = 1.0;	
	if(uboFS.castedByShadows != 0)
	{
		// shadow = GetShadowFull(depthSampler, uboFS.shadows, inViewPosition, inWorldPosition, 0.02);

		uint cascadeIndex = 0;
		for(uint i = 0; i < SHADOW_MAP_CASCADE_COUNT - 1; i++) 
		{
			if(inViewPosition.z < uboFS.shadows.cascades[i].split.x)
			{
				cascadeIndex = i + 1;
			}
		}

	    vec4 shadowCoord = uboFS.shadows.cascades[cascadeIndex].viewProjectionMatrix * vec4(inWorldPosition, 1.0);
		vec4 normalizedShadowCoord = shadowCoord / shadowCoord.w;
		shadow = GetShadow(depthSampler, normalizedShadowCoord, cascadeIndex, 0.02);
	}

	const vec3 unitNormal = normalize(normal);
	const vec3 unitToCameraVector = normalize(viewDirectionTangentSpace);

	vec3 totalDiffuse = vec3(0.0);
	vec3 totalSpecular = vec3(0.0);
	for (uint i = 0; i < uboFS.lightning.realCountOfLights; i++)
	{
		const Light light = uboFS.lightning.lights[i];

		const vec3 toLightVector = inToLightVectorTangentSpace[i] - inWorldPositionTangentSpace;
		const vec3 unitToLightVector = normalize(toLightVector);

		const float attenuationFactor = GetAttenuationFactor(light.attenuation.xyz, toLightVector);
		totalDiffuse += GetDiffuseColor(unitNormal, unitToLightVector, light.color.xyz, attenuationFactor);
		totalSpecular += GetSpecularColor(unitNormal, unitToLightVector, unitToCameraVector, light.color.xyz, attenuationFactor, shineDamper, reflectivity);
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
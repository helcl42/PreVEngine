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
} uboFS;

layout(binding = 2) uniform texture2D colorTexture0;
layout(binding = 3) uniform texture2D colorTexture1;
layout(binding = 4) uniform texture2D colorTexture2;
layout(binding = 5) uniform texture2D colorTexture3;
layout(binding = 6) uniform sampler colorSampler;
layout(binding = 7) uniform texture2DArray depthTexture;
layout(binding = 8) uniform sampler depthSampler;

vec4 sampleColorTexture(uint idx, vec2 uv, vec2 ddx, vec2 ddy) {
    if (idx == 0) return textureGrad(sampler2D(colorTexture0, colorSampler), uv, ddx, ddy);
    else if (idx == 1) return textureGrad(sampler2D(colorTexture1, colorSampler), uv, ddx, ddy);
    else if (idx == 2) return textureGrad(sampler2D(colorTexture2, colorSampler), uv, ddx, ddy);
    else return textureGrad(sampler2D(colorTexture3, colorSampler), uv, ddx, ddy);
}

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

    const float heightRange = abs(uboFS.maxHeight) + abs(uboFS.minHeight);
    const float normalizedHeight = (inWorldPosition.y + abs(uboFS.minHeight)) / heightRange;

    vec4 textureColor = vec4(1.0, 1.0, 0.0, 1.0);
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
				shineDamper = uboFS.material[i].shineDamper;
				reflectivity = uboFS.material[i].reflectivity;
				break;
			}
        }
        else
        {
			textureColor = sampleColorTexture(i, inTextureCoord, ddx, ddy);
			shineDamper = uboFS.material[i].shineDamper;
			reflectivity = uboFS.material[i].reflectivity;
            break;
        }
    }

	float shadow = 1.0;
	if(uboFS.castedByShadows != 0)
	{
				shadow = GetShadow(depthTexture, depthSampler, uboFS.shadows, inViewPosition, inWorldPosition, 0.02);
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
		totalSpecular += GetSpecularColor(unitNormal, unitToLightVector, unitToCameraVector, light.color.xyz, attenuationFactor, shineDamper, reflectivity);
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
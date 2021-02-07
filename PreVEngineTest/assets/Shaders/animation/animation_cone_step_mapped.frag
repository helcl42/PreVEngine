#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "../common/shadows_use.glsl"
#include "../common/lights.glsl"
#include "../common/cone_step_mapping_use.glsl"
#include "../common/normal_mapping_use.glsl"

layout(std140, binding = 1) uniform UniformBufferObject {
	Shadows shadows;

	Lightning lightning;

	Material material;

	vec4 fogColor;

	vec4 selectedColor;

	uint selected;
	uint castedByShadows;
	float heightScale;
	uint numLayers;
} uboFS;

layout(binding = 2) uniform sampler2D colorSampler;
layout(binding = 3) uniform sampler2D normalSampler;
layout(binding = 4) uniform sampler2D heightSampler;
layout(binding = 5) uniform sampler2DArray depthSampler;

layout(location = 0) in vec2 inTextureCoord;
layout(location = 1) in vec3 inWorldPosition;
layout(location = 2) in vec3 inViewPosition;
layout(location = 3) in float inVisibility;
layout(location = 4) in vec3 inTangent;
layout(location = 5) in vec3 inBiTangent;
layout(location = 6) in vec3 inNornal;
layout(location = 7) in vec3 inToCameraVectorTangentSpace;
layout(location = 8) in vec3 inWorldPositionTangentSpace;
layout(location = 9) in vec3 inToLightVectorTangentSpace[MAX_LIGHT_COUNT];

layout(location = 0) out vec4 outColor;

void main() 
{
	const float nDotV = dot(inNornal, -inViewPosition);
	const float faceSign = sign(nDotV);
	const mat3 tbnMat = mat3(inTangent, inBiTangent, inNornal * faceSign);
    const vec3 rayDirection = normalize(inverse(tbnMat) * inViewPosition);
	const vec2 uv = ConeStepMapping(heightSampler, uboFS.heightScale.x, uboFS.numLayers, inTextureCoord, rayDirection);

	float shadow = 1.0;	
	if(uboFS.castedByShadows != 0)
	{
		shadow = GetShadow(depthSampler, uboFS.shadows, inViewPosition, inWorldPosition, 0.005);
	}

	const vec3 normal = NormalMapping(normalSampler, uv);
	const vec4 textureColor = texture(colorSampler, uv);

	// if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) 
	// {
	// 	discard;
	// }

	if (textureColor.a < 0.5) 
	{
		discard;
	}

	const vec3 unitToCameraVector = normalize(inToCameraVectorTangentSpace - inWorldPositionTangentSpace);

	vec3 totalDiffuse = vec3(0.0);
	vec3 totalSpecular = vec3(0.0);
	for (uint i = 0; i < uboFS.lightning.realCountOfLights; i++)
	{
		const Light light = uboFS.lightning.lights[i];

		const vec3 toLightVector = inToLightVectorTangentSpace[i] - inWorldPositionTangentSpace;
		const vec3 unitToLightVector = normalize(toLightVector);

		const float attenuationFactor = GetAttenuationFactor(light.attenuation.xyz, toLightVector);
		totalDiffuse += GetDiffuseColor(normal, unitToLightVector, light.color.xyz, attenuationFactor);
		if(shadow > 0.99) {
			totalSpecular += GetSpecularColor(normal, unitToLightVector, unitToCameraVector, light.color.xyz, attenuationFactor, uboFS.material.shineDamper, uboFS.material.reflectivity);
		}
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
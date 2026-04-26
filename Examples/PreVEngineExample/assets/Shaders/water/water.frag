#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable
#ifdef ENABLE_XR
#extension GL_EXT_multiview : enable
#endif

#include "../common/utils.glsl"
#include "../common/shadows_use.glsl"

struct Light {
	vec4 position;

	vec4 color;
};

layout(std140, binding = 1) uniform UniformBufferObject {
    Shadows shadows;

	vec4 fogColor;

    vec4 waterColor;

    Light light;

    vec4 nearFarClippinPlane;

    float moveFactor;
} uboFS;

layout(binding = 2) uniform texture2DArray depthTexture;
layout(binding = 3) uniform sampler depthSampler;
#ifdef ENABLE_XR
layout(binding = 4) uniform texture2DArray reflectionTexture;
layout(binding = 5) uniform sampler reflectionSampler;
layout(binding = 6) uniform texture2DArray refractionTexture;
layout(binding = 7) uniform sampler refractionSampler;
layout(binding = 8) uniform texture2DArray depthMapTexture;
layout(binding = 9) uniform sampler depthMapSampler;
#else
layout(binding = 4) uniform texture2D reflectionTexture;
layout(binding = 5) uniform sampler reflectionSampler;
layout(binding = 6) uniform texture2D refractionTexture;
layout(binding = 7) uniform sampler refractionSampler;
layout(binding = 8) uniform texture2D depthMapTexture;
layout(binding = 9) uniform sampler depthMapSampler;
#endif
layout(binding = 10) uniform texture2D dudvMapTexture;
layout(binding = 11) uniform sampler dudvMapSampler;
layout(binding = 12) uniform texture2D normalMapTexture;
layout(binding = 13) uniform sampler normalMapSampler;

const float waveStrength = 0.04;
const float shineDamper = 20.0;
const float reflectivity = 0.45;
const float waterReflectivness = 0.7;

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec4 inClipSpaceCoord;
layout(location = 1) in vec2 inTextureCoord;
layout(location = 2) in vec3 inWorldPosition;
layout(location = 3) in vec3 inViewPosition;
layout(location = 4) in vec3 inToCameraVector;
layout(location = 5) in float inVisibility;

float SampleDepth(in vec2 texCoords)
{
#ifdef ENABLE_XR
	float depth = texture(sampler2DArray(depthMapTexture, depthMapSampler), vec3(texCoords, gl_ViewIndex)).r;
#else
	float depth = texture(sampler2D(depthMapTexture, depthMapSampler), texCoords).r;
#endif
	return depth;
}

vec4 SampleRefraction(in vec2 texCoord)
{
#ifdef ENABLE_XR
	vec4 refractColor = texture(sampler2DArray(refractionTexture, refractionSampler), vec3(texCoord, gl_ViewIndex));
#else
	vec4 refractColor = texture(sampler2D(refractionTexture, refractionSampler), texCoord);
#endif
	return refractColor;
}

vec4 SampleReflection(in vec2 texCoord)
{
	#ifdef ENABLE_XR
	vec4 reflectColor = texture(sampler2DArray(reflectionTexture, reflectionSampler), vec3(texCoord, gl_ViewIndex));
	#else
	vec4 reflectColor = texture(sampler2D(reflectionTexture, reflectionSampler), texCoord);
	#endif
	return reflectColor;
}

float CalculateWaterDepth(in vec2 texCoords)
{
	float depth = SampleDepth(texCoords);
	float floorDistance = LinearizeDepth(depth, uboFS.nearFarClippinPlane.x, uboFS.nearFarClippinPlane.y);
	depth = gl_FragCoord.z;
	float waterDistance = LinearizeDepth(depth, uboFS.nearFarClippinPlane.x, uboFS.nearFarClippinPlane.y);
	return floorDistance - waterDistance;
}

void main()
{
	float shadow = GetShadow(depthTexture, depthSampler, uboFS.shadows, inViewPosition, inWorldPosition, 0.005);
	if(shadow < 0.999) {
		shadow = 0.0;
	}

	vec2 normalizedDeviceSapceCoord = (inClipSpaceCoord.xy / inClipSpaceCoord.w) / 2.0 + 0.5;

	vec2 reflectTexCoord = vec2(normalizedDeviceSapceCoord.x, 1.0 - normalizedDeviceSapceCoord.y);
	vec2 refractTexCoord = vec2(normalizedDeviceSapceCoord.x, normalizedDeviceSapceCoord.y);

    // calculate water depth
	float waterDepth = CalculateWaterDepth(refractTexCoord);

    // distortion
	vec2 distortedTexCoords = texture(sampler2D(dudvMapTexture, dudvMapSampler), vec2(inTextureCoord.x + uboFS.moveFactor, inTextureCoord.y)).rg * 0.1;
	distortedTexCoords = inTextureCoord + vec2(distortedTexCoords.x, distortedTexCoords.y + uboFS.moveFactor);
	vec2 totalDistortion = (texture(sampler2D(dudvMapTexture, dudvMapSampler), distortedTexCoords).rg * 2.0 - 1.0) * waveStrength * clamp(waterDepth / 12.0, 0.0, 1.0);

	reflectTexCoord += totalDistortion;
	reflectTexCoord = clamp(reflectTexCoord, 0.001, 0.999);

	refractTexCoord += totalDistortion;
	refractTexCoord = clamp(refractTexCoord, 0.001, 0.999);

	vec4 reflectColor = SampleReflection(reflectTexCoord);
	vec4 refractColor = SampleRefraction(refractTexCoord);
	vec4 finalWaterColor = uboFS.waterColor * shadow;

	// normal
	vec4 normalMapColor = texture(sampler2D(normalMapTexture, normalMapSampler), distortedTexCoords);
	vec3 normal = vec3(normalMapColor.r * 2.0 - 1.0, normalMapColor.g * 3.0, normalMapColor.b * 2.0 - 1.0);
	normal = normalize(normal);

	vec3 viewVector = normalize(inToCameraVector);
	float refractiveFactor = dot(viewVector, normal);
	refractiveFactor = pow(refractiveFactor, waterReflectivness);
	refractiveFactor = clamp(refractiveFactor, 0.001, 0.999);

    vec3 toLightVector = uboFS.light.position.xyz - inWorldPosition.xyz;
	vec3 reflectedLight = reflect(normalize(-toLightVector), normal);
	float specular = max(dot(reflectedLight, viewVector), 0.0);
	specular = pow(specular, shineDamper);
	vec3 specularHighlights = uboFS.light.color.xyz * specular * reflectivity * clamp(waterDepth / 5.0, 0.0, 1.0);

	vec4 baseResultColor = mix(reflectColor, refractColor, refractiveFactor);
	baseResultColor = clamp(mix(baseResultColor, finalWaterColor, 0.2) + vec4(specularHighlights, 1.0), 0.0, 1.0);
	//outColor = mix(vec4(uboFS.fogColor.xyz, 1.0), baseResultColor, inVisibility);
	outColor = baseResultColor;
	outColor.a = clamp(waterDepth / 5.0, 0.0, 1.0);
}
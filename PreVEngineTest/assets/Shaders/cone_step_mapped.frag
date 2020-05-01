#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "shadows_use.glsl"
#include "lights.glsl"

layout(std140, binding = 1) uniform UniformBufferObject {
	Shadows shadows;

	Lightning lightning;

	Material material;

	vec4 fogColor;

	vec4 selectedColor;

	uint selected;
	uint castedByShadows;
	float heightScale;
	float numLayers;
} uboFS;

layout(binding = 2) uniform sampler2D textureSampler;
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

vec2 ConeStepMapping(in vec2 uv, in vec3 texDir3D)
{
    vec2 R = normalize(vec2(length(texDir3D.xy), texDir3D.z)); 
    vec2 P = R * uboFS.heightScale / texDir3D.z; 

    vec2 textureSize = textureSize(heightSampler, 0);
    vec2 minTextureStep = normalize(texDir3D.xy) / textureSize;
    float minStep = length(minTextureStep) * 1.0 / R.x;

    float t = 0.0;
    for (int i = 0; i < uboFS.numLayers; ++i)
    {
        vec3 sample_pt = vec3(uv.xy, uboFS.heightScale) + texDir3D * t;

        vec2 h_and_c = clamp(texture(heightSampler, sample_pt.xy).rg, 0.0, 1.0);
        float h = h_and_c.x * uboFS.heightScale;
        float c = h_and_c.y * h_and_c.y / uboFS.heightScale;

        vec2 C = P + R * t;
        if (C.y <= h)
		{
        	break;
		}

        vec2 Q = vec2(C.x, h);
        vec2 S = normalize(vec2(c, 1.0));
        float new_t = dot(Q - P, vec2(S.y, -S.x)) / dot(R, vec2(S.y, -S.x));
        t = max(t + minStep, new_t);
    }
    
    vec2  texC = uv.xy + texDir3D.xy * t;
    return texC.xy;
}

void main() 
{
	const float faceSign = sign(dot(inNornal, -inViewPosition));
	mat3 tbnMat = mat3(inTangent, inBiTangent, inNornal * faceSign);
    vec3 rayDirection = normalize(inverse(tbnMat) * inViewPosition);
	vec2 uv = ConeStepMapping(inTextureCoord, rayDirection);

	float shadow = 1.0;	
	if(uboFS.castedByShadows != 0)
	{
		// shadow = GetShadowFull(depthSampler, uboFS.shadows, inViewPosition, inWorldPosition, 0.005);

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
		shadow = GetShadow(depthSampler, normalizedShadowCoord, cascadeIndex, 0.005);
	}

	const vec3 normalMapValue = 2.0 * texture(normalSampler, uv).rgb - 1.0;
	const vec4 textureColor = texture(textureSampler, uv);

	if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) 
    {
		discard;
	}

	if (textureColor.a < 0.5) 
	{
		discard;
	}

	const vec3 unitNormal = normalize(normalMapValue);
	const vec3 unitToCameraVector = normalize(inToCameraVectorTangentSpace - inWorldPositionTangentSpace);

	vec3 totalDiffuse = vec3(0.0);
	vec3 totalSpecular = vec3(0.0);
	for (uint i = 0; i < uboFS.lightning.realCountOfLights; i++)
	{
		const Light light = uboFS.lightning.lights[i];

		const vec3 toLightVector = inToLightVectorTangentSpace[i] - inWorldPositionTangentSpace;
		const vec3 unitToLightVector = normalize(toLightVector);

		const float attenuationFactor = GetAttenuationFactor(light.attenuation.xyz, toLightVector);
		totalDiffuse += GetDiffuseColor(unitNormal, unitToLightVector, light.color.xyz, attenuationFactor);
		totalSpecular += GetSpecularColor(unitNormal, unitToLightVector, unitToCameraVector, light.color.xyz, attenuationFactor, uboFS.material.shineDamper, uboFS.material.reflectivity);
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
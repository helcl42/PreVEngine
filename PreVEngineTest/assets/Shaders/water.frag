#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "shadows_use.glsl"

struct Light {
	vec4 position;

	vec4 color;
};

layout(std140, binding = 1) uniform UniformBufferObject {
    Shadows shadows;
    
	vec4 fogColor;

    vec4 waterColor;

    Light light;

    vec2 nearFarClippinPlane;

    float moveFactor;
} uboFS;

layout(binding = 2) uniform sampler2DArray depthSampler;
layout(binding = 3) uniform sampler2D reflectionTexture;
layout(binding = 4) uniform sampler2D refractionTexture;
layout(binding = 5) uniform sampler2D dudvMapTexture;
layout(binding = 6) uniform sampler2D normalMapTexture;
layout(binding = 7) uniform sampler2D depthMapTexture;

const float waveStrength = 0.12;
const float shineDamper = 20.0;
const float reflectivity = 0.5;
const float waterReflectivness = 0.7;

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec4 inClipSpaceCoord;
layout(location = 1) in vec3 inWorldPosition;
layout(location = 2) in vec3 inViewPosition;
layout(location = 3) in vec3 inToCameraVector;
layout(location = 4) in float inVisibility;

void main()
{
	float shadow = 1.0;		
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
    shadow = GetShadowInternal(depthSampler, normalizedShadowCoord, cascadeIndex, 0.02);

    vec3 color = vec3(0.0, 0.0, 1.0) * shadow;
    outColor = vec4(color, 1.0);
}
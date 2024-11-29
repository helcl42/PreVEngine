
#version 450
#extension GL_ARB_separate_shader_objects : enable
#ifdef ENABLE_XR
#extension GL_EXT_multiview : enable
#endif

layout(std140, binding = 0) uniform UniformBufferObject {
    vec4 translations[MAX_VIEW_COUNT];
    vec4 scale;
} uboVS;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTextureCoord;
layout(location = 2) in vec3 inNormal;

void main()
{
#ifdef ENABLE_XR
    const int viewIndex = gl_ViewIndex;
#else
    const int viewIndex = 0;
#endif

    vec3 translation = uboVS.translations[viewIndex].xyz;
	vec2 screenPosition = inPosition.xy * uboVS.scale.xy + translation.xy;
	gl_Position = vec4(screenPosition, translation.z, 1.0);
}

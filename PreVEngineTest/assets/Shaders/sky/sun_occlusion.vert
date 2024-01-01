
#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(std140, binding = 0) uniform UniformBufferObject {
    vec4 translation;
    vec4 scale;
} uboVS;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTextureCoord;
layout(location = 2) in vec3 inNormal;

void main()
{
	vec2 screenPosition = inPosition.xy * uboVS.scale.xy + uboVS.translation.xy;
	gl_Position = vec4(screenPosition, uboVS.translation.z, 1.0);
}

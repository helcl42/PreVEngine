
#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(std140, binding = 0) uniform UniformBufferObject {
    vec4 translation;
    vec4 scale;
} uboVS;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTextureCoord;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec2 outTextureCoord;

void main()
{
	outTextureCoord = inPosition.xy + vec2(0.5, 0.5);

	vec2 screenPosition = inPosition.xy * uboVS.scale.xy + uboVS.translation.xy;
	gl_Position = vec4(screenPosition, 0.0, 1.0);
}

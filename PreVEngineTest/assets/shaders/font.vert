
#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(std140, binding = 0) uniform UniformBufferObject {
    vec4 translation;
} uboVS;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTextureCoord;

layout(location = 0) out vec2 outTextureCoord;

void main()
{
	gl_Position = vec4(vec2(inPosition.x, 1.0 - inPosition.y) + uboVS.translation.xy * vec2(2.0, -2.0), 0.0, 1.0);
	outTextureCoord = inTextureCoord;
}

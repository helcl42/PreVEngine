#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(std140, binding = 1) uniform UniformBufferObject {	
	vec4 color;
} uboFS;

layout(location = 0) out vec4 outColor;

void main() 
{
	outColor = uboFS.color;
}
#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(std140, binding = 2) uniform UniformBufferObject {
    float alpha;
} uboFS;

layout(location = 0) in vec3 inColor;

layout(location = 0) out vec4 outColor;

void main() 
{
	outColor = vec4(inColor, uboFS.alpha);
}
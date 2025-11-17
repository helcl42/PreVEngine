#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(std140, binding = 1) uniform UniformBufferObject {
    vec4 brightness;
} uboFS;

layout(binding = 2) uniform sampler2D colorSampler;

layout(location = 0) in vec2 inTextureCoord;

layout(location = 0) out vec4 outColor;

void main()
{
	outColor = texture(colorSampler, inTextureCoord);
    outColor.a *= uboFS.brightness.x;
}
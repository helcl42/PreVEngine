#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(std140, binding = 1) uniform UniformBufferObject {
    vec4 brightness;
} uboFS;

layout(binding = 2) uniform texture2D colorTexture;
layout(binding = 3) uniform sampler colorSampler;

layout(location = 0) in vec2 inTextureCoord;

layout(location = 0) out vec4 outColor;

void main()
{
	outColor = texture(sampler2D(colorTexture, colorSampler), inTextureCoord);
    outColor.a *= uboFS.brightness.x;
}
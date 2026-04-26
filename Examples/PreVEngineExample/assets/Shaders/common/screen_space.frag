#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 inTextureCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform texture2D imageTexture;
layout(binding = 1) uniform sampler imageSampler;

void main()
{
        vec4 color = texture(sampler2D(imageTexture, imageSampler), inTextureCoord);
	outColor = color;
}
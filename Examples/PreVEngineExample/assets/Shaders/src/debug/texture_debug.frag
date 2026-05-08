#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 0) uniform texture2D imageTexture;
layout (binding = 1) uniform sampler imageSampler;

layout (location = 0) in vec2 textureCoord;

layout (location = 0) out vec4 outFragColor;

void main() 
{
        const vec4 color = texture(sampler2D(imageTexture, imageSampler), textureCoord);
	outFragColor = color;
}
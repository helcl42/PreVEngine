#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 0) uniform sampler2D imageSampler;

layout (location = 0) in vec2 textureCoord;

layout (location = 0) out vec4 outFragColor;

void main() 
{
	const vec4 color = texture(imageSampler, textureCoord);
	outFragColor = color;
}
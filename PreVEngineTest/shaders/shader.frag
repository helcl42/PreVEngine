#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec2 textureCoord;

layout(location = 0) out vec4 outColor;

void main() {
	//outColor = vec4(textureCoord, 0.0, 1.0);
	outColor = texture(texSampler, textureCoord);
}
#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 modelMatrix;
    mat4 viewMatrix;
    mat4 projMatrix;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTextureCoord;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec2 outTextureCoord;

void main() {
	outTextureCoord	= inTextureCoord;
	gl_Position = ubo.projMatrix * ubo.viewMatrix * ubo.modelMatrix * vec4(inPosition, 1.0);
}

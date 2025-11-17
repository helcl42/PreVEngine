#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(std140, binding = 0) uniform UniformBufferObject {
    mat4 modelMatrix;
    mat4 viewMatrix;
    mat4 projectionMatrix;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTextureCoord;
layout(location = 2) in vec3 inNormal;

void main() {
	gl_Position = ubo.projectionMatrix * ubo.viewMatrix * ubo.modelMatrix * vec4(inPosition, 1.0);
}

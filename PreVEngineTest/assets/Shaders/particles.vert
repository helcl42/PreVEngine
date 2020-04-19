#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(std140, binding = 0) uniform UniformBufferObject {
    mat4 modelMatrix;

    mat4 viewMatrix;
    
	mat4 projectionMatrix;

    uint textureNumberOfRows;
} uboVS;

layout(binding = 2) uniform sampler2D textureSampler;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTextureCoord;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec2 outTextureCoord;

void main() 
{
	gl_Position = uboVS.projectionMatrix * uboVS.viewMatrix * uboVS.modelMatrix * vec4(inPosition, 1.0);
    
	outTextureCoord = (inTextureCoord / uboVS.textureNumberOfRows);
}

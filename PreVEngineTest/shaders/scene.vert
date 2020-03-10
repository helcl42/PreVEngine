#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 modelMatrix;
    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat4 normalMatrix;
} uboVS;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTextureCoord;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec2 outTextureCoord;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outViewPosition;
layout(location = 3) out vec3 outWorldPosition;

void main() 
{
	gl_Position = uboVS.projectionMatrix * uboVS.viewMatrix * uboVS.modelMatrix * vec4(inPosition, 1.0);
	outTextureCoord = inTextureCoord;
	outNormal = (uboVS.normalMatrix * vec4(inNormal, 0.0)).xyz;
	outViewPosition = (uboVS.viewMatrix * uboVS.modelMatrix * vec4(inPosition, 1.0)).xyz;
	outWorldPosition = (uboVS.modelMatrix * vec4(inPosition, 1.0)).xyz;
}

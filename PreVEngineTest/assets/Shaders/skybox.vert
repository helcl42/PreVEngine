#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(std140, binding = 0) uniform UniformBufferObject {
    mat4 modelMatrix;

    mat4 viewMatrix;
    
	mat4 projectionMatrix;    
} uboVS;

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec3 outTextureCoord;

void main()
{
	gl_Position = uboVS.projectionMatrix * uboVS.viewMatrix * uboVS.modelMatrix * vec4(inPosition, 1.0);
	outTextureCoord = inPosition;
}

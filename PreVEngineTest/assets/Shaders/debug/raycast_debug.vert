#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(std140, binding = 0) uniform UniformBufferObject {
    vec3 color;
} uboVS;

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec3 outColor;

void main()
{
    gl_Position = vec4(inPosition, 1.0);
    outColor = uboVS.color;
}
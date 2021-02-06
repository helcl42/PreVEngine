#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(std140, binding = 1) uniform UniformBufferObject {
	vec4 color;
} uboFS;

layout(binding = 2) uniform sampler2D colorSampler;

layout(location = 0) in vec2 inTextureCoord;
layout(location = 1) in vec2 inCurrentStageTextureOffset;
layout(location = 2) in vec2 inNextStageTextureOffset;
layout(location = 3) in float inCurrentNextStageBlendFactor;

layout(location = 0) out vec4 outColor;

void main() 
{
    vec4 stage1Color = texture(colorSampler, inTextureCoord + inCurrentStageTextureOffset);
    vec4 stage2Color = texture(colorSampler, inTextureCoord + inNextStageTextureOffset);
    outColor = uboFS.color * mix(stage1Color, stage2Color, inCurrentNextStageBlendFactor);
}
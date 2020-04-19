#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(std140, binding = 1) uniform UniformBufferObject {	
	vec4 color;
    
    vec4 textureOffsetStage1;

    vec4 textureOffsetStage2;

    float stagesBlendFactor;
} uboFS;

layout(binding = 2) uniform sampler2D textureSampler;

layout(location = 0) in vec2 inTextureCoord;

layout(location = 0) out vec4 outColor;

void main() 
{
    vec4 stage1Color = texture(textureSampler, inTextureCoord + uboFS.textureOffsetStage1.xy);
    vec4 stage2Color = texture(textureSampler, inTextureCoord + uboFS.textureOffsetStage2.xy);
    outColor = mix(stage1Color, stage2Color, uboFS.stagesBlendFactor);
}
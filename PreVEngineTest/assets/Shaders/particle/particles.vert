#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(std140, binding = 0) uniform UniformBufferObject {
    mat4 viewMatrix;

	mat4 projectionMatrix;

    uint textureNumberOfRows;
} uboVS;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTextureCoord;
layout(location = 2) in vec3 inNormal;

layout(location = 3) in mat4 modelMatrix;
layout(location = 7) in vec2 textureOffsetsCurrent;
layout(location = 8) in vec2 textureOffsetNext;
layout(location = 9) in float blendFactor;

layout(location = 0) out vec2 outCurrentStageTextureCoord;
layout(location = 1) out vec2 outNextStageTextureCoord;
layout(location = 2) out float outCurrentNextStageBlendFactor;

void main()
{
	gl_Position = uboVS.projectionMatrix * uboVS.viewMatrix * modelMatrix * vec4(inPosition, 1.0);

	vec2 textureCoordBase = inTextureCoord / uboVS.textureNumberOfRows;
    outCurrentStageTextureCoord = textureCoordBase + textureOffsetsCurrent;
    outNextStageTextureCoord = textureCoordBase + textureOffsetNext;
    outCurrentNextStageBlendFactor = blendFactor;
}

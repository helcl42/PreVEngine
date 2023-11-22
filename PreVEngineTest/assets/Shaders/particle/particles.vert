#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "../common/utils.glsl"

layout(std140, binding = 0) uniform UniformBufferObject {
    mat4 viewMatrix;

	mat4 projectionMatrix;

    uint textureNumberOfRows;
} uboVS;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTextureCoord;
layout(location = 2) in vec3 inNormal;

layout(location = 3) in vec3 position;
layout(location = 4) in vec2 scale;
layout(location = 5) in float rotation;
layout(location = 6) in vec2 textureOffsetsCurrent;
layout(location = 7) in vec2 textureOffsetNext;
layout(location = 8) in float blendFactor;

layout(location = 0) out vec2 outCurrentStageTextureCoord;
layout(location = 1) out vec2 outNextStageTextureCoord;
layout(location = 2) out float outCurrentNextStageBlendFactor;

void main()
{
    const vec3 cameraRightWorldSpace = vec3(uboVS.viewMatrix[0][0], uboVS.viewMatrix[1][0], uboVS.viewMatrix[2][0]);
    const vec3 cameraUpWorldSpace = vec3(uboVS.viewMatrix[0][1], uboVS.viewMatrix[1][1], uboVS.viewMatrix[2][1]);

    vec2 localPosition = MakeRotation(rotation) * vec2(inPosition.x * scale.x, inPosition.y * scale.y);

    vec3 vertexPositionWorldspace = position + cameraRightWorldSpace * localPosition.x + cameraUpWorldSpace * localPosition.y;

	gl_Position = uboVS.projectionMatrix * uboVS.viewMatrix * vec4(vertexPositionWorldspace, 1.0);

	vec2 textureCoordBase = inTextureCoord / uboVS.textureNumberOfRows;
    outCurrentStageTextureCoord = textureCoordBase + textureOffsetsCurrent;
    outNextStageTextureCoord = textureCoordBase + textureOffsetNext;
    outCurrentNextStageBlendFactor = blendFactor;
}

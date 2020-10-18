#version 450
#extension GL_ARB_separate_shader_objects : enable

const int MAX_BONES_COUNT = 100;

layout(std140, binding = 0) uniform UniformBufferObject {
    mat4 modelMatrix;
    mat4 viewMatrix;
    mat4 projMatrix;
    mat4 bones[MAX_BONES_COUNT];
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTextureCoord;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in ivec4 inBoneIds;
layout(location = 4) in vec4 inWeights;

void main() {
	mat4 boneTransform = ubo.bones[inBoneIds[0]] * inWeights[0];
	boneTransform += ubo.bones[inBoneIds[1]] * inWeights[1];
	boneTransform += ubo.bones[inBoneIds[2]] * inWeights[2];
	boneTransform += ubo.bones[inBoneIds[3]] * inWeights[3];

	vec4 positionL = boneTransform * vec4(inPosition, 1.0);

	gl_Position = ubo.projMatrix * ubo.viewMatrix * ubo.modelMatrix * vec4(positionL.xyz, 1.0);
}

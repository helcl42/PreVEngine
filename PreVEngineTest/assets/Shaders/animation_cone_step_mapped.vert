#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "lights.glsl"

const int MAX_BONES_COUNT = 100;

layout(std140, binding = 0) uniform UniformBufferObject {
	mat4 bones[MAX_BONES_COUNT];

    mat4 modelMatrix;

    mat4 viewMatrix;
    
	mat4 projectionMatrix;
    
	mat4 normalMatrix;
	
	vec4 clipPlane;

	vec4 textureOffset;

	uint textureNumberOfRows;

	float density;
	float gradient;
} uboVS;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTextureCoord;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in ivec4 inBoneIds;
layout(location = 4) in vec4 inWeights;
layout(location = 5) in vec3 inTangent;
layout(location = 6) in vec3 inBiTangent;

layout(location = 0) out vec2 outTextureCoord;
layout(location = 1) out vec3 outWorldPosition;
layout(location = 2) out vec3 outViewPosition;
layout(location = 3) out float outVisibility;
layout(location = 4) out vec3 outTangent;
layout(location = 5) out vec3 outBiTangent;
layout(location = 6) out vec3 outNornal;

void main()
{
	mat4 boneTransform = uboVS.bones[inBoneIds[0]] * inWeights[0];
	boneTransform += uboVS.bones[inBoneIds[1]] * inWeights[1];
	boneTransform += uboVS.bones[inBoneIds[2]] * inWeights[2];
	boneTransform += uboVS.bones[inBoneIds[3]] * inWeights[3];

	vec4 positionL = boneTransform * vec4(inPosition, 1.0);
	vec4 normalL = boneTransform * vec4(inNormal, 0.0);

	vec4 worldPosition = uboVS.modelMatrix * vec4(positionL.xyz, 1.0);
	outWorldPosition = worldPosition.xyz;

	gl_ClipDistance[0] = dot(worldPosition, uboVS.clipPlane);

	vec4 viewPosition = uboVS.viewMatrix * worldPosition;
	outViewPosition = viewPosition.xyz;

	gl_Position = uboVS.projectionMatrix * viewPosition;

	outTextureCoord = (inTextureCoord / uboVS.textureNumberOfRows) + uboVS.textureOffset.xy;

	float vertexToCameraDistance = length(viewPosition.xyz);
	outVisibility = clamp(exp(-pow(vertexToCameraDistance * uboVS.density, uboVS.gradient)), 0.0, 1.0);

	mat3 mv3 = mat3(uboVS.viewMatrix) * mat3(uboVS.modelMatrix);
	outTangent = normalize(mv3 * inTangent);
	outBiTangent = normalize(mv3 * inBiTangent);
	outNornal = normalize(mv3 * inNormal);
}

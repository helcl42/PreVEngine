#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable
#ifdef ENABLE_XR
#extension GL_EXT_multiview : enable
#endif

#include "../common/common.glsl"
#include "../common/lights.glsl"

const int MAX_BONES_COUNT = 100;

layout(std140, binding = 0) uniform UniformBufferObject {
	mat4 bones[MAX_BONES_COUNT];

    mat4 modelMatrix;

	mat4 normalMatrix;

	mat4 viewMatrices[MAX_VIEW_COUNT];

	mat4 projectionMatrices[MAX_VIEW_COUNT];

	vec4 cameraPositions[MAX_VIEW_COUNT];

	vec4 clipPlane;

	Lightning lightning;

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
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outWorldPosition;
layout(location = 3) out vec3 outViewPosition;
layout(location = 4) out float outVisibility;
layout(location = 5) out vec3 outToCameraVectorTangentSpace;
layout(location = 6) out vec3 outPositionTangentSpace;
layout(location = 7) out vec3 outToLightVectorTangentSpace[MAX_LIGHT_COUNT];

void main() 
{
#ifdef ENABLE_XR
	const int viewIndex = gl_ViewIndex;
#else
	const int viewIndex = 0;
#endif

	mat4 boneTransform = uboVS.bones[inBoneIds[0]] * inWeights[0];
	boneTransform += uboVS.bones[inBoneIds[1]] * inWeights[1];
	boneTransform += uboVS.bones[inBoneIds[2]] * inWeights[2];
	boneTransform += uboVS.bones[inBoneIds[3]] * inWeights[3];

	vec4 positionL = boneTransform * vec4(inPosition, 1.0);
	vec4 normalL = boneTransform * vec4(inNormal, 0.0);

	vec4 worldPosition = uboVS.modelMatrix * vec4(positionL.xyz, 1.0);
	outWorldPosition = worldPosition.xyz;

	gl_ClipDistance[0] = dot(worldPosition, uboVS.clipPlane);

	vec4 viewPosition = uboVS.viewMatrices[viewIndex] * worldPosition;
	outViewPosition = viewPosition.xyz;

	gl_Position = uboVS.projectionMatrices[viewIndex] * viewPosition;

	outTextureCoord = (inTextureCoord / uboVS.textureNumberOfRows) + uboVS.textureOffset.xy;
	outNormal = (uboVS.normalMatrix * vec4(normalL.xyz, 0.0)).xyz;

	outVisibility = GetVisibility(viewPosition.xyz, uboVS.gradient, uboVS.density);

	mat3 TBN = CreateTBNMatrix(mat3(uboVS.modelMatrix), inNormal, inTangent, inBiTangent);

	outToCameraVectorTangentSpace = TBN * uboVS.cameraPositions[viewIndex].xyz;
	outPositionTangentSpace = TBN * worldPosition.xyz;

	for (int i = 0; i < uboVS.lightning.realCountOfLights; i++)
	{
		const Light light = uboVS.lightning.lights[i];
		outToLightVectorTangentSpace[i] = TBN * light.position.xyz;
	}
}

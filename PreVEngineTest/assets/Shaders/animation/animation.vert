#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "../common/common.glsl"
#include "../common/lights.glsl"

const int MAX_BONES_COUNT = 100;

layout(std140, binding = 0) uniform UniformBufferObject {
	mat4 bones[MAX_BONES_COUNT];

    mat4 modelMatrix;

    mat4 viewMatrix;
    
	mat4 projectionMatrix;
    
	mat4 normalMatrix;
	
	vec4 clipPlane;

	vec4 cameraPosition;

	Lightning lightning;
	
	vec4 textureOffset;

	uint textureNumberOfRows;
	uint useFakeLightning;

	float density;
	float gradient;
} uboVS;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTextureCoord;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in ivec4 inBoneIds;
layout(location = 4) in vec4 inWeights;

layout(location = 0) out vec2 outTextureCoord;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outWorldPosition;
layout(location = 3) out vec3 outViewPosition;
layout(location = 4) out vec3 outToCameraVector;
layout(location = 5) out float outVisibility;
layout(location = 6) out vec3 outToLightVector[MAX_LIGHT_COUNT];

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

	vec3 normal = normalL.xyz;
	if (uboVS.useFakeLightning != 0)
	{
		normal = vec3(0.0, 1.0, 0.0);
	}
	outNormal = (uboVS.normalMatrix * vec4(normal, 0.0)).xyz;

	for (int i = 0; i < uboVS.lightning.realCountOfLights; i++)
	{
		const Light light = uboVS.lightning.lights[i];
		outToLightVector[i] = light.position.xyz - worldPosition.xyz;
	}

	//vec3 cameraPosition = (inverse(viewMatrix) * vec4(0.0, 0.0, 0.0, 1.0)).xyz; // OPT - passed in UBO
	outToCameraVector = uboVS.cameraPosition.xyz - worldPosition.xyz;

	outVisibility = GetVisibility(viewPosition.xyz, uboVS.gradient, uboVS.density);
}

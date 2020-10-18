#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "../common/lights.glsl"

layout(std140, binding = 0) uniform UniformBufferObject {
    mat4 modelMatrix;

    mat4 viewMatrix;

	mat4 projectionMatrix;

	mat4 normalMatrix;

	vec4 clipPlane;

	vec4 cameraPosition;

	Lightning lightning;

	float density;
	float gradient;
} uboVS;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTextureCoord;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec3 inBiTangent;

layout(location = 0) out vec2 outTextureCoord;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outWorldPosition;
layout(location = 3) out vec3 outViewPosition;
layout(location = 4) out float outVisibility;
layout(location = 5) out vec3 outToCameraVectorTangentSpace;
layout(location = 6) out vec3 outWorldPositionTangentSpace;
layout(location = 7) out vec3 outToLightVectorTangentSpace[MAX_LIGHT_COUNT];

void main()
{
	vec4 worldPosition = uboVS.modelMatrix * vec4(inPosition, 1.0);
	outWorldPosition = worldPosition.xyz;

	gl_ClipDistance[0] = dot(worldPosition, uboVS.clipPlane);

	vec4 viewPosition = uboVS.viewMatrix * worldPosition;
	outViewPosition = viewPosition.xyz;

	gl_Position = uboVS.projectionMatrix * viewPosition;

	outTextureCoord = inTextureCoord;

	outNormal = (uboVS.normalMatrix * vec4(inNormal, 0.0)).xyz;

	float vertexToCameraDistance = length(viewPosition.xyz);
	outVisibility = clamp(exp(-pow(vertexToCameraDistance * uboVS.density, uboVS.gradient)), 0.0, 1.0);

	vec3 T = normalize(mat3(uboVS.modelMatrix) * inTangent);
	vec3 B = normalize(mat3(uboVS.modelMatrix) * inBiTangent);
	vec3 N = normalize(mat3(uboVS.modelMatrix) * inNormal);
	mat3 toTangentSpaceMatrix = transpose(mat3(T, B, N));

	outToCameraVectorTangentSpace = toTangentSpaceMatrix * uboVS.cameraPosition.xyz;
	outWorldPositionTangentSpace = toTangentSpaceMatrix * worldPosition.xyz;

	for (int i = 0; i < uboVS.lightning.realCountOfLights; i++)
	{
		const Light light = uboVS.lightning.lights[i];
		outToLightVectorTangentSpace[i] = toTangentSpaceMatrix * light.position.xyz;
	}
}

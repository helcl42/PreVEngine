#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable
#ifdef ENABLE_XR
#extension GL_EXT_multiview : enable
#endif

#include "../common/common.glsl"
#include "../common/lights.glsl"

layout(std140, binding = 0) uniform UniformBufferObject {
    mat4 modelMatrix;

	mat4 normalMatrix;

	mat4 viewMatrices[MAX_VIEW_COUNT];

	mat4 projectionMatrices[MAX_VIEW_COUNT];

	vec4 cameraPositions[MAX_VIEW_COUNT];

	vec4 clipPlane;

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
layout(location = 1) out vec3 outWorldPosition;
layout(location = 2) out vec3 outViewPosition;
layout(location = 3) out float outVisibility;
layout(location = 4) out vec3 outToCameraVectorTangentSpace;
layout(location = 5) out vec3 outPositionTangentSpace;
layout(location = 6) out vec3 outToLightVectorTangentSpace[MAX_LIGHT_COUNT];

void main()
{
#ifdef ENABLE_XR
	const int viewIndex = gl_ViewIndex;
#else
	const int viewIndex = 0;
#endif

	vec4 worldPosition = uboVS.modelMatrix * vec4(inPosition, 1.0);
	outWorldPosition = worldPosition.xyz;

	gl_ClipDistance[0] = dot(worldPosition, uboVS.clipPlane);

	vec4 viewPosition = uboVS.viewMatrices[viewIndex] * worldPosition;
	outViewPosition = viewPosition.xyz;

	gl_Position = uboVS.projectionMatrices[viewIndex] * viewPosition;

	outTextureCoord = inTextureCoord;

	outVisibility = GetVisibility(viewPosition.xyz, uboVS.gradient, uboVS.density);

	mat3 mv3 = mat3(uboVS.viewMatrices[viewIndex]) * mat3(uboVS.modelMatrix);
	mat3 TBN = CreateTBNMatrix(mv3, inNormal, inTangent, inBiTangent);

	vec4 cameraPositionViewSpaceVec4 = uboVS.viewMatrices[viewIndex] * vec4(uboVS.cameraPositions[viewIndex].xyz, 1.0);
	vec3 cameraPositionViewSpace = cameraPositionViewSpaceVec4.xyz / cameraPositionViewSpaceVec4.w;

	outToCameraVectorTangentSpace = TBN * cameraPositionViewSpace;
	outPositionTangentSpace = TBN * viewPosition.xyz;

	for (int i = 0; i < uboVS.lightning.realCountOfLights; i++)
	{
		const Light light = uboVS.lightning.lights[i];

		vec3 lightPositionViewSpace = (uboVS.viewMatrices[viewIndex] * vec4(light.position.xyz, 1.0)).xyz;
		outToLightVectorTangentSpace[i] = TBN * lightPositionViewSpace;
	}
}

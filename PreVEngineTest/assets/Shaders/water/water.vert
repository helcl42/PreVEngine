#version 450
#extension GL_ARB_separate_shader_objects : enable
#ifdef ENABLE_XR
#extension GL_EXT_multiview : enable
#endif

const float textureTilingFactor = 1.0;

layout(std140, binding = 0) uniform UniformBufferObject {
    mat4 modelMatrix;

    mat4 viewMatrices[MAX_VIEW_COUNT];

	mat4 projectionMatrices[MAX_VIEW_COUNT];

	vec4 cameraPositions[MAX_VIEW_COUNT];

	float density;
	float gradient;
} uboVS;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTextureCoord;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec4 outClipSpaceCoord;
layout(location = 1) out vec2 outTextureCoord;
layout(location = 2) out vec3 outWorldPosition;
layout(location = 3) out vec3 outViewPosition;
layout(location = 4) out vec3 outToCameraVector;
layout(location = 5) out float outVisibility;

void main()
{
#ifdef ENABLE_XR
	const int viewIndex = gl_ViewIndex;
#else
	const int viewIndex = 0;
#endif

	gl_Position = uboVS.projectionMatrices[viewIndex] * uboVS.viewMatrices[viewIndex] * uboVS.modelMatrix * vec4(inPosition, 1.0);

    vec4 worldPosition = uboVS.modelMatrix * vec4(inPosition, 1.0);
	outWorldPosition = worldPosition.xyz;

	vec4 viewPosition = uboVS.viewMatrices[viewIndex] * worldPosition;
	outViewPosition = viewPosition.xyz;

    outClipSpaceCoord = uboVS.projectionMatrices[viewIndex] * viewPosition;
	gl_Position = outClipSpaceCoord;

	outTextureCoord = vec2(inPosition.xz / 2.0 + 0.5) * textureTilingFactor;

	//vec3 cameraPosition = (inverse(viewMatrices[viewIndex]) * vec4(0.0, 0.0, 0.0, 1.0)).xyz; // OPT - passed in UBO
	outToCameraVector = uboVS.cameraPositions[viewIndex].xyz - worldPosition.xyz;

	float vertexToCameraDistance = length(viewPosition.xyz);
	outVisibility = clamp(exp(-pow(vertexToCameraDistance * uboVS.density, uboVS.gradient)), 0.0, 1.0);
}

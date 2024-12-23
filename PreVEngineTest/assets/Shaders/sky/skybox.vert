#version 450
#extension GL_ARB_separate_shader_objects : enable
#ifdef ENABLE_XR
#extension GL_EXT_multiview : enable
#endif

layout(std140, binding = 0) uniform UniformBufferObject {
    mat4 modelMatrix;

	mat4 viewMatrices[MAX_VIEW_COUNT];

	mat4 projectionMatrices[MAX_VIEW_COUNT];
} uboVS;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTextureCoord;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec3 outTextureCoord;

void main()
{
#ifdef ENABLE_XR
	const int viewIndex = gl_ViewIndex;
#else
	const int viewIndex = 0;
#endif

	gl_Position = uboVS.projectionMatrices[viewIndex] * uboVS.viewMatrices[viewIndex] * uboVS.modelMatrix * vec4(inPosition, 1.0);
	outTextureCoord = inPosition;
}

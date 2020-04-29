#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "lights.glsl"

layout(std140, binding = 0) uniform UniformBufferObject {
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
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec3 inBiTangent;

layout(location = 0) out vec2 outTextureCoord;
layout(location = 1) out vec3 outWorldPosition;
layout(location = 2) out vec3 outViewPosition;
layout(location = 3) out float outVisibility;
layout(location = 4) out vec3 outTangent;
layout(location = 5) out vec3 outBiTangent;
layout(location = 6) out vec3 outNornal;

void main() 
{
	vec4 worldPosition = uboVS.modelMatrix * vec4(inPosition, 1.0);
	outWorldPosition = worldPosition.xyz;

	gl_ClipDistance[0] = dot(worldPosition, uboVS.clipPlane);

	vec4 viewPosition = uboVS.viewMatrix * worldPosition;
	outViewPosition = viewPosition.xyz;

	gl_Position = uboVS.projectionMatrix * viewPosition;

	outTextureCoord = (inTextureCoord / uboVS.textureNumberOfRows) + uboVS.textureOffset.xy;

	float vertexToCameraDistance = length(viewPosition.xyz);
	outVisibility = clamp(exp(-pow(vertexToCameraDistance * uboVS.density, uboVS.gradient)), 0.0, 1.0);

	outTangent = normalize(mat3(uboVS.viewMatrix) * mat3(uboVS.modelMatrix) * inTangent);
	outBiTangent = normalize(mat3(uboVS.viewMatrix) * mat3(uboVS.modelMatrix) * inBiTangent);
	outNornal = normalize(mat3(uboVS.viewMatrix) * mat3(uboVS.modelMatrix) * inNormal);
}

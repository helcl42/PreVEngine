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

	vec4 cameraPosition;

	Lightning lightning;	
	
	vec4 textureOffset;

	uint textureNumberOfRows;

	float density;
	float gradient;
} uboVS;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTextureCoord;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec3 inBiTnagent;

layout(location = 0) out vec2 outTextureCoord;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outWorldPosition;
layout(location = 3) out vec3 outViewPosition;
layout(location = 4) out vec3 outToCameraVector;
layout(location = 5) out float outVisibility;
layout(location = 6) out vec3 outToLightVector[MAX_LIGHT_COUNT];

void main() 
{
	vec4 worldPosition = uboVS.modelMatrix * vec4(inPosition, 1.0);
	outWorldPosition = worldPosition.xyz;

	gl_ClipDistance[0] = dot(worldPosition, uboVS.clipPlane);

	vec4 viewPosition = uboVS.viewMatrix * worldPosition;
	outViewPosition = viewPosition.xyz;

	gl_Position = uboVS.projectionMatrix * viewPosition;

	outTextureCoord = (inTextureCoord / uboVS.textureNumberOfRows) + uboVS.textureOffset.xy;

	vec3 normal = inNormal;
	outNormal = (uboVS.normalMatrix * vec4(inNormal, 0.0)).xyz;

	mat4 viewSpaceNormalMatrix = uboVS.viewMatrix * uboVS.normalMatrix;
	vec3 norm = normalize((viewSpaceNormalMatrix * vec4(inNormal, 0.0)).xyz);
	vec3 tangent = normalize((viewSpaceNormalMatrix * vec4(inTangent, 1.0)).xyz);
	vec3 biTangent = normalize(cross(norm, tangent));

	mat3 toTangentSpaceMatrix = mat3(
		tangent.x, biTangent.x, norm.x,
		tangent.y, biTangent.y, norm.y,
		tangent.z, biTangent.z, norm.z
	);

	for (int i = 0; i < uboVS.lightning.realCountOfLights; i++)
	{
		const Light light = uboVS.lightning.lights[i];
		outToLightVector[i] = toTangentSpaceMatrix * (light.position.xyz - viewPosition.xyz);
	}

	outToCameraVector = toTangentSpaceMatrix * -viewPosition.xyz;

	float vertexToCameraDistance = length(viewPosition.xyz);
	outVisibility = clamp(exp(-pow(vertexToCameraDistance * uboVS.density, uboVS.gradient)), 0.0, 1.0);
}
